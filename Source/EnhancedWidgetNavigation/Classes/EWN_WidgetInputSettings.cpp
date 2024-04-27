// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetInputSettings.h"

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#endif

#include "InputMappingContext.h"

#if WITH_EDITOR
#define LOCTEXT_NAMESPACE "EWN_WidgetInputSettings"

FName UEWN_WidgetInputSettings::GetCategoryName() const
{
	return TEXT( "Plugins" );
}

FText UEWN_WidgetInputSettings::GetSectionText() const
{
	return LOCTEXT( "UEWN_WidgetInputSettings::GetSectionText", "EnhancedWidgetNavigation" );
}

#undef LOCTEXT_NAMESPACE
#endif

void UEWN_WidgetInputSettings::TryLoadObjects()
{
	if ( WidgetInputConfig )
	{
		WidgetInputConfig->RemoveFromRoot();
		WidgetInputConfig = nullptr;
	}

	if ( UObject* LoadedObject = WidgetInputConfigName.TryLoad() )
	{
		WidgetInputConfig = CastChecked<UEWN_WidgetInputConfig>( LoadedObject );
		WidgetInputConfig->AddToRoot();
	}
#if WITH_EDITOR
	else if ( GIsEditor )
	{
		FMessageLog AssetCheckLog( "AssetCheck" );

		const FText Message(
			NSLOCTEXT( "EWN", "WidgetInputConfigNotFound", "WidgetInputConfig not specified (or failed to load)." ) );
		AssetCheckLog.Error( Message );

		AssetCheckLog.Notify( Message, EMessageSeverity::Error, true );
	}
#endif
}

UInputMappingContext* UEWN_WidgetInputSettings::GetOptionalInputMappingContext() const
{
	return WidgetInputConfig ? WidgetInputConfig->IMC_Options : nullptr;
}

UInputMappingContext* UEWN_WidgetInputSettings::BuildInputMappingContext(
	const TFunctionRef<void( FName, UInputAction* )>& Callback ) const
{
	return WidgetInputConfig ? BuildInputMappingContext( WidgetInputConfig->InputMappingDefault, Callback ) : nullptr;
}

UInputMappingContext* UEWN_WidgetInputSettings::BuildInputMappingContext(
	const FEWN_WidgetInputMappingContainer& InjectionSettings, const TFunctionRef<void( FName, UInputAction* )>& Callback ) const
{
	auto* NewIMC = NewObject<UInputMappingContext>();

	for ( const FEWN_WidgetInputMapping& InputMapping : InjectionSettings.InputMappings )
	{
		if ( InputMapping.InputName.IsNone() )
		{
			UE_LOG( LogTemp, Warning, TEXT( "invalid key found in input mapping." ) );
			continue;
		}

		auto* NewInputAction = NewObject<UInputAction>( NewIMC );
		NewInputAction->bConsumeInput = false;
		Algo::Transform( InputMapping.Modifiers, NewInputAction->Modifiers,	   //
			[&]( const auto& Row ) { return DuplicateObject( Row, NewInputAction ); } );
		Algo::Transform( InputMapping.Triggers, NewInputAction->Triggers,	 //
			[&]( const auto& Row ) { return DuplicateObject( Row, NewInputAction ); } );

		for ( const FEWN_WidgetInputKeyMapping& KeyMapping : InputMapping.KeyMappings )
		{
			FEnhancedActionKeyMapping& NewMapping = NewIMC->MapKey( NewInputAction, KeyMapping.Key );
			Algo::Transform( KeyMapping.Modifiers, NewMapping.Modifiers,	//
				[&]( const auto& Row ) { return DuplicateObject( Row, NewIMC ); } );
			Algo::Transform( KeyMapping.Triggers, NewMapping.Triggers,	  //
				[&]( const auto& Row ) { return DuplicateObject( Row, NewIMC ); } );
		}

		switch ( EWN::Enum::FindValueByName<EEWN_WidgetInputType>( InputMapping.InputName ) )
		{
		case EEWN_WidgetInputType::Accept:
		{
			FEnhancedActionKeyMapping& NewMapping = NewIMC->MapKey( NewInputAction, EKeys::Virtual_Accept );
			NewMapping.Triggers.Emplace( NewObject<UInputTriggerPressed>() );
		}
		break;

		case EEWN_WidgetInputType::Back:
		{
			FEnhancedActionKeyMapping& NewMapping = NewIMC->MapKey( NewInputAction, EKeys::Virtual_Back );
			NewMapping.Triggers.Emplace( NewObject<UInputTriggerPressed>() );
		}
		break;

		default:;
		}

		Callback( InputMapping.InputName, NewInputAction );
	}

	return NewIMC;
}
