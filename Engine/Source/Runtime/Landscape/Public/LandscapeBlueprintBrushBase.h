// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"

#include "LandscapeBlueprintBrushBase.generated.h"

UCLASS(Abstract, NotBlueprintable)
class LANDSCAPE_API ALandscapeBlueprintBrushBase : public AActor
{
	GENERATED_UCLASS_BODY()

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	class ALandscape* OwningLandscape;

	UPROPERTY(Category = "Settings", EditAnywhere)
	bool AffectHeightmap;

	UPROPERTY(Category = "Settings", EditAnywhere)
	bool AffectWeightmap;

	UPROPERTY(Category = "Settings", EditAnywhere)
	TArray<FName> AffectedWeightmapLayers;

	UPROPERTY(Transient)
	bool bIsVisible;

	uint32 LastRequestLayersContentUpdateFrameNumber;
#endif

public:
	UFUNCTION(BlueprintImplementableEvent)
	UTextureRenderTarget2D* Render(bool InIsHeightmap, UTextureRenderTarget2D* InCombinedResult, const FName& InWeightmapLayerName);

	UFUNCTION(BlueprintImplementableEvent)
	void Initialize(const FTransform& InLandscapeTransform, const FIntPoint& InLandscapeSize, const FIntPoint& InLandscapeRenderTargetSize);

	UFUNCTION(BlueprintCallable, Category = "Landscape")
	void RequestLandscapeUpdate();

#if WITH_EDITOR
	virtual void SetOwningLandscape(class ALandscape* InOwningLandscape);
	class ALandscape* GetOwningLandscape() const;

	bool IsAffectingHeightmap() const { return AffectHeightmap; }
	bool IsAffectingWeightmap() const { return AffectWeightmap; }
	bool IsAffectingWeightmapLayer(const FName& InLayerName) const;
	bool IsVisible() const { return bIsVisible; }
	
	void SetIsVisible(bool bInIsVisible);
	void SetAffectsHeightmap(bool bInAffectsHeightmap);
	void SetAffectsWeightmap(bool bInAffectsWeightmap);

	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void Destroyed() override;
#endif
};