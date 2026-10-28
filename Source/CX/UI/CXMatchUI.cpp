#include "UI/CXMatchUI.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "CXPlayerState.h"
#include "CXGameState.h"
#include "CXPlayerController.h"
TSharedRef<SWidget> UCXMatchUI::RebuildWidget()
{
    if (!WidgetTree) WidgetTree = NewObject<UWidgetTree>(this);
    UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>();
    WidgetTree->RootWidget = Canvas;
    auto Panel = [&](FVector2D Position, FVector2D Size)
    {
        UBorder* Border = WidgetTree->ConstructWidget<UBorder>();
        Border->SetBrushColor(FLinearColor(.015f,.02f,.03f,.8f));
        Border->SetPadding(FMargin(12.f));
        UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Border);
        Slot->SetPosition(Position); Slot->SetSize(Size);
        UVerticalBox* Box = WidgetTree->ConstructWidget<UVerticalBox>();
        Border->SetContent(Box);
        return Box;
    };
    auto Label = [&](UVerticalBox* Box, const FString& Text, int32 Size)
    {
        UTextBlock* Block = WidgetTree->ConstructWidget<UTextBlock>();
        Block->SetText(FText::FromString(Text));
        FSlateFontInfo Font = Block->GetFont(); Font.Size = Size; Block->SetFont(Font);
        Box->AddChildToVerticalBox(Block); return Block;
    };
    UVerticalBox* Top = Panel(FVector2D(20,20),FVector2D(460,145));
    Label(Top,TEXT("CH4_2  COIN RACE"),24);
    Stats = Label(Top,TEXT("Coins: 0   Bonus Max Speed: +0 km/h"),18);
    State = Label(Top,TEXT("Waiting"),20);
    UVerticalBox* Chat = Panel(FVector2D(20,190),FVector2D(460,290));
    Label(Chat,TEXT("CHAT  |  Enter to type"),16);
    ChatLog = Label(Chat,TEXT(""),15);
    ChatLog->SetAutoWrapText(true);
    ChatInput = WidgetTree->ConstructWidget<UEditableTextBox>();
    ChatInput->SetForegroundColor(FLinearColor::Black);
    ChatInput->SetHintText(FText::FromString(TEXT("Message + Enter")));
    Chat->AddChildToVerticalBox(ChatInput);
    ChatInput->OnTextCommitted.AddUniqueDynamic(this,&UCXMatchUI::CommitChat);
    Label(Chat,TEXT("WASD Drive | Space Brake | LMB Attack"),12);
    Label(Chat,TEXT("R Reset | C Camera | Gold +10 | Blue +10 km/h"),12);
    return Super::RebuildWidget();
}
void UCXMatchUI::NativeTick(const FGeometry& Geometry, float Delta)
{
    Super::NativeTick(Geometry,Delta);
    APlayerController* PC = GetOwningPlayer();
    ACXPlayerState* PS = PC ? PC->GetPlayerState<ACXPlayerState>() : nullptr;
    ACXGameState* GS = GetWorld()->GetGameState<ACXGameState>();
    if (PS && Stats) Stats->SetText(FText::FromString(FString::Printf(TEXT("Coins: %d   Bonus Max Speed: +%.0f km/h"),PS->Coins,PS->BonusMaxSpeed)));
    if (!GS || !State) return;
    FString Status;
    if (GS->MatchState == ECXMatchState::Finished)
    {
        const bool bWinner = GS->Winners.Contains(PS);
        Status = bWinner ? (GS->Winners.Num()>1 ? TEXT("JOINT WINNER") : TEXT("YOU WIN")) : TEXT("YOU LOSE");
    }
    else Status = FString::Printf(TEXT("%s   %02d:%02d"),GS->MatchState == ECXMatchState::Waiting ? TEXT("Waiting") : TEXT("Playing"),GS->RemainingTime/60,GS->RemainingTime%60);
    State->SetText(FText::FromString(Status));
}
void UCXMatchUI::AddMessage(const FString& Sender,const FString& Message)
{
    Messages += Sender + TEXT(": ") + Message + TEXT("\n");
    TArray<FString> Lines; Messages.ParseIntoArrayLines(Lines);
    while (Lines.Num()>7) Lines.RemoveAt(0);
    Messages = FString::Join(Lines,TEXT("\n")) + TEXT("\n");
    if (ChatLog) ChatLog->SetText(FText::FromString(Messages));
}
void UCXMatchUI::FocusChat()
{
    if (!ChatInput) return;
    FInputModeGameAndUI Mode; Mode.SetWidgetToFocus(ChatInput->TakeWidget());
    GetOwningPlayer()->SetInputMode(Mode);
    GetOwningPlayer()->bShowMouseCursor = true;
    ChatInput->SetKeyboardFocus();
}
void UCXMatchUI::CommitChat(const FText& Text,ETextCommit::Type Method)
{
    if (Method != ETextCommit::OnEnter) return;
    if (ACXPlayerController* PC = GetOwningPlayer<ACXPlayerController>())
    {
        if (!Text.IsEmpty()) PC->SendChat(Text.ToString());
        ChatInput->SetText(FText::GetEmpty());
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }
}

