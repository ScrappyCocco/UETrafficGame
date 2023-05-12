#include "GameMode/Gameplay/TrafficGameplayGameMode.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Actors/Gameplay/VehicleFinishVolume.h"
#include "Actors/Gameplay/VehicleStartActor.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "GameState/Gameplay/TrafficGameplayGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Pawn/Gameplay/ReplayVehiclePawn.h"
#include "VehicleController/Player/PlayerVehicleController.h"
#include "TrafficGame/TrafficGameLogs.h"
#include "TimerManager.h"

void ATrafficGameplayGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Load the Level data from the Data Table
	LoadLevelSettings();

	// Spawn all the necessary Vehicle Pawns
	SpawnPawns();

	// Set the Overview camera and display UI
	PreGameplay();
}

void ATrafficGameplayGameMode::LoadLevelSettings()
{
	if (const UWorld* World = GetWorld())
	{
		// Generate the Level Name
		FString MapName = World->GetMapName();
		MapName.RemoveFromStart(World->StreamingLevelsPrefix);

		// Get the Level Settings
		if (LevelsDataTable)
		{
			const FTrafficGameLevelRow* FoundLevelRow = LevelsDataTable->FindRow<FTrafficGameLevelRow>(FName(*MapName), LevelsDataTable->GetName());
			if (FoundLevelRow)
			{
				LoadedLevelRow = MakeUnique<FTrafficGameLevelRow>(MoveTempIfPossible(*FoundLevelRow));

				// Find Level Overview Camera
				LocateOverviewCamera();

				// Load Gameplay Vehicles Rows
				if (LoadedLevelRow->LevelDataTable)
				{
					TArray<FTrafficGameLevelSettingsRow*> RowsPointers;
					LoadedLevelRow->LevelDataTable->GetAllRows(LoadedLevelRow->LevelDataTable->GetName(), RowsPointers);

					for (const FTrafficGameLevelSettingsRow* RowPointer : RowsPointers)
					{
						if (RowPointer)
						{
							LevelSettings.Add(*RowPointer);
						}
					}
				}
				else
				{
					UE_LOG(LogTrafficGameGameMode, Error, TEXT("LevelDataTable is invalid for Level %s"), *MapName);
				}
			}
			else
			{
				UE_LOG(LogTrafficGameGameMode, Error, TEXT("Can't find row for Level %s"), *MapName);
			}
		}
	}
}

void ATrafficGameplayGameMode::LocateOverviewCamera()
{
	if (const UWorld* World = GetWorld())
	{
		if (LoadedLevelRow.IsValid())
		{
			TArray<AActor*> FoundCameras;
			const FString SearchCameraTag = LoadedLevelRow->LevelOverviewCameraTag;

			// First try to find the ACameraActor directly
			UGameplayStatics::GetAllActorsOfClassWithTag(World, ACameraActor::StaticClass(), FName(*SearchCameraTag), FoundCameras);
			if (FoundCameras.Num() > 1)
			{
				UE_LOG(LogTrafficGameGameMode, Error, TEXT("Found %d Cameras with the given Tag! This is wrong!"), FoundCameras.Num());
			}

			if (FoundCameras.IsValidIndex(0))
			{
				if (const ACameraActor* CameraActor = Cast<ACameraActor>(FoundCameras[0]))
				{
					OverviewCamera = CameraActor->GetCameraComponent();
					UE_LOG(LogTrafficGameGameMode, Display, TEXT("Succesfully found CameraComponent in CameraActor"));
				}
			}

			if (!OverviewCamera.IsValid())
			{
				// Try to find the Camera as Component of a generic AActor
				FoundCameras.Empty();
				UGameplayStatics::GetAllActorsOfClassWithTag(World, AActor::StaticClass(), FName(*SearchCameraTag), FoundCameras);
				if (FoundCameras.Num() > 1)
				{
					UE_LOG(LogTrafficGameGameMode, Error, TEXT("Found %d Actors with the given Tag searching the camera! This is wrong!"), FoundCameras.Num());
				}

				if (FoundCameras.IsValidIndex(0))
				{
					if (UCameraComponent* CameraComponent = Cast<UCameraComponent>(FoundCameras[0]->GetComponentByClass(UCameraComponent::StaticClass())))
					{
						OverviewCamera = CameraComponent;
						UE_LOG(LogTrafficGameGameMode, Display, TEXT("Succesfully found CameraComponent inside an Actor"));
					}
				}
				else
				{
					UE_LOG(LogTrafficGameGameMode, Error, TEXT("Unable to find the Camera from tag %s!"), *SearchCameraTag);
				}
			}

			if (LoadedLevelRow->bLevelUseSequencerAsPreview)
			{
				TArray<AActor*> FoundSequences;
				UGameplayStatics::GetAllActorsOfClass(World, ALevelSequenceActor::StaticClass(), FoundSequences);

				if (FoundSequences.Num() > 1)
				{
					UE_LOG(LogTrafficGameGameMode, Error, TEXT("Found %d ALevelSequenceActor with the given Tag searching the Sequence! This is wrong!"), FoundSequences.Num());
				}

				if (FoundSequences.IsValidIndex(0))
				{
					if (const ALevelSequenceActor* FoundLevelSequence = Cast<ALevelSequenceActor>(FoundSequences[0]))
					{
						LevelSequence = FoundLevelSequence->GetSequencePlayer();
						UE_LOG(LogTrafficGameGameMode, Display, TEXT("Succesfully found LevelSequenceActor and SequencePlayer"));
					}
				}
				else
				{
					UE_LOG(LogTrafficGameGameMode, Error, TEXT("Unable to find ALevelSequenceActor but bLevelUseSequencerAsPreview is true!"));
				}
			}
		}
	}
}

