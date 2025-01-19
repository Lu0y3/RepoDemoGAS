// YanWei.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//UKismetSystemLibrary::PrintString(this, FString("ActivateAbility in Cpp"), true, true, FLinearColor::Blue, 3);

	//服务器生成和处理Projectile 本地不生成但CanSee一个复制的版本  ActorReplicated
	//AAuraProjectile bReplicates = true; //此类在服务器运行，然后复制到每个客户端
	
	/* True if this is the server or single player */
	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer) return;

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		//TODO:: set the Projectile Rotation
		//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		//TODO::GiveProjectile aGE to causing Damage.
		
		//确保变换设置被正确应用
		Projectile->FinishSpawning(SpawnTransform);
	}
	

	
	
}
