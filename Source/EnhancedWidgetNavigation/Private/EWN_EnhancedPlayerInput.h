// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "EnhancedPlayerInput.h"
#include "Interfaces/EWN_Interface_PlayerInputExtension.h"

#include "EWN_EnhancedPlayerInput.generated.h"

/**
 * Sample class for running EnhancedWidgetNavigation.
 * It is recommended that a class be prepared on the project side and implemented in the same way.
 */
UCLASS()
class UEWN_EnhancedPlayerInput final : public UEnhancedPlayerInput, public IEWN_Interface_PlayerInputExtension
{
	GENERATED_BODY()

protected:
	virtual void ProcessInputStack(
		const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused ) override;

	virtual TMap<uint32, FEWN_InputMappingOverrides>& GetInputMappingOverrides() override { return InputMappingOverrides; }

protected:
	UPROPERTY( Transient )
	TMap<uint32, FEWN_InputMappingOverrides> InputMappingOverrides;
};
