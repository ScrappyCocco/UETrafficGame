#pragma once

#include "Pawn/BaseVehiclePawn.h"

#include "ReplayVehiclePawn.generated.h"

class UVehicleBaseReplayComponent;
class UVehicleArrowNavigatorComponent;

/**
 * This is a Vehicle who can be Replayed
 */
UCLASS()
class TRAFFICGAME_API AReplayVehiclePawn final : public ABaseVehiclePawn
{
	GENERATED_BODY()

public:

	AReplayVehiclePawn(const FObjectInitializer& ObjectInitializer);

	// Called when the Player is using/finish using this vehicle
	void EnablePlayerVehicleComponents() const;
	void DisablePlayerVehicleComponents() const;

	// Replay
	void StartRecordReplay() const;
	void StartPlaybackReplay() const;
	void StopAllReplay() const;
	void DeleteRecordedReplay() const;

protected:

	UPROPERTY()
	TObjectPtr<UVehicleBaseReplayComponent> ReplayComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Vehicle Components")
	TObjectPtr<UVehicleArrowNavigatorComponent> ArrowNavigatorComponent;
};
