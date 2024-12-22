// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class APlayerState;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	/** 将这四个WCParam封装到结构体中 **/
	FWidgetControllerParams(){}
	FWidgetControllerParams(APlayerController* PC , APlayerState* PS , UAbilitySystemComponent* ASC , UAttributeSet* AS)
		:PlayerController(PC),PlayerState(PS),AbilitySystemComponent(ASC),AttributeSet(AS){}
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
	
};
/**
 * 
 */
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)  //在蓝图需要时可设置WCParams
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	//广播初始值
	UFUNCTION(BlueprintCallable) //用于在蓝图中调用初始化
	virtual void BroadcastInitialValues();  //子类重写
	//表示绑定回调函数以响应外部依赖的变化
	virtual void BindCallbacksToDependencies();
protected:
	//从下列四个TObjectPtr中获取数据  然后传给Data即在Widget中访问他们
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;

};
