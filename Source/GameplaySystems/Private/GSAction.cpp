// Fill out your copyright notice in the Description page of Project Settings.


#include "GSAction.h"

#include "GSActionComponent.h"

UGSAction::UGSAction()
{
	bAutoStart = false;
	ActionName = NAME_None;
	bIsRunning = false;
	TimeStarted = 0.f;
}

void UGSAction::Initialize(UGSActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;
}

bool UGSAction::IsRunning() const
{
	return bIsRunning;
}

bool UGSAction::CanStart_Implementation(AActor* Instigator) const
{
	if (IsRunning())
	{
		return false;
	}

	const UGSActionComponent* Comp = GetOwningComponent();
	
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}

	return true;
}

void UGSAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Started: %s"), *GetNameSafe(this));

	UGSActionComponent* Comp = GetOwningComponent();	
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);
	
	bIsRunning = true;
	
	TimeStarted = GetWorld()->TimeSeconds;

	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);
}

void UGSAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));

	UGSActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;
	
	GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);
}

UGSActionComponent* UGSAction::GetOwningComponent() const
{
	return ActionComp;
}

UWorld* UGSAction::GetWorld() const
{
	// Outer is set when creating action via NewObject<T>
	AActor* Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}

	return nullptr;
}