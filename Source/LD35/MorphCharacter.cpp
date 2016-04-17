// See License.txt for copyright info
// Derived from Unreal Flight Template Game

#include "LD35.h"
#include "MorphCharacter.h"


// Sets default values
AMorphCharacter::AMorphCharacter()
{
	// Create the root component
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;

	// Initialize configuration values
	FlyingAcceleration = 500.f;
	FlyingTurnSpeed = 50.f;
	FlyingMaxSpeed = 4000.f;
	FlyingMinSpeed = 500.f;
	FlyingCapsuleRadius = 30.f;
	FlyingCapsuleHalfHeight = 30.f;

	WalkingAcceleration = 2000.f;
	WalkingTurnSpeed = 50.f;
	WalkingMaxSpeed = 500.f;
	WalkingMinSpeed = -500.f;
	WalkingCapsuleRadius = 30.f;
	WalkingCapsuleHalfHeight = 30.f;
	WalkCollisionBuffer = 0.1f;

	Gravity = -981.f;

	MaxBerries = 30;
	CurrentBerries = 0;

	// Default to the dude state
	CurrentMorphType = EMorphType::MT_Dude;
	ResetWalkConfig();

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMorphCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (CurrentMorphType == EMorphType::MT_Bird)
	{
		ResetFlightConfig();
	}
	else
	{
		ResetWalkConfig();
	}

}

// Called every frame
void AMorphCharacter::Tick( float DeltaTime )
{
	FVector WalkBuffer = FVector(0, 0, WalkCollisionBuffer);
	if (CurrentMorphType == EMorphType::MT_Dude)
	{
		AddActorLocalOffset(WalkBuffer);
	}
	FVector LocalMove = FVector(CurrentForwardSpeed * DeltaTime, CurrentRightSpeed * DeltaTime, 0.f);

	// Move character forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	if (CurrentMorphType == EMorphType::MT_Dude)
	{
		AddActorLocalOffset(-WalkBuffer);
	}

	CalculateGravity();
	FVector GravityMove = FVector(0.f, 0.f, CurrentDownSpeed * DeltaTime);
	FHitResult floorHit;
	AddActorLocalOffset(GravityMove, true, &floorHit);
	CurrentDownSpeed = floorHit.bBlockingHit ? floorHit.Distance : CurrentDownSpeed;

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaTime;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaTime;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaTime;

	// Rotate character
	AddActorLocalRotation(DeltaRotation);

	// Call any parent class Tick implementation
	Super::Tick(DeltaTime);
}

// Handle collisions
void AMorphCharacter::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (CurrentMorphType == EMorphType::MT_Bird)
	{
		FRotator CurrentRotation = GetActorRotation(RootComponent);
		SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
	}
	else
	{
		FVector ActorOrigin, ActorExtents;
		GetActorBounds(true, ActorOrigin, ActorExtents);

		const FVector traceLoc = ActorOrigin;
		const FVector traceDir = FVector(0, 0, -1000.f);

		FHitResult TraceResult;
		FCollisionQueryParams TraceParams(FName(TEXT("WalkingTrace")), true, this);
		TraceParams.bTraceAsyncScene = true;

		GetWorld()->LineTraceSingleByChannel(TraceResult, traceLoc, traceLoc + traceDir, ECollisionChannel::ECC_Visibility, TraceParams);
		if (TraceResult.bBlockingHit && TraceResult.Distance < ActorExtents.Z)
		{
				AddActorLocalOffset(FVector(0, 0, ActorExtents.Z - TraceResult.Distance + WalkCollisionBuffer));
		}
	}
}

void AMorphCharacter::UseBerry(int Quantity)
{
	if (CurrentMorphType == EMorphType::MT_Bird)
	{
		if (Quantity > CurrentBerries)
		{
			Transform();
		}
		else {
			CurrentBerries -= Quantity;
		}
	}
}

void AMorphCharacter::GrabBerry(int Quantity)
{
	CurrentBerries = FMath::Min(CurrentBerries + Quantity, MaxBerries);
}

// Called to bind functionality to input
void AMorphCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Bind our control axis' to callback functions
	InputComponent->BindAxis("Move Forward", this, &AMorphCharacter::MoveForwardInput);
	InputComponent->BindAxis("Move Right", this, &AMorphCharacter::MoveRightInput);
	InputComponent->BindAxis("Look Up", this, &AMorphCharacter::LookUpInput);
	InputComponent->BindAxis("Look Right", this, &AMorphCharacter::LookRightInput);
}

