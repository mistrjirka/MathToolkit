#pragma once

#include "CoreMinimal.h"


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
