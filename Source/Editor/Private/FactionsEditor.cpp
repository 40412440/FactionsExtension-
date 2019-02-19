// Copyright 2015-2019 Piperift. All Rights Reserved.

#include "FactionsEditor.h"

#include "Kismet2/KismetEditorUtilities.h"

#include "FactionCustomization.h"
#include "FactionInfoCustomization.h"
#include "FactionRelationCustomization.h"
#include "FactionTableCustomization.h"
#include "RelationTableCustomization.h"

#include "Customizations/FactionPinFactory.h"


DEFINE_LOG_CATEGORY(LogFactionsEditor)

#define LOCTEXT_NAMESPACE "FactionsEditor"

void FFactionsEditorModule::StartupModule()
{
	UE_LOG(LogFactionsEditor, Log, TEXT("FactionsEditor: Log Started"));

	RegisterPropertyTypeCustomizations();
	PrepareAutoGeneratedDefaultEvents();
}

void FFactionsEditorModule::ShutdownModule()
{
	UE_LOG(LogFactionsEditor, Log, TEXT("FactionsEditor: Log Ended"));


	// Unregister all the asset types
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();

	// Cleanup all information for auto generated default event nodes by this module
	FKismetEditorUtilities::UnregisterAutoBlueprintNodeCreation(this);

	// Unregister all pin customizations
	for (auto& FactoryPtr : CreatedPinFactories)
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(FactoryPtr);
	}
	CreatedPinFactories.Empty();
}


void FFactionsEditorModule::RegisterPropertyTypeCustomizations()
{
	RegisterCustomPropertyTypeLayout("Faction",         FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFactionCustomization::MakeInstance));
	RegisterCustomPropertyTypeLayout("FactionInfo",     FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFactionInfoCustomization::MakeInstance));
	RegisterCustomPropertyTypeLayout("FactionRelation", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFactionRelationCustomization::MakeInstance));
	RegisterCustomPropertyTypeLayout("FactionTable",    FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFactionTableCustomization::MakeInstance));
	RegisterCustomPropertyTypeLayout("RelationTable",   FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRelationTableCustomization::MakeInstance));

	RegisterCustomPinFactory<FFactionPinFactory>();
}

void FFactionsEditorModule::PrepareAutoGeneratedDefaultEvents()
{}


void FFactionsEditorModule::RegisterCustomClassLayout(FName ClassName, FOnGetDetailCustomizationInstance DetailLayoutDelegate)
{
	check(ClassName != NAME_None);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomClassLayout(ClassName, DetailLayoutDelegate);
}

void FFactionsEditorModule::RegisterCustomPropertyTypeLayout(FName PropertyTypeName, FOnGetPropertyTypeCustomizationInstance PropertyTypeLayoutDelegate)
{
	check(PropertyTypeName != NAME_None);

	static FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor);
	PropertyModule.RegisterCustomPropertyTypeLayout(PropertyTypeName, PropertyTypeLayoutDelegate);
}

template<class T>
void FFactionsEditorModule::RegisterCustomPinFactory()
{
	TSharedPtr<T> PinFactory = MakeShareable(new T());
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);
	CreatedPinFactories.Add(PinFactory);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FFactionsEditorModule, FactionsEditor);