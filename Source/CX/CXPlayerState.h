#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CXPlayerState.generated.h"
UCLASS()
class CX_API ACXPlayerState : public APlayerState
{
    GENERATED_BODY()
public:
    UPROPERTY(Replicated, BlueprintReadOnly) int32 Coins = 0;
    UPROPERTY(Replicated, BlueprintReadOnly) float BonusMaxSpeed = 0.f;
    void AddCoins(int32 Amount);
    bool RemoveCoins(int32 Amount);
    void AddBonusMaxSpeed(float Amount);
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
