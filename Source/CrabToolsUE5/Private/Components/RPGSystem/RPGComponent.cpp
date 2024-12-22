#include "Components/RPGSystem/RPGComponent.h"
#include "UObject/UnrealTypePrivate.h"



#pragma region Component Code
URPGComponent::URPGComponent(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer) {
	this->bWantsInitializeComponent = true;
	this->bAutoActivate = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void URPGComponent::InitializeComponent()
{
	Super::InitializeComponent();

	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FStructProperty* f = *FIT;

		if (f->Struct == FIntAttribute::StaticStruct())
		{
			auto Value = f->ContainerPtrToValuePtr<FIntAttribute>(this);
			Value->SetOwner(this);
			Value->Initialize(this);
		}
		else if (f->Struct == FIntResource::StaticStruct())
		{
			auto Value = f->ContainerPtrToValuePtr<FIntResource>(this);
			Value->SetOwner(this);
			Value->Initialize(this);
		}
		else if (f->Struct == FFloatAttribute::StaticStruct())
		{
			auto Value = f->ContainerPtrToValuePtr<FFloatAttribute>(this);
			Value->SetOwner(this);
			Value->Initialize(this);
		}
		else if (f->Struct == FFloatResource::StaticStruct())
		{
			auto Value = f->ContainerPtrToValuePtr<FFloatResource>(this);
			Value->SetOwner(this);
			Value->Initialize(this);
		}
	}

	for (auto& Status : this->Statuses)
	{
		if (Status)
		{
			Status->Apply_Internal(this);
		}
	}
}

void URPGComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
	for (auto& Status : this->Statuses) {
		if (IsValid(Status)) {
			Status->Tick(DeltaTime);
		}
	}
}

void URPGComponent::Turn()
{
	for (auto& Status : this->Statuses)
	{
		if (IsValid(Status))
		{
			Status->Turn();
		}
	}
}

