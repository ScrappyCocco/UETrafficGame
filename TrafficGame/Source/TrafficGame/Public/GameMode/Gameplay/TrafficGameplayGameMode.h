#pragma once

#include "GameMode/TrafficGameGameModeBase.h"

#include "Engine/DataTable.h"
#include "Data/TrafficGameStructs.h"

#include "TrafficGameplayGameMode.generated.h"

class AReplayVehiclePawn;
class ULevelSequencePlayer;

USTRUCT()
struct FRoundVehicle
{
	GENERATED_BODY()

	TObjectPtr<AReplayVehiclePawn> VehiclePawn = nullptr;

	FTransform IdleSpawnLocation;

	FTransform StartSpawnLocation;

	FString FinishTriggerTag;

	TObjectPtr<AActor> FinishVolume = nullptr;

};

/**
 * Game-play Game Mode for the Traffic Game
 * Manage Spawn/Record/Replay of Vehicles
 */
UCLASS()
class TRAFFICGAME_API ATrafficGameplayGameMode final : public ATrafficGameGameModeBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	// Called by the Finish Volume when the Player reach it
	void FinishTriggerReached(const FString& TriggerTag);

	// Called to cancel the current round if the Player request it
	void CancelCurrentRound();

protected:

	// BlueprintCallable

	// The user pressed Play on the UI for the Next Round
	UFUNCTION(BlueprintCallable)
	void UIUserStartRound();

	UFUNCTION(BlueprintCallable)
	FDateTime GetRoundPlayTime() const;
	UFUNCTION(BlueprintCallable)
	FDateTime GetTotalPlayTime() const;

	UFUNCTION(BlueprintCallable)
	FString GetCurrentRoundVehicleName() const;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentRound() const;
	UFUNCTION(BlueprintCallable)
	int32 GetNumOfRounds() const;

	// BlueprintImplementableEvent 

	UFUNCTION(BlueprintImplementableEvent)
	void ShowGameplayStartWidget(TSubclassOf<UUserWidget> Widget);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowNextRoundWidget(TSubclassOf<UUserWidget> Widget);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowGameplayEndWidgetWidget(TSubclassOf<UUserWidget> Widget);

	// Data

	UPROPERTY(EditInstanceOnly, Category = "Gameplay")
	TObjectPtr<UDataTable> VehiclePawnsDataTable;

	UPROPERTY(EditInstanceOnly, Category = "Gameplay")
	TObjectPtr<UDataTable> LevelsDataTable;

private:

	void LoadLevelSettings();
	void LocateOverviewCamera();
	AActor* LocateSpawnPointActor() const;
	void SpawnPawns();
	FTransform GetVehicleGameplaySpawnLocation(const FTrafficGameLevelSettingsRow& RoundRow) const;
	AActor* GetVehicleGameplayFinishActor(const FTrafficGameLevelSettingsRow& RoundRow) const;

	void PreGameplay();
	void PostGameplay();
	void PostEndRound();

	void EnableOverviewCamera() const;
	void DisableOverviewCamera() const;

	void StartRound();
	// Setup common data
	void InitializeRoundVehicleData(const FRoundVehicle& RoundVehicle);
	// Setup player specific data
	void InitializeRoundPlayerVehicle(const FRoundVehicle& RoundVehicle);
	void InitializeRoundFinishTrigger(const FRoundVehicle& RoundVehicle);
	// Setup replay vehicle data
	void InitializeRoundOtherVehicle(const FRoundVehicle& RoundVehicle);

	void EndRound(const bool bGoToNextRound = true);
	// Un-Init common data
	void UnInitializeRoundVehicleData(const FRoundVehicle& RoundVehicle);
	// Un-Init player specific data
	void UnInitializeRoundPlayerVehicle(const FRoundVehicle& RoundVehicle);
	void UnInitializeRoundFinishTrigger(const FRoundVehicle& RoundVehicle);
	// Un-Init replay vehicle data
	void UnInitializeRoundOtherVehicle(const FRoundVehicle& RoundVehicle);

	// Background replay when the user has finished one or every round
	void StartBackgroundReplay();
	void EndBackgroundReplay();
	void TimerEndAndRestartBackgroundReplay();

	TUniquePtr<FTrafficGameLevelRow> LoadedLevelRow = nullptr;

	TArray<FTrafficGameLevelSettingsRow> LevelSettings;

	bool bVehicleIsUsingFrontCamera = false;

	// This indicate the current round, or when finished, the next round that will be played
	int32 CurrentRound = 0;
	bool bIsUserPlaying = false;

	FTimespan TotalLevelPlayTime;
	FTimespan RoundPlayTime;
	FDateTime CurrentRoundStartTime;
	// Used for background replay
	FTimespan LongestRoundPlayed;
	FTimerHandle BackgroundReplayTimer;

	TWeakObjectPtr<UCameraComponent> OverviewCamera = nullptr;
	TWeakObjectPtr<ULevelSequencePlayer> LevelSequence = nullptr;
	TWeakObjectPtr<APawn> DefaultGamePawn = nullptr;

	UPROPERTY()
	TArray<FRoundVehicle> Vehicles;

};
