#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CXItem.generated.h"
class USphereComponent;
class UStaticMeshComponent;
class UParticleSystem;
class USoundBase;
UCLASS()
class CX_API ACXItem : public AActor
{
    GENERATED_BODY()
public:
    ACXItem();
    UPROPERTY(VisibleAnywhere) TObjectPtr<USphereComponent> Collision;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY(EditDefaultsOnly) TObjectPtr<UParticleSystem> PickupEffect;
    UPROPERTY(EditDefaultsOnly) TObjectPtr<USoundBase> PickupSound;
protected:
    virtual void BeginPlay() override;
    virtual void GiveItem(class ACXPlayerState* PS);
    UFUNCTION() void OnOverlap(UPrimitiveComponent* Component, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool FromSweep, const FHitResult& Hit);
    UFUNCTION(NetMulticast, Reliable) void MulticastPickup();
    bool bCollected = false;
};
UCLASS()
class CX_API ACXSpeedBuff : public ACXItem
{
    GENERATED_BODY()
public:
    ACXSpeedBuff();
protected:
    virtual void GiveItem(class ACXPlayerState* PS) override;
};
