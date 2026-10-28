

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Subsystem/ChatSubsystem.h"
#include "CXPlayerController.generated.h"

class UInputMappingContext;
class ACXPawn;
class UCXUI;


UCLASS(abstract)
class CX_API ACXPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    void SendChat(const FString& Message) { CTSChatMessage(Message); }
protected:
    void OpenChat();
    UPROPERTY(EditDefaultsOnly, Category="UI") TSubclassOf<class UCXMatchUI> MatchUIClass;
    UPROPERTY() TObjectPtr<class UCXMatchUI> MatchUI;
    friend class UChatUI;
	friend class ACXGameMode;

	UFUNCTION(Client, Reliable)
	void STCChatLog(const TArray<FChatData>& ChatLog);

	UFUNCTION(Server, Reliable)
	void CTSChatMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void STCChatMessage(const FString& Sender, const FString& Message);

protected:

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bUseSteeringWheelControls = false;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(EditCondition="bUseSteeringWheelControls"))
	UInputMappingContext* SteeringWheelInputMappingContext;

	
	UPROPERTY(Replicated)
	TObjectPtr<ACXPawn> VehiclePawn;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<UCXUI> VehicleUIClass;

	
	TObjectPtr<UCXUI> VehicleUI;

	

	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:

	virtual void Tick(float Delta) override;

	

	
protected:

	virtual void OnPossess(APawn* InPawn) override;

	
};
