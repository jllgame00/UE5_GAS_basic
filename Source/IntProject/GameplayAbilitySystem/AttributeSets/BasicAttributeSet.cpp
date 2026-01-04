// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagContainer.h"
#include "BasicAttributeSet.h"
#include "Net/UnrealNetwork.h"

UBasicAttributeSet::UBasicAttributeSet()
{
	Health = 100.f;
	MaxHealth = 100.f;
}

void UBasicAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBasicAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UBasicAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float ClampedHealth = FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth());
		SetHealth(ClampedHealth);

		if (ClampedHealth <= 0.0f)
		{
			UAbilitySystemComponent* TargetASC = &Data.Target;

			const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
			if (!TargetASC->HasMatchingGameplayTag(DeadTag))
			{
				TargetASC->AddLooseGameplayTag(DeadTag);

				AActor* Avatar = TargetASC->GetAvatarActor();
				UE_LOG(LogTemp, Warning, TEXT("[GAS] State.Dead tag added to %s"), *GetNameSafe(Avatar));
			}
		}
	}
}


