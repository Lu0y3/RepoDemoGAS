// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * 工作流程
 *	客户端控制：
 *		客户端通过 Activate() 判断是否是本地控制。
 *		如果是本地控制，调用 SendMouseCursorData() 发送鼠标点击位置的数据到服务器。
 *		使用 FScopedPredictionWindow 创建预测窗口，发送数据时确保客户端与服务器状态同步。
 *		
 *	服务器控制：
 *		服务器端注册回调，监听从客户端发送的目标数据。
 *		一旦数据被接收，OnTargetDataReplicatedCallback 被调用，在服务器上执行同步逻辑。
 *		
 *	数据同步：
 *		通过 ServerSetReplicatedTargetData，客户端将目标数据同步到服务器。
 *		服务器端通过注册委托（AbilityTargetDataSetDelegate）接收并处理目标数据
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
public:
	//TODO:: Custom自定义 AbilityTask  latent 异步蓝图节点
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta=(DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	//使用动态多播委托来增加执行引脚
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;

private:
	
	//这个Gameplay/AbilityTask一旦被激活就会调用这个函数
	virtual void Activate() override;

	//客户端向服务器端提交数据
	void SendMouseCursorData();

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};
