// 

#include "BCAttributeSetBase.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UBCAttributeSetBase::UBCAttributeSetBase()
	: Health(1.f)
	, MaxHealth(1.f)
{
}

void UBCAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBCAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBCAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UBCAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UBCAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UBCAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
}

void UBCAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldHealth)
{
}
