// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GSAttributeSet.h"
#include "Components/ActorComponent.h"
#include "GSActionComponent.generated.h"


class UGSAction;

USTRUCT(BlueprintType)
struct FGSActionContext
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Instigator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UObject> TargetObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EventMagnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer ContextTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitResult;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChanged, class UGSActionComponent*, OwningComp, UGSAction*, Action);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYSYSTEMS_API UGSActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGSActionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void AddAction(AActor* Instigator, TSubclassOf<UGSAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void RemoveAction(UGSAction* ActionToRemove);

	/* Returns first occurrence of action matching the class provided */
	UFUNCTION(BlueprintCallable, Category = "Actions")
	UGSAction* GetAction(TSubclassOf<UGSAction> ActionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StartActionByName(AActor* Instigator, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool StopActionByName(AActor* Instigator, FName ActionName);

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStarted;

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStopped;

	UFUNCTION(BlueprintPure, Category = "Attributes", meta = (DisplayName = "Get Attribute"))
	bool K2_GetAttribute(FGameplayTag AttributeTag, float& CurrentValue, float& BaseValue, float& Additive, float& Multiplier);

	UFUNCTION(BlueprintCallable, Category = "Attribute")
	float ApplyAttributeChange(const FGSAttributeModification& InAttributeMod);
	
	UFUNCTION(BlueprintCallable, Category = "Attribute", meta = (AutoCreateRefTerm = "Event", DisplayName = "Add Attribute Listiner", AdvancedDisplay = "bCallNow"))
	void AddAttributeChangedListener(FGameplayTag AttributeTag, const FAttributeChangedSignature& Event, bool bCallNow);

	TArray<TPair<FGameplayTag, FAttributeChangedSignature>> AttributeChangedTriggers;

	void BroadcastAttributeChanged(FGameplayTag InAttributeTag, UGSActionComponent* InstigatorComponent, float InNewValue, float InDelta, FGameplayTagContainer InContextTags, EGSAttributeModifierOperation ModOperation);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	UGSAttributeSet*  AttributeSet;
	
	UPROPERTY(EditDefaultsOnly, NoClear, Category = "Attributes")
	TSubclassOf<UGSAttributeSet> AttributeSetClass;
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<UGSAction>> DefaultActions;

	UPROPERTY(BlueprintReadOnly)
	TArray<UGSAction*> Actions;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
