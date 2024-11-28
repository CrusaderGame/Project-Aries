// 

#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BCAttributeSetBase.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
UCLASS()
class PROJECTARIES_API UBCAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBCAttributeSetBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, Health);
	
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, MaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData MeleeDamage;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, MeleeDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData RangedDamage;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, RangedDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData MeleeArmorPiercing;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, MeleeArmorPiercing);

	UPROPERTY(BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData RangedArmorPiercing;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, RangedArmorPiercing);

	UPROPERTY(BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, Ammo);

	UPROPERTY(BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData ChargeBonus;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, ChargeBonus);

	UPROPERTY(BlueprintReadOnly, Category = "Offense")
	FGameplayAttributeData AttackActionPoints;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, AttackActionPoints);

	UPROPERTY(BlueprintReadOnly, Category = "Defense")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, Armor);

	UPROPERTY(BlueprintReadOnly, Category = "Defense")
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, Shield);

	UPROPERTY(BlueprintReadOnly, Category = "Defense")
	FGameplayAttributeData ChargeAbsorption;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, ChargeAbsorption);

	UPROPERTY(BlueprintReadOnly, Category = "Neutral")
	FGameplayAttributeData Speed;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, Speed);
	
	UPROPERTY(BlueprintReadOnly, Category = "Neutral")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UBCAttributeSetBase, Stamina);

protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
};