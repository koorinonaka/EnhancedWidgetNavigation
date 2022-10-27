// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Components/Widget.h"

#include "EWN_WidgetTypes.generated.h"

UENUM( BlueprintType )
enum class EEWN_WidgetInputMethod : uint8
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

UENUM( BlueprintType )
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

	UPROPERTY( EditAnywhere, Category = "User Interface|Navigation" )
	FKey Key;

	UPROPERTY( EditAnywhere, Category = "User Interface|Navigation", Instanced, AdvancedDisplay )
	TArray<class UInputTrigger*> Triggers;

	UPROPERTY( EditAnywhere, Category = "User Interface|Navigation", Instanced, AdvancedDisplay )
	TArray<class UInputModifier*> Modifiers;
};

USTRUCT( BlueprintType )
struct ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetInputMapping
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, Category = "User Interface|Navigation" )
	FName InputName;

	UPROPERTY( EditAnywhere, Category = "User Interface|Navigation", meta = ( TitleProperty = "Key" ) )
	TArray<FEWN_WidgetInputKeyMapping> KeyMappings;

	UPROPERTY( EditAnywhere, Category = "User Interface|Navigation", Instanced, AdvancedDisplay )
	TArray<class UInputTrigger*> Triggers;

	UPROPERTY( EditAnywhere, Category = "User Interface|Navigation", Instanced, AdvancedDisplay )
	TArray<class UInputModifier*> Modifiers;
};

USTRUCT( BlueprintType )
struct ENHANCEDWIDGETNAVIGATION_API FEWN_WidgetInputMappingContainer
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, Category = "User Interface|Navigation", meta = ( TitleProperty = "InputName" ) )
	TArray<FEWN_WidgetInputMapping> InputMappings;
};

USTRUCT()
struct ENHANCEDWIDGETNAVIGATION_API FEWN_InputMappingOverrides
{
	GENERATED_BODY()

	UPROPERTY( Transient )
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY( Transient )
	TMap<FName, TObjectPtr<class UInputAction>> InputActions;
};
