// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_EnhancedInputComponent.h"

#include "EWN_WidgetInputSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerInput.h"
#include "Interfaces/EWN_Interface_PlayerInputExtension.h"

void UEWN_EnhancedInputComponent::OnRegister()
{
	Super::OnRegister();

	if ( const auto* OwnerPC = GetOwner<APlayerController>() )
	{
		if ( const auto* LocalPlayer = Cast<ULocalPlayer>( OwnerPC->Player ) )
		{
			auto* WidgetInputSubsystem = LocalPlayer->GetSubsystem<UEWN_WidgetInputSubsystem>();
			check( WidgetInputSubsystem );

			WidgetInputSubsystem->InitPlayerInput( Cast<IEWN_Interface_PlayerInputExtension>( OwnerPC->PlayerInput ) );
		}
	}
}