AActor* ATrafficGameplayGameMode::LocateSpawnPointActor() const
{
	if (const UWorld* World = GetWorld())
	{
		TArray<AActor*> Actors;
		const FString SpawnPointTag = LoadedLevelRow.IsValid() ? LoadedLevelRow->IdleSpawnPointTag : FString();
		UGameplayStatics::GetAllActorsWithTag(World, FName(*SpawnPointTag), Actors);

		if (Actors.Num() > 1)
		{
			UE_LOG(LogTrafficGameGameMode, Error, TEXT("Found %d Actors with the Spawn Tag! This is wrong!"), Actors.Num());
		}

		if (Actors.IsValidIndex(0))
		{
			return Actors[0];
		}

		UE_LOG(LogTrafficGameGameMode, Error, TEXT("Unable to find the SpawnPointActor from tag %s!"), *SpawnPointTag);
	}

	return nullptr;
}

void ATrafficGameplayGameMode::SpawnPawns()
{
	if (UWorld* World = GetWorld())
	{
		const AActor* SpawnPointActor = LocateSpawnPointActor();
		FVector SpawnPosition = IsValid(SpawnPointActor) ? SpawnPointActor->GetActorLocation() : FVector::Zero();
		bool bSpawnFlipFlop = true;

		for (const FTrafficGameLevelSettingsRow LevelRound : LevelSettings)
		{
			const FString VehicleID = LevelRound.VehicleID;
			if (const FVehiclePawnRow* VehicleRow = FindVehicle(VehicleID))
			{
				FTransform SpawnTransform;
				SpawnTransform.SetLocation(SpawnPosition);

				UE_LOG(LogTrafficGameGameMode, Display, TEXT("IdleSpawnLocation %f %f %f"), SpawnPosition.X, SpawnPosition.Y, SpawnPosition.Z);
				AReplayVehiclePawn* SpawnedVehicle = SpawnVehicle(VehicleRow, SpawnTransform);

				FRoundVehicle RoundVehicleStruct;
				RoundVehicleStruct.VehiclePawn = SpawnedVehicle;
				RoundVehicleStruct.IdleSpawnLocation = SpawnTransform;
				RoundVehicleStruct.StartSpawnLocation = GetVehicleGameplaySpawnLocation(LevelRound);
				RoundVehicleStruct.FinishTriggerTag = LevelRound.DestinationPointActorTag;
				RoundVehicleStruct.FinishVolume = GetVehicleGameplayFinishActor(LevelRound);
				Vehicles.Add(RoundVehicleStruct);

				// This switch between +/- to go left/right from the given center
				if (bSpawnFlipFlop)
				{
					SpawnPosition += FVector(0, 500, 0);
				}
				SpawnPosition.Set(SpawnPosition.X, SpawnPosition.Y * -1, SpawnPosition.Z);
				bSpawnFlipFlop = !bSpawnFlipFlop;

				UE_LOG(LogTrafficGameGameMode, Display, TEXT("Correctly Spawned VehicleID: %s"), *VehicleID);
			}
			else
			{
				UE_LOG(LogTrafficGameGameMode, Error, TEXT("Unable to find FVehiclePawnRow for VehicleID: %s"), *VehicleID);
			}
		}
	}
}

