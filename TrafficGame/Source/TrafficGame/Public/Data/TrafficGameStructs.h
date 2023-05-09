#pragma once

#include "Engine/DataTable.h"

#include "Blueprint/UserWidget.h" 
#include "Pawn/BaseVehiclePawn.h"

#include "TrafficGameStructs.generated.h"

/*
 * Structs for creating Game Data Tables
 */

USTRUCT(BlueprintType)
struct FPlayableLevels : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> LevelRef = nullptr;

};

USTRUCT(BlueprintType)
struct FVehiclePawnRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseVehiclePawn> VehiclePawn = nullptr;

};

USTRUCT(BlueprintType)
struct FTrafficGameLevelRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UDataTable> LevelDataTable = nullptr;

	// Where to spawn all the Level Vehicles when started
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString IdleSpawnPointTag = "SpawnActor";

	// The camera to use at begin/end of a round
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LevelOverviewCameraTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> GameplayStartWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> NextRoundWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> GameplayEndWidget = nullptr;

	// If the game should re-play the last round in the end round/game screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldPlayBackgroundReplay = true;

	// If the level has a Sequencer track as preview of the map
	// If so the Game Mode will Stop/Resume it
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLevelUseSequencerAsPreview = false;

};

USTRUCT(BlueprintType)
struct FTrafficGameLevelSettingsRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString VehicleID;

	// The Name to show in UI
	// VehicleID is used if this is Empty
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString VehicleDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SpawnPointActorTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DestinationPointActorTag;

};
