#pragma once

#include "GameFramework/Actor.h"

#include "VehicleStartActor.generated.h"

/**
 * Base Vehicle Spawn Point Actor
 * Could also inherit from Player Start, it's basically the same
 */
UCLASS(Blueprintable)
class TRAFFICGAME_API AVehicleStartActor final : public AActor
{
	GENERATED_BODY()

};