FVehiclePawnRow* ATrafficGameplayGameMode::FindVehicle(const FString& VehicleID) const
{
	if (VehiclePawnsDataTable)
	{
		return VehiclePawnsDataTable->FindRow<FVehiclePawnRow>(FName(*VehicleID), VehiclePawnsDataTable->GetName());
	}

	UE_LOG(LogTrafficGameGameMode, Error, TEXT("VehiclePawnsDataTable is not set!"));
	return nullptr;
}

AReplayVehiclePawn* ATrafficGameplayGameMode::SpawnVehicle(const FVehiclePawnRow* VehicleRow, const FTransform& SpawnTransform) const
{
	if (UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AReplayVehiclePawn* SpawnedVehicle = World->SpawnActor<AReplayVehiclePawn>(VehicleRow->VehiclePawn, SpawnTransform, SpawnParameters);

		return SpawnedVehicle;
	}

	return nullptr;
}

FTransform ATrafficGameplayGameMode::GetVehicleGameplaySpawnLocation(const FTrafficGameLevelSettingsRow& RoundRow) const
{
	if (RoundRow.SpawnPointActorTag.IsEmpty())
	{
		UE_LOG(LogTrafficGameGameMode, Error, TEXT("SpawnPointActorTag is empty! Fix the Data Table"));
		return FTransform();
	}

	if (const UWorld* World = GetWorld())
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClassWithTag(World, AVehicleStartActor::StaticClass(), FName(*RoundRow.SpawnPointActorTag), Actors);

		if (Actors.Num() > 1)
		{
			UE_LOG(LogTrafficGameGameMode, Error, TEXT("Found %d Actors with the tag %s! This is wrong!"), Actors.Num(), *RoundRow.SpawnPointActorTag);
		}

		if (Actors.IsValidIndex(0))
		{
			return Actors[0]->GetActorTransform();
		}
	}

	UE_LOG(LogTrafficGameGameMode, Error, TEXT("Unable to find the Spawn for the round from tag %s!"), *RoundRow.SpawnPointActorTag);
	return FTransform();
}

AActor* ATrafficGameplayGameMode::GetVehicleGameplayFinishActor(const FTrafficGameLevelSettingsRow& RoundRow) const
{
	if (RoundRow.DestinationPointActorTag.IsEmpty())
	{
		UE_LOG(LogTrafficGameGameMode, Error, TEXT("DestinationPointActorTag is empty! Fix the Data Table"));
		return nullptr;
	}

	if (const UWorld* World = GetWorld())
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClassWithTag(World, AVehicleFinishVolume::StaticClass(), FName(*RoundRow.DestinationPointActorTag), Actors);

		if (Actors.Num() > 1)
		{
			UE_LOG(LogTrafficGameGameMode, Error, TEXT("Found %d Actors with the tag %s! This is wrong!"), Actors.Num(), *RoundRow.DestinationPointActorTag);
		}

		if (Actors.IsValidIndex(0))
		{
			return Actors[0];
		}
	}

	UE_LOG(LogTrafficGameGameMode, Error, TEXT("Unable to find the Finish Actor for the round from tag %s!"), *RoundRow.DestinationPointActorTag);
	return nullptr;
}

void ATrafficGameplayGameMode::PreGameplay()
{
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		DefaultGamePawn = PlayerController->GetPawn();
	}

	EnableOverviewCamera();

	if (LoadedLevelRow.IsValid())
	{
		ShowGameplayStartWidget(LoadedLevelRow->GameplayStartWidget);
	}
}

