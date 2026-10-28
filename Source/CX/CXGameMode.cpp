

#include "CXGameMode.h"
#include "CXGameState.h"
#include "CXPlayerState.h"
#include "TimerManager.h"
#include "CXPlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Subsystem/ChatSubsystem.h"

ACXGameMode::ACXGameMode()
{
	PlayerControllerClass = ACXPlayerController::StaticClass();
    PlayerStateClass = ACXPlayerState::StaticClass();
    GameStateClass = ACXGameState::StaticClass();
}

void ACXGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
    if (ACXGameState* GS = GetGameState<ACXGameState>())
    {
        if (GS->MatchState == ECXMatchState::Waiting && !GetWorldTimerManager().IsTimerActive(MatchTimer))
        {
            GS->RemainingTime = WaitingDuration;
            GetWorldTimerManager().SetTimer(MatchTimer, this, &ACXGameMode::UpdateMatch, 1.f, true);
        }
    }
	if (IsValid(NewPlayer) == false)
	{
		return;
	}

	if (ACXPlayerController* CXPlayerController = Cast<ACXPlayerController>(NewPlayer))
	{
		
		if (UChatSubsystem* ChatSubsystem = GetWorld()->GetSubsystem<UChatSubsystem>())
		{
			const TArray<FChatData> ChatLog = ChatSubsystem->GetChatLog();
			if (!ChatLog.IsEmpty())
			{
				CXPlayerController->STCChatLog(ChatLog);
			}
		}
	}
}

AActor* ACXGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<APlayerStart*> AvailablePlayerStarts;

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (IsPlayerStartAvailable(PlayerStart))
		{
			AvailablePlayerStarts.Add(PlayerStart);
		}
	}

	if (AvailablePlayerStarts.IsEmpty()) return Super::ChoosePlayerStart_Implementation(Player);

	const int32 RandomIndex = FMath::RandHelper(AvailablePlayerStarts.Num());
	return Cast<AActor>(AvailablePlayerStarts[RandomIndex]);
}

bool ACXGameMode::IsPlayerStartAvailable(const APlayerStart* PlayerStart) const
{
	if (IsValid(PlayerStart) == false || IsValid(GetWorld()) == false)
	{
		return false;
	}

	const FTransform PlayerStartTransform = PlayerStart->GetActorTransform();
	const FVector CheckLocation = PlayerStartTransform.TransformPosition(VehicleSpawnCheckOffset);
	const FQuat CheckRotation = PlayerStartTransform.GetRotation();
	const FCollisionShape CheckShape = FCollisionShape::MakeBox(VehicleSpawnCheckHalfExtent);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);

	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(VehiclePlayerStartCheck), false);

	const bool bVehicleExists =
		GetWorld()->OverlapAnyTestByObjectType(
			CheckLocation,
			CheckRotation,
			ObjectQueryParams,
			CheckShape,
			QueryParams
		);

	return !bVehicleExists;
}
void ACXGameMode::UpdateMatch()
{
    ACXGameState* GS = GetGameState<ACXGameState>();
    if (!GS) return;
    GS->RemainingTime = FMath::Max(0, GS->RemainingTime - 1);
    if (GS->RemainingTime > 0) return;
    if (GS->MatchState == ECXMatchState::Waiting)
    {
        GS->MatchState = ECXMatchState::Playing;
        GS->RemainingTime = PlayingDuration;
    }
    else if (GS->MatchState == ECXMatchState::Playing)
    {
        GS->Winners.Empty();
        int32 Highest = -1;
        for (APlayerState* Player : GS->PlayerArray)
        {
            ACXPlayerState* PS = Cast<ACXPlayerState>(Player);
            if (!PS || PS->IsOnlyASpectator()) continue;
            if (PS->Coins > Highest) { Highest = PS->Coins; GS->Winners.Empty(); }
            if (PS->Coins == Highest) GS->Winners.Add(PS);
        }
        GS->MatchState = ECXMatchState::Finished;
        GetWorldTimerManager().ClearTimer(MatchTimer);
    }
    GS->ForceNetUpdate();
}
