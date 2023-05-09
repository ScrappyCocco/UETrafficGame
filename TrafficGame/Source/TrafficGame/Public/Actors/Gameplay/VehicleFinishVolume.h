#pragma once

#include "Engine/TriggerBox.h"

#include "VehicleFinishVolume.generated.h"

/**
 * Base Vehicle Finish Trigger Volume
 */
UCLASS(Blueprintable)
class TRAFFICGAME_API AVehicleFinishVolume final : public ATriggerBox
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	void EnableForCurrentRound();
	void DisableForCurrentRound();

protected:

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
};
