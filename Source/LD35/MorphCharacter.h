// See License.txt for copyright info
// Derived from Unreal Flight Template Game

#pragma once

#include "GameFramework/Pawn.h"
#include "MorphCharacter.generated.h"

UENUM(BlueprintType)
enum class EMorphType : uint8
{
	MT_Bird UMETA(DisplayName="Bird"),
	MT_Dude UMETA(DisplayName="Dude")
};

UCLASS()
class LD35_API AMorphCharacter : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* Capsule;

public:
	/** How quickly forward speed changes while flying */
	UPROPERTY(Category = Flying, EditAnywhere)
	float FlyingAcceleration;

	/** How quickly pawn can steer while flying */
	UPROPERTY(Category = Flying, EditAnywhere)
	float FlyingTurnSpeed;

	/** Max forward flying speed */
	UPROPERTY(Category = Flying, EditAnywhere)
	float FlyingMaxSpeed;

	/** Min forward flying speed */
	UPROPERTY(Category = Flying, EditAnywhere)
	float FlyingMinSpeed;

	/** Flying capsule radius */
	UPROPERTY(Category = Walking, EditAnywhere)
	float FlyingCapsuleRadius;

	/** Flying capsule half height */
	UPROPERTY(Category = Walking, EditAnywhere)
	float FlyingCapsuleHalfHeight;

	/** How quickly forward speed changes while walking */
	UPROPERTY(Category = Walking, EditAnywhere)
	float WalkingAcceleration;

	/** How quickly pawn can steer while walking */
	UPROPERTY(Category = Walking, EditAnywhere)
	float WalkingTurnSpeed;

	/** Max forward walking speed */
	UPROPERTY(Category = Walking, EditAnywhere)
	float WalkingMaxSpeed;

	/** Min forward walking speed */
	UPROPERTY(Category = Walking, EditAnywhere)
	float WalkingMinSpeed;

	/** Walking capsule radius */
	UPROPERTY(Category = Walking, EditAnywhere)
	float WalkingCapsuleRadius;

	/** Walking capsule half height */
	UPROPERTY(Category = Walking, EditAnywhere)
	float WalkingCapsuleHalfHeight;

	/** Gravity */
	UPROPERTY(Category = Walking, EditAnywhere)
	float Gravity;
	
	/** Current morph type of the character */
	UPROPERTY(Category = Morph, EditAnywhere, BlueprintReadOnly)
	EMorphType CurrentMorphType;

	/** Current moving forward speed */
	UPROPERTY(Category = Movement, BlueprintReadOnly)
	float CurrentForwardSpeed;
	
	AMorphCharacter();

	// Begin AActor overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides	

	/** Transforms the character into the alternate mode */
	UFUNCTION(BlueprintCallable, Category = Morph)
	void Transform();

	/** Event called when character transforms */
	UFUNCTION(BlueprintImplementableEvent, Category = Morph)
	void OnTransform(EMorphType TargetMorphType, EMorphType PrevMorphType);

	/** Returns Capsule subobject **/
	FORCEINLINE class UCapsuleComponent* GetCapsule() const { return Capsule; }

protected:

	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	// End APawn overrides
	
	void ResetFlightConfig();

	void ResetWalkConfig();

	/** Bound to the horizontal movement axis */
	void MoveForwardInput(float Val);

	/** Bound to the horizontal movement axis */
	void MoveRightInput(float Val);

	/** Bound to the vertical view axis */
	void LookUpInput(float Val);

	/** Bound to the horizontal view axis */
	void LookRightInput(float Val);

	/** Move a walking character down due to gravity */
	void CalculateGravity();

private:

	/** How quickly forward speed changes */
	UPROPERTY(Category = Flying, EditAnywhere)
	float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category = Flying, EditAnywhere)
	float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Flying, EditAnywhere)
	float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = Flying, EditAnywhere)
	float MinSpeed;

	/** Current moving right speed */
	float CurrentRightSpeed;

	/** Current moving down speed */
	float CurrentDownSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;
};
