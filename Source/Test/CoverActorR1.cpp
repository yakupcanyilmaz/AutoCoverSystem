#include "CoverActorR1.h"

#include "Components/BoxComponent.h"
#include "TestCharacterR1.h"

#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACoverActorR1::ACoverActorR1()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PivotPoint"));
	SetRootComponent(PivotPoint);

	ForwardDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ForwardDirection->SetupAttachment(RootComponent);

	SlideArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SlidingArea"));
	SlideArea->SetupAttachment(RootComponent);
	CoverArea = CreateDefaultSubobject<UBoxComponent>(TEXT("CoverArea"));
	CoverArea->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACoverActorR1::BeginPlay()
{
	Super::BeginPlay();

	SlideArea->OnComponentBeginOverlap.AddDynamic(this, &ACoverActorR1::OnSlideAreaBeginOverlap);
	SlideArea->OnComponentEndOverlap.AddDynamic(this, &ACoverActorR1::OnSlideAreaEndOverlap);
	CoverArea->OnComponentBeginOverlap.AddDynamic(this, &ACoverActorR1::OnCoverAreaBeginOverlap);
	CoverArea->OnComponentEndOverlap.AddDynamic(this, &ACoverActorR1::OnCoverAreaEndOverlap);
}

// Called every frame
void ACoverActorR1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACoverActorR1::OnSlideAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ATestCharacterR1* TestCharacter = Cast<ATestCharacterR1>(OtherActor);
		if (TestCharacter)
		{
			FVector CharacterLocation = TestCharacter->GetActorLocation();
			FVector SlidePointLocation = SlideArea->GetComponentLocation();
			FVector SlideDirection = (SlidePointLocation - CharacterLocation).GetSafeNormal();
			float DotProductValue = FVector::DotProduct(SlideDirection, SlideArea->GetForwardVector());

			// Check if the player enters from the correct direction
			if (DotProductValue >= 0)
			{
				TestCharacter->SetActorRelativeRotation(GetActorForwardVector().Rotation());
				TestCharacter->OnSlideBegin(GetActorForwardVector(), GetActorLocation());
			}
		}
	}
}

void ACoverActorR1::OnSlideAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ATestCharacterR1* TestCharacter = Cast<ATestCharacterR1>(OtherActor);
		if (TestCharacter)
		{
			TestCharacter->OnSlideEnd();
		}
	}
}


void ACoverActorR1::OnCoverAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ATestCharacterR1* TestCharacter = Cast<ATestCharacterR1>(OtherActor);
		if (TestCharacter)
		{
			TestCharacter->OnCovered(GetActorForwardVector(), GetActorLocation());
		}
	}
}

void ACoverActorR1::OnCoverAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ATestCharacterR1* TestCharacter = Cast<ATestCharacterR1>(OtherActor);
		if (TestCharacter)
		{
			TestCharacter->OnIsNotCovered();
		}
	}
}

