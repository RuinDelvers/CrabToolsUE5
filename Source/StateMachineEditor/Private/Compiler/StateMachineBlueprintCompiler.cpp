#include "Compiler/StateMachineBlueprintCompiler.h"
#include "Components/StateMachineComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "Blueprint/UserWidget.h"
#include "StateMachine/CompositeNode.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"

#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/KismetReinstanceUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Editor/WidgetCompilerLog.h"


#define LOCTEXT_NAMESPACE "StateMachineEditor"

#define CPF_Instanced (CPF_PersistentInstance | CPF_ExportObject | CPF_InstancedReference)

extern COREUOBJECT_API bool GMinimalCompileOnLoad;

class FStateMachineBlueprintNodeVerificationContext : public FNodeVerificationContext
{
private:

	FKismetCompilerContext& Context;

public:

	FStateMachineBlueprintNodeVerificationContext(
		UClass* GeneratedClass,
		FKismetCompilerContext& KismetContext)
	: FNodeVerificationContext(GeneratedClass), Context(KismetContext) {}

	virtual void Error(FString& Msg, const UObject* Obj) override
	{
		this->Context.MessageLog.Error(*Msg, Obj);
	}

	virtual void Warning(FString& Msg, const UObject* Obj) override
	{
		this->Context.MessageLog.Warning(*Msg, Obj);
	}
	
	virtual void Note(FString& Msg, const UObject* Obj) override
	{
		this->Context.MessageLog.Note(*Msg, Obj);
	}
};

//////////////////////////////////////////////////////////////////////////
// FStateMachineBlueprintCompiler::FCreateVariableContext
FStateMachineBlueprintCompilerContext::FCreateVariableContext::FCreateVariableContext(FStateMachineBlueprintCompilerContext& InContext)
	: Context(InContext)
{}

FProperty* FStateMachineBlueprintCompilerContext::FCreateVariableContext::CreateVariable(const FName Name, const FEdGraphPinType& Type) const
{
	return Context.CreateVariable(Name, Type);
}

void FStateMachineBlueprintCompilerContext::FCreateVariableContext::AddGeneratedFunctionGraph(UEdGraph* Graph) const
{
	Context.GeneratedFunctionGraphs.Add(Graph);
}

UStateMachineBlueprint* FStateMachineBlueprintCompilerContext::FCreateVariableContext::GetStateMachineBlueprint() const
{
	return Context.StateMachineBlueprint();
}

UStateMachineBlueprintGeneratedClass* FStateMachineBlueprintCompilerContext::FCreateVariableContext::GetGeneratedClass() const
{
	return Context.NewStateMachineBlueprintClass;
}


EKismetCompileType::Type FStateMachineBlueprintCompilerContext::FCreateVariableContext::GetCompileType() const
{
	return Context.CompileOptions.CompileType;
}


//////////////////////////////////////////////////////////////////////////
// FStateMachineBlueprintCompiler::FCreateFunctionContext
FStateMachineBlueprintCompilerContext::FCreateFunctionContext::FCreateFunctionContext(FStateMachineBlueprintCompilerContext& InContext)
	: Context(InContext)
{}

void FStateMachineBlueprintCompilerContext::FCreateFunctionContext::AddGeneratedFunctionGraph(UEdGraph* Graph) const
{
	Context.GeneratedFunctionGraphs.Add(Graph);
}

UStateMachineBlueprintGeneratedClass* FStateMachineBlueprintCompilerContext::FCreateFunctionContext::GetGeneratedClass() const
{
	return Context.NewStateMachineBlueprintClass;
}

//////////////////////////////////////////////////////////////////////////
// FStateMachineBlueprintCompiler
FStateMachineBlueprintCompiler::FStateMachineBlueprintCompiler()
	: //ReRegister(nullptr)
	 CompileCount(0)
{

}

bool FStateMachineBlueprintCompiler::CanCompile(const UBlueprint* Blueprint)
{
	return Cast<UStateMachineBlueprint>(Blueprint) != nullptr;
}

void FStateMachineBlueprintCompiler::PreCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions)
{
	if (CanCompile(Blueprint) && CompileOptions.CompileType == EKismetCompileType::Full)
	{

	}

	CompileCount++;
}

