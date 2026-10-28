#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CXGameState.generated.h"
UENUM(BlueprintType)
enum class ECXMatchState : uint8 { Waiting, Playing, Finished };
UCLASS()
class CX_API ACXGameState : public AGameStateBase
{
    GENERATED_BODY()
public:
    UPROPERTY(Replicated, BlueprintReadOnly) ECXMatchState MatchState = ECXMatchState::Waiting;
    UPROPERTY(Replicated, BlueprintReadOnly) int32 RemainingTime = 10;
    UPROPERTY(Replicated, BlueprintReadOnly) TArray<APlayerState*> Winners;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
