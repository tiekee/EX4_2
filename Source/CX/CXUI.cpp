


#include "CXUI.h"
#include "UI/ChatUI.h"

void UCXUI::OnNewChatMessage(const FString& InSender, const FString& InMessage)
{
	if (UI_Chat) UI_Chat->OnNewChatMessage(InSender, InMessage);
}

void UCXUI::UpdateSpeed(float NewSpeed)
{
	
	float FormattedSpeed = FMath::Abs(NewSpeed) * (bIsMPH ? 0.022f : 0.036f);

	
	OnSpeedUpdate(FormattedSpeed);
}

void UCXUI::UpdateGear(int32 NewGear)
{
	
	OnGearUpdate(NewGear);
}