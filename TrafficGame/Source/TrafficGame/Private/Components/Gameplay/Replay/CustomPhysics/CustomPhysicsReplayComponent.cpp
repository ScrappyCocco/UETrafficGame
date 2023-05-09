#include "Components/Gameplay/Replay/CustomPhysics/CustomPhysicsReplayComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Pawn/BaseVehiclePawn.h"
#include "PhysicsEngine/BodyInstance.h"
#include "TrafficGame/TrafficGameLogs.h"

void UCustomPhysicsReplayComponent::BeginPlay()
{
	Super::BeginPlay();

	BindCustomPhysics();
}

void UCustomPhysicsReplayComponent::BeginDestroy()
{
	UnbindCustomPhysics();

	Super::BeginDestroy();
}

void UCustomPhysicsReplayComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerVehicle.IsValid())
	{
		if (const USkeletalMeshComponent* VehicleSktMesh = OwnerVehicle->GetMesh())
		{
			if (FBodyInstance* BodyInstance = VehicleSktMesh->GetBodyInstance())
			{
				BodyInstance->AddCustomPhysics(OnCalculateCustomPhysics);
			}
		}
	}
}

void UCustomPhysicsReplayComponent::SetIsPlayingReplay(const bool InIsPlayingReplay)
{
	Super::SetIsPlayingReplay(InIsPlayingReplay);

	ResetReplayVariables();
}

void UCustomPhysicsReplayComponent::SetIsRecordingReplay(const bool InIsRecordingReplay)
{
	Super::SetIsRecordingReplay(InIsRecordingReplay);

	ResetReplayVariables();
}

void UCustomPhysicsReplayComponent::DeleteRecordedReplay()
{
	Super::DeleteRecordedReplay(); // Empty

	if (bIsPlayingReplay || bIsRecordingReplay)
	{
		UE_LOG(LogTrafficGameVehicle, Error, TEXT("You can not delete a replay while is being played or recorded!"));
	}
	else
	{
		RecordedReplay.Empty();
	}
}

void UCustomPhysicsReplayComponent::ResetReplayVariables()
{
	ReplayTime = 0;
	CurrentReplayIndex = 0;
}

void UCustomPhysicsReplayComponent::BindCustomPhysics()
{
	OnCalculateCustomPhysics.BindUObject(this, &ThisClass::CustomPhysics);
}

void UCustomPhysicsReplayComponent::UnbindCustomPhysics()
{
	OnCalculateCustomPhysics.Unbind();
}

void UCustomPhysicsReplayComponent::CustomPhysics(const float DeltaTime, FBodyInstance* BodyInstance)
{
	if (bIsRecordingReplay)
	{
		RecordReplay(DeltaTime);
	}
	else if (bIsPlayingReplay)
	{
		PlayReplay(DeltaTime);
	}
}

void UCustomPhysicsReplayComponent::RecordReplay(const float DeltaTime)
{
	ReplayTime += DeltaTime;

	FVehicleReplayEntry Entry;
	Entry.Time = ReplayTime;
	StoreVehicleData(Entry);

	RecordedReplay.Add(Entry);
}

void UCustomPhysicsReplayComponent::StoreVehicleData(FVehicleReplayEntry& Entry) const
{
	if (OwnerVehicle.IsValid())
	{
		Entry.bBrakeLightsActive = OwnerVehicle->GetBrakeLightsEnabled();

		if (const UChaosWheeledVehicleMovementComponent* MovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(OwnerVehicle->GetVehicleMovementComponent()))
		{
			Entry.VehicleSnapshot = MovementComponent->GetSnapshot();
		}
	}
}

void UCustomPhysicsReplayComponent::PlayReplay(const float DeltaTime)
{
	if (OwnerVehicle.IsValid())
	{
		ReplayTime += DeltaTime;

		if (RecordedReplay.IsValidIndex(CurrentReplayIndex))
		{
			RestoreVehicleData(RecordedReplay[CurrentReplayIndex]);

			// Just go over the end when reached so we no longer apply vehicle data
			// We just let the vehicle be so it can be pushed/moved
			CurrentReplayIndex++;
		}
	}
}

void UCustomPhysicsReplayComponent::RestoreVehicleData(const FVehicleReplayEntry& Entry) const
{
	if (OwnerVehicle.IsValid())
	{
		OwnerVehicle->SetBrakeLightsEnabled(Entry.bBrakeLightsActive);

		if (UChaosWheeledVehicleMovementComponent* MovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(OwnerVehicle->GetVehicleMovementComponent()))
		{
			MovementComponent->SetSnapshot(Entry.VehicleSnapshot);
		}
	}
}

