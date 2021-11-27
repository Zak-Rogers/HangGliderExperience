// Fill out your copyright notice in the Description page of Project Settings.


#include "Glider_Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DestructibleComponent.h"


AGlider_Pawn::AGlider_Pawn()
{
	PrimaryActorTick.bCanEverTick = true;

	GliderMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glider Mesh Component"));
	GliderMeshComponent->SetVisibility(false);
	RootComponent = GliderMeshComponent;
	
	PilotMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Pilot Mesh Component"));
	PilotMeshComponent->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArm->SetupAttachment(PilotMeshComponent);
	SpringArm->TargetArmLength = 200.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	Camera->SetupAttachment(SpringArm);	

	GliderMeshDestructible = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Glider Mesh Destructible"));
	GliderMeshDestructible->SetupAttachment(RootComponent);
}

void AGlider_Pawn::Tick(float DeltaTime)
{
	if (Crashing) return;

	if (!Launched)
	{
		Launch(DeltaTime);
	}
	else
	{ 
		TargetRotation = CalculateRotation(DeltaTime);

		MoveCamera(DeltaTime);

		bool FailedLaunch = ForwardSpeed < StallSpeed && Launching;
		if (FailedLaunch)
		{
			Crash();
			GliderMeshDestructible->ApplyDamage(10000.0f, GetActorLocation(), FVector(0,0,-1), 5.0f);
		}
		else
		{
			Launching = false;
		}

		AddActorLocalRotation(TargetRotation);

		FRotator CurrentRotation = GetActorRotation();
		AdjustIfStalling(CurrentRotation, DeltaTime);
		SetActorRotation(CurrentRotation);

		ClimbSpeed = CalculateClimbSpeed();//-1
		ClimbSpeed = FMath::Clamp(ClimbSpeed, MinClimbSpeed, MaxClimbSpeed);

		ForwardSpeed = FMath::FInterpTo(ForwardSpeed, ForwardSpeed - ClimbSpeed, DeltaTime, 2.0f);
		ForwardSpeed = FMath::Clamp(ForwardSpeed, MinSpeed, MaxSpeed);
		
		if(ForwardSpeed <= StallSpeed)
		{
			Stalling = true;
			StallHeight = GetActorLocation().Z;
		}

		GliderVelocity = FVector(ForwardSpeed * DeltaTime, 0.0f, (ClimbSpeed * DeltaTime ) - (GRAVITY * GravityMultiplier * DeltaTime));
	}

	AddActorLocalOffset(GliderVelocity, true);

	ForwardSpeed -= Drag * DeltaTime;
}

void AGlider_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	InputComponent->BindAxis("Move_X", this, &AGlider_Pawn::Input_XAxis);
	InputComponent->BindAxis("Move_Y", this, &AGlider_Pawn::Input_YAxis);

	InputComponent->BindAction("LaunchL", IE_Pressed, this, &AGlider_Pawn::LFootPress);
	InputComponent->BindAction("LaunchR", IE_Pressed, this, &AGlider_Pawn::RFootPress);
}

void AGlider_Pawn::LFootPress()
{
	if (!Launched && (LastFootDown == "RightFoot" || LastFootDown == ""))
	{
		LaunchPower += LegPower;
		LastFootDown = "LeftFoot";
	}
}

void AGlider_Pawn::RFootPress()
{
	if (!Launched && (LastFootDown == "LeftFoot" || LastFootDown == ""))
	{
		LaunchPower += LegPower;
		LastFootDown = "RightFoot";
	}
}

void AGlider_Pawn::Input_XAxis(float AxisValue)
{
	TargetRollSpeed = AxisValue * RotationSpeed;
	TargetYawSpeed = AxisValue * RotationSpeed;

	if (AxisValue == 0)
	{
		TargetRollSpeed = GetActorRotation().Roll * -2.f;
	}
}

void AGlider_Pawn::Input_YAxis(float AxisValue)
{
	TargetPitchSpeed = AxisValue * RotationSpeed ;

	if (AxisValue == 0)
	{
		TargetPitchSpeed = GetActorRotation().Pitch * -2.f;
	}
}

