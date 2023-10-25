// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "EWN_WidgetTypes.h"

#include "EWN_Interface_PlayerInputExtension.generated.h"

UINTERFACE( meta = ( CannotImplementInterfaceInBlueprint ) )
class ENHANCEDWIDGETNAVIGATION_API UEWN_Interface_PlayerInputExtension : public UInterface
{
	GENERATED_BODY()
};

class ENHANCEDWIDGETNAVIGATION_API IEWN_Interface_PlayerInputExtension
{
	GENERATED_BODY()

	friend class UEWN_WidgetInputSubsystem;
	virtual TMap<uint32, FEWN_InputMappingOverrides>& GetInputMappingOverrides() = 0;

public:
	FSimpleMulticastDelegate ProcessInputStackDelegate;
};
