#include "Actors/Gameplay/VehicleFinishVolume.h"

#include "Engine/World.h"
#include "GameState/Gameplay/TrafficGameplayGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Pawn/BaseVehiclePawn.h"

void AVehicleFinishVolume::BeginPlay()
{
	Super::BeginPlay();

	// Make sure to hide the Actor
	DisableForCurrentRound();
}

void AVehicleFinishVolume::BeginDestroy()
{
	// Make sure to unbind the delegate
	DisableForCurrentRound();

	Super::BeginDestroy();
}

void AVehicleFinishVolume::EnableForCurrentRound()
{
	if (!OnActorBeginOverlap.IsAlreadyBound(this, &ThisClass::OnOverlapBegin))
	{
		OnActorBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	}

	SetActorHiddenInGame(false);
}

void AVehicleFinishVolume::DisableForCurrentRound()
{
	OnActorBeginOverlap.RemoveDynamic(this, &ThisClass::OnOverlapBegin);

	SetActorHiddenInGame(true);
}

void AVehicleFinishVolume::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	// Check if is a Vehicle
	if (const ABaseVehiclePawn* Vehicle = Cast<ABaseVehiclePawn>(OtherActor))
	{
		// Check if is a Player Controlled Vehicle
		if (IsValid(Cast<APlayerController>(Vehicle->GetController())))
		{
			if (const UWorld* World = GetWorld())
			{
				if (const ATrafficGameplayGameState* TrafficGameState = Cast<ATrafficGameplayGameState>(World->GetGameState()))
				{
					const FString TagToReturn = Tags.IsValidIndex(0) ? Tags[0].ToString() : FString();
					TrafficGameState->FinishTriggerReached(TagToReturn);
				}
			}
		}
	}
}
