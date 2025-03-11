#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "CircularBufferMT.h"

BEGIN_DEFINE_SPEC(FMathToolkitTest, "MathToolkit.CircularBuffer",
    EAutomationTestFlags::ApplicationContextMask | 
    EAutomationTestFlags::ProductFilter)
END_DEFINE_SPEC(FMathToolkitTest)

void FMathToolkitTest::Define()
{
    Describe("CircularBufferMT", [this]()
    {
        It("Should handle basic operations", [this]()
        {
            CircularBufferMT<int, 3> buffer;
            TestTrue("Buffer should be empty initially", buffer.empty());
            TestEqual("Size should be 0 initially", buffer.size(), 0);
            
            buffer.put(1);
            TestEqual("Size should be 1 after first insert", buffer.size(), 1);
            TestFalse("Buffer should not be empty", buffer.empty());
            
            buffer.put(2);
            buffer.put(3);
            TestTrue("Buffer should be full", buffer.full());
            
            buffer.put(4);  // Should overwrite oldest element (1)
            
            int sum = 0;
            buffer.for_each([&sum](int val) { sum += val; });
            TestEqual("Sum should be 2+3+4=9", sum, 9);
        });
        
        It("Should preserve order", [this]()
        {
            CircularBufferMT<int, 4> buffer;
            for(int i = 1; i <= 4; ++i) {
                buffer.put(i);
            }
            
            int expected = 1;
            buffer.for_each([&](int val) {
                TestEqual(FString::Printf(TEXT("Value should be %d"), expected), val, expected);
                expected++;
            });
        });
    });
}
