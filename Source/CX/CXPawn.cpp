

#include "CXPawn.h"
#include "CXPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "CXWheelFront.h"
#include "CXWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);

ACXPawn::ACXPawn()
{
	bReplicates = true;
    SetReplicateMovement(true);
    
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

}

void ACXPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AttackAction) EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ACXPawn::Attack);
        
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ACXPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ACXPawn::Steering);

		
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ACXPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ACXPawn::Throttle);

		
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ACXPawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ACXPawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ACXPawn::StopBrake);

		
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ACXPawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ACXPawn::StopHandbrake);

		
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ACXPawn::LookAround);

		
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ACXPawn::ToggleCamera);

		
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ACXPawn::ResetVehicle);
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACXPawn::Tick(float Delta)
{
	Super::Tick(Delta);
    if (HasAuthority() || IsLocallyControlled())
    {
        if (ACXPlayerState* PS = GetPlayerState<ACXPlayerState>())
        {
            const float MaxSpeed = (120.f + PS->BonusMaxSpeed) / .036f;
            FVector Velocity = GetMesh()->GetPhysicsLinearVelocity();
            const float Speed = FVector(Velocity.X,Velocity.Y,0.f).Size();
            if (Speed > MaxSpeed)
            {
                Velocity.X *= MaxSpeed / Speed;
                Velocity.Y *= MaxSpeed / Speed;
                GetMesh()->SetPhysicsLinearVelocity(Velocity);
            }
        }
    }

	
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
}

void ACXPawn::Steering(const FInputActionValue& Value)
{
	
	float SteeringValue = Value.Get<float>();

	
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void ACXPawn::Throttle(const FInputActionValue& Value)
{
	
	float ThrottleValue = Value.Get<float>();

	
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
}

void ACXPawn::Brake(const FInputActionValue& Value)
{
	
	float BreakValue = Value.Get<float>();

	
	ChaosVehicleMovement->SetBrakeInput(BreakValue);
}

void ACXPawn::StartBrake(const FInputActionValue& Value)
{
	
	BrakeLights(true);
}

void ACXPawn::StopBrake(const FInputActionValue& Value)
{
	
	BrakeLights(false);

	
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ACXPawn::StartHandbrake(const FInputActionValue& Value)
{
	
	ChaosVehicleMovement->SetHandbrakeInput(true);

	
	BrakeLights(true);
}

void ACXPawn::StopHandbrake(const FInputActionValue& Value)
{
	
	ChaosVehicleMovement->SetHandbrakeInput(false);

	
	BrakeLights(false);
}

void ACXPawn::LookAround(const FInputActionValue& Value)
{
	
	float LookValue = Value.Get<float>();

	
	BackSpringArm->AddLocalRotation(FRotator(0.0f, LookValue, 0.0f));
}

void ACXPawn::ToggleCamera(const FInputActionValue& Value)
{
	
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

void ACXPawn::ResetVehicle(const FInputActionValue& Value)
{
	
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;
	
	
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	UE_LOG(LogTemplateVehicle, Error, TEXT("Reset Vehicle"));
}

#undef LOCTEXT_NAMESPACE
void ACXPawn::Attack(const FInputActionValue&) { ServerAttack(); }
void ACXPawn::ServerAttack_Implementation()
{
    if (!Controller || GetWorld()->GetTimeSeconds() - LastAttackTime < .25f) return;
    LastAttackTime = GetWorld()->GetTimeSeconds();
    const FVector Start = GetActorLocation() + GetActorForwardVector() * 240.f + FVector(0.f,0.f,60.f);
    const FVector End = Start + GetActorForwardVector() * 800.f;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    FHitResult Hit;
    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    ACXPawn* Target = bHit ? Cast<ACXPawn>(Hit.GetActor()) : nullptr;
    if (Target) UGameplayStatics::ApplyDamage(Target, 5.f, Controller, this, nullptr);
    MulticastAttack(Start, bHit ? Hit.ImpactPoint : End, Target != nullptr);
}
float ACXPawn::TakeDamage(float Amount, const FDamageEvent& Event, AController* DamageInstigator, AActor* Causer)
{
    if (!HasAuthority() || !DamageInstigator || DamageInstigator == Controller) return 0.f;
    ACXPlayerState* Victim = GetPlayerState<ACXPlayerState>();
    ACXPlayerState* Attacker = DamageInstigator->GetPlayerState<ACXPlayerState>();
    if (!Victim || !Attacker || !Victim->RemoveCoins(5)) return 0.f;
    Attacker->AddCoins(5);
    return Super::TakeDamage(5.f, Event, DamageInstigator, Causer);
}
void ACXPawn::MulticastAttack_Implementation(const FVector& Start, const FVector& End, bool bHit)
{
    if (GetNetMode() == NM_DedicatedServer) return;
    DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false, 1.f, 0, 3.f);
    if (bHit)
    {
        UParticleSystem* Effect = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
        UGameplayStatics::SpawnEmitterAtLocation(this, Effect, End, FRotator::ZeroRotator, FVector(.3f));
    }
}

