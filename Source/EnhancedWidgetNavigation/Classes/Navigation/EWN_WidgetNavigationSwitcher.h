// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//
#include "EWN_WidgetTypes.h"
#include "Interfaces/EWN_Interface_WidgetNavigationSwitcher.h"

#include "EWN_WidgetNavigationSwitcher.generated.h"

UCLASS( Blueprintable )
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigationSwitcher : public UObject, public IEWN_Interface_WidgetNavigationSwitcher
{
	GENERATED_BODY()

	struct FWidgetNavigationOverride
	{
		TObjectKey<class UObject> NavigationObject;
		EEWN_WidgetCursor WidgetCursor = EEWN_WidgetCursor::None;

		FORCEINLINE bool operator==( const FWidgetNavigationOverride& Other ) const
		{
			return NavigationObject == Other.NavigationObject && WidgetCursor == Other.WidgetCursor;
		}

		friend uint32 GetTypeHash( const FWidgetNavigationOverride& Row )
		{
			return GetTypeHash( Row.NavigationObject ) ^ GetTypeHash( (uint8) Row.WidgetCursor );
		}
	};

protected:
	virtual EEWN_WidgetInputType TickNavigation( float DeltaTime ) override;

	virtual UWidget* GetCurrentWidget() const override;
	virtual bool TestFocus( EEWN_WidgetCursor WidgetCursor ) const override;
	virtual void ForEachWidgetNavigation( const TFunctionRef<void( class UEWN_WidgetNavigation* )> Callback ) override;

	virtual void InvalidateNavigation() override;

public:
	UFUNCTION( BlueprintCallable )
	int32 GetActiveIndex() const;

	UFUNCTION( BlueprintCallable )
	bool IsActive( TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> INavigation ) const;

	UFUNCTION( BlueprintCallable )
	TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> GetNavigationAt( int32 Index ) const;

	UFUNCTION( BlueprintCallable )
	TArray<TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher>> GetAllNavigations() const;

	UFUNCTION( BlueprintCallable )
	void Register( TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> INavigation );

	UFUNCTION( BlueprintCallable )
	void Unregister( TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> INavigation );

	UFUNCTION( BlueprintCallable )
	void AddRoute( EEWN_WidgetCursor WidgetCursor,	  //
		TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> Source,
		TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> Destination );

	UFUNCTION( BlueprintCallable )
	void RemoveRoute( EEWN_WidgetCursor WidgetCursor,	 //
		TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> Source,
		TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher> Destination );

public:
	UFUNCTION( BlueprintCallable )
	void SetLoopNavigation( bool bNewFlag ) { bLoopNavigation = bNewFlag; }

private:
	bool MoveFocusOverride(
		IEWN_Interface_WidgetNavigationSwitcher* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation );
	bool MoveFocusFallback(
		IEWN_Interface_WidgetNavigationSwitcher* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation );

	void OnNavigationUpdated(
		IEWN_Interface_WidgetNavigationSwitcher* INavigation, int32 OldIndex, int32 NewIndex, bool bFromOperation );

private:
	UPROPERTY( Transient )
	TArray<TScriptInterface<IEWN_Interface_WidgetNavigationSwitcher>> WidgetNavigations;

	int32 ActiveIndex = INDEX_NONE;
	TMultiMap<FWidgetNavigationOverride, TWeakObjectPtr<UObject>> WidgetNavigationOverrides;

	bool bLoopNavigation = false;
};
