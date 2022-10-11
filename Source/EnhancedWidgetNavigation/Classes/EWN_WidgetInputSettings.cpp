// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetInputSettings.h"

//
#include "InputMappingContext.h"

#if WITH_EDITOR
#include "IMessageLogListing.h"
#include "MessageLogModule.h"
#endif

template <typename T>
void DeepCopyPtrArray( const TArray<T*>& From, TArray<T*>& To )
{
	To.Empty( From.Num() );

	for ( T* ToDuplicate : From )
	{
		if ( ToDuplicate )
		{
			To.Add( DuplicateObject<T>( ToDuplicate, nullptr ) );
		}
	}
}

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

		FText Message( NSLOCTEXT( "EWN", "WidgetInputConfigNotFound", "WidgetInputConfig not specified (or failed to load)." ) );
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
	const TFunctionRef<void( FName, UInputAction* )> Callback ) const
{
	return WidgetInputConfig ? BuildInputMappingContext( WidgetInputConfig->InputMappingDefault, Callback ) : nullptr;
}

UInputMappingContext* UEWN_WidgetInputSettings::BuildInputMappingContext(
	const FEWN_WidgetInputMappingContainer& InjectionSettings, const TFunctionRef<void( FName, UInputAction* )> Callback ) const
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
		DeepCopyPtrArray<UInputModifier>( InputMapping.Modifiers, NewInputAction->Modifiers );
		DeepCopyPtrArray<UInputTrigger>( InputMapping.Triggers, NewInputAction->Triggers );

		for ( const FEWN_WidgetInputKeyMapping& KeyMapping : InputMapping.KeyMappings )
		{
			FEnhancedActionKeyMapping& NewMapping = NewIMC->MapKey( NewInputAction, KeyMapping.Key );
			DeepCopyPtrArray<UInputModifier>( KeyMapping.Modifiers, NewMapping.Modifiers );
			DeepCopyPtrArray<UInputTrigger>( KeyMapping.Triggers, NewMapping.Triggers );
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
		}

		Callback( InputMapping.InputName, NewInputAction );
	}

	return NewIMC;
}
