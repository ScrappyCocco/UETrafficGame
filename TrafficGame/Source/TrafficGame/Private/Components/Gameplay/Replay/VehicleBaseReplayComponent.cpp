#include "Components/Gameplay/Replay/VehicleBaseReplayComponent.h"

#include "Pawn/BaseVehiclePawn.h"

UVehicleBaseReplayComponent::UVehicleBaseReplayComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleBaseReplayComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerVehicle = GetOwner<ABaseVehiclePawn>();
}

void UVehicleBaseReplayComponent::BeginDestroy()
{
	SetIsRecordingReplay(false);
	SetIsPlayingReplay(false);
	OwnerVehicle.Reset();

	Super::BeginDestroy();
}

void UVehicleBaseReplayComponent::SetIsPlayingReplay(const bool InIsPlayingReplay)
{
	bIsPlayingReplay = InIsPlayingReplay;
}

void UVehicleBaseReplayComponent::SetIsRecordingReplay(const bool InIsRecordingReplay)
{
	bIsRecordingReplay = InIsRecordingReplay;
}

void UVehicleBaseReplayComponent::DeleteRecordedReplay()
{
	// Nothing to do here for now
}
