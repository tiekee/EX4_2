#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "CXPlayerState.h"
#include "CXGameState.h"
#include "CXGameMode.h"
#include "CXItem.h"
#include "CXItemSpawner.h"
#include "TimerManager.h"
#include "CXPawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCXRulesTest, "CX.Required.ServerRules", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCXRulesTest::RunTest(const FString&)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("CXRulesTestWorld"), nullptr, true, ERHIFeatureLevel::Num, nullptr, false);
    FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
    Context.SetCurrentWorld(World);
    World->InitializeActorsForPlay(FURL());
    ACXPlayerState* First = World->SpawnActor<ACXPlayerState>();
    ACXPlayerState* Second = World->SpawnActor<ACXPlayerState>();
    First->AddCoins(10);
    TestEqual(TEXT("Coin pickup grants ten"), First->Coins, 10);
    TestTrue(TEXT("Five coins can be stolen"), First->RemoveCoins(5));
    Second->AddCoins(5);
    TestEqual(TEXT("Victim loses five"), First->Coins, 5);
    TestEqual(TEXT("Attacker gains five"), Second->Coins, 5);
    First->RemoveCoins(5);
    TestFalse(TEXT("Insufficient coins cannot be stolen"), First->RemoveCoins(5));
    TestEqual(TEXT("Balance never becomes negative"), First->Coins, 0);
    First->AddCoins(-10);
    TestEqual(TEXT("Negative grant rejected"), First->Coins, 0);
    First->AddBonusMaxSpeed(10.f);
    First->AddBonusMaxSpeed(10.f);
    First->AddBonusMaxSpeed(-10.f);
    TestEqual(TEXT("Speed pickups stack by ten"), First->BonusMaxSpeed, 20.f);

    ACXGameMode* GM = World->SpawnActor<ACXGameMode>();
    ACXGameState* GS = World->SpawnActor<ACXGameState>();
    GM->GameState = GS;
    GS->PlayerArray = {First, Second};
    GS->RemainingTime = 1;
    GM->UpdateMatch();
    TestTrue(TEXT("Waiting transitions to Playing"), GS->MatchState == ECXMatchState::Playing);
    TestEqual(TEXT("Playing countdown starts at 120"), GS->RemainingTime, 120);
    First->AddCoins(5);
    GS->RemainingTime = 1;
    GM->UpdateMatch();
    TestTrue(TEXT("Playing transitions to Finished"), GS->MatchState == ECXMatchState::Finished);
    TestEqual(TEXT("Equal highest balances yield joint winners"), GS->Winners.Num(), 2);
    GS->MatchState = ECXMatchState::Playing;
    First->AddCoins(10);
    GS->RemainingTime = 1;
    GM->UpdateMatch();
    TestEqual(TEXT("Unequal balances yield one winner"), GS->Winners.Num(), 1);
    TestTrue(TEXT("Highest balance wins"), GS->Winners.Contains(First));
    ACXItem* Coin = World->SpawnActor<ACXItem>();
    TestTrue(TEXT("Coin is replicated"), Coin->GetIsReplicated());
    TestEqual(TEXT("Item update frequency is one"), Coin->GetNetUpdateFrequency(), 1.f);

    UClass* CarClass = LoadClass<ACXPawn>(nullptr, TEXT("/Game/VehicleTemplate/Blueprints/SportsCar/BP_SportsCar_Pawn.BP_SportsCar_Pawn_C"));
    ACXPawn* Car = World->SpawnActor<ACXPawn>(CarClass, FVector(0,0,200), FRotator::ZeroRotator);
    APlayerController* Driver = World->SpawnActor<APlayerController>();
    APlayerController* Attacker = World->SpawnActor<APlayerController>();
    Driver->SetPlayerState(First);
    Attacker->SetPlayerState(Second);
    Driver->Possess(Car);
    const int32 BeforeDamage = First->Coins;
    const int32 AttackerBefore = Second->Coins;
    UGameplayStatics::ApplyDamage(Car,5.f,Attacker,nullptr,nullptr);
    TestEqual(TEXT("Vehicle damage transfers five from victim"), First->Coins, BeforeDamage-5);
    TestEqual(TEXT("Vehicle damage transfers five to attacker"), Second->Coins, AttackerBefore+5);
    First->RemoveCoins(First->Coins);
    const int32 NoTransfer = Second->Coins;
    UGameplayStatics::ApplyDamage(Car,5.f,Attacker,nullptr,nullptr);
    TestEqual(TEXT("Vehicle with fewer than five transfers nothing"), Second->Coins, NoTransfer);
    Coin->DispatchBeginPlay();
    Coin->Collision->OnComponentBeginOverlap.Broadcast(Coin->Collision,Car,Car->GetMesh(),0,false,FHitResult());
    TestEqual(TEXT("Vehicle overlap awards exactly ten coins"), First->Coins, 10);
    TestTrue(TEXT("Collected coin is destroyed"), Coin->IsActorBeingDestroyed());
    ACXSpeedBuff* Buff = World->SpawnActor<ACXSpeedBuff>();
    Buff->DispatchBeginPlay();
    Buff->Collision->OnComponentBeginOverlap.Broadcast(Buff->Collision,Car,Car->GetMesh(),0,false,FHitResult());
    TestEqual(TEXT("Vehicle overlap adds ten max speed"), First->BonusMaxSpeed, 30.f);
    TestTrue(TEXT("Collected speed buff is destroyed"), Buff->IsActorBeingDestroyed());
    ACXItemSpawner* Spawner = World->SpawnActor<ACXItemSpawner>();
    Spawner->DispatchBeginPlay();
    TestTrue(TEXT("Spawner creates an item"), IsValid(Spawner->SpawnedItem));
    if (IsValid(Spawner->SpawnedItem))
    {
        Spawner->SpawnedItem->Destroy();
        TestNull(TEXT("Spawner observes item destruction"), Spawner->SpawnedItem.Get());
    }

    World->DestroyWorld(false);
    GEngine->DestroyWorldContext(World);
    return true;
}
#endif



