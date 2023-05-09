#pragma once

#include "Components/Gameplay/Replay/VehicleBaseReplayComponent.h"

#include "PhysicsPublic.h"
#include "SnapshotData.h"

#include "CustomPhysicsReplayComponent.generated.h"

USTRUCT()
struct FVehicleReplayEntry : public FBaseVehicleReplayEntry
{
	GENERATED_BODY()

	FWheeledSnaphotData VehicleSnapshot;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TRAFFICGAME_API UCustomPhysicsReplayComponent final : public UVehicleBaseReplayComponent
{
	GENERATED_BODY()

public:

	virtual void SetIsPlayingReplay(const bool InIsPlayingReplay) override;
	virtual void SetIsRecordingReplay(const bool InIsRecordingReplay) override;
	virtual void DeleteRecordedReplay() override;

protected:

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	// Custom physics
	void BindCustomPhysics();
	void UnbindCustomPhysics();

	FCalculateCustomPhysics OnCalculateCustomPhysics;
	void CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance);

	void RecordReplay(const float DeltaTime);
	void StoreVehicleData(FVehicleReplayEntry& Entry) const;
	void PlayReplay(const float DeltaTime);
	void RestoreVehicleData(const FVehicleReplayEntry& Entry) const;
	void ResetReplayVariables();

	TArray<FVehicleReplayEntry> RecordedReplay;
	float ReplayTime = 0;
	int32 CurrentReplayIndex = 0;
};
