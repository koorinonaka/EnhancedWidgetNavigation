// Copyright 2022 koorinonaka, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EWN_WidgetTypes.h"
#include "Interfaces/EWN_Interface_WidgetNavigation.h"
#include "UObject/ObjectKey.h"

#include "EWN_WidgetNavigationConnector.generated.h"

UCLASS( Blueprintable )
class ENHANCEDWIDGETNAVIGATION_API UEWN_WidgetNavigationConnector : public UObject, public IEWN_Interface_WidgetNavigation
{
	GENERATED_BODY()

	struct FWidgetNavigationOverride
	{
		TObjectKey<UObject> NavigationObject;
		EEWN_WidgetCursor WidgetCursor = EEWN_WidgetCursor::None;

		FORCEINLINE bool operator==( const FWidgetNavigationOverride& Other ) const
		{
			return NavigationObject == Other.NavigationObject && WidgetCursor == Other.WidgetCursor;
		}

		friend uint32 GetTypeHash( const FWidgetNavigationOverride& Row )
		{
			return GetTypeHash( Row.NavigationObject ) ^ GetTypeHash( static_cast<uint8>( Row.WidgetCursor ) );
		}
	};

public:
	virtual EEWN_WidgetInputType TickNavigation( float DeltaTime ) override;

protected:
	virtual UWidget* GetCurrentWidget() const override;
	virtual bool TestFocus( EEWN_WidgetCursor WidgetCursor ) const override;
	virtual void ForEachWidgetNavigation( const TFunctionRef<void( UEWN_WidgetNavigation* )> Callback ) override;

	virtual void InvalidateNavigation() override;

public:
	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	int32 GetActiveIndex() const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	bool IsActive( TScriptInterface<IEWN_Interface_WidgetNavigation> INavigation ) const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	TScriptInterface<IEWN_Interface_WidgetNavigation> GetNavigationAt( int32 Index ) const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	TArray<TScriptInterface<IEWN_Interface_WidgetNavigation>> GetAllNavigations() const;

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void Register( TScriptInterface<IEWN_Interface_WidgetNavigation> INavigation );

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void Unregister( TScriptInterface<IEWN_Interface_WidgetNavigation> INavigation );

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void AddRoute( EEWN_WidgetCursor WidgetCursor,	  //
		TScriptInterface<IEWN_Interface_WidgetNavigation> Source, TScriptInterface<IEWN_Interface_WidgetNavigation> Destination );

	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void RemoveRoute( EEWN_WidgetCursor WidgetCursor,	 //
		TScriptInterface<IEWN_Interface_WidgetNavigation> Source, TScriptInterface<IEWN_Interface_WidgetNavigation> Destination );

public:
	UFUNCTION( BlueprintCallable, Category = "User Interface|Navigation" )
	void SetLoopNavigation( bool bNewFlag ) { bLoopNavigation = bNewFlag; }

private:
	bool MoveFocusOverride( IEWN_Interface_WidgetNavigation* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation );
	bool MoveFocusFallback( IEWN_Interface_WidgetNavigation* INavigation, EEWN_WidgetCursor WidgetCursor, bool bFromOperation );

	void OnNavigationUpdated( IEWN_Interface_WidgetNavigation* INavigation, int32 OldIndex, int32 NewIndex, bool bFromOperation );

private:
	UPROPERTY( Transient )
	TArray<TScriptInterface<IEWN_Interface_WidgetNavigation>> WidgetNavigations;

	int32 ActiveIndex = INDEX_NONE;
	TMultiMap<FWidgetNavigationOverride, TWeakObjectPtr<>> WidgetNavigationOverrides;

	bool bLoopNavigation = false;
};
