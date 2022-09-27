// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "Framework/Application/NavigationConfig.h"

//
#include "InputTriggers.h"

//
#include "EWN_WidgetTypes.h"
#include "Interfaces/EWN_Interface_WidgetNavigation.h"

#include "EWN_WidgetNavigation.generated.h"

namespace EWN::WidgetNavigation
{
class FCursorHandler;
}

UCLASS( Blueprintable )
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigation : public UObject, public IEWN_Interface_WidgetNavigation
{
	GENERATED_BODY()

	friend class EWN::WidgetNavigation::FCursorHandler;
	friend class UEWN_WidgetNavigationConnector;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams( FFocusDelegate, class UEWN_WidgetNavigation*, Navigation,	//
		int32, OldIndex, int32, NewIndex, bool, bFromOperation );
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FAcceptDelegate, class UEWN_WidgetNavigation*, Navigation,	//
		int32, Index );

protected:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

public:
	virtual EEWN_WidgetInputType TickNavigation( float DeltaTime ) override;

protected:
	virtual UWidget* GetCurrentWidget() const;
	virtual bool TestFocus( EEWN_WidgetCursor WidgetCursor ) const;
	virtual void ForEachWidgetNavigation( const TFunctionRef<void( UEWN_WidgetNavigation* )> Callback ) override;

	virtual void InvalidateNavigation() override;

public:
	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void SetInputMappingContext( const FEWN_WidgetInputMappingContainer& InjectionSettings );

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void ClearInputMappingContext();

public:
	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	bool WasJustTriggered( EEWN_WidgetInputType InputType ) const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	UWidget* GetChildAt( int32 Index ) const;

	template <class T>
	FORCEINLINE T* GetChildAt( int32 Index ) const
	{
		return Cast<T>( GetChildAt( Index ) );
	}

	int32 GetChildIndex( UWidget* Widget ) const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	int32 GetFocusIndex() const { return FocusIndex; }

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void SetFocusIndex( int32 NewIndex ) { UpdateFocusIndex( NewIndex, false ); }

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
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
	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void SetLoopNavigation( bool bNewFlag ) { bLoopNavigation = bNewFlag; }

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void SetIndependentNavigation( bool bNewFlag ) { bIndependentNavigation = bNewFlag; }

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void SetDistanceBasedNavigation( bool bNewFlag ) { bDistanceBasedNavigation = bNewFlag; }

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void SetWrapLines( bool bNewFlag ) { bWrapLines = bNewFlag; }

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void SetMouseEnabled( bool bNewFlag ) { bMouseEnabled = bNewFlag; }

public:
	UPROPERTY( BlueprintAssignable, Category = "User Interface|Navigation" )
	FFocusDelegate FocusUpdatedDelegate;

	UPROPERTY( BlueprintAssignable, Category = "User Interface|Navigation" )
	FAcceptDelegate FocusAcceptedDelegate;

protected:
	// navigation that loops horizontally and vertically.
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "User Interface|Navigation" )
	bool bLoopNavigation = false;

	// whether to disable navigation when trying to key focus.
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "User Interface|Navigation" )
	bool bIndependentNavigation = false;

	// navigation by distance based on each cell.
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "User Interface|Navigation" )
	bool bDistanceBasedNavigation = false;

	// whether to wrap navigation on line breaks such as grid panels.
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "User Interface|Navigation" )
	bool bWrapLines = false;

	//
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "User Interface|Navigation" )
	bool bMouseEnabled = true;

private:
	int32 FocusIndex = INDEX_NONE;
	int32 LastValidFocusIndex = INDEX_NONE;
	TWeakObjectPtr<class UEWN_WidgetNavigationInputMappingContext> NavigationIMC;
	TSharedPtr<class EWN::WidgetNavigation::FCursorHandler> CursorHandler;

	UPROPERTY( Transient )
	TObjectPtr<class UInputMappingContext> IMC_Navigation;

	UPROPERTY( Transient )
	TMap<EEWN_WidgetInputType, TObjectPtr<class UInputAction>> IA_NavigationActions;
};
