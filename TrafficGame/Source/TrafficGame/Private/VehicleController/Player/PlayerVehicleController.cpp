#include "VehicleController/Player/PlayerVehicleController.h"

#include "Engine/LocalPlayer.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Cheats/TrafficCheatManager.h"
#include "Kismet/GameplayStatics.h"
#include "Pawn/BaseVehiclePawn.h"

APlayerVehicleController::APlayerVehicleController()
{
	CheatClass = UTrafficCheatManager::StaticClass();
}

void APlayerVehicleController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void APlayerVehicleController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ThisClass::VehicleThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ThisClass::VehicleThrottle);

		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ThisClass::VehicleSteering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ThisClass::VehicleSteering);

		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &ThisClass::VehicleReset);

		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ThisClass::VehicleBrakeTriggered);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ThisClass::VehicleBrakeStarted);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ThisClass::VehicleBrakeCompleted);

		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Triggered, this, &ThisClass::VehicleHandbrakeTriggered);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ThisClass::VehicleHandbrakeCompleted);

		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ThisClass::LookAround);
		EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Triggered, this, &ThisClass::CameraToggle);

		EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Triggered, this, &ThisClass::Quit);
	}
}

void APlayerVehicleController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Get the Vehicle and the Movement Component
	VehiclePawn = Cast<ABaseVehiclePawn>(InPawn);
	if (VehiclePawn)
	{
		VehicleMovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(VehiclePawn->GetVehicleMovementComponent());
	}
}

void APlayerVehicleController::OnUnPossess()
{
	// Reset the data to make sure the Vehicle is Reset and we don't control anything
	if (VehicleMovementComponent)
	{
		VehicleMovementComponent->ResetVehicle();
	}

	VehiclePawn = nullptr;
	VehicleMovementComponent = nullptr;

	Super::OnUnPossess();
}

void APlayerVehicleController::VehicleThrottle(const FInputActionValue& Value)
{
	if (VehicleMovementComponent)
	{
		VehicleMovementComponent->SetThrottleInput(Value.Get<float>());
	}
}

void APlayerVehicleController::VehicleSteering(const FInputActionValue& Value)
{
	if (VehicleMovementComponent)
	{
		VehicleMovementComponent->SetSteeringInput(Value.Get<float>());
	}
}

void APlayerVehicleController::VehicleReset(const FInputActionValue& Value)
{
	if (VehiclePawn)
	{
		VehiclePawn->ResetVehiclePosition();
	}
}

void APlayerVehicleController::VehicleBrakeTriggered(const FInputActionValue& Value)
{
	if (VehicleMovementComponent)
	{
		VehicleMovementComponent->SetBrakeInput(Value.Get<float>());
	}
}

void APlayerVehicleController::VehicleBrakeStarted(const FInputActionValue& Value)
{
	if (VehiclePawn)
	{
		VehiclePawn->SetBrakeLightsEnabled(true);
	}
}

void APlayerVehicleController::VehicleBrakeCompleted(const FInputActionValue& Value)
{
	if (VehiclePawn)
	{
		VehiclePawn->SetBrakeLightsEnabled(false);
	}

	if (VehicleMovementComponent)
	{
		VehicleMovementComponent->SetBrakeInput(0.0f);
	}
}

void APlayerVehicleController::VehicleHandbrakeTriggered(const FInputActionValue& Value)
{
	if (VehiclePawn)
	{
		VehiclePawn->SetBrakeLightsEnabled(true);
	}

	if (VehicleMovementComponent)
	{
		VehicleMovementComponent->SetHandbrakeInput(true);
	}
}

void APlayerVehicleController::VehicleHandbrakeCompleted(const FInputActionValue& Value)
{
	if (VehiclePawn)
	{
		VehiclePawn->SetBrakeLightsEnabled(false);
	}

	if (VehicleMovementComponent)
	{
		VehicleMovementComponent->SetHandbrakeInput(false);
	}
}

void APlayerVehicleController::LookAround(const FInputActionValue& Value)
{
	if (VehiclePawn)
	{
		VehiclePawn->LookAround(Value.Get<float>());
	}
}

void APlayerVehicleController::CameraToggle(const FInputActionValue& Value)
{
	if (VehiclePawn)
	{
		VehiclePawn->CameraToggle();
	}
}

void APlayerVehicleController::Quit(const FInputActionValue& Value)
{
	if (VehiclePawn)
	{
		VehiclePawn->RequestCancelRound();
	}
}
