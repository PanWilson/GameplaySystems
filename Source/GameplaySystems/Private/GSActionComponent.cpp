// Fill out your copyright notice in the Description page of Project Settings.


#include "GSActionComponent.h"

#include "GSAction.h"


// Sets default values for this component's properties
UGSActionComponent::UGSActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGSActionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TSubclassOf<UGSAction> ActionClass : DefaultActions)
	{
		AddAction(GetOwner(), ActionClass);
	}
}

void UGSActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray<UGSAction*> ActionsCopy = Actions;
	for (UGSAction* Action : ActionsCopy)
	{
		if (Action && Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}
	
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UGSActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
									   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGSActionComponent::AddAction(AActor* Instigator, TSubclassOf<UGSAction> ActionClass)
{
	UGSAction* NewAction = NewObject<UGSAction>(GetOwner(), ActionClass);
	if (ensure(NewAction))
	{
		NewAction->Initialize(this);

		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

void UGSActionComponent::RemoveAction(UGSAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}

	Actions.Remove(ActionToRemove);
}

UGSAction* UGSActionComponent::GetAction(TSubclassOf<UGSAction> ActionClass) const
{
	for (UGSAction* Action : Actions)
	{
		if (Action && Action->IsA(ActionClass))
		{
			return Action;
		}
	}

	return nullptr;
}

bool UGSActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (UGSAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
				continue;
			}
			
			// Bookmark for Unreal Insights
			TRACE_BOOKMARK(TEXT("StartAction::%s"), *GetNameSafe(Action));

			Action->StartAction(Instigator);
			return true;
		}
	}

	return false;
}

bool UGSActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (UGSAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				Action->StopAction(Instigator);
				return true;
			}
		}
	}

	return false;
}

bool UGSActionComponent::K2_GetAttribute(FGameplayTag AttributeTag, float& CurrentValue, float& BaseValue,
	float& Additive, float& Multiplier)
{
}

float UGSActionComponent::ApplyAttributeChange(const FGSAttributeModification& InAttributeMod)
{
	AttributeSet->ApplyAttributeChange(InAttributeMod);
}

void UGSActionComponent::AddAttributeChangedListener(FGameplayTag AttributeTag, const FAttributeChangedSignature& Event,
													 bool bCallNow)
{
	new(AttributeChangedTriggers) TPair<FGameplayTag, FAttributeChangedSignature>(AttributeTag, Event);

	if (bCallNow)
	{
		FGSAttribute Attribute;
		if (AttributeSet)
		{
			AttributeSet->GetAttribute(AttributeTag, Attribute);
		}

		FGameplayTagContainer EmptyContainer;
		Event.Execute(this, this, AttributeTag, Attribute.GetValue(), Attribute.GetValue(), EmptyContainer, EGSAttributeModifierOperation::Invalid);
	}
}

void UGSActionComponent::BroadcastAttributeChanged(FGameplayTag InAttributeTag,
	UGSActionComponent* InstigatorComponent, float InNewValue, float InDelta, FGameplayTagContainer InContextTags,
	EGSAttributeModifierOperation ModOperation)
{
	for (int32 i = 0; i < AttributeChangedTriggers.Num(); i++)
	{
		TPair<FGameplayTag, FAttributeChangedSignature>& SearchPair = AttributeChangedTriggers[i];
		if (SearchPair.Key.MatchesTag(InAttributeTag))
		{
			SearchPair.Value.ExecuteIfBound(this, InstigatorComponent, InAttributeTag, InNewValue, InDelta, InContextTags, ModOperation);
		}
	}
}