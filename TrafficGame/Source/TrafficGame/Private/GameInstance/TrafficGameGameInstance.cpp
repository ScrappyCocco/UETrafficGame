#include "GameInstance/TrafficGameGameInstance.h"

#include "Kismet/GameplayStatics.h"

void UTrafficGameGameInstance::RequestLevelLoad(const FName& LevelName)
{
	UGameplayStatics::OpenLevel(this, LevelName);
}

void UTrafficGameGameInstance::RequestLevelLoadByRef(const TSoftObjectPtr<UWorld> Level)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, Level);
}
