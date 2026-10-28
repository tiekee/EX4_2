#include "CXItem.h"
#include "CXPlayerState.h"
#include "CXPawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"
ACXItem::ACXItem()
{
    bReplicates = true;
    NetUpdateFrequency = 1.f;
    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SetRootComponent(Collision);
    Collision->SetSphereRadius(110.f);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Shape(TEXT("/Engine/BasicShapes/Cylinder"));
    Mesh->SetStaticMesh(Shape.Object);
    Mesh->SetRelativeScale3D(FVector(.6f,.6f,.15f));
    Mesh->SetRelativeRotation(FRotator(90.f,0.f,0.f));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> Gold(TEXT("/Game/StarterContent/Materials/M_Metal_Gold"));
    Mesh->SetMaterial(0, Gold.Object);
    static ConstructorHelpers::FObjectFinder<UParticleSystem> FX(TEXT("/Game/StarterContent/Particles/P_Explosion"));
    static ConstructorHelpers::FObjectFinder<USoundBase> Sound(TEXT("/Game/StarterContent/Audio/Explosion01"));
    PickupEffect = FX.Object;
    PickupSound = Sound.Object;
}
void ACXItem::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority()) Collision->OnComponentBeginOverlap.AddDynamic(this, &ACXItem::OnOverlap);
}
void ACXItem::OnOverlap(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
    if (!HasAuthority() || bCollected) return;
    ACXPawn* Pawn = Cast<ACXPawn>(Other);
    ACXPlayerState* PS = Pawn ? Pawn->GetPlayerState<ACXPlayerState>() : nullptr;
    if (!PS) return;
    bCollected = true;
    GiveItem(PS);
    MulticastPickup();
    Destroy();
}
void ACXItem::GiveItem(ACXPlayerState* PS) { PS->AddCoins(10); }
void ACXItem::MulticastPickup_Implementation()
{
    if (GetNetMode() == NM_DedicatedServer) return;
    UGameplayStatics::SpawnEmitterAtLocation(this, PickupEffect, GetActorLocation(), FRotator::ZeroRotator, FVector(.25f));
    UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation(), .3f);
}
ACXSpeedBuff::ACXSpeedBuff()
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Shape(TEXT("/Engine/BasicShapes/Cone"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/StarterContent/Materials/M_Tech_Hex_Tile"));
    Mesh->SetStaticMesh(Shape.Object);
    Mesh->SetRelativeRotation(FRotator::ZeroRotator);
    Mesh->SetRelativeScale3D(FVector(.65f));
    Mesh->SetMaterial(0,Material.Object);
}
void ACXSpeedBuff::GiveItem(ACXPlayerState* PS) { PS->AddBonusMaxSpeed(10.f); }
