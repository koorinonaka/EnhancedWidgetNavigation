// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationSubsystem.h"

//
#include "Components/Widget.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/NavigationConfig.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"
#include "Widgets/SViewport.h"

//
#include "Blueprint/EWN_MenuWidget.h"
#include "EWN_WidgetInputTriggers.h"
#include "Navigation/EWN_WidgetNavigation.h"

class FEWN_NavigationConfig : public FNavigationConfig
{
public:
	FEWN_NavigationConfig() { bKeyNavigation = false; }

	virtual EUINavigationAction GetNavigationActionFromKey( const FKeyEvent& InKeyEvent ) const override
	{
		// override specific keys embedded in the code that would consume input.
		// FNavigationConfig::GetNavigationActionForKey
		return EUINavigationAction::Invalid;
	}
};

UEWN_WidgetNavigationSubsystem* UEWN_WidgetNavigationSubsystem::Get( UWidget* Widget )
{
	ULocalPlayer* OwningLP = Widget ? Widget->GetOwningLocalPlayer() : nullptr;
	return OwningLP ? OwningLP->GetSubsystem<ThisClass>() : nullptr;
}

void UEWN_WidgetNavigationSubsystem::Initialize( FSubsystemCollectionBase& Collection )
{
	Super::Initialize( Collection );

	FSlateApplication::Get().SetNavigationConfig( MakeShared<FEWN_NavigationConfig>() );
}

void UEWN_WidgetNavigationSubsystem::Deinitialize()
{
	FSlateApplication::Get().SetNavigationConfig( MakeShared<FNavigationConfig>() );

#if WITH_EDITOR
	// Fix editor cursor bug
	// https://qiita.com/Mitsunagi/items/0dfded740f3be414b514
	FSlateApplication::Get().UsePlatformCursorForCursorUser( true );
	FSlateApplication::Get().GetCursorUser()->SetCursorVisibility( true );
#endif

	Super::Deinitialize();
}

TStatId UEWN_WidgetNavigationSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT( ThisClass, STATGROUP_Tickables );
}

ETickableTickType UEWN_WidgetNavigationSubsystem::GetTickableTickType() const
{
	// The CDO of this should never tick
	return IsTemplate() ? ETickableTickType::Never : ETickableTickType::Always;
}

void UEWN_WidgetNavigationSubsystem::Tick( float DeltaTime )
{
	if ( MenuCounter > 0 )
	{
		ULocalPlayer* LocalPlayer = GetLocalPlayerChecked();

		if ( APlayerController* PC = LocalPlayer->GetPlayerController( nullptr ) )
		{
			PC->CurrentMouseCursor = [&]() -> EMouseCursor::Type
			{
				for ( TWeakObjectPtr<UEWN_WidgetNavigation> Navigation : ActiveNavigationsOnLastFrame )
				{
					if ( UWidget* FocusWidget = Navigation->GetChildAt( Navigation->GetFocusIndex() ) )
					{
#if EWN_UE_VERSION_OR_LATER( 5, 1 )
						return FocusWidget->GetCursor();
#else
						return FocusWidget->Cursor;
#endif
					}
				}
				return EMouseCursor::Default;
			}();
		}
	}

	ActiveNavigationsOnLastFrame.Empty();
}

void UEWN_WidgetNavigationSubsystem::MarkOnThisFrame( UEWN_WidgetNavigation* Navigation )
{
	ActiveNavigationsOnLastFrame.Emplace( Navigation );
}

void UEWN_WidgetNavigationSubsystem::MenuConstruct( UEWN_MenuWidget* MenuWidget )
{
	if ( ULocalPlayer* LocalPlayer = GetLocalPlayer() )
	{
		if ( MenuCounter == 0 )
		{
			SetFocusAndLocking( false );
		}

		LocalPlayer->GetPlayerController( nullptr )->SetShowMouseCursor( ++MenuCounter > 0 );
	}
}

void UEWN_WidgetNavigationSubsystem::MenuDestruct( UEWN_MenuWidget* MenuWidget )
{
	if ( ULocalPlayer* LocalPlayer = GetLocalPlayer() )
	{
		LocalPlayer->GetPlayerController( nullptr )->SetShowMouseCursor( --MenuCounter > 0 );

		if ( MenuCounter == 0 )
		{
			SetFocusAndLocking( true );
		}
	}
}

void UEWN_WidgetNavigationSubsystem::SetFocusAndLocking( bool bCaptureMouse )
{
	if ( ULocalPlayer* LocalPlayer = GetLocalPlayer() )
	{
		if ( TSharedPtr<SViewport> ViewportWidget = LocalPlayer->ViewportClient->GetGameViewportWidget() )
		{
			FReply& SlateOperations = LocalPlayer->GetSlateOperations();

			if ( bCaptureMouse )
			{
				SlateOperations.UseHighPrecisionMouseMovement( ViewportWidget.ToSharedRef() );
			}
			else
			{
				SlateOperations.ReleaseMouseCapture();
			}

			if ( bCaptureMouse ? LocalPlayer->ViewportClient->LockDuringCapture()
							   : LocalPlayer->ViewportClient->ShouldAlwaysLockMouse() )
			{
				SlateOperations.LockMouseToWidget( ViewportWidget.ToSharedRef() );
			}
			else
			{
				SlateOperations.ReleaseMouseLock();
			}
		}
	}
}
