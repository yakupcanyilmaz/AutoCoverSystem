#include "TestCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

ATestCharacter::ATestCharacter() :
	StandingCapsuleHalfHeight(98.f),
	CrouchingCapsuleHalfHeight(49.f),
	JumpDistMultiplier(200.f),
	JumpHeightOffset(30.f),
	SkipSpeed(1.f),
	SlideSpeed(100.f),
	TurnRateInCover(10.f),
	CameraTurnSpeed(150.f),
	DeltaDegreeForCameraTurn(15.f),
	TraceForwardMultiplier(100.f),
	ObstWidthMultiplier(10.f),
	ObstHeightOffsetVal(200.f),
	VaultKeyTimer(1.5f),
	VaultAnimDelayTime(0.8f)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpMove || bVaulting)
	{
		UpdateCoverMovement(DeltaTime);
	}

	InterpCapsuleHalfHeight(DeltaTime);
}

void ATestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATestCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATestCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATestCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATestCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATestCharacter::TouchStopped);
}

void ATestCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ATestCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ATestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATestCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		FVector Direction{};
		if (bIsCovered && Value >= 0.0f)
		{
			const FRotator Rotation = MoveDirection.Rotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			FRotator NewRotation = FMath::RInterpTo(GetActorRotation(),YawRotation, GetWorld()->GetDeltaSeconds(), TurnRateInCover);
			SetActorRotation(NewRotation);

			VaultKeyHoldTime += GetWorld()->GetDeltaSeconds();
			if (VaultKeyHoldTime >= VaultKeyTimer)
			{
				CheckObstacle();
			}
		}
		else
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);
		}
	}
}

void ATestCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		FVector Direction{};
		if (bIsCovered)
		{
			Direction = FVector::CrossProduct(GetActorUpVector(), MoveDirection);
		}
		else
		{
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);
			Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		}
		AddMovementInput(Direction, Value);
	}
}

void ATestCharacter::SetMoveLocation(FVector NewLocation)
{
	MoveLocation = NewLocation;
}

void ATestCharacter::SetMoveDirection(FVector NewDirection)
{
	const FRotator Rotation = NewDirection.Rotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	MoveDirection = Direction;
}

void ATestCharacter::UpdateCoverMovement(float DeltaTime)
{
	// Rotate the camera while the player is sliding or vaulting
	RotateCamera(DeltaTime);

	if (bInterpMove)
	{
		// The movement in the cover system R0 version.
		AddMovementInput(MoveDirection * DeltaTime * SlideSpeed, 1);
	}

	if (bVaulting)
	{
		Skip(DeltaTime);
	}
}

void ATestCharacter::Skip(float DeltaTime)
{
	FVector StartLocation = GetActorLocation();
	FVector SkipLocation = FMath::VInterpTo(StartLocation, MoveLocation, DeltaTime, SkipSpeed);
	SetActorLocation(SkipLocation);
}

void ATestCharacter::Slide(float DeltaTime)
{
	if (IsAlignedWithCoverLine())
	{
		bInterpMove = false;
	}
	else
	{
		AddMovementInput(MoveDirection * DeltaTime * SlideSpeed, 1);
	}
}

void ATestCharacter::RotateCamera(float DeltaTime)
{
	float DeltaYaw{};
	if (MoveDirection.Rotation().Yaw - GetControlRotation().Yaw < -180)
	{
		DeltaYaw = (MoveDirection.Rotation().Yaw - GetControlRotation().Yaw) + 360;
	}
	else
	{
		DeltaYaw = (MoveDirection.Rotation().Yaw - GetControlRotation().Yaw);
	}

	if (bVaulting && DeltaYaw > 180 - DeltaDegreeForCameraTurn
		|| bInterpMove && DeltaYaw > DeltaDegreeForCameraTurn)
	{
		AddControllerYawInput(1 * CameraTurnSpeed * DeltaTime);
	}

	if ((bVaulting && DeltaYaw < 180 + DeltaDegreeForCameraTurn)
		|| bInterpMove && DeltaYaw < -DeltaDegreeForCameraTurn)
	{
		AddControllerYawInput(-1 * CameraTurnSpeed * DeltaTime);
	}
}

