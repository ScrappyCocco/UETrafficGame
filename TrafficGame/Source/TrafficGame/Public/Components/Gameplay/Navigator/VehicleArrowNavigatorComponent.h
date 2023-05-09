#pragma once

#include "CoreMinimal.h"

#include "Components/ArrowComponent.h"

#include "VehicleArrowNavigatorComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TRAFFICGAME_API UVehicleArrowNavigatorComponent final : public UArrowComponent
{
	GENERATED_BODY()

public:

	UVehicleArrowNavigatorComponent();

	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	// Color to use when far from the objective
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	FColor StartArrowColor;
	// Color to use when far near the objective
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	FColor EndArrowColor;

	// Size to use when far from the objective
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	float StartArrowSize;
	// Size to use when near from the objective
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	float EndArrowSize;

	// Length to use when far from the objective
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	float StartArrowLength;
	// Length to use when near from the objective
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	float EndArrowLength;

private:

	TWeakObjectPtr<AActor> Destination = nullptr;
	TWeakObjectPtr<AActor> OwnerVehicle = nullptr;
	float TotalDistance = 0.f;
};
