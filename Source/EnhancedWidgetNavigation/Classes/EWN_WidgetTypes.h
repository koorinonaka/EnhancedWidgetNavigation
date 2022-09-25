// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Components/Widget.h"

#include "EWN_WidgetTypes.generated.h"

UENUM( BlueprintType )
enum class EEWN_WidgetInputMode : uint8
{
	Gamepad,
	Mouse,
	Keyboard,
	Touch
};

UENUM()
enum class EEWN_WidgetCursor : uint8
{
	None,
	Up,
	Down,
	Left,
	Right,
};

UENUM()
enum class EEWN_WidgetInputType : uint8
{
	None,
	Up,
	Down,
	Left,
	Right,
	Prev,
	Next,
	Accept,
	Back,
};

USTRUCT( BlueprintType )
struct ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetInputKeyMapping
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, Category = "UI|Navigation" )
	FKey Key;

	UPROPERTY( EditAnywhere, Category = "UI|Navigation", Instanced, AdvancedDisplay )
	TArray<class UInputTrigger*> Triggers;

	UPROPERTY( EditAnywhere, Category = "UI|Navigation", Instanced, AdvancedDisplay )
	TArray<class UInputModifier*> Modifiers;
};

USTRUCT( BlueprintType )
struct ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetInputMapping
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, Category = "UI|Navigation" )
	EEWN_WidgetInputType InputType = EEWN_WidgetInputType::None;

	UPROPERTY( EditAnywhere, Category = "UI|Navigation", meta = ( TitleProperty = "Key" ) )
	TArray<FEWN_WidgetInputKeyMapping> KeyMappings;

	UPROPERTY( EditAnywhere, Category = "UI|Navigation", Instanced, AdvancedDisplay )
	TArray<class UInputTrigger*> Triggers;

	UPROPERTY( EditAnywhere, Category = "UI|Navigation", Instanced, AdvancedDisplay )
	TArray<class UInputModifier*> Modifiers;
};

USTRUCT( BlueprintType )
struct ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetInputMappingContainer
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, Category = "UI|Navigation", meta = ( TitleProperty = "InputType" ) )
	TArray<FEWN_WidgetInputMapping> InputMappings;
};