void AGlider_Pawn::MoveCamera(float DeltaTime)
{
	float SocketOffsetZ = SpringArm->SocketOffset.Z;
	SpringArm->SocketOffset.Z = FMath::FInterpConstantTo(SocketOffsetZ, TargetSocketOffsetZ, DeltaTime, SocketOffsetMoveSpeed);
	float TargetArmLength = SpringArm->TargetArmLength;
	SpringArm->TargetArmLength = FMath::FInterpConstantTo(TargetArmLength, TargetSpringArmLength, DeltaTime, ArmLengthMoveSpeed);
}

void AGlider_Pawn::Launch(float DeltaTime)
{
	LaunchPower -= DeltaTime * LaunchPowerReduction;
	LaunchPower = FMath::Clamp(LaunchPower, 0.0f, MaxLaunchPower);
	ForwardSpeed = LaunchPower;
	LaunchTimer += DeltaTime;

	if (LaunchTimer >= LaunchDuration)
	{
		LaunchTimer = 0;
		Launched = true;
		Launching = true;
	}

	GliderVelocity = FVector(ForwardSpeed * DeltaTime, 0.0f, 0.0f);
}

FRotator AGlider_Pawn::CalculateRotation(float DeltaTime)
{
	FRotator Rotation = FRotator(0.0f, 0.0f, 0.0f);
	Rotation.Roll = TargetRollSpeed  * DeltaTime;
	Rotation.Pitch = TargetPitchSpeed * DeltaTime;
	Rotation.Yaw = TargetYawSpeed * DeltaTime;

	return Rotation;
}

void AGlider_Pawn::AdjustIfStalling(FRotator& CurrentRotation, float DeltaTime)
{
	if (Stalling)
	{
		CurrentRotation.Pitch = FMath::FInterpTo(CurrentRotation.Pitch, -90.0f, DeltaTime, 2.0f);
		CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, -360.0f, DeltaTime, 5.0f);
		CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 320.0f, DeltaTime, 4.0f);
		if (GetActorLocation().Z <= StallHeight - StallDistance) Stalling = false;
	}
	else
	{
		CurrentRotation.Roll = FMath::Clamp(CurrentRotation.Roll, MinRollRotation, MaxRollRotation);
		CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch, MinPitchRotation, MaxPitchRotation); 
		if (CurrentRotation.Pitch > PointPitchStalls) Stalling = true;
		StallHeight = GetActorLocation().Z;
	}
}

float AGlider_Pawn::CalculateClimbSpeed()
{
	float Speed;
	const float Pi = 3.1415;
	float AngleOfAttackInRadians = (GetActorRotation().Pitch * Pi) / 180; 
	float CurrentPitch = GetActorRotation().Pitch;
	float CurrentRoll = GetActorRotation().Roll;

	Speed = ForwardSpeed * FMath::Sin(AngleOfAttackInRadians);	

	if (CurrentPitch < 0.5f && CurrentPitch >= NegativePitchSpeedModifier) Speed /= 2;
	if (CurrentPitch > 0.5f && CurrentPitch <= PositivePitchSpeedModifier) Speed /= 2;
	if (CurrentRoll < -0.5f || CurrentRoll > 0.5f) Speed /= 4;

	return Speed;
}

float AGlider_Pawn::GetForwardSpeed()
{
	return ForwardSpeed;	
}

float AGlider_Pawn::GetMaximumForwardSpeed()
{
	return MaxSpeed;
}

void AGlider_Pawn::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Crash();

	GliderMeshDestructible->ApplyDamage(10000.0f, HitLocation, HitNormal, 5.0f);

	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = false;

}

void AGlider_Pawn::Crash()
{
	GliderMeshComponent->SetSimulatePhysics(true);
	PilotMeshComponent->SetSimulatePhysics(true);
	GliderMeshDestructible->SetSimulatePhysics(true);
	Crashing = true;
}

bool AGlider_Pawn::GetLaunched()
{
	return Launched;
}

bool AGlider_Pawn::GetStalling()
{
	return Stalling;
}

bool AGlider_Pawn::GetCrashing()
{
	return Crashing;
}

FVector AGlider_Pawn::GetGliderVelocity()
{
	return GliderVelocity;
}