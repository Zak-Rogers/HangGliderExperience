// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Glider_Pawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UDestructibleComponent;

UCLASS()
class AGlider_Pawn : public APawn
{
	GENERATED_BODY()

public:
	AGlider_Pawn();

	/** Get function for the gliders launched varible.*/
	UFUNCTION(BlueprintCallable) bool GetLaunched();
	/** Get function for the gliders Stalling varible.*/
	UFUNCTION(BlueprintCallable) bool GetStalling();
	/** Get function for the gliders Crashing varible.*/
	UFUNCTION(BlueprintCallable) bool GetCrashing();

	/** Get function for the gliders Velocity varible.*/
	UFUNCTION(BlueprintCallable) FVector GetGliderVelocity();
	/** Get function for the gliders forward speed varible.*/
	UFUNCTION(BlueprintCallable) float GetForwardSpeed();
	/** Get function for the gliders maximum forward speed.*/
	UFUNCTION(BlueprintCallable) float GetMaximumForwardSpeed();


private:
	float ForwardSpeed = 0;
	float ClimbSpeed;
	FVector GliderVelocity; 
	const float GRAVITY = 9.8f;
	FRotator TargetRotation;
	float TargetRollSpeed = 0.0f;
	float TargetPitchSpeed = 0.0f;
	float TargetYawSpeed = 0.0f;
	float CurrentRollSpeed = 0.0f;
	float CurrentPitchSpeed = 0.0f;
	/** Rotation Speed.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|RotationSettings") float RotationSpeed = 25.0f;
	/** Minimum roll rotation.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|RotationSettings") float MinRollRotation = -30.0f;
	/** Maximum roll rotation.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|RotationSettings") float MaxRollRotation = 30.0f;
	/** Minimum pitch rotation.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|RotationSettings") float MinPitchRotation = -30.0f;
	/** Maximum pitch rotation.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|RotationSettings") float MaxPitchRotation = 20.0f;
	/** When the Pitch of the glider is below this Degree, the climb speed of the glider is halved*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|RotationSettings") float PositivePitchSpeedModifier = 7.0f;
	/** When the Pitch of the glider is above this Degree, the climb speed of the glider is halved.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|RotationSettings") float NegativePitchSpeedModifier = -15.0f;
	/** Multiplier for gravity.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings") float GravityMultiplier = 1.0f;
	/** Drag slows the glider of time.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings") float Drag = 5.0f;
	/** Minimum forward speed of the glider.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|SpeedSettings") float MinSpeed = 0.0f;
	/** Maximum forward speed of the glider.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|SpeedSettings") float MaxSpeed = 2000.0f;
	/** Minimum climb speed of the glider.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|SpeedSettings") float MinClimbSpeed = -150.0f;
	/** Maximum climb speed of the glider.*/
	UPROPERTY(EditAnywhere, Category = "FlightSettings|SpeedSettings") float MaxClimbSpeed = 150.0f;

	
	bool Stalling = false;
	float StallHeight = 0.0f;
	/** Speed at which the glider will begin to stall.*/
	UPROPERTY(EditAnywhere, Category="StallSettings") int StallSpeed = 300.0f;
	/** The distance the glider will fall when stalling.*/
	UPROPERTY(EditAnywhere, Category = "StallSettings") int StallDistance = 1000;
	/** The amount of positive degrees pitching  the glider can do before stalling.*/
	UPROPERTY(EditAnywhere, Category = "StallSettings") float PointPitchStalls = 13.0f;

	bool Crashing = false;

	float LaunchPower = 0.0f;
	float LaunchTimer = 0.0f;
	bool Launching = false;
	bool Launched = false;
	FString LastFootDown = "";
	/** The maximum launch power.*/
	UPROPERTY(EditAnywhere, Category = "LaunchSettings") float MaxLaunchPower = 400.0f;
	/** The amount the launch power reduces over time.*/
	UPROPERTY(EditAnywhere, Category = "LaunchSettings") float LaunchPowerReduction = 30;
	/** The amount of power each leg outputs during the launch.*/
	UPROPERTY(EditAnywhere, Category = "LaunchSettings") int LegPower = 10;
	/** The duration of the launch.*/
	UPROPERTY(EditAnywhere, Category = "LaunchSettings") float LaunchDuration = 7.5f;

	UCameraComponent* Camera;
	/** Target Z offset for camera for launch animation.*/
	UPROPERTY(EditAnywhere, Category = "CameraSettings|Targets") float TargetSocketOffsetZ = 100.0f;
	/** Target spring arm length for spirng arm length for launch animation.*/
	UPROPERTY(EditAnywhere, Category = "CameraSettings|Targets") float TargetSpringArmLength = 400.0f;
	/** Camera offset move speed for launch animation.*/
	UPROPERTY(EditAnywhere, Category = "CameraSettings|Speed") float SocketOffsetMoveSpeed = 30.0f;
	/** Spring arm move speed for launch animation.*/
	UPROPERTY(EditAnywhere, Category = "CameraSettings|Speed") float ArmLengthMoveSpeed = 90.0f;
	/** Spring arm component for camera.*/
	UPROPERTY(EditAnywhere, Category = "CameraSettings") USpringArmComponent* SpringArm;

	/** Static mesh component for glider collision.*/
	UPROPERTY(EditAnywhere, Category = "Components") UStaticMeshComponent* GliderMeshComponent;
	/** Skeletal mesh component for the pilot.*/
	UPROPERTY(EditAnywhere, Category = "Components") USkeletalMeshComponent* PilotMeshComponent;
	/** Destrucible mesh component for the glider model.*/
	UPROPERTY(EditAnywhere, Category = "Components")	UDestructibleComponent* GliderMeshDestructible;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void Input_XAxis(float AxisValue);
	void Input_YAxis(float AxisValue);
	void LFootPress();
	void RFootPress();

	void MoveCamera(float DeltaTime);
	void Launch(float DeltaTime);
	FRotator CalculateRotation(float DeltaTime);
	void AdjustIfStalling(FRotator& CurrentRotation, float DeltaTime);
	float CalculateClimbSpeed();
	void Crash();
}; 
