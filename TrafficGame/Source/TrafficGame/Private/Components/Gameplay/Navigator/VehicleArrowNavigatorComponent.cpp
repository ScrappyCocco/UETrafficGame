#include "Components/Gameplay/Navigator/VehicleArrowNavigatorComponent.h"

#include "Actors/Gameplay/VehicleFinishVolume.h"
#include "GameState/Gameplay/TrafficGameplayGameState.h"

UVehicleArrowNavigatorComponent::UVehicleArrowNavigatorComponent()
{
	ArrowSize = StartArrowSize;
	ArrowLength = StartArrowLength;
	UArrowComponent::SetArrowColor(StartArrowColor);

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UVehicleArrowNavigatorComponent::Activate(const bool bReset)
{
	Super::Activate(bReset);

	SetVisibility(true);

	if (const UWorld* World = GetWorld())
	{
		if (const ATrafficGameplayGameState* TrafficGameState = Cast<ATrafficGameplayGameState>(World->GetGameState()))
		{
			if (AVehicleFinishVolume* FinishVolume = TrafficGameState->GetCurrentFinishVolume())
			{
				Destination = FinishVolume;
			}
		}

		OwnerVehicle = GetOwner();
	}

	// Vehicle should already be spawned, let's calculate total distance
	if (Destination.IsValid() && OwnerVehicle.IsValid())
	{
		const FVector VehiclePosition = OwnerVehicle->GetActorLocation();
		const FVector DestinationLocation = Destination->GetActorLocation();
		TotalDistance = FVector::Dist(VehiclePosition, DestinationLocation);
	}
}

void UVehicleArrowNavigatorComponent::Deactivate()
{
	Super::Deactivate();

	SetVisibility(false);

	Destination.Reset();
	OwnerVehicle.Reset();
	TotalDistance = 0.0f;
}

void UVehicleArrowNavigatorComponent::BeginPlay()
{
	Super::BeginPlay();

	// Deactivated by default
	Deactivate();
}

void UVehicleArrowNavigatorComponent::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsActive() && Destination.IsValid() && OwnerVehicle.IsValid())
	{
		const FVector VehiclePosition = OwnerVehicle->GetActorLocation();
		// We could cache this but this way we could move the destination
		const FVector DestinationLocation = Destination->GetActorLocation();

		const float Distance = FVector::Dist(VehiclePosition, DestinationLocation);

		// Clamp between 0 and 1
		const float Alpha = FMath::Clamp(Distance / TotalDistance, 0, 1);
		const float NewArrowSize = FMath::Lerp(StartArrowSize, EndArrowSize, Alpha);
		const float NewArrowLength = FMath::Lerp(StartArrowLength, EndArrowLength, Alpha);
		const FLinearColor NewColor = FLinearColor::LerpUsingHSV(StartArrowColor.ReinterpretAsLinear(), EndArrowColor.ReinterpretAsLinear(), Alpha);

#if UE_BUILD_DEBUG
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ArrowNavigator Alpha %f"), Alpha));
		}
#endif

		ArrowSize = NewArrowSize;
		ArrowLength = NewArrowLength;
		SetArrowColor(NewColor);

		const FVector Direction = DestinationLocation - VehiclePosition;
		const FRotator Rotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		SetWorldRotation(Rotation);
	}
}
