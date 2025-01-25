// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	/**Begin IAbilitySystemInterface*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/**End IAbilitySystemInterface*/
	UAttributeSet* GetAttributeSet()const {return  AttributeSet;}

	//--ICombatInterface
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;//HitReact
	
	virtual void Die() override; //Death  OnServer 服务器端 
	UFUNCTION(NetMulticast, Reliable) //NetMulticast:在服务器执行，然后复制到每个客户端
	virtual void MulticastHandleDeath();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponTipSocketName;
	//这里是获取projectile的生成位置 武器尖端
	virtual FVector GetCombatSocketLocation() override;

	//GAS
	//在子类中构建
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	
	virtual void InitAbilityActorInfo();
	//在需要初始化主要属性的子类中调用
	//TODO::ApplyEffectToSelf
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const;
	virtual void InitializeDefaultAttributes() const;
	/*void InitializePrimaryAttributes() const;
	void InitializeSecondaryAttributes() const;*/

	//TODO::GameplayAbility--Grant
	void AddCharacterAbilities();

	//--Dissolve Effect 实现思路:是将avatar和weapon的材质叫换成下面的
	void Dissolve();
	//蓝图可实现事件
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(const TArray<UMaterialInstanceDynamic*>& DynamicMaterialInstance);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	//
	
private:
	//TODO::GameplayAbility Startup
	UPROPERTY(EditAnywhere, Category = "Attributes")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
