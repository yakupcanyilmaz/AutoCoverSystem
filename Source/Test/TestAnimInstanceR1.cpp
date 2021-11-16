// Fill out your copyright notice in the Description page of Project Settings.


#include "TestAnimInstanceR1.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "TestCharacterR1.h"

void UTestAnimInstanceR1::UpdateAnimationProperties(float DeltaTime)
{
	if (TestCharacter == nullptr)
	{
		TestCharacter = Cast<ATestCharacterR1>(TryGetPawnOwner());
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