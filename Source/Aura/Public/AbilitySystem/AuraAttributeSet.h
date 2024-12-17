// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAuraAttributeSet();
	//该函数负责注册需要进行网络同步的属性，并设置其复制条件与通知方式。
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//仅改变CurrentAttributeValue  还有一个可以Pre改变BaseValue
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Health,Category = "vital Attribute")
	FGameplayAttributeData Health; //当服务器将属性复制到客户端时，将调用一个该属性的Rep通知
	//GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UAuraAttributeSet,Health);
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Health);  //设置这个属性访问器后，可以进行Init Get Set 操作
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxHealth,Category = "vital Attribute")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxHealth);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_Mana,Category = "vital Attribute")
	FGameplayAttributeData Mana; //法力值
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Mana);

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing = OnRep_MaxMana,Category = "vital Attribute")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxMana);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const; //OnRep_Health 是当 Health 属性在网络复制时被更新时自动调用的回调函数。
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

protected:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};



