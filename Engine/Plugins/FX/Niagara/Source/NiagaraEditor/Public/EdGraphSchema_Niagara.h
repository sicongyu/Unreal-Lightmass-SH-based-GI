// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphSchema.h"
#include "ConnectionDrawingPolicy.h"
#include "EdGraphSchema_Niagara.generated.h"

class UEdGraph;
struct FNiagaraVariable;
struct FNiagaraVariableInfo;
struct FNiagaraTypeDefinition;
enum class ENiagaraDataType : uint8;

/** Action to add a node to the graph */
USTRUCT()
struct NIAGARAEDITOR_API FNiagaraSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	/** Template of node we want to create */
	UPROPERTY()
	class UEdGraphNode* NodeTemplate;

	UPROPERTY()
	FName InternalName;

	FNiagaraSchemaAction_NewNode() 
		: FEdGraphSchemaAction()
		, NodeTemplate(nullptr)
	{}

	FNiagaraSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FName InInternalName, FText InToolTip, const int32 InGrouping, FText InKeywords = FText())
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping, InKeywords)
		, NodeTemplate(nullptr), InternalName(InInternalName)
	{}

	//~ Begin FEdGraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
	//~ End FEdGraphSchemaAction Interface

	template <typename NodeType>
	static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location, bool bSelectNewNode = true)
	{
		FNiagaraSchemaAction_NewNode Action;
		Action.NodeTemplate = InTemplateNode;

		return Cast<NodeType>(Action.PerformAction(ParentGraph, NULL, Location, bSelectNewNode));
	}
};

UCLASS()
class NIAGARAEDITOR_API UEdGraphSchema_Niagara : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()

	// Allowable PinType.PinCategory values
	static const FName PinCategoryType;
	static const FName PinCategoryMisc;
	static const FName PinCategoryClass;
	static const FName PinCategoryEnum;

	//~ Begin EdGraphSchema Interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const override;
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
	virtual void ResetPinToAutogeneratedDefaultValue(UEdGraphPin* Pin, bool bCallModifyCallbacks = true) const override;
	virtual void OnPinConnectionDoubleCicked(UEdGraphPin* PinA, UEdGraphPin* PinB, const FVector2D& GraphPosition) const override; 
	virtual bool ShouldAlwaysPurgeOnModification() const override { return false; }
	//~ End EdGraphSchema Interface

	static FLinearColor GetTypeColor(const FNiagaraTypeDefinition& Type);

	TArray<TSharedPtr<FNiagaraSchemaAction_NewNode> > GetGraphContextActions(const UEdGraph* CurrentGraph, TArray<UObject*>& SelectedObjects, const UEdGraphPin* FromPin, UEdGraph* OwnerOfTemporaries) const;
	void PromoteSinglePinToParameter(UEdGraphPin* SourcePin);
	static bool CanPromoteSinglePinToParameter(const UEdGraphPin* SourcePin);
	void ToggleNodeEnabledState(class UNiagaraNode* InNode) const;
	void RefreshNode(UNiagaraNode* InNode) const;

	/** 
	  * Creates a niagara variable using the name, type, and default value stored on an ed graph pin.
	  * Pin The pin to create a variable from.
	  * bNeedsValue Whether or not the returned variable must be allocated to a valid value. When true if the pin doesn't have a valid default value itself the variable will be reset to default before return.
	  * returns The newly created variable.
	  */
	FNiagaraVariable PinToNiagaraVariable(const UEdGraphPin* Pin, bool bNeedsValue=false)const;

	/** 
	  * Tries to get a default value string for a graph pin from a niagara variable.
	  * @param Variable The variable to get a default pin value for.
	  * @param OutPinDefaultValue The pin default value string if this call was successful, otherwise empty.
	  * @returns Whether or not we could get a default value form the supplied variable.
	  */
	bool TryGetPinDefaultValueFromNiagaraVariable(const FNiagaraVariable& Variable, FString& OutPinDefaultValue) const;

	FNiagaraTypeDefinition PinToTypeDefinition(const UEdGraphPin* Pin)const;
	FEdGraphPinType TypeDefinitionToPinType(FNiagaraTypeDefinition TypeDef)const;
	
	bool IsSystemConstant(const FNiagaraVariable& Variable)const;

	class UNiagaraParameterCollection* VariableIsFromParameterCollection(const FNiagaraVariable& Var)const;
	class UNiagaraParameterCollection* VariableIsFromParameterCollection(const FString& VarName, bool bAllowPartialMatch, FNiagaraVariable& OutVar)const;
	

	FNiagaraTypeDefinition GetTypeDefForProperty(const UProperty* Property)const;

	static const FLinearColor NodeTitleColor_Attribute;
	static const FLinearColor NodeTitleColor_Constant;
	static const FLinearColor NodeTitleColor_SystemConstant;
	static const FLinearColor NodeTitleColor_FunctionCall;
	static const FLinearColor NodeTitleColor_CustomHlsl;
	static const FLinearColor NodeTitleColor_Event; 
	static const FLinearColor NodeTitleColor_TranslatorConstant;
	static const FLinearColor NodeTitleColor_RapidIteration;
	
private:
	void GetBreakLinkToSubMenuActions(class FMenuBuilder& MenuBuilder, UEdGraphPin* InGraphPin);
	void GetNumericConversionToSubMenuActions(class FMenuBuilder& MenuBuilder, UEdGraphPin* InGraphPin);
	void ConvertNumericPinToType(UEdGraphPin* InPin, FNiagaraTypeDefinition TypeDef);
	static bool CheckCircularConnection(const UEdGraphNode* InRootNode, const EEdGraphPinDirection InRootPinDirection, const UEdGraphPin* InPin, int32& OutDepth);
};

class FNiagaraConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
	FNiagaraConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraph);
	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params) override;

private:
	class UNiagaraGraph* Graph;
};
