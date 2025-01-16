// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class USplineComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookZoomAction;
	
	void Move(const FInputActionValue& InputActionValue);
	void LookZoom(const FInputActionValue& InputActionValue);

	//玩家控制器时刻都在进行的光标交互天生的
	void CursorTrace(); //追踪光标位置
	IEnemyInterface* LastActor = nullptr; //上一帧拾取的接口指针
	IEnemyInterface* CurrentActor = nullptr; //这一帧拾取的接口指针

	FHitResult CursorHit; //提升变量，，便于使用

	//TODO::BindAbilityActions
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	//TODO::Click to move  参考TopDown
	FVector CachedDestination = FVector::ZeroVector; //缓存鼠标点击目标
	float FollowTime = 0.f; //用于查看按住了多久  存储时间的积累,可能用于控制某些行为的延时或者循环。
	bool bAutoRunning = false; //是否自动移动 配合导航
	bool bTargeting = false; //当前鼠标是否选中敌人
	
	UPROPERTY(EditDefaultsOnly)
	float ShortPressThreshold = 0.3f; //定义鼠标悬停多长时间内算点击事件
	
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f; ////当角色和目标距离在此半径内时，将关闭自动寻路

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline; //自动寻路时生成的样条线

	void AutoRun();
};

