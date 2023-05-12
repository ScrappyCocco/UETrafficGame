#pragma once

#include "CoreMinimal.h"

#include "GameFramework/CheatManager.h"

#include "TrafficCheatManager.generated.h"

/**
 * Cheats to edit the game and debug
 * These are for debug purposes only and could break the game
 */
UCLASS(Within = PlayerController)
class TRAFFICGAME_API UTrafficCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:

	// Set the CurrentRound in the GameMode, allowing you to skip directly to a future or previous round
	// This can break the game if used during a round, and UI is not updated
	// If you ship from round 1 to round 10, middle rounds will not have the replay of course
	// If you go from round 10 to round 1 you will overwrite replays
	// Round goes from 0 to N-1
	UFUNCTION(exec)
	void SetCurrentRound(int32 Round);

	// Set the Vehicle to use for the Round
	// The Vehicle must be a valid VehicleID from the DataTable
	// This can break the game if used during a round
	// Round goes from 0 to N-1
	UFUNCTION(exec)
	void SetCurrentRoundVehicle(int32 Round, const FString& VehicleID);

	// Set the Vehicle to use for the Round
	// The Vehicle must be a valid VehicleID from the DataTable
	// This can break the game if used during a round
	UFUNCTION(exec)
	void SetAllRoundsVehicle(const FString& VehicleID);
	
};
