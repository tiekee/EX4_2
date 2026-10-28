#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CXItemSpawner.generated.h"
class ACXItem;
UCLASS()
class CX_API ACXItemSpawner : public AActor
{
    GENERATED_BODY()
public:
    ACXItemSpawner();
    UPROPERTY(EditAnywhere, Category="Item") TSubclassOf<ACXItem> ItemClass;
    UPROPERTY(VisibleInstanceOnly, Category="Item") TObjectPtr<ACXItem> SpawnedItem;
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    void SpawnItem();
    UFUNCTION() void OnItemDestroyed(AActor* Item);
    FTimerHandle RespawnTimer;
};
