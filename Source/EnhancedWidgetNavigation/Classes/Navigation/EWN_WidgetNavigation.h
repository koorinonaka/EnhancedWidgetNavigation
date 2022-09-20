// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Framework/Application/NavigationConfig.h"

//
#include "InputTriggers.h"

//
#include "EWN_WidgetTypes.h"
#include "Interfaces/EWN_Interface_WidgetNavigationChild.h"

#include "EWN_WidgetNavigation.generated.h"

UCLASS( Blueprintable )
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigation : public UObject
{
	GENERATED_BODY()

	friend class FEWN_WidgetNavigationCursorHandler;
	friend class UEWN_WidgetNavigationSwitcher;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams( FFocusDelegate, class UEWN_WidgetNavigation*, Navigation,	//
		int32, OldIndex, int32, NewIndex, bool, bFromOperation );
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FAcceptDelegate, class UEWN_WidgetNavigation*, Navigation,	//
		int32, Index );

	DECLARE_DELEGATE_RetVal_ThreeParams( bool, FMoveFocusDelegate, class UEWN_WidgetNavigation* Navigation,	   //
		EEWN_WidgetCursor WidgetCursor, bool bFromOperation );
	FMoveFocusDelegate OnMoveFocusOverride;
	FMoveFocusDelegate OnMoveFocusFallback;

protected:
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

public:
	UFUNCTION( BlueprintCallable )
	void SetInputMappingContext( const FEWN_WidgetInputMappingContainer& InjectionSettings );

	UFUNCTION( BlueprintCallable )
	void ClearInputMappingContext();

public:
	UFUNCTION( BlueprintCallable )
	EEWN_WidgetInputType TickNavigation( float DeltaTime );

	UFUNCTION( BlueprintCallable )
	bool WasJustTriggered( EEWN_WidgetInputType InputType ) const;

	UFUNCTION( BlueprintCallable )
	class UWidget* GetChildAt( int32 Index ) const;

	template <class T>
	FORCEINLINE T* GetChildAt( int32 Index ) const
	{
		return Cast<T>( GetChildAt( Index ) );
	}

	UFUNCTION( BlueprintCallable )
	int32 GetFocusIndex() const { return FocusIndex; }

	UFUNCTION( BlueprintCallable )
	void SetFocusIndex( int32 NewIndex ) { UpdateFocusIndex( NewIndex, false ); }

	UFUNCTION( BlueprintCallable )
	int32 FindHoveredIndex() const;

	void ForEachFocusable( const TFunctionRef<void( int32, UWidget* )> Callback ) const;

	UFUNCTION( BlueprintCallable )
	void ResetNavigation( bool bResetIndex = false );

protected:
	ETriggerEvent GetTriggerEvent( EEWN_WidgetInputType InputType ) const;

	bool MoveFocus( EEWN_WidgetCursor WidgetCursor, bool bFromOperation );
	bool RestoreFocus( bool bFromOperation );

	bool FocusIncrement( bool bFromOperation );
	bool FocusDecrement( bool bFromOperation );

	void UpdateFocusIndex( int32 NewIndex, bool bFromOperation );

	virtual void NotifyFocusUpdated( int32 OldIndex, int32 NewIndex, bool bFromOperation );
	virtual void NotifyFocusAccepted( int32 Index );

public:
	UFUNCTION( BlueprintCallable )
	bool IsLoopNavigation() const { return bLoopNavigation; }

	UFUNCTION( BlueprintCallable )
	void SetLoopNavigation( bool bNewFlag ) { bLoopNavigation = bNewFlag; }

	UFUNCTION( BlueprintCallable )
	bool IsDistanceBasedNavigation() const { return bDistanceBasedNavigation; }

	UFUNCTION( BlueprintCallable )
	void SetDistanceBasedNavigation( bool bNewFlag ) { bDistanceBasedNavigation = bNewFlag; }

	UFUNCTION( BlueprintCallable )
	bool IsWrapLines() const { return bWrapLines; }

	UFUNCTION( BlueprintCallable )
	void SetWrapLines( bool bNewFlag ) { bWrapLines = bNewFlag; }

	UFUNCTION( BlueprintCallable )
	bool IsMouseEnabled() const { return bMouseEnabled; }

	UFUNCTION( BlueprintCallable )
	void SetMouseEnabled( bool bNewFlag ) { bMouseEnabled = bNewFlag; }

	void SetMoveFocusOverride( FMoveFocusDelegate Delegate ) { OnMoveFocusOverride = Delegate; }
	void SetMoveFocusFallback( FMoveFocusDelegate Delegate ) { OnMoveFocusFallback = Delegate; }

public:
	UPROPERTY( BlueprintAssignable )
	FFocusDelegate OnFocusUpdatedDelegate;

	UPROPERTY( BlueprintAssignable )
	FAcceptDelegate OnFocusAcceptedDelegate;

protected:
	// navigation that loops horizontally and vertically.
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	bool bLoopNavigation = false;

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
