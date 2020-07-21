// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AnimGraphNode_Base.h"
#include "Animation/AnimNode_SubInput.h"
#include "Engine/MemberReference.h"
#include "AnimGraphNode_SubInput.generated.h"

class SEditableTextBox;

/** Required info for reconstructing a manually specified pin */
USTRUCT()
struct FAnimBlueprintFunctionPinInfo
{
	GENERATED_BODY()

	FAnimBlueprintFunctionPinInfo()
		: Name(NAME_None)
	{
		Type.ResetToDefaults();
	}

	FAnimBlueprintFunctionPinInfo(const FName& InName, const FEdGraphPinType& InType)
		: Name(InName)
		, Type(InType)
	{
	}

	/** The name of this parameter */
	UPROPERTY(EditAnywhere, Category = "Inputs")
	FName Name;

	/** The type of this parameter */
	UPROPERTY(EditAnywhere, Category = "Inputs")
	FEdGraphPinType Type;
};

UCLASS()
class ANIMGRAPH_API UAnimGraphNode_SubInput : public UAnimGraphNode_Base
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Inputs")
	FAnimNode_SubInput Node;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	TArray<FAnimBlueprintFunctionPinInfo> Inputs;

	/** Reference to the stub function we use to build our parameters */
	UPROPERTY()
	FMemberReference FunctionReference;

	/** The index of the input pose, used alongside FunctionReference to build parameters */
	UPROPERTY()
	int32 InputPoseIndex;

	UAnimGraphNode_SubInput();

	/** UObject interface */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	/** UEdGraphNode interface */
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanUserDeleteNode() const override;
	virtual bool CanDuplicateNode() const override;
	virtual void AllocateDefaultPins() override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void PostPlacedNewNode() override;

	/** UK2Node interface */
	virtual bool HasExternalDependencies(TArray<UStruct*>* OptionalOutput) const override;

	/** UAnimGraphNode_Base interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	/** Make a name widget for this sub-input node */
	TSharedRef<SWidget> MakeNameWidget(IDetailLayoutBuilder& DetailBuilder);

	/** @return whether this node is editable. Non-editable nodes are implemented from function interfaces */
	bool IsEditable() const { return CanUserDeleteNode(); }

	/** @return the number of parameter inputs this node provides. This is either determined via the Inputs array or via the FunctionReference. */
	int32 GetNumInputs() const;

	/** Promotes the node from being a part of an interface override to a full function that allows for parameter and result pin additions */
	void PromoteFromInterfaceOverride();

	/** Conform input pose name according to function */
	void ConformInputPoseName();

	/** Validate pose index against the function reference (used to determine whether we should exist or not) */
	bool ValidateAgainstFunctionReference() const;

	/** Helper function for iterating stub function parameters */
	void IterateFunctionParameters(TFunctionRef<void(const FName&, const FEdGraphPinType&)> InFunc) const;

private:
	// Helper function for common code in AllocateDefaultPins and ReallocatePinsDuringReconstruction
	void AllocatePinsInternal();

	/** Create pins from the user-defined Inputs array */
	void CreateUserDefinedPins();

	/** Create pins from the stub function FunctionReference */
	void CreatePinsFromStubFunction(const UFunction* Function);

private:
	/** UI helper functions */
	void HandleInputPoseArrayChanged();
	void HandleInputPinArrayChanged();
};