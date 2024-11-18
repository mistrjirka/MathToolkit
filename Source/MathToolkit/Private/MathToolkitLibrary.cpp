// Fill out your copyright notice in the Description page of Project Settings.


#include "MathToolkitLibrary.h"

FVector MathToolkitLibrary::ConvertROSToUE(const FVector &ROSVector)
{
  // Convert ROS coordinates (right-handed) to Unreal Engine coordinates (left-handed) and scale from cm to m
  FVector UEVector;
  UEVector.X = ROSVector.X*100.0f;
  UEVector.Y = -ROSVector.Y*100.0f;
  UEVector.Z = ROSVector.Z*100.0f;

  return UEVector;
}

FRotator MathToolkitLibrary::ConvertROSToUEAngleDegree(const FRotator &rotation)
{
  // Convert ROS angles (right-handed) to Unreal Engine angles (left-handed) in radians
  FRotator UERotation;
  UERotation.Pitch = -rotation.Pitch;
  UERotation.Roll = rotation.Roll;
  UERotation.Yaw = -rotation.Yaw;
  if (UERotation.Yaw < 0)
  {
    UERotation.Yaw += 2 * PI;
  }
  if (UERotation.Pitch < 0)
  {
    UERotation.Pitch += 2 * PI;
  }
  if (UERotation.Roll < 0)
  {
    UERotation.Roll += 2 * PI;
  }

  return UERotation;
}

FVector MathToolkitLibrary::ConvertUEToROS(const FVector &UEVector)
{
  // Convert Unreal Engine coordinates (left-handed) to ROS coordinates (right-handed)
  FVector ROSVector;
  ROSVector.X = UEVector.X/100.0f;
  ROSVector.Y = -UEVector.Y/100.0f;
  ROSVector.Z = UEVector.Z/100.0f;

  return ROSVector;
}

FVector MathToolkitLibrary::ConvertUEToROSAngleDegree(const FVector &rotation)
{
  // Convert Unreal Engine angles (left-handed) to ROS angles (right-handed)
  FVector ROSRotation;
  ROSRotation.X = -rotation.X;
  ROSRotation.Y = rotation.Y;
  ROSRotation.Z = -rotation.Z;
  if (ROSRotation.Z < 0)
  {
    ROSRotation.Z += 360;
  }
  if (ROSRotation.X < 0)
  {
    ROSRotation.X += 360;
  }
  if (ROSRotation.Y < 0)
  {
    ROSRotation.Y += 360;
  }

  return ROSRotation;
}

FRotator MathToolkitLibrary::ConvertUEToROSAngleDegree(const FRotator &rotation)
{
  // Convert Unreal Engine angles (left-handed) to ROS angles (right-handed)
  FRotator ROSRotation;
  ROSRotation.Pitch = -rotation.Pitch;
  ROSRotation.Roll = rotation.Roll;
  ROSRotation.Yaw = -rotation.Yaw;
  if (ROSRotation.Yaw < 0)
  {
    ROSRotation.Yaw += 360;
  }
  if (ROSRotation.Pitch < 0)
  {
    ROSRotation.Pitch += 360;
  }
  if (ROSRotation.Roll < 0)
  {
    ROSRotation.Roll += 360;
  }

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
