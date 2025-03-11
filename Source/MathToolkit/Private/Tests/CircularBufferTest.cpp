#include "Misc/AutomationTest.h"
#include "CircularBufferMT.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCircularBufferBasicTest, "MathToolkit.CircularBuffer.Basic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCircularBufferBasicTest::RunTest(const FString& Parameters)
{
    CircularBufferMT<int, 3> buffer;
    
    TestTrue(TEXT("Buffer should be empty initially"), buffer.empty());
    TestEqual(TEXT("Size should be 0 initially"), static_cast<int32>(buffer.size()), 0);
    
    buffer.put(1);
    TestEqual(TEXT("Size should be 1 after first insert"), static_cast<int32>(buffer.size()), 1);
    TestFalse(TEXT("Buffer should not be empty"), buffer.empty());
    
    buffer.put(2);
    buffer.put(3);
    TestTrue(TEXT("Buffer should be full"), buffer.full());
    
    buffer.put(4);  // Should overwrite oldest element (1)
    
    int32 sum = 0;
    buffer.for_each([&sum](int val) { sum += val; });
    TestEqual(TEXT("Sum should be 2+3+4=9"), sum, 9);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCircularBufferOrderTest, "MathToolkit.CircularBuffer.Order",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCircularBufferOrderTest::RunTest(const FString& Parameters)
{
    CircularBufferMT<int, 4> buffer;
    
    for(int32 i = 1; i <= 4; ++i) {
        buffer.put(i);
    }
    
    int32 expected = 1;
    buffer.for_each([&](int val) {
        TestEqual(FString::Printf(TEXT("Value should be %d"), expected), val, expected);
        expected++;
    });

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCircularBufferLargeTest, "MathToolkit.CircularBuffer.Large",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCircularBufferLargeTest::RunTest(const FString& Parameters)
{
    CircularBufferMT<int, 10> buffer;
    
    // Fill buffer with 1-10
    for(int32 i = 1; i <= 10; ++i) {
        buffer.put(i);
    }
    
    TestTrue(TEXT("Buffer should be full"), buffer.full());
    TestEqual(TEXT("Size should be 10"), static_cast<int32>(buffer.size()), 10);
    
    // Add more elements to test overwriting
    for(int32 i = 11; i <= 15; ++i) {
        buffer.put(i);
    }
    
    // Should now contain 6-15
    int32 expected = 6;
    buffer.for_each([&](int val) {
        TestEqual(FString::Printf(TEXT("Value should be %d"), expected), val, expected);
        expected++;
    });

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCircularBufferOverflowTest, "MathToolkit.CircularBuffer.Overflow",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCircularBufferOverflowTest::RunTest(const FString& Parameters)
{
    CircularBufferMT<int, 3> buffer;
    
    // Fill and overflow multiple times
    for(int32 i = 0; i < 10; ++i) {
        buffer.put(i);
    }
    
    // Should contain last 3 elements: 7,8,9
    int32 expected = 7;
    buffer.for_each([&](int val) {
        TestEqual(FString::Printf(TEXT("Value should be %d"), expected), val, expected);
        expected++;
    });

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCircularBufferReadWriteTest, "MathToolkit.CircularBuffer.ReadWrite",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCircularBufferReadWriteTest::RunTest(const FString& Parameters)
{
    CircularBufferMT<int32, 4> buffer;
    
    // Initial write sequence
    for(int32 i = 1; i <= 4; ++i) {
        buffer.put(i);
    }
    
    // Verify initial content (1,2,3,4)
    {
        TArray<int32> values;
        buffer.for_each([&values](int32 val) { values.Add(val); });
        
        TestEqual(TEXT("Buffer should contain 4 elements"), values.Num(), 4);
        TestEqual(TEXT("First element should be 1"), values[0], 1);
        TestEqual(TEXT("Last element should be 4"), values[3], 4);
    }
    
    // Add two more elements (should now contain 3,4,5,6)
    buffer.put(5);
    buffer.put(6);
    
    {
        TArray<int32> values;
        buffer.for_each([&values](int32 val) { values.Add(val); });
        
        TestEqual(TEXT("Buffer should still contain 4 elements"), values.Num(), 4);
        TestEqual(TEXT("First element should now be 3"), values[0], 3);
        TestEqual(TEXT("Last element should now be 6"), values[3], 6);
    }
    
    // Add more elements with verification after each add
    for(int32 i = 7; i <= 9; ++i) {
        buffer.put(i);
        
        TArray<int32> values;
        buffer.for_each([&values](int32 val) { values.Add(val); });
        
        TestEqual(TEXT("Buffer should maintain 4 elements"), values.Num(), 4);
        TestEqual(FString::Printf(TEXT("First element should be %d"), i-3), values[0], i-3);
        TestEqual(FString::Printf(TEXT("Last element should be %d"), i), values[3], i);
    }

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCircularBufferStressTest, "MathToolkit.CircularBuffer.Stress",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCircularBufferStressTest::RunTest(const FString& Parameters)
{
    CircularBufferMT<int32, 3> buffer;
    
    // Multiple cycles of fill, read, partial fill, read
    for(int32 cycle = 0; cycle < 5; ++cycle) {
        // Fill buffer
        for(int32 i = 1; i <= 3; ++i) {
            buffer.put(i + (cycle * 10));
        }
        
        // Verify contents
        {
            TArray<int32> values;
            buffer.for_each([&values](int32 val) { values.Add(val); });
            
            TestEqual(TEXT("Buffer size should be 3"), values.Num(), 3);
            for(int32 i = 0; i < 3; ++i) {
                TestEqual(
                    FString::Printf(TEXT("Element %d should be %d"), i, (i + 1 + (cycle * 10))),
                    values[i],
                    i + 1 + (cycle * 10)
                );
            }
        }
        
        // Partial update (replace middle element)
        buffer.put(99 + cycle);
        
        // Verify modified contents
        {
            TArray<int32> values;
            buffer.for_each([&values](int32 val) { values.Add(val); });
            
            TestEqual(TEXT("Buffer size should remain 3"), values.Num(), 3);
            TestEqual(TEXT("First element should be from original fill"), 
                     values[0], 2 + (cycle * 10));
            TestEqual(TEXT("Middle element should be from original fill"), 
                     values[1], 3 + (cycle * 10));
            TestEqual(TEXT("Last element should be newly added"), 
                     values[2], 99 + cycle);
        }
    }

    return true;
}
