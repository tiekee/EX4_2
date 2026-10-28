#include "CXPlayerState.h"
#include "Net/UnrealNetwork.h"
void ACXPlayerState::AddCoins(int32 Amount)
{
    if (HasAuthority() && Amount > 0) Coins += Amount;
}
bool ACXPlayerState::RemoveCoins(int32 Amount)
{
    if (!HasAuthority() || Amount <= 0 || Coins < Amount) return false;
    Coins -= Amount;
    return true;
}
void ACXPlayerState::AddBonusMaxSpeed(float Amount)
{
    if (HasAuthority() && FMath::IsFinite(Amount) && Amount > 0.f) BonusMaxSpeed += Amount;
}
void ACXPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(ACXPlayerState, Coins, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(ACXPlayerState, BonusMaxSpeed, COND_OwnerOnly);
}