void AMorphCharacter::ResetFlightConfig()
{
	Acceleration = FlyingAcceleration;
	TurnSpeed = FlyingTurnSpeed;
	MaxSpeed = FlyingMaxSpeed;
	MinSpeed = FlyingMinSpeed;
	CurrentForwardSpeed = MinSpeed;
	CurrentRightSpeed = 0.f;
	CurrentDownSpeed = 0.f;

	CurrentYawSpeed = 0.f;
	CurrentPitchSpeed = 0.f;
	CurrentRollSpeed = 0.f;

	Capsule->SetCapsuleRadius(FlyingCapsuleRadius);
	Capsule->SetCapsuleHalfHeight(FlyingCapsuleHalfHeight);
}

void AMorphCharacter::ResetWalkConfig()
{
	Acceleration = WalkingAcceleration;
	TurnSpeed = WalkingTurnSpeed;
	MaxSpeed = WalkingMaxSpeed;
	MinSpeed = WalkingMinSpeed;
	CurrentRightSpeed = 0.f;
	CurrentDownSpeed = 0.f;

	CurrentYawSpeed = 0.f;
	CurrentPitchSpeed = 0.f;
	CurrentRollSpeed = 0.f;

	FRotator resetRotation(0, 0, 0);
	resetRotation.Yaw = GetActorRotation().Yaw;
	SetActorRotation(resetRotation);

	Capsule->SetCapsuleRadius(WalkingCapsuleRadius);
	Capsule->SetCapsuleHalfHeight(WalkingCapsuleHalfHeight);
}

void AMorphCharacter::MoveForwardInput(float Val)
{
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	float CurrentAcc;
	if (CurrentMorphType == EMorphType::MT_Dude)
	{
		// Find the value needed to approach 0
		float StasisDirection = FMath::IsNearlyEqual(CurrentForwardSpeed, 0.f) ? 0.f : (-CurrentForwardSpeed * .5) / FMath::Abs(CurrentForwardSpeed);
		// If input is not held down, reduce speed
		CurrentAcc = bHasInput ? (Val * Acceleration) : (StasisDirection * Acceleration);
	}
	else {
		// If input is not held down, reduce speed
		CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	}
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void AMorphCharacter::MoveRightInput(float Val)
{
	// Move right if we're walking
	if (CurrentMorphType == EMorphType::MT_Dude)
	{
		// Determine if there is input
		bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
		// Find the value needed to approach 0
		float StasisDirection = FMath::IsNearlyEqual(CurrentRightSpeed, 0.f) ? 0.f : (-CurrentRightSpeed * .5) / FMath::Abs(CurrentRightSpeed);
		// Determine the current acceleration
		float CurrentAcc = bHasInput ? (Val * Acceleration) : (StasisDirection * Acceleration);
		// Apply the current acceleration
		float NewRightSpeed = CurrentRightSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
		// Clamp to the valid range
		CurrentRightSpeed = FMath::Clamp(NewRightSpeed, -MaxSpeed, MaxSpeed);
	}
}

void AMorphCharacter::LookUpInput(float Val)
{
	// Look up if we're flying
	if (CurrentMorphType == EMorphType::MT_Bird)
	{
		// Target pitch speed is based in input
		float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

		// When steering, we decrease pitch slightly
		TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

		// Smoothly interpolate to target pitch speed
		CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
	}
}

void AMorphCharacter::LookRightInput(float Val)
{
	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Roll if we're flying
	if (CurrentMorphType == EMorphType::MT_Bird)
	{
		// If turning, yaw value is used to influence roll
		// If not turning, roll to reverse current roll value
		float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

		// Smoothly interpolate roll speed
		CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
	}
}

void AMorphCharacter::CalculateGravity()
{
	// Fall if we're walking
	if (CurrentMorphType == EMorphType::MT_Dude)
	{
		CurrentDownSpeed = FMath::FInterpTo(CurrentDownSpeed, Gravity, GetWorld()->GetDeltaSeconds(), 2.f);
		CurrentDownSpeed = FMath::Clamp(CurrentDownSpeed, Gravity, 0.f);
	}
}

void AMorphCharacter::TickBerry()
{
	UseBerry(1);
}

void AMorphCharacter::Transform()
{
	EMorphType previousType = CurrentMorphType;
	if (CurrentBerries > 0)
	{
		CurrentMorphType = (CurrentMorphType == EMorphType::MT_Bird ? EMorphType::MT_Dude : EMorphType::MT_Bird);
	}
	else
	{
		CurrentMorphType = EMorphType::MT_Dude;
	}
	
	if (previousType != CurrentMorphType)
	{
		OnTransform(CurrentMorphType, previousType);
	}

	if (CurrentMorphType == EMorphType::MT_Bird)
	{
		ResetFlightConfig();
		GetWorldTimerManager().SetTimer<AMorphCharacter>(BerryTickTimer, this, &AMorphCharacter::TickBerry, 1.0f, true);
	}
	else
	{
		ResetWalkConfig();
		GetWorldTimerManager().ClearTimer(BerryTickTimer);
	}
}