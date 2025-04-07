// Fill out your copyright notice in the Description page of Project Settings.


#include "MathToolkitLibrary.h"

FVector MathToolkitLibrary::ConvertROSToUE(const FVector &ROSVector)
{
  // Convert ROS coordinates (right-handed) to Unreal Engine coordinates (left-handed) and scale from m to cm
  // ROS: X forward, Y left, Z up -> UE: X forward, Y right, Z up
  FVector UEVector;
  UEVector.X = ROSVector.X * 100.0f;  // Scale from m to cm
  UEVector.Y = -ROSVector.Y * 100.0f; // Flip Y axis
  UEVector.Z = ROSVector.Z * 100.0f;  // Scale from m to cm

  return UEVector;
}

FRotator MathToolkitLibrary::ConvertROSToUEAngleDegree(const FRotator &rotation)
{
  return FRotator(FMath::Fmod(rotation.Pitch + 360.0f, 360.0f),  FMath::Fmod(-rotation.Roll + 360.0f, 360.0f),  FMath::Fmod((-rotation.Yaw)+ 360.0f, 360.0f));
}

FVector MathToolkitLibrary::ConvertUEToROS(const FVector &UEVector)
{
  // Convert Unreal Engine coordinates (left-handed) to ROS coordinates (right-handed) and scale from cm to m
  // UE: X forward, Y right, Z up -> ROS: X forward, Y left, Z up
  FVector ROSVector;
  ROSVector.X = UEVector.X / 100.0f;  // Scale from cm to m
  ROSVector.Y = -UEVector.Y / 100.0f; // Flip Y axis
  ROSVector.Z = UEVector.Z / 100.0f;  // Scale from cm to m

  return ROSVector;
}

FVector MathToolkitLibrary::ConvertUEToROSAngleDegree(const FVector &rotation)
{
  // Convert Unreal Engine angles (left-handed) to ROS angles (right-handed)
  FVector ROSRotation;
  ROSRotation.X = rotation.X;         // Keep X (pitch) the same
  ROSRotation.Y = -rotation.Y;        // Flip Y (roll)
  ROSRotation.Z = -rotation.Z + 180.0f; // Flip Z (yaw) and add 180 degrees for proper orientation
  
  // Normalize angles to [0, 360)
  ROSRotation.X = FMath::Fmod(ROSRotation.X + 360.0f, 360.0f);
  ROSRotation.Y = FMath::Fmod(ROSRotation.Y + 360.0f, 360.0f);
  ROSRotation.Z = FMath::Fmod(ROSRotation.Z + 360.0f, 360.0f);

  return ROSRotation;
}

FRotator MathToolkitLibrary::ConvertUEToROSAngleDegree(const FRotator &rotation)
{
  // Convert Unreal Engine angles (left-handed) to ROS angles (right-handed)
  FRotator ROSRotation;
  ROSRotation.Pitch = rotation.Pitch;   // Keep pitch the same
  ROSRotation.Roll = -rotation.Roll;    // Flip roll
  ROSRotation.Yaw = -rotation.Yaw + 180.0f; // Flip yaw and add 180 degrees for proper orientation
  
  // Normalize angles to [0, 360)
  ROSRotation.Pitch = FMath::Fmod(ROSRotation.Pitch + 360.0f, 360.0f);
  ROSRotation.Roll = FMath::Fmod(ROSRotation.Roll + 360.0f, 360.0f);
  ROSRotation.Yaw = FMath::Fmod(ROSRotation.Yaw + 360.0f, 360.0f);

  return ROSRotation;
}

