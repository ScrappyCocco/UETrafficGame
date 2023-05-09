#include "Pawn/Gameplay/ReplayVehiclePawn.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "Components/Gameplay/Navigator/VehicleArrowNavigatorComponent.h"
#include "Components/Gameplay/Replay/CustomPhysics/CustomPhysicsReplayComponent.h"
#include "TrafficGame/TrafficGameLogs.h"

AReplayVehiclePawn::AReplayVehiclePawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ReplayComponent = ObjectInitializer.CreateDefaultSubobject<UCustomPhysicsReplayComponent>(this, TEXT("VehicleReplayComponent"));

	ArrowNavigatorComponent = ObjectInitializer.CreateDefaultSubobject<UVehicleArrowNavigatorComponent>(this, TEXT("ArrowNavigatorComponent"));
	const FAttachmentTransformRules TransformRules(EAttachmentRule::KeepRelative, false);
	ArrowNavigatorComponent->AttachToComponent(GetMesh(), TransformRules);
}

void AReplayVehiclePawn::EnablePlayerVehicleComponents() const
{
	if (ArrowNavigatorComponent)
	{
		ArrowNavigatorComponent->Activate(true);
	}
}

void AReplayVehiclePawn::DisablePlayerVehicleComponents() const
{
	if (ArrowNavigatorComponent)
	{
		ArrowNavigatorComponent->Deactivate();
	}
}

void AReplayVehiclePawn::StartRecordReplay() const
{
	if (ReplayComponent)
	{
		ReplayComponent->SetIsRecordingReplay(true);
		UE_LOG(LogTrafficGameVehicle, Display, TEXT("%s is StartRecordReplay"), *GetName());
	}
	else
	{
		UE_LOG(LogTrafficGameVehicle, Error, TEXT("%s is trying to StartRecordReplay but ReplayComponent is invalid?"), *GetName());
	}
}

void AReplayVehiclePawn::StartPlaybackReplay() const
{
	if (ReplayComponent)
	{
		ReplayComponent->SetIsPlayingReplay(true);
		UE_LOG(LogTrafficGameVehicle, Display, TEXT("%s is StartPlaybackReplay"), *GetName());
	}
	else
	{
		UE_LOG(LogTrafficGameVehicle, Error, TEXT("%s is trying to StartPlaybackReplay but ReplayComponent is invalid?"), *GetName());
	}
}

void AReplayVehiclePawn::StopAllReplay() const
{
	if (ReplayComponent)
	{
		ReplayComponent->SetIsPlayingReplay(false);
		ReplayComponent->SetIsRecordingReplay(false);
		UE_LOG(LogTrafficGameVehicle, Display, TEXT("StopAllReplay called for : %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTrafficGameVehicle, Error, TEXT("%s is trying to StopAllReplay but ReplayComponent is invalid?"), *GetName());
	}
}

void AReplayVehiclePawn::DeleteRecordedReplay() const
{
	if (ReplayComponent)
	{
		ReplayComponent->DeleteRecordedReplay();
		UE_LOG(LogTrafficGameVehicle, Display, TEXT("DeleteRecordedReplay called for : %s"), *GetName());
	}
	else
	{
		UE_LOG(LogTrafficGameVehicle, Error, TEXT("%s is trying to DeleteRecordedReplay but ReplayComponent is invalid?"), *GetName());
	}
}
