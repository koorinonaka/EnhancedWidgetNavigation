// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Framework/Application/NavigationConfig.h"

//
#include "InputTriggers.h"

//
#include "EWN_WidgetTypes.h"
#include "Interfaces/EWN_Interface_WidgetNavigationConnection.h"

#include "EWN_WidgetNavigation.generated.h"

UCLASS( Blueprintable )
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigation : public UObject, public IEWN_Interface_WidgetNavigationConnection
{
	GENERATED_BODY()

	friend class FEWN_WidgetNavigationCursorHandler;
	friend class UEWN_WidgetNavigationConnector;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams( FFocusDelegate, class UEWN_WidgetNavigation*, Navigation,	//
		int32, OldIndex, int32, NewIndex, bool, bFromOperation );
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FAcceptDelegate, class UEWN_WidgetNavigation*, Navigation,	//
		int32, Index );

protected:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

protected:
	virtual EEWN_WidgetInputType TickNavigation( float DeltaTime ) override;

	virtual UWidget* GetCurrentWidget() const;
	virtual bool TestFocus( EEWN_WidgetCursor WidgetCursor ) const;
	virtual void ForEachWidgetNavigation( const TFunctionRef<void( UEWN_WidgetNavigation* )> Callback ) override;

	virtual void InvalidateNavigation() override;

public:
	UFUNCTION( BlueprintCallable )
	void SetInputMappingContext( const FEWN_WidgetInputMappingContainer& InjectionSettings );

	UFUNCTION( BlueprintCallable )
	void ClearInputMappingContext();

public:
	UFUNCTION( BlueprintCallable )
	bool WasJustTriggered( EEWN_WidgetInputType InputType ) const;

	UFUNCTION( BlueprintCallable )
	UWidget* GetChildAt( int32 Index ) const;

	template <class T>
	FORCEINLINE T* GetChildAt( int32 Index ) const
	{
		return Cast<T>( GetChildAt( Index ) );
	}

	int32 GetChildIndex( UWidget* Widget ) const;

	UFUNCTION( BlueprintCallable )
	int32 GetFocusIndex() const { return FocusIndex; }

	UFUNCTION( BlueprintCallable )
	void SetFocusIndex( int32 NewIndex ) { UpdateFocusIndex( NewIndex, false ); }

	UFUNCTION( BlueprintCallable )
	void ResetNavigation( bool bResetIndex = false );

	void MoveWithMouseCursor();
	void ForEachFocusable( const TFunctionRef<void( int32, UWidget* )> Callback ) const;

protected:
	ETriggerEvent GetTriggerEvent( EEWN_WidgetInputType InputType ) const;

	int32 FindHoveredIndex() const;

	bool MoveFocus( EEWN_WidgetCursor WidgetCursor, bool bFromOperation );
	bool RestoreFocus( bool bFromOperation );

	bool FocusIncrement( bool bFromOperation );
	bool FocusDecrement( bool bFromOperation );

	void UpdateFocusIndex( int32 NewIndex, bool bFromOperation );

	virtual void NotifyFocusUpdated( int32 OldIndex, int32 NewIndex, bool bFromOperation );
	virtual void NotifyFocusAccepted( int32 Index );

public:
	UFUNCTION( BlueprintCallable )
	void SetLoopNavigation( bool bNewFlag ) { bLoopNavigation = bNewFlag; }

	UFUNCTION( BlueprintCallable )
	void SetIndependentNavigation( bool bNewFlag ) { bIndependentNavigation = bNewFlag; }

	UFUNCTION( BlueprintCallable )
	void SetDistanceBasedNavigation( bool bNewFlag ) { bDistanceBasedNavigation = bNewFlag; }

	UFUNCTION( BlueprintCallable )
	void SetWrapLines( bool bNewFlag ) { bWrapLines = bNewFlag; }

	UFUNCTION( BlueprintCallable )
	void SetMouseEnabled( bool bNewFlag ) { bMouseEnabled = bNewFlag; }

public:
	UPROPERTY( BlueprintAssignable )
	FFocusDelegate FocusUpdatedDelegate;

	UPROPERTY( BlueprintAssignable )
	FAcceptDelegate FocusAcceptedDelegate;

protected:
	// navigation that loops horizontally and vertically.
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	bool bLoopNavigation = false;

	// whether to disable navigation when trying to key focus.
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	bool bIndependentNavigation = false;

	// navigation by distance based on each cell.
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	bool bDistanceBasedNavigation = false;

	// whether to wrap navigation on line breaks such as grid panels.
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	bool bWrapLines = false;

	//
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	bool bMouseEnabled = true;

private:
	int32 FocusIndex = INDEX_NONE;
	int32 LastValidFocusIndex = INDEX_NONE;
	TWeakObjectPtr<class UEWN_WidgetNavigationInputMappingContext> NavigationIMC;
	TSharedPtr<class FEWN_WidgetNavigationCursorHandler> CursorHandler;

	UPROPERTY( Transient )
	TObjectPtr<class UInputMappingContext> IMC_Navigation;

	UPROPERTY( Transient )
	TMap<EEWN_WidgetInputType, TObjectPtr<class UInputAction>> IA_NavigationActions;
};
