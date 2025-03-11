#include "Misc/AutomationTest.h"
#include "CircularBufferMT.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCircularBufferTest, "MathToolkit.CircularBuffer",
    EAutomationTestFlags::ApplicationContextMask | 
    EAutomationTestFlags::ProductFilter)

bool FCircularBufferTest::RunTest(const FString& Parameters)
{
    // Basic operations test
    {
        CircularBufferMT<int, 3> buffer;
        TestTrue("Buffer should be empty initially", buffer.empty());
        TestTrue("Size should be 0 initially", buffer.size() == 0);
        
        buffer.put(1);
        TestTrue("Size should be 1 after first insert", buffer.size() == 1);
        TestFalse("Buffer should not be empty", buffer.empty());
        
        buffer.put(2);
        buffer.put(3);
        TestTrue("Buffer should be full", buffer.full());
        
        buffer.put(4);  // Should overwrite oldest element (1)
        
        int sum = 0;
        buffer.for_each([&sum](int val) { sum += val; });
        TestTrue("Sum should be 2+3+4=9", sum == 9);
    }
    
    // Order preservation test
    {
        CircularBufferMT<int, 4> buffer;
        for(int i = 1; i <= 4; ++i) {
            buffer.put(i);
        }
        
        int expected = 1;
        buffer.for_each([&](int val) {
            TestTrue(FString::Printf(TEXT("Value should be %d, got %d"), expected, val), 
                val == expected);
            expected++;
        });
    }
    
    return true;
}
