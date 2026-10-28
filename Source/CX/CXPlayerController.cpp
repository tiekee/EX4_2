


#include "CXPlayerController.h"
#include "UI/CXMatchUI.h"
#include "CXPawn.h"
#include "CXUI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"
#include "Subsystem/ChatSubsystem.h"

void ACXPlayerController::STCChatLog_Implementation(const TArray<FChatData>& ChatLog)
{
	
	
	if (UChatSubsystem* ChatSubsystem = GetWorld()->GetSubsystem<UChatSubsystem>())
	{
		ChatSubsystem->SetChatLog(ChatLog);
        if (MatchUI)
        {
            for (const FChatData& Entry : ChatLog) MatchUI->AddMessage(Entry.Sender,Entry.Message);
            ChatSubsystem->ClearChatLog();
        }
	}
}

void ACXPlayerController::CTSChatMessage_Implementation(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	const FString CleanMessage = Message.TrimStartAndEnd().Left(200);
    if (!PlayerState || CleanMessage.IsEmpty()) return;
    const FString PlayerName = PlayerState->GetPlayerName();
	if (UChatSubsystem* ChatSubsystem = GetWorld()->GetSubsystem<UChatSubsystem>())
	{
		ChatSubsystem->AddChatMessage(PlayerName, CleanMessage);
	}

	for(TActorIterator<ACXPlayerController> It(GetWorld()); It; ++It)
	{
		if (ACXPlayerController* PC = *It)
		{
			PC->STCChatMessage(PlayerName, CleanMessage);
		}
	}
}

void ACXPlayerController::STCChatMessage_Implementation(const FString& Sender, const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *Sender, *Message);

	if (MatchUI) MatchUI->AddMessage(Sender, Message);
}

void ACXPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}
	
	MatchUI = CreateWidget<UCXMatchUI>(this, MatchUIClass ? MatchUIClass.Get() : UCXMatchUI::StaticClass());
    if (MatchUI) MatchUI->AddToViewport(5);
    SetInputMode(FInputModeGameOnly());
    
	VehicleUI = CreateWidget<UCXUI>(this, VehicleUIClass);

	check(VehicleUI);
	VehicleUI->AddToViewport();

	if (UChatSubsystem* ChatSubsystem = GetWorld()->GetSubsystem<UChatSubsystem>())
	{
		const TArray<FChatData>& ChatLog = ChatSubsystem->GetChatLog();

		for (const FChatData& ChatData : ChatLog)
		{
			if (MatchUI) MatchUI->AddMessage(ChatData.Sender, ChatData.Message);
		}

		ChatSubsystem->ClearChatLog();
	}
}

void ACXPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
    InputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &ACXPlayerController::OpenChat);
	
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		
		Subsystem->AddMappingContext(InputMappingContext, 0);

		
		if (bUseSteeringWheelControls && SteeringWheelInputMappingContext)
		{
			Subsystem->AddMappingContext(SteeringWheelInputMappingContext, 1);
		}
	}
}

void ACXPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, VehiclePawn);
}

void ACXPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
	}
}

void ACXPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	
	VehiclePawn = CastChecked<ACXPawn>(InPawn);
}

void ACXPlayerController::OpenChat() { if (MatchUI) MatchUI->FocusChat(); }

