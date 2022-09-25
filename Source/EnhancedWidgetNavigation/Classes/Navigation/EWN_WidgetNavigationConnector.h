// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//
#include "UObject/ObjectKey.h"

//
#include "EWN_WidgetTypes.h"
#include "Interfaces/EWN_Interface_WidgetNavigationConnection.h"

#include "EWN_WidgetNavigationConnector.generated.h"

UCLASS( Blueprintable )
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigationConnector : public UObject, public IEWN_Interface_WidgetNavigationConnection
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
	UFUNCTION( BlueprintCallable, Category = "UI|Navigation" )
	int32 GetActiveIndex() const;

	UFUNCTION( BlueprintCallable, Category = "UI|Navigation" )
	bool IsActive( TScriptInterface<IEWN_Interface_WidgetNavigationConnection> INavigation ) const;

	UFUNCTION( BlueprintCallable, Category = "UI|Navigation" )
	TScriptInterface<IEWN_Interface_WidgetNavigationConnection> GetNavigationAt( int32 Index ) const;

	UFUNCTION( BlueprintCallable, Category = "UI|Navigation" )
	TArray<TScriptInterface<IEWN_Interface_WidgetNavigationConnection>> GetAllNavigations() const;

	UFUNCTION( BlueprintCallable, Category = "UI|Navigation" )
	void Register( TScriptInterface<IEWN_Interface_WidgetNavigationConnection> INavigation );

	UFUNCTION( BlueprintCallable, Category = "UI|Navigation" )
	void Unregister( TScriptInterface<IEWN_Interface_WidgetNavigationConnection> INavigation );

	UFUNCTION( BlueprintCallable, Category = "UI|Navigation" )
	void AddRoute( EEWN_WidgetCursor WidgetCursor,	  //
		TScriptInterface<IEWN_Interface_WidgetNavigationConnection> Source,
		TScriptInterface<IEWN_Interface_WidgetNavigationConnection> Destination );

	UFUNCTION( BlueprintCallable, Category = "UI|Navigation" )
	void RemoveRoute( EEWN_WidgetCursor WidgetCursor,	 //
		TScriptInterface<IEWN_Interface_WidgetNavigationConnection> Source,
		TScriptInterface<IEWN_Interface_WidgetNavigationConnection> Destination );

public:
	UFUNCTION( BlueprintCallable, Category = "UI|Navigation" )
	void SetLoopNavigation( bool bNewFlag ) { bLoopNavigation = bNewFlag; }

private:
	bool MoveFocusOverride(
		IEWN_Interface_WidgetNavigationConnection* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation );
	bool MoveFocusFallback(
		IEWN_Interface_WidgetNavigationConnection* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation );

	void OnNavigationUpdated(
		IEWN_Interface_WidgetNavigationConnection* INavigation, int32 OldIndex, int32 NewIndex, bool bFromOperation );

private:
	UPROPERTY( Transient )
	TArray<TScriptInterface<IEWN_Interface_WidgetNavigationConnection>> WidgetNavigations;

	int32 ActiveIndex = INDEX_NONE;
	TMultiMap<FWidgetNavigationOverride, TWeakObjectPtr<UObject>> WidgetNavigationOverrides;

	bool bLoopNavigation = false;
};
