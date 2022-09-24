// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Engine/LocalPlayer.h"

//
#include "Interfaces/EWN_Interface_LocalPlayerExtension.h"

#include "EWN_LocalPlayer.generated.h"

/**
 * EnhancedWidgetNavigationを動作させるためのサンプルクラス。
 * プロジェクト側でLocalPlayerクラスを用意し、同様に実装することを推奨します。
 */

UCLASS()
class UEWN_LocalPlayer final : public ULocalPlayer, public IEWN_Interface_LocalPlayerExtension
{
	GENERATED_BODY()

protected:
	virtual bool SpawnPlayActor( const FString& URL, FString& OutError, UWorld* InWorld ) override;
};
