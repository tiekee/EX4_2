#include "CXGameState.h"
#include "Net/UnrealNetwork.h"
void ACXGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACXGameState, MatchState);
    DOREPLIFETIME(ACXGameState, RemainingTime);
    DOREPLIFETIME(ACXGameState, Winners);
}