bool ATestCharacter::IsAlignedWithCoverLine()
{
	FVector2D CharacterLocation = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	FVector2D CoverPointLocation = FVector2D(MoveLocation.X, MoveLocation.Y);
	FVector2D CoverPointDirection = CoverPointLocation - CharacterLocation;
	FVector2D CoverDirection = FVector2D(MoveDirection.X, MoveDirection.Y);
	float DotProductValue = FVector2D::DotProduct(CoverPointDirection.GetSafeNormal(), CoverDirection);
	float Angle = UKismetMathLibrary::DegAcos(DotProductValue);

	if (Angle >= 90)
	{
		 return true;
	}

	return false;
}

void ATestCharacter::OnSlideBegin(FVector NewDirection)
{
	bInterpMove = true;
	bSliding = true;
	SetMoveDirection(NewDirection);
}

void ATestCharacter::OnSlideEnd()
{
	bSliding = false;
	bInterpMove = false;
}

void ATestCharacter::OnAutoCoverBegin(FVector NewDirection)
{
	bInterpMove = true;
	bIsCovered = true;
	bSliding = false;
	SetMoveDirection(NewDirection);
}

void ATestCharacter::OnAutoCoverEnd()
{
	bInterpMove = false;
	bIsCovered = false;
}

void ATestCharacter::OnCovered(FVector NewDirection)
{
	bInterpMove = false;
	bIsCovered = true;
	bSliding = false;
	SetMoveDirection(NewDirection);
}

void ATestCharacter::OnIsNotCovered()
{
	bInterpMove = false;
	bIsCovered = false;
}

bool ATestCharacter::LineTrace(FHitResult& HitResult, FVector TraceStart, FVector TraceEnd)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult ObstacleHitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldStatic, QueryParams))
	{
		return true;
	}
	return false;

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.0f);
	//DrawDebugBox(GetWorld(), HitResult.Location, FVector(5, 5, 5), FColor::Red, false, 2.f);
}

void ATestCharacter::CheckObstacle()
{
	// First trace if there is an obstacle
	FVector TraceStart{};
	if (bIsCovered)
	{
		TraceStart = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()/2);
	}
	else
	{
		TraceStart = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	}
	FVector TraceEnd = TraceStart + (GetActorForwardVector() * TraceForwardMultiplier);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	if (LineTrace(HitResult, TraceStart, TraceEnd))
	{
		ObstacleLocation = HitResult.Location;
		ObstacleNormal = HitResult.Normal;
		JumpOverObstacle();
	}
}

void ATestCharacter::JumpOverObstacle()
{
	// Second trace to find the obstacle height
	FRotator ObstacleRotation = UKismetMathLibrary::MakeRotFromX(-ObstacleNormal);
	FVector ObstacleForwardVector = UKismetMathLibrary::GetForwardVector(ObstacleRotation);
	FVector ObstacleTraceStart = ObstacleLocation +
								(ObstacleForwardVector * ObstWidthMultiplier) +
								FVector(0.f, 0.f, ObstHeightOffsetVal);
	FVector ObstacleTraceEnd = ObstacleTraceStart - FVector(0.f, 0.f, ObstHeightOffsetVal);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult ObstacleHitResult;
	if (LineTrace(ObstacleHitResult, ObstacleTraceStart, ObstacleTraceEnd))
	{
		ObstacleHeight = ObstacleHitResult.Location;

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMovementComponent()->MovementState.bCanFly = true;

		// Determining jump location
		FVector JumpLength = GetActorLocation() + ObstacleForwardVector * JumpDistMultiplier;
		FVector JumpLocation = FVector(JumpLength.X, JumpLength.Y, ObstacleHeight.Z + JumpHeightOffset);

		DrawDebugBox(GetWorld(), JumpLocation, FVector(10, 10, 10), FColor::Blue, false, 2.f);

		bVaulting = true;
		SetMoveLocation(JumpLocation);

		GetWorldTimerManager().ClearTimer(VaultAnimationTimer);
		GetWorldTimerManager().SetTimer(VaultAnimationTimer, this, &ATestCharacter::FinishVault, VaultAnimDelayTime);

		if (VaultMontage)
		{
			PlayAnimMontage(VaultMontage);
		}
	}
}

void ATestCharacter::FinishVault()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMovementComponent()->MovementState.bCanFly = false;
	bVaulting = false;
	VaultKeyHoldTime = 0.f;
}

void ATestCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight{};
	if (bIsCovered || bSliding)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}
	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHeight{ FMath::FInterpTo(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, DeltaTime, 20.f) };

	// Negative value if crouching; Positive value if standing
	const float DeltaCapsuleHalfHeight{ InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset{ 0.f, 0.f, -DeltaCapsuleHalfHeight };
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

