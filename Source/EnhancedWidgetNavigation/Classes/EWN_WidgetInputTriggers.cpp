// Copyright 2022 koorinonaka, All Rights Reserved.

#include "EWN_WidgetInputTriggers.h"

//
#include "EnhancedPlayerInput.h"

ETriggerState UEWN_WidgetInputTriggerTimed::UpdateState_Implementation(
	const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime )
{
	ETriggerState State = ETriggerState::None;

	// Transition to Ongoing on actuation. Update the held duration.
	if ( IsActuated( ModifiedValue ) )
	{
		State = ETriggerState::Ongoing;
		HeldDuration = CalculateHeldDuration( PlayerInput, DeltaTime );
	}
	else
	{
		// Reset duration
		HeldDuration = 0.0f;
	}

	return State;
}

float UEWN_WidgetInputTriggerTimed::CalculateHeldDuration(
	const UEnhancedPlayerInput* const PlayerInput, const float DeltaTime ) const
{
	if ( ensureMsgf( PlayerInput && PlayerInput->GetOuterAPlayerController(),
			 TEXT( "No Player Input was given to Calculate with! Returning 1.0" ) ) )
	{
		const float TimeDilation = PlayerInput->GetOuterAPlayerController()->GetActorTimeDilation();

		// Calculates the new held duration, applying time dilation if desired
		return HeldDuration + ( !bAffectedByTimeDilation ? DeltaTime : DeltaTime * TimeDilation );
	}

	return 1.f;
}

FString UEWN_WidgetInputTriggerPressedAndPulse::GetDebugState() const
{
	if ( HeldDuration )
	{
		float Threshold = FirstDelay + ( Interval * TriggerCount );
		FString IntervalString = FString::Printf( TEXT( "Interval:%.2f/%.2f" ),	   //
			HeldDuration / Threshold, TriggerCount < 1 ? FirstDelay : Interval );
		return FString::Printf( TEXT( "Triggeres:%d, %s" ), TriggerCount, *IntervalString );
	}
	return FString();
}

ETriggerState UEWN_WidgetInputTriggerPressedAndPulse::UpdateState_Implementation(
	const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime )
{
	ETriggerState State = Super::UpdateState_Implementation( PlayerInput, ModifiedValue, DeltaTime );

	if ( State == ETriggerState::Ongoing )
	{
		if ( TriggerCount < 1 )
		{
			++TriggerCount;
			State = ETriggerState::Triggered;
		}
		else
		{
			float Threshold = FirstDelay + Interval * TriggerCount;
			if ( Threshold < HeldDuration )
			{
				++TriggerCount;
				State = ETriggerState::Triggered;
			}
		}
	}
	else
	{
		TriggerCount = 0;
	}

	return State;
}
