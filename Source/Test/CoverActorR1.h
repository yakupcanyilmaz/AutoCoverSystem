#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoverActorR1.generated.h"

UCLASS()
class TEST_API ACoverActorR1 : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACoverActorR1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
		void OnSlideAreaBeginOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void OnSlideAreaEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UFUNCTION()
		void OnCoverAreaBeginOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void OnCoverAreaEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover Properties", meta = (AllowPrivateAccess = "true"))
		class USceneComponent* PivotPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cover Properties", meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* ForwardDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cover Properties", meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* SlideArea;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cover Properties", meta = (AllowPrivateAccess = "true"))
	//	UBoxComponent* AutoCoverEnterArea;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cover Properties", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* CoverArea;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
