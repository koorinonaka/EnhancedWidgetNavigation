// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputTriggers.h"

#include "EWN_WidgetInputTriggers.generated.h"

// UInputTriggerTimedBaseが非公開なのでコピペ
UCLASS( Abstract, MinimalAPI, Config = Input )
class UEWN_WidgetInputTriggerTimed : public UInputTrigger
{
	GENERATED_BODY()

protected:
	float HeldDuration = 0.f;

	virtual ETriggerState UpdateState_Implementation(
		const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime ) override;

	float CalculateHeldDuration( const UEnhancedPlayerInput* const PlayerInput, const float DeltaTime ) const;

public:
	virtual ETriggerEventsSupported GetSupportedTriggerEvents() const override { return ETriggerEventsSupported::Ongoing; }

	UPROPERTY( EditAnywhere, Config, BlueprintReadWrite, Category = "Trigger Settings" )
	bool bAffectedByTimeDilation = false;

	virtual FString GetDebugState() const override
	{
		return HeldDuration ? FString::Printf( TEXT( "Held:%.2f" ), HeldDuration ) : FString();
	}
};

UCLASS( NotBlueprintable, MinimalAPI, meta = ( DisplayName = "Pressed And Pulse" ) )
class UEWN_WidgetInputTriggerPressedAndPulse final : public UEWN_WidgetInputTriggerTimed
{
	GENERATED_BODY()

protected:
	virtual ETriggerState UpdateState_Implementation(
		const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime ) override;
	virtual FString GetDebugState() const override;

protected:
	UPROPERTY( EditDefaultsOnly )
	float FirstDelay;

	UPROPERTY( EditDefaultsOnly )
	float Interval = 1.f;

private:
	int32 TriggerCount = 0;
};
