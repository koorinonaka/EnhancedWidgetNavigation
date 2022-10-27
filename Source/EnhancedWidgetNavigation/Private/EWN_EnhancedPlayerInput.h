// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

//
#include "EnhancedPlayerInput.h"

//
#include "Interfaces/EWN_Interface_PlayerInputExtension.h"

#include "EWN_EnhancedPlayerInput.generated.h"

/**
 * EnhancedWidgetNavigationを動作させるためのサンプルクラス。
 * プロジェクト側でクラスを用意し、同様に実装することを推奨します。
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
