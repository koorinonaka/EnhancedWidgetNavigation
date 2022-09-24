// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "EWN_MenuWidget.generated.h"

UCLASS( Abstract )
class ENHANCEDWIDGETNAVIGATION_API UEWN_MenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION( BlueprintCallable, BlueprintNativeEvent )
	void TickMenu( float DeltaTime );
	virtual void TickMenu_Implementation( float DeltaTime ) {}

protected:
	UFUNCTION( BlueprintCallable, BlueprintNativeEvent )
	void InitMenu();
	virtual void InitMenu_Implementation() {}
};