void ATrafficGameplayGameMode::PostGameplay()
{
	EnableOverviewCamera();

	if (LoadedLevelRow.IsValid())
	{
		ShowGameplayEndWidgetWidget(LoadedLevelRow->GameplayEndWidget);
	}
}

void ATrafficGameplayGameMode::PostEndRound()
{
	EnableOverviewCamera();

	if (LoadedLevelRow.IsValid())
	{
		ShowNextRoundWidget(LoadedLevelRow->NextRoundWidget);
	}
}

void ATrafficGameplayGameMode::CancelCurrentRound()
{
	if (bIsUserPlaying)
	{
		// Terminate the current round (don't increment to next round)
		EndRound(false);

		// Delete the Replay of the current round since it's not complete
		if (Vehicles.IsValidIndex(CurrentRound))
		{
			Vehicles[CurrentRound].VehiclePawn->DeleteRecordedReplay();
		}
	}
}

void ATrafficGameplayGameMode::EnableOverviewCamera() const
{
	if (OverviewCamera.IsValid())
	{
		OverviewCamera->Activate();

		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PlayerController->SetViewTarget(OverviewCamera->GetOwner());
		}
	}

	if (LoadedLevelRow.IsValid())
	{
		if (LoadedLevelRow->bLevelUseSequencerAsPreview && LevelSequence.IsValid())
		{
			LevelSequence->Play();
		}
	}
}

void ATrafficGameplayGameMode::DisableOverviewCamera() const
{
	if (LoadedLevelRow.IsValid())
	{
		if (LoadedLevelRow->bLevelUseSequencerAsPreview && LevelSequence.IsValid())
		{
			LevelSequence->Stop();
		}
	}

	if (OverviewCamera.IsValid())
	{
		OverviewCamera->Deactivate();
	}
}

void ATrafficGameplayGameMode::UIUserStartRound()
{
	EndBackgroundReplay();

	DisableOverviewCamera();

	StartRound();
}

FDateTime ATrafficGameplayGameMode::GetRoundPlayTime() const
{
	return FDateTime(RoundPlayTime.GetTicks());
}

FDateTime ATrafficGameplayGameMode::GetTotalPlayTime() const
{
	return FDateTime(TotalLevelPlayTime.GetTicks());
}

FString ATrafficGameplayGameMode::GetCurrentRoundVehicleName() const
{
	if (LevelSettings.IsValidIndex(CurrentRound))
	{
		if (!LevelSettings[CurrentRound].VehicleDisplayName.IsEmpty())
		{
			return LevelSettings[CurrentRound].VehicleDisplayName;
		}

		return LevelSettings[CurrentRound].VehicleID;
	}

	return FString();
}

int32 ATrafficGameplayGameMode::GetCurrentRound() const
{
	return CurrentRound;
}

int32 ATrafficGameplayGameMode::GetNumOfRounds() const
{
	return LevelSettings.Num();
}

void ATrafficGameplayGameMode::StartRound()
{
	bIsUserPlaying = true;

	// Teleport all previous vehicles + the current one to their spawn
	for (int32 Index = 0; Index <= CurrentRound; Index++)
	{
		if (Vehicles.IsValidIndex(Index))
		{
			const FRoundVehicle& RoundVehicle = Vehicles[Index];

			InitializeRoundVehicleData(RoundVehicle);

			// Current Round Settings
			if (Index == CurrentRound)
			{
				InitializeRoundFinishTrigger(RoundVehicle);
				InitializeRoundPlayerVehicle(RoundVehicle);
			}
			else
			{
				// Other Vehicle Settings (not the vehicle of the current round)
				InitializeRoundOtherVehicle(RoundVehicle);
			}
		}
	}

	// Broadcast Start Round to reset the map
	if (const ATrafficGameplayGameState* TrafficGameState = GetGameState<ATrafficGameplayGameState>())
	{
		TrafficGameState->EventStartRound(true);
	}

	CurrentRoundStartTime = FDateTime::UtcNow();
}

