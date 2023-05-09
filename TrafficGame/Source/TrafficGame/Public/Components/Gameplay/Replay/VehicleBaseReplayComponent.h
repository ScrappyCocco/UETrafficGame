#pragma once

#include "Components/ActorComponent.h"

#include "VehicleBaseReplayComponent.generated.h"

class ABaseVehiclePawn;

/*
 * Suggested base Struct to use to save the replay
 */
USTRUCT()
struct FBaseVehicleReplayEntry
{
	GENERATED_BODY()

	float Time;

	bool bBrakeLightsActive;
};

/*
 * Base Vehicle Replay Component to be implemented by SubClasses
 * This way more Replay Methods can be tested just switching Components
 */
UCLASS(Abstract)
class TRAFFICGAME_API UVehicleBaseReplayComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UVehicleBaseReplayComponent();

	virtual void SetIsPlayingReplay(const bool InIsPlayingReplay);
	virtual void SetIsRecordingReplay(const bool InIsRecordingReplay);
	virtual void DeleteRecordedReplay();

protected:

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	TWeakObjectPtr<ABaseVehiclePawn> OwnerVehicle;

	bool bIsPlayingReplay = false;
	bool bIsRecordingReplay = false;

};
