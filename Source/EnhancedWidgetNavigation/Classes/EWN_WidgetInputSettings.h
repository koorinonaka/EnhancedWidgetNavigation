// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"

//
#include "Engine/DataAsset.h"

//
#include "EWN_WidgetTypes.h"

#include "EWN_WidgetInputSettings.generated.h"

UCLASS()
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetInputConfig : public UDataAsset
{
	GENERATED_BODY()

	friend class UEWN_WidgetInputSettings;

protected:
	UPROPERTY( EditDefaultsOnly, Category = "User Interface|Navigation" )
	TObjectPtr<class UInputMappingContext> IMC_Options;

	UPROPERTY( EditDefaultsOnly, Category = "User Interface|Navigation" )
	FEWN_WidgetInputMappingContainer InputMappingDefault;
};

UCLASS( config = Game, defaultconfig )
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetInputSettings : public UDeveloperSettings
{
	GENERATED_BODY()

protected:
#if WITH_EDITOR
	virtual FName GetCategoryName() const override;
	virtual FText GetSectionText() const override;
#endif

public:
	class UInputMappingContext* GetOptionalInputMappingContext() const;

	class UInputMappingContext* BuildInputMappingContext(
		const TFunctionRef<void( EEWN_WidgetInputType, class UInputAction* )> Callback ) const;
	class UInputMappingContext* BuildInputMappingContext( const FEWN_WidgetInputMappingContainer& InjectionSettings,
		const TFunctionRef<void( EEWN_WidgetInputType, class UInputAction* )> Callback ) const;

public:
	virtual void TryLoadObjects();

protected:
	UPROPERTY(
		Config, EditDefaultsOnly, Category = "User Interface|Navigation", meta = ( AllowedClasses = "EWN_WidgetInputConfig" ) )
	FSoftObjectPath WidgetInputConfigName;

private:
	UPROPERTY( Transient )
	TObjectPtr<UEWN_WidgetInputConfig> WidgetInputConfig;
};
