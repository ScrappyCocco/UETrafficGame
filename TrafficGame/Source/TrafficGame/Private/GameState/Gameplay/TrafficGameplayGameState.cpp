#include "GameState/Gameplay/TrafficGameplayGameState.h"

#include "Actors/Gameplay/VehicleFinishVolume.h"
#include "GameMode/Gameplay/TrafficGameplayGameMode.h"
#include "Kismet/GameplayStatics.h"

void ATrafficGameplayGameState::FinishTriggerReached(const FString& Tag) const
{
	if (ATrafficGameplayGameMode* TrafficGameMode = Cast<ATrafficGameplayGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		TrafficGameMode->FinishTriggerReached(Tag);
	}
}

void ATrafficGameplayGameState::SetCurrentFinishVolume(AVehicleFinishVolume* FinishVolume)
{
	CurrentFinishVolume = FinishVolume;
}

AVehicleFinishVolume* ATrafficGameplayGameState::GetCurrentFinishVolume() const
{
	if (CurrentFinishVolume.IsValid())
	{
		return CurrentFinishVolume.Get();
	}

	return nullptr;
}

void ATrafficGameplayGameState::ClearCurrentFinishVolume()
{
	CurrentFinishVolume.Reset();
}

void ATrafficGameplayGameState::EventStartRound(const bool bIsPlayingRound) const
{
	RoundStartedEventDelegate.Broadcast(bIsPlayingRound);
}

void ATrafficGameplayGameState::EventEndRound(const bool bIsPlayingRound, const bool bCancelled) const
{
	RoundEndedEventDelegate.Broadcast(bIsPlayingRound, bCancelled);
}

void ATrafficGameplayGameState::CancelCurrentRound() const
{
	if (ATrafficGameplayGameMode* TrafficGameMode = Cast<ATrafficGameplayGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		TrafficGameMode->CancelCurrentRound();
	}
}

