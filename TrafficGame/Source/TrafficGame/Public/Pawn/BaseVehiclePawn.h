#pragma once

#include "WheeledVehiclePawn.h"

#include "BaseVehiclePawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

/**
 * This is the base class for a moving Vehicle
 */
UCLASS()
class TRAFFICGAME_API ABaseVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:

	ABaseVehiclePawn(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Vehicle
	void ResetVehiclePosition();

	void SetBrakeLightsEnabled(bool bEnabled);
	bool GetBrakeLightsEnabled() const;

	// Camera
	void EnableCamera(bool bFrontCamera = false) const;
	void DisableCameras() const;
	bool IsFrontCameraActive() const;
	bool IsBackCameraActive() const;
	void CameraToggle() const;
	void LookAround(float AxisValue) const;

	// Gameplay
	void RequestCancelRound() const;
	virtual void FellOutOfWorld(const UDamageType& DmgType) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> FrontCameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FrontCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> BackCameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> BackCamera;

	UFUNCTION(BlueprintImplementableEvent)
	void SetBrakeLights(bool bEnabled);

private:

	void InitializeVehicle();

	void InterpCameraToOriginalLocation() const;

	bool CanResetVehiclePosition() const;
	void ExecuteResetVehiclePosition();

	bool bBrakeLightsActive = false;
};
