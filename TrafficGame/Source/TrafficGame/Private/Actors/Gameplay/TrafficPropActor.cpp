#include "Actors/Gameplay/TrafficPropActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameState/Gameplay/TrafficGameplayGameState.h"

ATrafficPropActor::ATrafficPropActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATrafficPropActor::BeginPlay()
{
	Super::BeginPlay();

	if (const UWorld* World = GetWorld())
	{
		if (ATrafficGameplayGameState* TrafficGameState = Cast<ATrafficGameplayGameState>(World->GetGameState()))
		{
			TrafficGameState->RoundStartedEventDelegate.AddDynamic(this, &ThisClass::RoundStarted);
			TrafficGameState->RoundEndedEventDelegate.AddDynamic(this, &ThisClass::RoundEnded);
		}
	}

	// Cache the position
	SavedPropLocation = GetActorTransform();
}

void ATrafficPropActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (ATrafficGameplayGameState* TrafficGameState = Cast<ATrafficGameplayGameState>(World->GetGameState()))
		{
			TrafficGameState->RoundStartedEventDelegate.RemoveDynamic(this, &ThisClass::RoundStarted);
			TrafficGameState->RoundEndedEventDelegate.RemoveDynamic(this, &ThisClass::RoundEnded);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ATrafficPropActor::RoundStarted(bool bIsPlayingRound)
{
	RestorePropInitialPosition();
}

void ATrafficPropActor::RoundEnded(bool bIsPlayingRound, bool bCancelled)
{
	RestorePropInitialPosition();
}

void ATrafficPropActor::RestorePropInitialPosition()
{
	if (UStaticMeshComponent* PropMesh = GetStaticMeshComponent())
	{
		PropMesh->SetPhysicsLinearVelocity(FVector::Zero());
		PropMesh->SetPhysicsAngularVelocityInDegrees(FVector::Zero());
	}

	// Restore saved Transform
	SetActorTransform(SavedPropLocation, false, nullptr, ETeleportType::ResetPhysics);
}