void FStateMachineBlueprintCompiler::Compile(UBlueprint* Blueprint, const FKismetCompilerOptions & CompileOptions, FCompilerResultsLog & Results)
{
	if (UStateMachineBlueprint* StateMachineBlueprint = CastChecked<UStateMachineBlueprint>(Blueprint))
	{
		FStateMachineBlueprintCompilerContext Compiler(StateMachineBlueprint, Results, CompileOptions);
		Compiler.Compile();
		check(Compiler.NewClass);
	}
}

void FStateMachineBlueprintCompiler::PostCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions)
{
	CompileCount--;

	if (CompileCount == 0)
	{
		if (GIsEditor && GEditor)
		{
			GEditor->RedrawAllViewports(true);
		}
	}
}

bool FStateMachineBlueprintCompiler::GetBlueprintTypesForClass(UClass* ParentClass, UClass*& OutBlueprintClass, UClass*& OutBlueprintGeneratedClass) const
{
	if (ParentClass == UStateMachine::StaticClass() || ParentClass->IsChildOf(UStateMachine::StaticClass()))
	{
		OutBlueprintClass = UStateMachineBlueprint::StaticClass();
		OutBlueprintGeneratedClass = UStateMachineBlueprintGeneratedClass::StaticClass();
		return true;
	}

	return false;
}

FStateMachineBlueprintCompilerContext::FStateMachineBlueprintCompilerContext(UStateMachineBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions)
	: Super(SourceSketch, InMessageLog, InCompilerOptions)
	, NewStateMachineBlueprintClass(nullptr)
{

}

FStateMachineBlueprintCompilerContext::~FStateMachineBlueprintCompilerContext()
{

}

void FStateMachineBlueprintCompilerContext::CreateFunctionList()
{
	Super::CreateFunctionList();
}

void FStateMachineBlueprintCompilerContext::ValidateStateMachineNames()
{
	UStateMachineBlueprint* WidgetBP = StateMachineBlueprint();

	TSharedPtr<FKismetNameValidator> ParentBPNameValidator;
	if ( WidgetBP->ParentClass != nullptr )
	{
		UBlueprint* ParentBP = Cast<UBlueprint>(WidgetBP->ParentClass->ClassGeneratedBy);
		if ( ParentBP != nullptr )
		{
			ParentBPNameValidator = MakeShareable(new FKismetNameValidator(ParentBP));
		}
	}
}

void FStateMachineBlueprintCompilerContext::CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOutOldCDO)
{
	Super::CleanAndSanitizeClass(ClassToClean, InOutOldCDO);

	if (auto BPGClass = Cast<UStateMachineBlueprintGeneratedClass>(ClassToClean))
	{
		BPGClass->CleanAndSanitize();
	}

	if (auto SMObj = Cast<UStateMachine>(InOutOldCDO))
	{
		//SMObj->StartState = NAME_None;
	}
}

void FStateMachineBlueprintCompilerContext::SaveSubObjectsFromCleanAndSanitizeClass(FSubobjectCollection& SubObjectsToSave, UBlueprintGeneratedClass* ClassToClean)
{
	Super::SaveSubObjectsFromCleanAndSanitizeClass(SubObjectsToSave, ClassToClean);

	// Make sure our typed pointer is set
	check(ClassToClean == NewClass);
	NewStateMachineBlueprintClass = CastChecked<UStateMachineBlueprintGeneratedClass>((UObject*)NewClass);

}

void FStateMachineBlueprintCompilerContext::CreateClassVariablesFromBlueprint()
{
	Super::CreateClassVariablesFromBlueprint();

	UStateMachineBlueprint* StateMachineBP = StateMachineBlueprint();
	if (StateMachineBP == nullptr)
	{
		return;
	}

	for (auto& SubGraph : StateMachineBP->GetSubgraphs())
	{
		if (SubGraph->IsVariable())
		{
			FEdGraphPinType PinType(
				UEdGraphSchema_K2::PC_Object,
				NAME_None,
				SubGraph->GetMachineArchetype()->GetClass(),
				EPinContainerType::None,
				false,
				FEdGraphTerminalType());

			FProperty* SMProp = CreateVariable(SubGraph->GetFName(), PinType);

			SMProp->SetMetaData(TEXT("Category"), SubGraph->GetCategoryName().ToString());

			SMProp->SetPropertyFlags(
				CPF_BlueprintVisible | 
				CPF_BlueprintReadOnly | 
				CPF_DisableEditOnInstance |
				CPF_Instanced |
				CPF_RepSkip);
		}
	}

	ValidateStateMachineNames();
}

