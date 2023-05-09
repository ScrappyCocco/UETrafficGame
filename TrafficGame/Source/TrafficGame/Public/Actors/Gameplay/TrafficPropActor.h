#pragma once

#include "CoreMinimal.h"

#include "Engine/StaticMeshActor.h"

#include "TrafficPropActor.generated.h"

/*
 * To save memory and performance the Physics Props don't save their own replay
 * This is just an actor which save and restore its own position, useful for physical moving objects
 */
UCLASS()
class TRAFFICGAME_API ATrafficPropActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	ATrafficPropActor();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UFUNCTION()
	void RoundStarted(bool bIsPlayingRound);
	UFUNCTION()
	void RoundEnded(bool bIsPlayingRound, bool bCancelled);

	void RestorePropInitialPosition();

	FTransform SavedPropLocation;
};
