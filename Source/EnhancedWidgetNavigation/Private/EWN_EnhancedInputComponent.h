// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "EnhancedInputComponent.h"

#include "EWN_EnhancedInputComponent.generated.h"

/**
 * Sample class for running EnhancedWidgetNavigation.
 * It is recommended that a class be prepared on the project side and implemented in the same way.
 */
UCLASS()
class UEWN_EnhancedInputComponent final : public UEnhancedInputComponent
{
	GENERATED_BODY()

protected:
	virtual void OnRegister() override;
};
