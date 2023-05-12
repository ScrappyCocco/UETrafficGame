#pragma once

#include "GameFramework/PlayerController.h"

#include "InputActionValue.h"

#include "PlayerVehicleController.generated.h"

class UInputMappingContext;
class UInputAction;
class APawn;
class ABaseVehiclePawn;
class UChaosWheeledVehicleMovementComponent;

/**
 *
 */
UCLASS()
class TRAFFICGAME_API APlayerVehicleController final : public APlayerController
{
	GENERATED_BODY()

public:

	APlayerVehicleController();

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:

	/** MappingContext */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputAction* ResetAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputAction* CameraToggleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	UInputAction* QuitAction;

	void VehicleThrottle(const FInputActionValue& Value);
	void VehicleSteering(const FInputActionValue& Value);
	void VehicleReset(const FInputActionValue& Value);

	void VehicleBrakeTriggered(const FInputActionValue& Value);
	void VehicleBrakeStarted(const FInputActionValue& Value);
	void VehicleBrakeCompleted(const FInputActionValue& Value);

	void VehicleHandbrakeTriggered(const FInputActionValue& Value);
	void VehicleHandbrakeCompleted(const FInputActionValue& Value);

	void LookAround(const FInputActionValue& Value);
	void CameraToggle(const FInputActionValue& Value);

	void Quit(const FInputActionValue& Value);

private:

	UPROPERTY()
	TObjectPtr<ABaseVehiclePawn> VehiclePawn = nullptr;

	UPROPERTY()
	TObjectPtr<UChaosWheeledVehicleMovementComponent> VehicleMovementComponent = nullptr;
};

