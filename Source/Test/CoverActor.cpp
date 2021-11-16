#include "CoverActor.h"

#include "Components/BoxComponent.h"
#include "TestCharacter.h"

#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACoverActor::ACoverActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PivotPoint = CreateDefaultSubobject<USceneComponent>(TEXT("PivotPoint"));
	SetRootComponent(PivotPoint);

	ForwardDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ForwardDirection->SetupAttachment(RootComponent);

	SlideArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SlidingArea"));
	SlideArea->SetupAttachment(RootComponent);
	AutoCoverEnterArea = CreateDefaultSubobject<UBoxComponent>(TEXT("AutoCoverEnterArea"));
	AutoCoverEnterArea->SetupAttachment(RootComponent);
	CoverArea = CreateDefaultSubobject<UBoxComponent>(TEXT("CoverArea"));
	CoverArea->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACoverActor::BeginPlay()
{
	Super::BeginPlay();

	SlideArea->OnComponentBeginOverlap.AddDynamic(this, &ACoverActor::OnSlideAreaBeginOverlap);
	SlideArea->OnComponentEndOverlap.AddDynamic(this, &ACoverActor::OnSlideAreaEndOverlap);
	AutoCoverEnterArea->OnComponentBeginOverlap.AddDynamic(this, &ACoverActor::OnAutoCoverBeginOverlap);
	CoverArea->OnComponentBeginOverlap.AddDynamic(this, &ACoverActor::OnCoverAreaBeginOverlap);
	CoverArea->OnComponentEndOverlap.AddDynamic(this, &ACoverActor::OnCoverAreaEndOverlap);
}

// Called every frame
void ACoverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACoverActor::OnSlideAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ATestCharacter* TestCharacter = Cast<ATestCharacter>(OtherActor);
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
				TestCharacter->OnSlideBegin(GetActorForwardVector());
			}
		}
	}
}

void ACoverActor::OnSlideAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ATestCharacter* TestCharacter = Cast<ATestCharacter>(OtherActor);
		if (TestCharacter)
		{
			TestCharacter->OnSlideEnd();
		}
	}
}

void ACoverActor::OnAutoCoverBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ATestCharacter* TestCharacter = Cast<ATestCharacter>(OtherActor);
		if (TestCharacter)
		{
			FVector CharacterLocation = TestCharacter->GetActorLocation();
			FVector AutoCoverLocation = AutoCoverEnterArea->GetComponentLocation();
			FVector AutoCoverDirection = (AutoCoverLocation - CharacterLocation).GetSafeNormal();
			float DotProductValue = FVector::DotProduct(AutoCoverDirection, AutoCoverEnterArea->GetForwardVector());

			// Check if the player enters from the correct direction
			if (DotProductValue >= 0)
			{
				TestCharacter->SetActorRelativeRotation(GetActorForwardVector().Rotation());
				TestCharacter->OnAutoCoverBegin(GetActorForwardVector());
			}
		}
	}
}

void ACoverActor::OnAutoCoverEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ATestCharacter* TestCharacter = Cast<ATestCharacter>(OtherActor);
		if (TestCharacter)
		{
			TestCharacter->OnAutoCoverEnd();
		}
	}
}

void ACoverActor::OnCoverAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ATestCharacter* TestCharacter = Cast<ATestCharacter>(OtherActor);
		if (TestCharacter)
		{
			TestCharacter->OnCovered(GetActorForwardVector());
		}
	}
}

void ACoverActor::OnCoverAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ATestCharacter* TestCharacter = Cast<ATestCharacter>(OtherActor);
		if (TestCharacter)
		{
			TestCharacter->OnIsNotCovered();
		}
	}
}