void ATrafficGameplayGameMode::InitializeRoundVehicleData(const FRoundVehicle& RoundVehicle)
{
	RoundVehicle.VehiclePawn->SetActorTransform(RoundVehicle.StartSpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
	RoundVehicle.VehiclePawn->DisableCameras();
}

void ATrafficGameplayGameMode::InitializeRoundPlayerVehicle(const FRoundVehicle& RoundVehicle)
{
	// This vehicle is the current one and should record its replay
	RoundVehicle.VehiclePawn->StartRecordReplay();
	RoundVehicle.VehiclePawn->EnablePlayerVehicleComponents();

	// Possess the Current Round Vehicle
	if (APlayerVehicleController* PlayerVehicleController = Cast<APlayerVehicleController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		PlayerVehicleController->Possess(RoundVehicle.VehiclePawn);
	}

	RoundVehicle.VehiclePawn->EnableCamera(bVehicleIsUsingFrontCamera);
}

void ATrafficGameplayGameMode::InitializeRoundFinishTrigger(const FRoundVehicle& RoundVehicle)
{
	// Enable the Finish Trigger
	if (AVehicleFinishVolume* CurrentRoundFinishVolume = Cast<AVehicleFinishVolume>(RoundVehicle.FinishVolume))
	{
		CurrentRoundFinishVolume->EnableForCurrentRound();

		// Save the current Finish Trigger to Game State
		if (ATrafficGameplayGameState* TrafficGameState = GetGameState<ATrafficGameplayGameState>())
		{
			TrafficGameState->SetCurrentFinishVolume(CurrentRoundFinishVolume);
		}
	}
}

void ATrafficGameplayGameMode::InitializeRoundOtherVehicle(const FRoundVehicle& RoundVehicle)
{
	// This vehicle is from a previous round so it should play its replay
	RoundVehicle.VehiclePawn->StartPlaybackReplay();
}

void ATrafficGameplayGameMode::FinishTriggerReached(const FString& TriggerTag)
{
	if (Vehicles.IsValidIndex(CurrentRound))
	{
		const FRoundVehicle& CurrentRoundStruct = Vehicles[CurrentRound];
		if (CurrentRoundStruct.FinishTriggerTag.Equals(TriggerTag))
		{
			EndRound();
		}
	}
}

void ATrafficGameplayGameMode::EndRound(const bool bGoToNextRound)
{
	bIsUserPlaying = false;

	const FDateTime RoundEndTime = FDateTime::UtcNow();
	RoundPlayTime = RoundEndTime - CurrentRoundStartTime;
	TotalLevelPlayTime += RoundPlayTime;

	if (RoundPlayTime > LongestRoundPlayed)
	{
		LongestRoundPlayed = RoundPlayTime;
	}

	if (APlayerVehicleController* PlayerVehicleController = Cast<APlayerVehicleController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		// Unpossess the Current Pawn
		PlayerVehicleController->UnPossess();

		// Re-Possess the Default One
		if (DefaultGamePawn.IsValid())
		{
			PlayerVehicleController->Possess(DefaultGamePawn.Get());
		}
	}

	// Reset the Vehicles until this Round
	for (int32 Index = 0; Index <= CurrentRound; Index++)
	{
		if (Vehicles.IsValidIndex(Index))
		{
			const FRoundVehicle& RoundVehicle = Vehicles[Index];

			UnInitializeRoundVehicleData(RoundVehicle);

			// Current Round Settings
			if (Index == CurrentRound)
			{
				UnInitializeRoundPlayerVehicle(RoundVehicle);
				UnInitializeRoundFinishTrigger(RoundVehicle);
			}
			else
			{
				UnInitializeRoundOtherVehicle(RoundVehicle);
			}
		}
	}

	// Broadcast End Round to reset the map
	if (const ATrafficGameplayGameState* TrafficGameState = GetGameState<ATrafficGameplayGameState>())
	{
		// If not bGoToNextRound the round is cancelled
		TrafficGameState->EventEndRound(true, !bGoToNextRound);
	}

	// Ignored if the user cancel the current round
	if (bGoToNextRound)
	{
		CurrentRound++;
	}

	if (LevelSettings.IsValidIndex(CurrentRound))
	{
		// There are more rounds later
		PostEndRound();
	}
	else
	{
		// Level Ended
		PostGameplay();
	}

	// Played at least one round and replay enabled
	if (CurrentRound > 0 && LoadedLevelRow.IsValid() && LoadedLevelRow->bShouldPlayBackgroundReplay)
	{
		StartBackgroundReplay();
	}
}

void ATrafficGameplayGameMode::UnInitializeRoundVehicleData(const FRoundVehicle& RoundVehicle)
{
	RoundVehicle.VehiclePawn->SetActorTransform(RoundVehicle.IdleSpawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
	RoundVehicle.VehiclePawn->ResetVehiclePosition();

	// Stop Play/Record replay
	RoundVehicle.VehiclePawn->StopAllReplay();
}

void ATrafficGameplayGameMode::UnInitializeRoundPlayerVehicle(const FRoundVehicle& RoundVehicle)
{
	bVehicleIsUsingFrontCamera = RoundVehicle.VehiclePawn->IsFrontCameraActive();
	RoundVehicle.VehiclePawn->DisablePlayerVehicleComponents();
	RoundVehicle.VehiclePawn->DisableCameras();
}

void ATrafficGameplayGameMode::UnInitializeRoundFinishTrigger(const FRoundVehicle& RoundVehicle)
{
	// Disable the Finish Trigger just used
	if (AVehicleFinishVolume* CurrentRoundFinishVolume = Cast<AVehicleFinishVolume>(RoundVehicle.FinishVolume))
	{
		CurrentRoundFinishVolume->DisableForCurrentRound();

		// Remove the Trigger from Game State
		if (ATrafficGameplayGameState* TrafficGameState = GetGameState<ATrafficGameplayGameState>())
		{
			TrafficGameState->ClearCurrentFinishVolume();
		}
	}
}

void ATrafficGameplayGameMode::UnInitializeRoundOtherVehicle(const FRoundVehicle& RoundVehicle)
{
	RoundVehicle.VehiclePawn->DisableCameras();
}

void ATrafficGameplayGameMode::StartBackgroundReplay()
{
	// Teleport all previous vehicles
	// (Current Round is the next one that I still have not played! So -1)
	for (int32 Index = 0; Index <= CurrentRound - 1; Index++)
	{
		if (Vehicles.IsValidIndex(Index))
		{
			const FRoundVehicle& RoundVehicle = Vehicles[Index];

			InitializeRoundVehicleData(RoundVehicle);
			InitializeRoundOtherVehicle(RoundVehicle);
		}
	}

	// Broadcast Start Round
	if (const ATrafficGameplayGameState* TrafficGameState = GetGameState<ATrafficGameplayGameState>())
	{
		TrafficGameState->EventStartRound(false);
	}

	// Set End Time to end and re-start the replay (add a little delay)
	const float TimerDuration = LongestRoundPlayed.GetTotalSeconds() + 5.f;
	GetWorldTimerManager().SetTimer(BackgroundReplayTimer, this, &ThisClass::TimerEndAndRestartBackgroundReplay, 1.0f, false, TimerDuration);
}

void ATrafficGameplayGameMode::EndBackgroundReplay()
{
	GetWorldTimerManager().ClearTimer(BackgroundReplayTimer);

	// Reset the Vehicles until this Round
	for (int32 Index = 0; Index <= CurrentRound - 1; Index++)
	{
		if (Vehicles.IsValidIndex(Index))
		{
			const FRoundVehicle& RoundVehicle = Vehicles[Index];

			UnInitializeRoundVehicleData(RoundVehicle);
			UnInitializeRoundOtherVehicle(RoundVehicle);
		}
	}

	// Broadcast End Round
	if (const ATrafficGameplayGameState* TrafficGameState = GetGameState<ATrafficGameplayGameState>())
	{
		TrafficGameState->EventEndRound(false, false);
	}
}

void ATrafficGameplayGameMode::TimerEndAndRestartBackgroundReplay()
{
	EndBackgroundReplay();
	StartBackgroundReplay();
}
