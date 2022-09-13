// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GSActionComponent.h"
#include "Math/UnitConversion.h"
#include "UObject/Object.h"
#include "GSAttributeSet.generated.h"


UENUM(BlueprintType)
enum class EGSAttributeModifierOperation
{
	AddDelta,
	AddMultiplier,
	AddBase,
	OverrideBase,
	Invalid
};

USTRUCT(BlueprintType)
struct FGSAttribute
{
	GENERATED_BODY()
	
public:
	
	FGSAttribute(float DefaultValue)
	{
		BaseValue = DefaultValue;
		Delta = 0.f;
		Multiplier = 1.f;
		bClampToZero = true;
	}
	
	FGSAttribute()
	{
		BaseValue = 0.f;
		Delta = 0.f;
		Multiplier = 1.f;
		bClampToZero = true;
	}

	float GetValue() const
	{
		float Value = (BaseValue + Delta) * Multiplier;
		if (bClampToZero)
		{
			return FMath::Max(0.f, Value);
		}
		return Value;
	}

	UPROPERTY(EditDefaultsOnly, SaveGame, BlueprintReadWrite)
	float BaseValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AdvancedDisplay))
	float Delta;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AdvancedDisplay))
	float Multiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AdvancedDisplay))
	bool bClampToZero;
	
};

USTRUCT(BlueprintType)
struct FGSAttributeModification
{
	GENERATED_BODY()
	
public:

	FGSAttributeModification()
	{
		ModifierOperation = EGSAttributeModifierOperation::AddBase;
		Magnitude = 0.f;
	}

	float GetMagnitude() const { return Magnitude; }

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UGSActionComponent> InstigatorComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AttributeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EGSAttributeModifierOperation ModifierOperation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Magnitude;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer AddedTags;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnActionListChangedSignature, UGSActionComponent*, ActionComponent, UGSAction*, Action);
DECLARE_DYNAMIC_DELEGATE_SevenParams(FAttributeChangedSignature, UGSActionComponent*, ActionComponent, UGSActionComponent*, InstigatorComponent, FGameplayTag, AttributeTag, float, NewValue, float, Delta, const FGameplayTagContainer&, ContextTags, EGSAttributeModifierOperation, ModType);

UCLASS()
class GAMEPLAYSYSTEMS_API UGSAttributeSet : public UObject
{
	GENERATED_BODY()

public:

	UGSAttributeSet();
	
	virtual void Initialize(UGSActionComponent* NewOwner);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_Initialize();

	bool GetAttribute(const FGameplayTag& InTag, FGSAttribute& OutAttribute) const;
	
	bool GetAttribute(const FName& AttributeName, FGSAttribute& OutAttribute) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float ApplyAttributeChange(const FGSAttributeModification& ModToApply);

	UFUNCTION(BlueprintNativeEvent)
	void PostAttributeChange(const FGSAttributeModification& AppliedMod);

	virtual UWorld* GetWorld() const override;

	UGSActionComponent* GetOwningComponent() const { return OwningComp;}

protected:

	UGSActionComponent* OwningComp;
	
	UPROPERTY(BlueprintReadWrite, EditInline)
	TArray<FGSAttribute> Attributes;

	UFUNCTION(BlueprintPure, Category = "AttributeSet")
	float GetAttributeValue(const FGameplayTag& AttributeTag, bool& bFound) const;
	
	bool GetAttributeName(const FGameplayTag& AttributeTag, FName& OutName) const;
};
