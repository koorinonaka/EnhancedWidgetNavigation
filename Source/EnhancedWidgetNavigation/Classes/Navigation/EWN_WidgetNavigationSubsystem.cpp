// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetNavigationSubsystem.h"

//
#include "Components/Widget.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/NavigationConfig.h"
#include "Framework/Application/SlateApplication.h"

//
#include "Navigation/EWN_WidgetNavigation.h"

class FEWN_NavigationConfig : public FNavigationConfig
{
public:
	FEWN_NavigationConfig() { bKeyNavigation = false; }

	virtual EUINavigationAction GetNavigationActionFromKey( const FKeyEvent& InKeyEvent ) const override
	{
		// 特定のキーがコード上に埋め込まれていて、入力を消費してしまうので上書き
		// FNavigationConfig::GetNavigationActionForKeyを参照
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
	if ( APlayerController* PC = GetLocalPlayerChecked()->GetPlayerController( nullptr ) )
	{
		EMouseCursor::Type CurrentMouseCursor = [&]() -> EMouseCursor::Type
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
		PC->CurrentMouseCursor = CurrentMouseCursor;
	}

	ActiveNavigationsOnLastFrame.Empty();
}

void UEWN_WidgetNavigationSubsystem::MarkOnThisFrame( UEWN_WidgetNavigation* Navigation )
{
	ActiveNavigationsOnLastFrame.Emplace( Navigation );
}

void UEWN_WidgetNavigationSubsystem::MenuConstruct()
{
	if ( ULocalPlayer* LocalPlayer = GetLocalPlayer() )
	{
		LocalPlayer->GetPlayerController( nullptr )->SetShowMouseCursor( ++MenuCounter > 0 );
	}
}

void UEWN_WidgetNavigationSubsystem::MenuDestruct()
{
	if ( ULocalPlayer* LocalPlayer = GetLocalPlayer() )
	{
		LocalPlayer->GetPlayerController( nullptr )->SetShowMouseCursor( --MenuCounter > 0 );
	}
}
