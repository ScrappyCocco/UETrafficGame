#include "Pawn/BaseVehiclePawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h" 
#include "ChaosWheeledVehicleMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameState/Gameplay/TrafficGameplayGameState.h"
#include "Kismet/GameplayStatics.h"
#include "TrafficGame/TrafficGameLogs.h"

ABaseVehiclePawn::ABaseVehiclePawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Front Camera Arm and Camera
	FrontCameraSpringArm = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("FrontCameraSpringArm"));
	FrontCameraSpringArm->SetupAttachment(RootComponent);
	FrontCameraSpringArm->TargetArmLength = 0.0f;
	FrontCameraSpringArm->bDoCollisionTest = false;
	FrontCameraSpringArm->bEnableCameraRotationLag = true;
	FrontCameraSpringArm->CameraRotationLagSpeed = 15.0f;

	FrontCamera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FrontCamera"));
	FrontCamera->SetupAttachment(FrontCameraSpringArm, USpringArmComponent::SocketName);
	FrontCamera->SetAutoActivate(false);

	// Back Camera Arm and Camera
	BackCameraSpringArm = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("BackCameraSpringArm"));
	BackCameraSpringArm->SetupAttachment(RootComponent);
	BackCameraSpringArm->SocketOffset = FVector(0.0f, 0.0f, 150.0f);
	BackCameraSpringArm->TargetArmLength = 650.0f;
	BackCameraSpringArm->bDoCollisionTest = false;
	BackCameraSpringArm->bInheritPitch = false;
	BackCameraSpringArm->bInheritYaw = true;
	BackCameraSpringArm->bInheritRoll = false;
	BackCameraSpringArm->bEnableCameraRotationLag = true;
	BackCameraSpringArm->CameraRotationLagSpeed = 2.0f;

	BackCamera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("BackCamera"));
	BackCamera->SetupAttachment(BackCameraSpringArm, USpringArmComponent::SocketName);
}

void ABaseVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	InitializeVehicle();
}

void ABaseVehiclePawn::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (const UChaosWheeledVehicleMovementComponent* MovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		if (USkeletalMeshComponent* VehicleSktMesh = GetMesh())
		{
			const float Damping = MovementComponent->IsMovingOnGround() ? 0.0f : 3.0f;
			VehicleSktMesh->SetAngularDamping(Damping);
		}

		InterpCameraToOriginalLocation();
	}
}

void ABaseVehiclePawn::ResetVehiclePosition()
{
	if(CanResetVehiclePosition())
	{
		ExecuteResetVehiclePosition();
	}
}

void ABaseVehiclePawn::SetBrakeLightsEnabled(const bool bEnabled)
{
	if (bEnabled != bBrakeLightsActive)
	{
		bBrakeLightsActive = bEnabled;
		SetBrakeLights(bEnabled);
	}
}

bool ABaseVehiclePawn::GetBrakeLightsEnabled() const
{
	return bBrakeLightsActive;
}

void ABaseVehiclePawn::EnableCamera(const bool bFrontCamera) const
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (FrontCamera && bFrontCamera)
		{
			FrontCamera->Activate();

			PlayerController->SetViewTarget(FrontCamera->GetOwner());
		}
		else if (BackCamera)
		{
			BackCamera->Activate();

			PlayerController->SetViewTarget(BackCamera->GetOwner());
		}
	}
}

void ABaseVehiclePawn::DisableCameras() const
{
	if (FrontCamera && BackCamera)
	{
		FrontCamera->Deactivate();
		BackCamera->Deactivate();
	}
}

bool ABaseVehiclePawn::IsFrontCameraActive() const
{
	return FrontCamera ? FrontCamera->IsActive() : false;
}

bool ABaseVehiclePawn::IsBackCameraActive() const
{
	return BackCamera ? BackCamera->IsActive() : false;
}

void ABaseVehiclePawn::CameraToggle() const
{
	if (FrontCamera && BackCamera)
	{
		if (FrontCamera->IsActive())
		{
			FrontCamera->Deactivate();
			BackCamera->Activate();
		}
		else if (BackCamera->IsActive())
		{
			BackCamera->Deactivate();
			FrontCamera->Activate();
		}
	}
}

void ABaseVehiclePawn::LookAround(const float AxisValue) const
{
	if (BackCameraSpringArm)
	{
		BackCameraSpringArm->AddLocalRotation(FRotator(0.0f, AxisValue, 0.0f));
	}
}

