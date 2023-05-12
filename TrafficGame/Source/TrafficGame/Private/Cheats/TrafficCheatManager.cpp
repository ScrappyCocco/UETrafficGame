#include "Cheats/TrafficCheatManager.h"

#include "GameMode/Gameplay/TrafficGameplayGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Pawn/Gameplay/ReplayVehiclePawn.h"
#include "TrafficGame/TrafficGameLogs.h"

void UTrafficCheatManager::SetCurrentRound(const int32 Round)
{
	if (ATrafficGameplayGameMode* TrafficGameMode = Cast<ATrafficGameplayGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		TrafficGameMode->CurrentRound = Round;
		UE_LOG(LogTrafficCheatManager, Warning, TEXT("Correctly set TrafficGameMode->CurrentRound with %d"), Round);
	}
	else
	{
		UE_LOG(LogTrafficCheatManager, Error, TEXT("Unable to execute SetCurrentRound for current GameMode!"));
	}
}

void UTrafficCheatManager::SetCurrentRoundVehicle(int32 Round, const FString& VehicleID)
{
	if (ATrafficGameplayGameMode* TrafficGameMode = Cast<ATrafficGameplayGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (const FVehiclePawnRow* VehicleRow = TrafficGameMode->FindVehicle(VehicleID))
		{
			if (TrafficGameMode->Vehicles.IsValidIndex(Round))
			{
				FRoundVehicle& RoundToEdit = TrafficGameMode->Vehicles[Round];
				AReplayVehiclePawn* OldVehicle = RoundToEdit.VehiclePawn;

				const FTransform OldVehicleTransform = OldVehicle->GetActorTransform();

				// Delete the old vehicle
				OldVehicle->Destroy();

				// Spawn the new Vehicle in the same position of the deleted one, other data is still valid
				AReplayVehiclePawn* SpawnedVehicle = TrafficGameMode->SpawnVehicle(VehicleRow, OldVehicleTransform);

				// Replace the reference
				RoundToEdit.VehiclePawn = SpawnedVehicle;

				UE_LOG(LogTrafficCheatManager, Warning, TEXT("Correctly changed Vehicle for Round %d"), Round);
			}
			else
			{
				UE_LOG(LogTrafficCheatManager, Error, TEXT("Round is not valid"));
			}
		}
		else
		{
			UE_LOG(LogTrafficCheatManager, Error, TEXT("Unable to find VehicleID, check the DataTable"));
		}
	}
	else
	{
		UE_LOG(LogTrafficCheatManager, Error, TEXT("Unable to execute SetCurrentRoundVehicle for current GameMode!"));
	}
}

void UTrafficCheatManager::SetAllRoundsVehicle(const FString& VehicleID)
{
	if (ATrafficGameplayGameMode* TrafficGameMode = Cast<ATrafficGameplayGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (const FVehiclePawnRow* VehicleRow = TrafficGameMode->FindVehicle(VehicleID))
		{
			for(FRoundVehicle& RoundVehicle : TrafficGameMode->Vehicles)
			{
				AReplayVehiclePawn* OldVehicle = RoundVehicle.VehiclePawn;

				const FTransform OldVehicleTransform = OldVehicle->GetActorTransform();

				// Delete the old vehicle
				OldVehicle->Destroy();

				// Spawn the new Vehicle in the same position of the deleted one, other data is still valid
				AReplayVehiclePawn* SpawnedVehicle = TrafficGameMode->SpawnVehicle(VehicleRow, OldVehicleTransform);

				// Replace the reference
				RoundVehicle.VehiclePawn = SpawnedVehicle;
			}

			UE_LOG(LogTrafficCheatManager, Warning, TEXT("Correctly changed all rounds Vehicle "));
		}
		else
		{
			UE_LOG(LogTrafficCheatManager, Error, TEXT("Unable to find VehicleID, check the DataTable"));
		}
	}
	else
	{
		UE_LOG(LogTrafficCheatManager, Error, TEXT("Unable to execute SetAllRoundsVehicle for current GameMode!"));
	}
}