std::pair<FVector, FVector> MathToolkitLibrary::CalculateSphericalFromDepth(
    float Depth,
    float x,
    float y,
    float FOVH,
    uint32 width,
    uint32 height)
{
    Depth *= 0.01f;

    float NDC_X = (2.0f * x / width) - 1.0f;
    float NDC_Y = 1.0f - (2.0f * y / height);

    float AspectRatio = static_cast<float>(width) / height;
    float FOVHRad = FMath::DegreesToRadians(FOVH);
    float FOVVRad = 2.0f * FMath::Atan(FMath::Tan(FOVHRad / 2.0f) / AspectRatio);

    float halfFOVHRad = FOVHRad / 2.0f;
    float halfFOVVRad = FOVVRad / 2.0f;
    float tanHalfFOVHRad = FMath::Tan(halfFOVHRad);
    float tanHalfFOVVRad = FMath::Tan(halfFOVVRad);

    float CameraX = Depth;
    float CameraY = NDC_X * Depth * tanHalfFOVHRad;
    float CameraZ = NDC_Y * Depth * tanHalfFOVVRad;

    FVector point(CameraX * 100.0f, CameraY * 100.0f, CameraZ * 100.0f);

    float r = FMath::Sqrt(FMath::Square(point.X) + FMath::Square(point.Y) + FMath::Square(point.Z));
    float vCoord = FMath::Acos(point.Z / r);
    float hCoord = FMath::Atan2(point.Y, point.X);

    vCoord = (PI / 2.0f) - vCoord;
    FVector spherical(r, hCoord, vCoord);

    return std::pair<FVector, FVector>(spherical, point);
}

std::pair<FVector, FVector> MathToolkitLibrary::CalculateSphericalFromDepth(
    float Depth,
    float x,
    float y,
    float tanHalfFOVHRad,
    float tanHalfFOVVRad,
    uint32 width,
    uint32 height)
{
    Depth *= 0.01f;

    float NDC_X = (2.0f * x / width) - 1.0f;
    float NDC_Y = 1.0f - (2.0f * y / height);

    float CameraX = Depth;
    float CameraY = NDC_X * Depth * tanHalfFOVHRad;
    float CameraZ = NDC_Y * Depth * tanHalfFOVVRad;

    FVector point(CameraX * 100.0f, CameraY * 100.0f, CameraZ * 100.0f);

    float r = FMath::Sqrt(FMath::Square(point.X) + FMath::Square(point.Y) + FMath::Square(point.Z));
    float vCoord = FMath::Acos(point.Z / r);
    float hCoord = FMath::Atan2(point.Y, point.X);

    vCoord = (PI / 2.0f) - vCoord;
    FVector spherical(r, hCoord, vCoord);

    return std::pair<FVector, FVector>(spherical, point);
}



std::pair<float, float> MathToolkitLibrary::CalculateNDCCoordinates(
    float beta,
    float alpha,
    float FOVH,
    uint32 width,
    uint32 height)
{
    float AspectRatio = static_cast<float>(height) / width;
    float FOVHRad = FMath::DegreesToRadians(FOVH);
    float FOVVRad = FMath::DegreesToRadians(FOVH) * AspectRatio;

    float a = FMath::Tan(FOVHRad / 2.0f);
    float b = FMath::Tan(FOVVRad / 2.0f);

    float cos_alpha = FMath::Cos(alpha);
    float sin_alpha = FMath::Sin(alpha);
    float sin_beta = FMath::Sin(beta);

    
    // For ndc_x, you should use the appropriate formula. Assuming you need ndc_x as:
    float ndc_x = FMath::Sqrt(-((FMath::Square(sin_alpha) * FMath::Square(sin_beta)) / (FMath::Square(a) * (-1 + FMath::Square(cos_alpha) + FMath::Square(sin_alpha) * FMath::Square(sin_beta)))));    //UE_LOG(LogTemp, Warning, TEXT("COS_alpha: %f, SIN_alpha: %f, SIN_beta: %f, a: %f, b: %f"), cos_alpha, sin_alpha, sin_beta, a, b);
    UE_LOG(LogTemp, Warning, TEXT("NDC: %f"), ndc_x);

    float x = (ndc_x + 1.0f) * width / 2.0f;
    float y = (1.0f - 0) * height / 2.0f;

    return std::pair<float, float>(x, y);
}
  
float MathToolkitLibrary::calculateHorizontalFOV(float senzorWidth, float focalLength)
{
  return FMath::RadiansToDegrees(2.0f * FMath::Atan(senzorWidth / (2.0f * focalLength)));
}

