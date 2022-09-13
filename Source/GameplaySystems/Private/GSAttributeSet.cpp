// Fill out your copyright notice in the Description page of Project Settings.


#include "GSAttributeSet.h"

UGSAttributeSet::UGSAttributeSet()
{
	
}

void UGSAttributeSet::Initialize(UGSActionComponent* NewOwner)
{
	OwningComp = NewOwner;

	K2_Initialize();
}

bool UGSAttributeSet::GetAttribute(const FGameplayTag& InTag, FGSAttribute& OutAttribute) const
{
	FName AttributeName;
	if (GetAttributeName(InTag, AttributeName))
	{
		return GetAttribute(AttributeName, OutAttribute);
	}
	return false;
}

bool UGSAttributeSet::GetAttribute(const FName& AttributeName, FGSAttribute& OutAttribute) const
{
	FStructProperty* Property = CastField<FStructProperty>(GetClass()->FindPropertyByName(AttributeName));
	if (Property)
	{
		const FGSAttribute* FoundAttribute = Property->ContainerPtrToValuePtr<FGSAttribute>(this);
		OutAttribute = *FoundAttribute;
		return true;
	}
	return false;
}

float UGSAttributeSet::ApplyAttributeChange(const FGSAttributeModification& ModToApply)
{
	FName AttributeName;
	if (GetAttributeName(ModToApply.AttributeTag, AttributeName))
	{
		FStructProperty* Property = CastField<FStructProperty>(GetClass()->FindPropertyByName(AttributeName));
		if (Property)
		{
			FGSAttribute* FoundAttribute = Property->ContainerPtrToValuePtr<FGSAttribute>(this);
			const float OldValue = FoundAttribute->GetValue();
			const float Delta = ModToApply.GetMagnitude();

			switch (ModToApply.ModifierOperation)
			{
			case EGSAttributeModifierOperation::AddDelta:
				FoundAttribute->Delta += Delta;
				if (FoundAttribute->bClampToZero)
				{
					FoundAttribute->Delta = FMath::Max(FoundAttribute->Delta, 0.0f);
				}
				break;
			case EGSAttributeModifierOperation::AddBase:
				FoundAttribute->BaseValue += Delta;
				if (FoundAttribute->bClampToZero)
				{
					FoundAttribute->BaseValue = FMath::Max(FoundAttribute->BaseValue, 0.0f);
				}
				break;
			case EGSAttributeModifierOperation::AddMultiplier:
				FoundAttribute->Multiplier += Delta;
				if (FoundAttribute->bClampToZero)
				{
					FoundAttribute->Multiplier = FMath::Max(FoundAttribute->Multiplier, 0.0f);
				}
				break;
			case EGSAttributeModifierOperation::OverrideBase:
				FoundAttribute->BaseValue = Delta;
				if (FoundAttribute->bClampToZero)
				{
					FoundAttribute->BaseValue = FMath::Max(FoundAttribute->BaseValue, 0.0f);
				}
				break;
			}

			PostAttributeChange(ModToApply);
			
			GetOwningComponent()->BroadcastAttributeChanged(ModToApply.AttributeTag, ModToApply.InstigatorComponent.Get(), FoundAttribute->GetValue(), Delta, ModToApply.AddedTags, ModToApply.ModifierOperation);
			
			return Delta;
		}
	}

	return 0.f;
}

UWorld* UGSAttributeSet::GetWorld() const
{
	AActor* Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}

	return nullptr;
}

void UGSAttributeSet::PostAttributeChange_Implementation(const FGSAttributeModification& AppliedMod)
{
}

float UGSAttributeSet::GetAttributeValue(const FGameplayTag& AttributeTag, bool& bFound) const
{
	FGSAttribute Attribute;
	if (GetAttribute(AttributeTag, Attribute))
	{
		bFound = true;
		return Attribute.GetValue();
	}
	
	bFound = false;
	return 0.f;
}

bool UGSAttributeSet::GetAttributeName(const FGameplayTag& AttributeTag, FName& OutName) const
{
	const FName TagName = AttributeTag.GetTagName();
	FString LeftString;
	FString RightString;
	TagName.ToString().Split(".", &LeftString, &RightString, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (RightString.IsEmpty())
	{
		return false;
	}
	OutName = FName(RightString);
	return true;
}
