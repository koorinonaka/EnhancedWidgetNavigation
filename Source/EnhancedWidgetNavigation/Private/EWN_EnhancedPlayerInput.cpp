// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_EnhancedPlayerInput.h"

void UEWN_EnhancedPlayerInput::ProcessInputStack(
	const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused )
{
	Super::ProcessInputStack( InputComponentStack, DeltaTime, bGamePaused );

	ProcessInputStackDelegate.Broadcast();
}
