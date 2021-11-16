// Fill out your copyright notice in the Description page of Project Settings.


#include "TestAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "TestCharacter.h"

void UTestAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (TestCharacter == nullptr)
	{
		TestCharacter = Cast<ATestCharacter>(TryGetPawnOwner());
	}

	if (TestCharacter)
	{
		FVector Velocity{ TestCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();


		if (TestCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		bIsCovered = TestCharacter->IsCovered();
		bIsSliding = TestCharacter->IsSliding();
	}
}