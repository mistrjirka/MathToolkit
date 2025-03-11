#include "Misc/AutomationTest.h"
#include "MathToolkitLibrary.h"
#include "CircularBufferMT.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLinearFitBasicTest, "MathToolkit.LinearFit.Basic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLinearFitBasicTest::RunTest(const FString& Parameters)
{
    // Create buffer with known values that should produce y = 2x + 1
    CircularBufferMT<TPair<FVector, uint32>, 3> buffer;
    
    buffer.put(TPair<FVector, uint32>(FVector(1,1,1), 0));
    buffer.put(TPair<FVector, uint32>(FVector(3,3,3), 1));
    buffer.put(TPair<FVector, uint32>(FVector(5,5,5), 2));

    FVector fit_a, fit_b;
    MathToolkitLibrary::calculateLinearFit(buffer, fit_a, fit_b, false);

    // Should approximate y = 2x + 1
    TestTrue(TEXT("Slope should be approximately 2"), 
        FMath::IsNearlyEqual(fit_a.X, 2.0f, 0.01f) &&
        FMath::IsNearlyEqual(fit_a.Y, 2.0f, 0.01f) &&
        FMath::IsNearlyEqual(fit_a.Z, 2.0f, 0.01f));

    TestTrue(TEXT("Intercept should be approximately 1"), 
        FMath::IsNearlyEqual(fit_b.X, 1.0f, 0.01f) &&
        FMath::IsNearlyEqual(fit_b.Y, 1.0f, 0.01f) &&
        FMath::IsNearlyEqual(fit_b.Z, 1.0f, 0.01f));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLinearFitConstantTest, "MathToolkit.LinearFit.Constant",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLinearFitConstantTest::RunTest(const FString& Parameters)
{
    CircularBufferMT<TPair<FVector, uint32>, 5> buffer;
    FVector constantVector(1.0f, 2.0f, 3.0f);
    
    // Add same vector multiple times with different timestamps
    for(uint32 i = 0; i < 5; ++i) {
        buffer.put(TPair<FVector, uint32>(constantVector, i));
    }

    FVector fit_a, fit_b;
    MathToolkitLibrary::calculateLinearFit(buffer, fit_a, fit_b, false);

    // Slope should be zero since all points are the same
    TestTrue(TEXT("Slope should be zero"), 
        FMath::IsNearlyZero(fit_a.X) &&
        FMath::IsNearlyZero(fit_a.Y) &&
        FMath::IsNearlyZero(fit_a.Z));

    // Intercept should be equal to the constant vector
    TestTrue(TEXT("Intercept should match input vector"), 
        FMath::IsNearlyEqual(fit_b.X, constantVector.X, 0.01f) &&
        FMath::IsNearlyEqual(fit_b.Y, constantVector.Y, 0.01f) &&
        FMath::IsNearlyEqual(fit_b.Z, constantVector.Z, 0.01f));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLinearFitPerfectLineTest, "MathToolkit.LinearFit.PerfectLine",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLinearFitPerfectLineTest::RunTest(const FString& Parameters)
{
    CircularBufferMT<TPair<FVector, uint32>, 5> buffer;
    
    // Create perfect line: y = 2x + 1 for each component
    for(uint32 i = 0; i < 5; ++i) {
        float x = static_cast<float>(i);
        FVector value(2.0f * x + 1.0f, 2.0f * x + 1.0f, 2.0f * x + 1.0f);
        buffer.put(TPair<FVector, uint32>(value, i));
    }

    FVector fit_a, fit_b;
    MathToolkitLibrary::calculateLinearFit(buffer, fit_a, fit_b, false);

    // Should exactly match y = 2x + 1
    TestTrue(TEXT("Slope should be 2"), 
        FMath::IsNearlyEqual(fit_a.X, 2.0f, 0.01f) &&
        FMath::IsNearlyEqual(fit_a.Y, 2.0f, 0.01f) &&
        FMath::IsNearlyEqual(fit_a.Z, 2.0f, 0.01f));

    TestTrue(TEXT("Intercept should be 1"), 
        FMath::IsNearlyEqual(fit_b.X, 1.0f, 0.01f) &&
        FMath::IsNearlyEqual(fit_b.Y, 1.0f, 0.01f) &&
        FMath::IsNearlyEqual(fit_b.Z, 1.0f, 0.01f));

    return true;
}
