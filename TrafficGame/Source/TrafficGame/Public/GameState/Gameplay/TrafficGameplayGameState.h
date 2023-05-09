#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameState.h"

#include "TrafficGameplayGameState.generated.h"

class AVehicleFinishVolume;

// bIsPlayingRound is used to mark if the current round is a playing round or a "background replay" round
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoundStartedEvent, bool, bIsPlayingRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoundEndedEvent, bool, bIsPlayingRound, bool, bCancelled);

/**
 * Simple game play GameState to store game data, such as the current finish trigger
 * And notify events such as RoundStarted/RoundEnded
 */
UCLASS()
class TRAFFICGAME_API ATrafficGameplayGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	// Trigger Volume notify Game Mode
	void FinishTriggerReached(const FString& Tag) const;

	// The Game Mode set the current round Finish Volume to let other Actors read it
	void SetCurrentFinishVolume(AVehicleFinishVolume* FinishVolume);
	AVehicleFinishVolume* GetCurrentFinishVolume() const;
	void ClearCurrentFinishVolume();

	// Game Mode called Events
	void EventStartRound(bool bIsPlayingRound) const;
	void EventEndRound(bool bIsPlayingRound, bool bCancelled) const;

	FRoundStartedEvent RoundStartedEventDelegate;
	FRoundEndedEvent RoundEndedEventDelegate;

	// Player request the end of the current round to replay it
	void CancelCurrentRound() const;

private:

	TWeakObjectPtr<AVehicleFinishVolume> CurrentFinishVolume = nullptr;

};
