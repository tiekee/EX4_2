#include "CXItemSpawner.h"
#include "CXItem.h"
#include "TimerManager.h"
ACXItemSpawner::ACXItemSpawner()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    ItemClass = ACXItem::StaticClass();
}
void ACXItemSpawner::BeginPlay() { Super::BeginPlay(); if (HasAuthority()) SpawnItem(); }
void ACXItemSpawner::SpawnItem()
{
    if (!HasAuthority() || !ItemClass || IsValid(SpawnedItem)) return;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnedItem = GetWorld()->SpawnActor<ACXItem>(ItemClass, GetActorTransform(), Params);
    if (SpawnedItem) SpawnedItem->OnDestroyed.AddDynamic(this, &ACXItemSpawner::OnItemDestroyed);
}
void ACXItemSpawner::OnItemDestroyed(AActor*)
{
    SpawnedItem = nullptr;
    if (HasAuthority()) GetWorldTimerManager().SetTimer(RespawnTimer, this, &ACXItemSpawner::SpawnItem, 2.f, false);
}
void ACXItemSpawner::EndPlay(const EEndPlayReason::Type Reason)
{
    GetWorldTimerManager().ClearTimer(RespawnTimer);
    if (IsValid(SpawnedItem)) SpawnedItem->OnDestroyed.RemoveDynamic(this, &ACXItemSpawner::OnItemDestroyed);
    Super::EndPlay(Reason);
}