TArray<FString> URPGComponent::GetIntAttributeNames() const {
	TArray<FString> Names;
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		FStructProperty* f = *FIT;

		if (f->Struct == FIntAttribute::StaticStruct()) {
			Names.Add(f->GetFName().ToString());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> URPGComponent::GetFloatAttributeNames() const {
	TArray<FString> Names;
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		FStructProperty* f = *FIT;

		if (f->Struct == FFloatAttribute::StaticStruct()) {
			Names.Add(f->GetFName().ToString());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

#if WITH_EDITOR
void URPGComponent::PostEditChangeProperty(struct FPropertyChangedEvent& e) {
	Super::PostEditChangeProperty(e);
	this->Validate();
}
#endif

void URPGComponent::PostLoad() {
	Super::PostLoad();
	this->Validate();
}

void URPGComponent::Validate() {
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		FStructProperty* f = *FIT;

		if (f->Struct == FIntAttribute::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FIntAttribute>(this);
			Value->SetOwner(this);
		}
		else if (f->Struct == FIntResource::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FIntResource>(this);
			Value->SetOwner(this);
		}
		else if (f->Struct == FFloatAttribute::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FFloatAttribute>(this);
			Value->SetOwner(this);
		}
		else if (f->Struct == FFloatResource::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FFloatResource>(this);
			Value->SetOwner(this);
		}
	}
}

void URPGComponent::ApplyStatus(UStatus* Status) {
	this->Statuses.Add(Status);
	Status->Apply_Internal(this);
}

void URPGComponent::RemoveStatus(UStatus* Status) {
	this->Statuses.Remove(Status);
	Status->Remove_Internal();
}

UStatus* URPGComponent::GetStatus(TSubclassOf<UStatus> SClass, ESearchResult& Result) {
	for (auto& Status : this->Statuses) {
		if (Status->IsA(SClass.Get())) {
			Result = ESearchResult::Found;
			return Status;
		}
	}

	Result = ESearchResult::NotFound;
	return nullptr;
}

#pragma endregion

#pragma region Integer Attributes & Resources

void FIntAttribute::SetOwner(URPGComponent* UOwner) { 
	this->Owner = UOwner; 

	for (auto& Op : this->Operators) {
		if (Op) {
			Op->SetOwner(UOwner);
		}
	}
}

int FIntAttribute::GetValue() const {
	return this->CompValue;
}


void FIntAttribute::Operate(UIntOperator* Op) {
	Op->SetOwner(this->Owner);
	this->Operators.Add(Op);

	this->Refresh();
}

void FIntAttribute::UnOperate(UIntOperator* Op) {
	auto Removed = this->Operators.Remove(Op);

	if (Removed > 0) {
		Op->SetOwner(nullptr);
	}

	this->Refresh();
}

void FIntAttribute::Initialize(URPGComponent* UOwner) {
	this->SetOwner(UOwner);

	for (auto& Op : this->Operators) {
		if (Op) {
			Op->SetOwner(UOwner);
			Op->Initialize();
		}
	}	
	this->Refresh();
}

void FIntAttribute::Refresh() {
	// Sort operators as necessary.
	this->Operators.Sort([](const UIntOperator& A, const UIntOperator& B) { 
		return A.GetPriority() > B.GetPriority(); 
	});

	this->CompValue = this->BaseValue;
	for (auto& Op : this->Operators) {
		if (Op) {
			this->CompValue = Op->Operate(this->CompValue);
		}
	}

	this->ValueChangedEvent.Broadcast(this->CompValue);
}

void FIntAttribute::AddDependency(FIntResource* Dep) {
	if (Dep) {
		this->Dependencies.Add(Dep);
	}
}

void FIntResource::SetOwner(URPGComponent* UOwner) {
	this->Owner = UOwner;
}

void FIntResource::Initialize(URPGComponent* UOwner) {
	this->SetOwner(UOwner);
	FProperty* MaxProp = UOwner->GetClass()->FindPropertyByName(this->MaxAttribute);
	FProperty* MinProp = UOwner->GetClass()->FindPropertyByName(this->MinAttribute);

	if (FStructProperty* SProp = CastField<FStructProperty>(MaxProp)) {
		if (SProp->Struct == FIntAttribute::StaticStruct()) {
			this->MaxAttributeRef = SProp->ContainerPtrToValuePtr<FIntAttribute>(UOwner);

			if (this->MaxAttributeRef) {
				this->MaxAttributeRef->AddDependency(this);
			}
		}
	}

	if (FStructProperty* SProp = CastField<FStructProperty>(MinProp)) {
		if (SProp->Struct == FIntAttribute::StaticStruct()) {
			this->MinAttributeRef = SProp->ContainerPtrToValuePtr<FIntAttribute>(UOwner);

			if (this->MinAttributeRef) {
				this->MinAttributeRef->AddDependency(this);
			}
		}
	}
}

void FIntResource::SetValue(int UValue) {
	this->Value = UValue;
	this->Refresh();
}

void FIntResource::Refresh() {
	this->Value = FMath::Clamp(this->Value, this->GetMin(), this->GetMax());
	this->ValueChangedEvent.Broadcast(this->Value);
}

int FIntResource::GetMax() const {
	if (this->MaxAttributeRef) {
		return this->MaxAttributeRef->GetValue();
	}
	else {
		return MAX_int32;
	}
}

int FIntResource::GetMin() const {
	if (this->MinAttributeRef) {
		return this->MinAttributeRef->GetValue();
	}
	else {
		return MIN_int32;
	}
}

float FIntResource::GetPercent() const {
	int Min = this->GetMin();
	int Max = this->GetMax();
	int Diff = Max - Min;

	if (Diff == 0) {
		return 0.0;
	}
	else {
		float MinFloat = (float) Min;
		float MaxFloat = (float) Max;

		return FMath::Clamp(this->GetValue() / (MaxFloat - MinFloat), 0.0, 1.0);
	}

	
}

#pragma endregion

#pragma region Float Attributes & Resources


void FFloatAttribute::SetOwner(URPGComponent* UOwner) {
	this->Owner = UOwner;

	for (auto& Op : this->Operators) {
		if (Op) {
			Op->SetOwner(UOwner);
		}
	}
}

float FFloatAttribute::GetValue() const {
	return this->CompValue;
}


void FFloatAttribute::Operate(UFloatOperator* Op) {
	Op->SetOwner(this->Owner);
	this->Operators.Add(Op);

	this->Refresh();
}

void FFloatAttribute::UnOperate(UFloatOperator* Op) {
	auto Removed = this->Operators.Remove(Op);

	if (Removed > 0) {
		Op->SetOwner(nullptr);
	}

	this->Refresh();
}

void FFloatAttribute::Initialize(URPGComponent* UOwner) {
	this->SetOwner(UOwner);

	for (auto& Op : this->Operators) {
		if (Op) {
			Op->SetOwner(UOwner);
			Op->Initialize();
		}
	}

	this->Refresh();
}

void FFloatAttribute::Refresh() {
	// Sort operators as necessary.
	this->Operators.Sort([](const UFloatOperator& A, const UFloatOperator& B) {
		return A.GetPriority() > B.GetPriority();
		});

	this->CompValue = this->BaseValue;
	for (auto& Op : this->Operators) {
		if (Op) {
			this->CompValue = Op->Operate(this->CompValue);
		}
	}

	this->ValueChangedEvent.Broadcast(this->CompValue);
}

void FFloatAttribute::AddDependency(FFloatResource* Dep) {
	if (Dep) {
		this->Dependencies.Add(Dep);
	}
}

void FFloatResource::SetOwner(URPGComponent* UOwner) {
	this->Owner = UOwner;
}

void FFloatResource::Initialize(URPGComponent* UOwner) {
	this->SetOwner(UOwner);
	FProperty* MaxProp = UOwner->GetClass()->FindPropertyByName(this->MaxAttribute);
	FProperty* MinProp = UOwner->GetClass()->FindPropertyByName(this->MinAttribute);

	if (FStructProperty* SProp = CastField<FStructProperty>(MaxProp)) {
		if (SProp->Struct == FFloatAttribute::StaticStruct()) {
			this->MaxAttributeRef = SProp->ContainerPtrToValuePtr<FFloatAttribute>(UOwner);

			if (this->MaxAttributeRef) {
				this->MaxAttributeRef->AddDependency(this);
			}
		}
	}

	if (FStructProperty* SProp = CastField<FStructProperty>(MinProp)) {
		if (SProp->Struct == FFloatAttribute::StaticStruct()) {
			this->MinAttributeRef = SProp->ContainerPtrToValuePtr<FFloatAttribute>(UOwner);

			if (this->MinAttributeRef) {
				this->MinAttributeRef->AddDependency(this);
			}
		}
	}
}

void FFloatResource::SetValue(float UValue) {
	this->Value = UValue;
	this->Refresh();
}

void FFloatResource::Refresh() {
	this->Value = FMath::Clamp(this->Value, this->GetMin(), this->GetMax());
	this->ValueChangedEvent.Broadcast(this->Value);
}

float FFloatResource::GetMax() const {
	if (this->MaxAttributeRef) {
		return this->MaxAttributeRef->GetValue();
	}
	else {
		return std::numeric_limits<float>::infinity();
	}
}

float FFloatResource::GetMin() const {
	if (this->MinAttributeRef) {
		return this->MinAttributeRef->GetValue();
	}
	else {
		return -std::numeric_limits<float>::infinity();
	}
}

float FFloatResource::GetPercent() const {
	float Min = this->GetMin();
	float Max = this->GetMax();
	bool IsMinFinite = FMath::IsFinite(Min);
	bool IsMaxFinite = FMath::IsFinite(Max);

	if (IsMinFinite && IsMaxFinite) {
		return FMath::Clamp(this->GetValue() / (Max - Min), 0.0, 1.0);
	}
	else {
		return 0.0;
	}
}

#pragma endregion

#pragma region Statuses

void UStatus::Apply_Internal(URPGComponent* Comp) {
	this->Owner = Comp;
	this->Apply();
}

void UStatus::Remove_Internal() {	
	this->Remove();
	this->Owner = nullptr;
}

URPGComponent* UStatus::GetOwner() const
{
	if (this->Owner.IsValid())
	{
		return this->Owner.Get();
	}
	else
	{
		return nullptr;
	}
}

#pragma endregion
