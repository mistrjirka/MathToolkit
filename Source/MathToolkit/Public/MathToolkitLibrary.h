#pragma once

#include "CoreMinimal.h"
#include "CircularBufferMT.h"


/**
 * 
 */
class MATHTOOLKIT_API MathToolkitLibrary
{
public:
    static FVector ConvertUEToROS(const FVector& UEVector);

    static FVector ConvertUEToROSAngleDegree(const FVector& rotation);

    static FRotator ConvertUEToROSAngleDegree(const FRotator& rotation);

    static FRotator ConvertROSToUEAngleDegree(const FRotator& rotation);

    static FVector ConvertROSToUE(const FVector& ROSVector);

    template <typename T, size_t S>
    static void calculateLinearFit(const CircularBufferMT<T, S>& circBuffer, FVector& vector_fit_a, FVector& vector_fit_b, bool print = false);

    static std::pair<FVector,FVector> CalculateSphericalFromDepth(
        float distance, 
        float x, 
        float y, 
        float FOVH, 
        uint32 width,
        uint32 height
    );
    
    static std::pair<float, float> CalculateNDCCoordinates(
    float alpha,
    float beta,
    float FOVH,
    uint32 width,
    uint32 height);
    
    static float calculateHorizontalFOV(float senzorWidth, float focalLength);
};

template <typename T, size_t S>
void MathToolkitLibrary::calculateLinearFit(const CircularBufferMT<T, S>& circBuffer, FVector& vector_fit_a, FVector& vector_fit_b, bool print)
{
    double sum_x = 0.0;
    double sum_y[3] = {0.0, 0.0, 0.0};
    double sum_xy[3] = {0.0, 0.0, 0.0};
    double sum_xx = 0.0;
    int n = 0;

    // Calculate sums for regression using the iterator
    circBuffer.for_each([&](const T& elem) {
        double x = elem.Value;
        sum_x += x;
        sum_xx += x * x;

        sum_y[0] += elem.Key.X;
        sum_y[1] += elem.Key.Y;
        sum_y[2] += elem.Key.Z;

        sum_xy[0] += x * elem.Key.X;
        sum_xy[1] += x * elem.Key.Y;
        sum_xy[2] += x * elem.Key.Z;

        n++;
    });

    if (n < 2)
    {
        vector_fit_a = FVector::ZeroVector;
        vector_fit_b = FVector::ZeroVector;
        return;
    }

    double denominator = n * sum_xx - sum_x * sum_x;
    if (FMath::IsNearlyZero(denominator))
    {
        vector_fit_a = FVector::ZeroVector;
        vector_fit_b = FVector::ZeroVector;
        return;
    }

    vector_fit_a.X = (n * sum_xy[0] - sum_x * sum_y[0]) / denominator;
    vector_fit_a.Y = (n * sum_xy[1] - sum_x * sum_y[1]) / denominator;
    vector_fit_a.Z = (n * sum_xy[2] - sum_x * sum_y[2]) / denominator;

    vector_fit_b.X = (sum_y[0] - vector_fit_a.X * sum_x) / n;
    vector_fit_b.Y = (sum_y[1] - vector_fit_a.Y * sum_x) / n;
    vector_fit_b.Z = (sum_y[2] - vector_fit_a.Z * sum_x) / n;

    if (print)
    {
        UE_LOG(LogTemp, Warning, TEXT("Linear fit parameters:"));
        UE_LOG(LogTemp, Warning, TEXT("a: %f, %f, %f"), vector_fit_a.X, vector_fit_a.Y, vector_fit_a.Z);
        UE_LOG(LogTemp, Warning, TEXT("b: %f, %f, %f"), vector_fit_b.X, vector_fit_b.Y, vector_fit_b.Z);
    }
}
