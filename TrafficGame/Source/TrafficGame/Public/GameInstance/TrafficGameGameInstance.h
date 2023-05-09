#pragma once

#include "CoreMinimal.h"

#include "Engine/GameInstance.h"

#include "TrafficGameGameInstance.generated.h"

/**
 * Functions needed by the whole game, as single point of access
 */
UCLASS()
class TRAFFICGAME_API UTrafficGameGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void RequestLevelLoad(const FName& LevelName);

	UFUNCTION(BlueprintCallable)
	void RequestLevelLoadByRef(const TSoftObjectPtr<UWorld> Level);
};