void ABaseVehiclePawn::RequestCancelRound() const
{
	if (const UWorld* World = GetWorld())
	{
		if (const ATrafficGameplayGameState* TrafficGameState = Cast<ATrafficGameplayGameState>(World->GetGameState()))
		{
			TrafficGameState->CancelCurrentRound();
		}
	}
}

void ABaseVehiclePawn::FellOutOfWorld(const UDamageType& DmgType)
{
	Super::FellOutOfWorld(DmgType);

	// Vehicle is Player Controlled
	if (IsValid(Cast<APlayerController>(GetController())))
	{
		RequestCancelRound();
	}
}

void ABaseVehiclePawn::InterpCameraToOriginalLocation() const
{
	if (BackCameraSpringArm)
	{
		const FRotator RelativeRotation = BackCameraSpringArm->GetRelativeRotation();
		const float WorldDeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

		const float Result = FMath::FInterpTo(RelativeRotation.Yaw, 0.0f, WorldDeltaSeconds, 1.0f);
		const FRotator NewRotation(0.0f, Result, 0.0f);
		BackCameraSpringArm->SetRelativeRotation(NewRotation);
	}
}

bool ABaseVehiclePawn::CanResetVehiclePosition() const
{
	// If the Movement Component says we're on ground we trust him
	if (const UChaosWheeledVehicleMovementComponent* MovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		if(MovementComponent->IsMovingOnGround())
		{
			UE_LOG(LogTrafficGameVehicle, Display, TEXT("MovementComponent->IsMovingOnGround() success, CanResetVehiclePosition true"));
			return true;
		}
	}

	// We are not on the ground (maybe over a cone?) do a Raycast to check the distance
	if(UWorld* World = GetWorld())
	{
		const FVector StartingPoint = GetActorLocation();
		const FVector EndingPoint = GetActorLocation() - FVector(0, 0, 200);
		constexpr ECollisionChannel TerrainChannel = ECollisionChannel::ECC_GameTraceChannel1; // Defined in Project Settings, deprecated?
		const FCollisionQueryParams CollisionQueryParams;

		FHitResult OutHitResult;
		const bool bResult = World->LineTraceSingleByChannel(OutHitResult, StartingPoint, EndingPoint, TerrainChannel);

#if UE_BUILD_DEBUG
		DrawDebugLine(World, StartingPoint, EndingPoint, FColor::Emerald, false, 10, 0, 10);
#endif

		if(bResult && IsValid(OutHitResult.GetActor()))
		{
			UE_LOG(LogTrafficGameVehicle, Display, TEXT("Raycast hit : %s"), *OutHitResult.GetActor()->GetName());
			UE_LOG(LogTrafficGameVehicle, Display, TEXT("Raycast hit distance : %f"), OutHitResult.Distance);

			constexpr float MaxAllowedDistance = 50.f; // Could be a UProp if we want to change it
			UE_LOG(LogTrafficGameVehicle, Display, TEXT("Checking MaxAllowedDistance for Raycast %f < %f"), OutHitResult.Distance, MaxAllowedDistance);
			return OutHitResult.Distance < MaxAllowedDistance;
		}
	}

	UE_LOG(LogTrafficGameVehicle, Display, TEXT("CanResetVehiclePosition returned false"));
	return false;
}

void ABaseVehiclePawn::ExecuteResetVehiclePosition()
{
	const FVector NewPosition = GetActorLocation() + FVector(0, 0, 50);

	const FRotator NewRotation(0.0f, GetActorRotation().Yaw, 0.0f);

	const FVector NewScale(1.0f);

	const FTransform NewTransform(NewRotation, NewPosition, NewScale);
	SetActorTransform(NewTransform, false, nullptr, ETeleportType::TeleportPhysics);

	if (USkeletalMeshComponent* VehicleSktMesh = GetMesh())
	{
		VehicleSktMesh->SetPhysicsAngularVelocityInDegrees(FVector::Zero());
		VehicleSktMesh->SetPhysicsLinearVelocity(FVector::Zero());
	}

	UE_LOG(LogTrafficGameVehicle, Display, TEXT("ResetVehiclePosition called for : %s"), *GetName());
}

void ABaseVehiclePawn::InitializeVehicle()
{
	// Call the function if present to reset the material
	// This avoid problems if the material is set different than the BP
	SetBrakeLightsEnabled(false);
}
