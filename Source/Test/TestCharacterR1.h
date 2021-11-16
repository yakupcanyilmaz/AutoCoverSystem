#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TestCharacterR1.generated.h"

UCLASS(config = Game)
class ATestCharacterR1 : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

public:
	ATestCharacterR1();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cover, meta = (AllowPrivateAccess = "true"))
	float SlideSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cover, meta = (AllowPrivateAccess = "true"))
	float TurnRateInCover;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cover, meta = (AllowPrivateAccess = "true"))
	float CameraTurnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cover, meta = (AllowPrivateAccess = "true"))
	float DeltaDegreeForCameraTurn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vault, meta = (AllowPrivateAccess = "true"))
	float JumpDistMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vault, meta = (AllowPrivateAccess = "true"))
	float JumpHeightOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vault, meta = (AllowPrivateAccess = "true"))
	float SkipSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Vault, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* VaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Trace", meta = (AllowPrivateAccess = "true"))
	float TraceForwardMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Trace", meta = (AllowPrivateAccess = "true"))
	float ObstWidthMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Trace", meta = (AllowPrivateAccess = "true"))
	float ObstHeightOffsetVal;

	float VaultKeyTimer;
	float VaultKeyHoldTime;
	float VaultAnimDelayTime;
	FTimerHandle VaultAnimationTimer;

	// True when the player is in the cover area
	bool bIsCovered;

	// True when the player is in the sliding area
	bool bSliding;

	// True when the player is vaulting
	bool bVaulting;

	// True when the player is slidingand automatically moving to the cover area
	bool bInterpMove;

	FVector MoveLocation;
	FVector MoveDirection;
	FVector ObstacleLocation;
	FVector ObstacleNormal;
	FVector ObstacleHeight;

	void SetMoveDirection(FVector NewDirection);
	void SetMoveLocation(FVector NewLocation);

	void UpdateCoverMovement(float DeltaTime);
	// Skip to the move location
	void Skip(float DeltaTime);
	// Sliding in a direction. Used in the cover system R1 version.
	void Slide(float DeltaTime);
	void RotateCamera(float DeltaTime);
	// Check if the player is aligned with the cover line. Used in the cover system R1 version.
	bool IsAlignedWithCoverLine();

	bool LineTrace(FHitResult& HitResult, FVector TraceStart, FVector TraceEnd);
	void CheckObstacle();
	void JumpOverObstacle();
	void FinishVault();

	// Interps capsule half height when crouching/standing
	void InterpCapsuleHalfHeight(float DeltaTime);

	// Current half height of the capsule
	float CurrentCapsuleHalfHeight;

	// Half height of the capsule when not crouching
	float StandingCapsuleHalfHeight;

	// Half height of the capsule when crouching
	float CrouchingCapsuleHalfHeight;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool IsCovered() { return bIsCovered; }
	FORCEINLINE bool IsSliding() { return bSliding; }

	void OnSlideBegin(FVector NewDirection, FVector NewLocation);
	void OnSlideEnd();
	void OnCovered(FVector NewDirection, FVector NewLocation);
	void OnIsNotCovered();
};

