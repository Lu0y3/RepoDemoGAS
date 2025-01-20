// YanWei.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	//是否由客户端控制
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		//如果是客户端控制器控制，实现将数据发射到服务器端
		SendMouseCursorData();
	}else
	{
		//TODO:: On Server so to Listen Target Data
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		//服务器端监听  在客户端发送数据时，服务器会调用该回调来处理同步的数据
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(),GetActivationPredictionKey())
		.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		//检查是否已经接收到了目标数据： 检查是否有与给定能力句柄和预测键（AbilityHandle 和 AbilityOriginalPredictionKey）相关联的目标数据，并根据这些数据触发适当的委托。
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle,ActivationPredictionKey);
		if (!bCalledDelegate) //目标数据尚未准备好（可能客户端还没有上传）
		{
			SetWaitingOnRemotePlayerData(); //表示当前服务器正在等待远程玩家的数据。
		}
	}
	
	/*
	 *		详细工作流程
	 *	数据同步请求：
	 *	客户端通过 ServerSetReplicatedTargetData 将目标数据（TargetDataHandle）上传到服务器，附带相应的 ActivationPredictionKey。
	 *	
	 *	服务器逻辑：
	 *	服务器尝试使用 CallReplicatedTargetDataDelegatesIfSet 检查是否已经接收到了目标数据：
	 *	如果接收到目标数据，调用目标数据委托处理逻辑，例如触发能力效果。
	 *	如果尚未接收到数据，则设置等待标志（SetWaitingOnRemotePlayerData），进入等待状态。
	 *	
	 *	目标数据到达：
	 *	一旦目标数据同步完成，绑定的委托（通过 TargetDataSetDelegate 注册的回调）被调用。
	 *	委托会自动解除等待标志，继续执行能力激活的后续逻辑。
	 */
	
	/*APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	ValidData.Broadcast(CursorHit.Location); //一旦进行广播 对应的执行引脚将激活*/
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	//创建一个预测窗口，使用预测键和 FScopedPredictionWindow，可以使客户端在不等待服务器响应的情况下预测技能执行，并在服务器确认后进行回滚操作。
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	//获取鼠标拾取结果
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);

	//创建需要上传服务器端的TargetData 封装与目标相关的数据
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	//创建TargetData句柄，上传到服务器端需要上传句柄
	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);
	
	// 用于从客户端向服务器发送目标数据，目的是在能力激活时提供相关的输入或信息，例如击中目标、能力位置或者其他关键数据。
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(), //返回当前激活能力的预测键（FPredictionKey）。预测键用于支持 GAS 的网络预测系统，确保客户端能够快速执行并响应，而不需要等待服务器确认。
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey); //当前的 FPredictionKey，用于确保客户端和服务器之间的操作同步。Scoped Prediction Key 是一个局部对象，用于追踪和锁定客户端的当前预测范围。
	
	//决定事件是否应该广播  防止重复广播
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle); // 触发广播，向外发送目标数据
	}

}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	// 处理从服务器端接收到的目标数据
	//通知客户端 服务器端已经接收并处理了从客户端复制的目标数据（将服务器的TargetData应用到客户端，并清除掉缓存）
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	//判断服务器端是否通过验证
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
	
}

