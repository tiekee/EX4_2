#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CXMatchUI.generated.h"
class UTextBlock;
class UEditableTextBox;
UCLASS()
class CX_API UCXMatchUI : public UUserWidget
{
    GENERATED_BODY()
public:
    void AddMessage(const FString& Sender, const FString& Message);
    void FocusChat();
protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeTick(const FGeometry& Geometry, float DeltaTime) override;
    UFUNCTION() void CommitChat(const FText& Text, ETextCommit::Type Method);
    UPROPERTY() TObjectPtr<UTextBlock> Stats;
    UPROPERTY() TObjectPtr<UTextBlock> State;
    UPROPERTY() TObjectPtr<UTextBlock> ChatLog;
    UPROPERTY() TObjectPtr<UEditableTextBox> ChatInput;
    FString Messages;
};
