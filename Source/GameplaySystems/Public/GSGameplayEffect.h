// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GSAttributeSet.h"
#include "UObject/Object.h"
#include "GSGameplayEffect.generated.h"

UENUM()
enum class EGameplayEffectDurationType : uint8
{
	/** This effect applies instantly */
	Instant,
	/** This effect lasts forever */
	Infinite,
	/** The duration of this effect will be specified by a magnitude */
	HasDuration
};

UCLASS()
class GAMEPLAYSYSTEMS_API UGSGameplayEffect : public UObject
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, Category=GameplayEffect)
	EGameplayEffectDurationType DurationPolicy;
	
	UPROPERTY(EditDefaultsOnly, Category=GameplayEffect)
	float DurationMagnitude;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=GameplayEffect)
	float Period;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=GameplayEffect)
	bool bExecutePeriodicEffectOnApplication;
	
	UPROPERTY(EditDefaultsOnly, Category=GameplayEffect, meta=(GameplayAttribute="True"))
	float ChanceToApplyToTarget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=GameplayEffect, meta=(TitleProperty=Attribute))
	TArray<FGSAttributeModification> Modifiers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags)
	FGameplayTagContainer GameplayEffectTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags)
	FGameplayTagContainer AppliedTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags)
	FGameplayTagContainer ApplicationTagRequirements;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags)
	FGameplayTagContainer RemovalTagRequirements;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Granted Abilities")
	TArray<TSubclassOf<UGSAction>>	GrantedAbilities;
};
