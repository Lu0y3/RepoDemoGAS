// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	/**
	 * 1、InitAbilityActorInfo()
	 * PossessedBy 用于角色被新控制器控制时的处理。
	 * OnRep_PlayerState 用于处理网络复制时玩家状态的变化。
	 * 2、Mixed Replication Mode
	 * OwnerActor 的 Owner 必须是 Controller：在 Pawn 类中，Owner 会在 PossessedBy() 函数中自动设置为 Controller。
	 * PlayerState 的 Owner 会自动设置为 Controller：这保证了网络状态同步过程中，PlayerState 与 Controller 保持关联。
	 * 手动调用 SetOwner()：如果 OwnerActor 不是 PlayerState 并且你使用 Mixed Replication Mode，需要手动调用 SetOwner()，将 OwnerActor 的 Owner 设置为 Controller。
	 * 
	 */
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;  //如果 PlayerState 的某些属性在网络中发生变化，OnRep_PlayerState 会被调用，确保客户端更新相关数据。

private:
	void InitAbilityActorInfo();
};
