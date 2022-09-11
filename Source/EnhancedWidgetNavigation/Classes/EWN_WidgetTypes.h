// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

	UPROPERTY( EditAnywhere )
	FKey Key;

	UPROPERTY( EditAnywhere, Instanced, AdvancedDisplay )
	TArray<class UInputTrigger*> Triggers;

	UPROPERTY( EditAnywhere, Instanced, AdvancedDisplay )
	TArray<class UInputModifier*> Modifiers;
};

USTRUCT( BlueprintType )
struct ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetInputMapping
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere )
	EEWN_WidgetInputType InputType = EEWN_WidgetInputType::None;

	UPROPERTY( EditAnywhere, meta = ( TitleProperty = "Key" ) )
	TArray<FEWN_WidgetInputKeyMapping> KeyMappings;

	UPROPERTY( EditAnywhere, Instanced, AdvancedDisplay )
	TArray<class UInputTrigger*> Triggers;

	UPROPERTY( EditAnywhere, Instanced, AdvancedDisplay )
	TArray<class UInputModifier*> Modifiers;
};

USTRUCT( BlueprintType )
struct ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetInputMappingContainer
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, meta = ( TitleProperty = "InputType" ) )
	TArray<FEWN_WidgetInputMapping> InputMappings;
};