void FStateMachineBlueprintCompilerContext::CopyTermDefaultsToDefaultObject(UObject* DefaultObject)
{
	Super::CopyTermDefaultsToDefaultObject(DefaultObject);

	if (this->bIsFullCompile)
	{
		if (auto StateMachine = Cast<UStateMachine>(DefaultObject))
		{
			if (auto SMBP = this->StateMachineBlueprint())
			{

			}
		}
	}	
}


void FStateMachineBlueprintCompilerContext::FinishCompilingClass(UClass* Class)
{
	if (Class == nullptr)
		return;

	UStateMachineBlueprint* StateMachineBP = StateMachineBlueprint();

	if (StateMachineBP == nullptr)
		return;

	UClass* ParentClass = StateMachineBP->ParentClass;

	if (ParentClass == nullptr)
		return;
	
	const bool bIsSkeletonOnly = CompileOptions.CompileType == EKismetCompileType::SkeletonOnly;

	UStateMachineBlueprintGeneratedClass* BPGClass = CastChecked<UStateMachineBlueprintGeneratedClass>(Class);

	if (BPGClass == nullptr)
		return;

	if (this->bIsFullCompile && !bIsSkeletonOnly)
	{
		if (auto SMBP = this->StateMachineBlueprint())
		{	
			BPGClass->Interfaces.Append(SMBP->GetInterfaces());

			FStateMachineBlueprintNodeVerificationContext Context(BPGClass, *this);

			SMBP->Verify(Context);
			BPGClass->AddStateMachine(SMBP->GetMainGraph()->CompileStateMachine(Context));

			for (auto& SubGraph : SMBP->GetSubgraphs())
			{
				auto SubSM = SubGraph->CompileStateMachine(Context);
				BPGClass->AddStateMachine(SubSM, SubGraph->GetGraphName());
				BPGClass->SubArchetypes[SubGraph->GetGraphName()].AppendEvents(SubGraph->DefinedEvents());
			}

			BPGClass->Archetype.AppendEvents(SMBP->GetMainGraph()->DefinedEvents());
		}
	}

	Super::FinishCompilingClass(Class);
}

void FStateMachineBlueprintCompilerContext::OnPostCDOCompiled(const UObject::FPostCDOCompiledContext& Context)
{
	Super::OnPostCDOCompiled(Context);

	if (Context.bIsSkeletonOnly)
	{
		return;
	}
}

void FStateMachineBlueprintCompilerContext::EnsureProperGeneratedClass(UClass*& TargetUClass)
{
	if ( TargetUClass && !( (UObject*)TargetUClass )->IsA(UStateMachineBlueprintGeneratedClass::StaticClass()) )
	{
		FKismetCompilerUtilities::ConsignToOblivion(TargetUClass, Blueprint->bIsRegeneratingOnLoad);
		TargetUClass = nullptr;
	}
}

void FStateMachineBlueprintCompilerContext::SpawnNewClass(const FString& NewClassName)
{
	NewStateMachineBlueprintClass = FindObject<UStateMachineBlueprintGeneratedClass>(Blueprint->GetOutermost(), *NewClassName);

	if ( NewStateMachineBlueprintClass == nullptr )
	{
		NewStateMachineBlueprintClass = NewObject<UStateMachineBlueprintGeneratedClass>(Blueprint->GetOutermost(), FName(*NewClassName), RF_Public | RF_Transactional);
	}
	else
	{
		// Already existed, but wasn't linked in the Blueprint yet due to load ordering issues
		FBlueprintCompileReinstancer::Create(NewStateMachineBlueprintClass);
	}
	NewClass = NewStateMachineBlueprintClass;
}

void FStateMachineBlueprintCompilerContext::OnNewClassSet(UBlueprintGeneratedClass* ClassToUse)
{
	NewStateMachineBlueprintClass = CastChecked<UStateMachineBlueprintGeneratedClass>(ClassToUse);
}

bool FStateMachineBlueprintCompilerContext::ValidateGeneratedClass(UBlueprintGeneratedClass* Class)
{
	const bool bSuperResult = Super::ValidateGeneratedClass(Class);

	return bSuperResult;
}

#undef LOCTEXT_NAMESPACE