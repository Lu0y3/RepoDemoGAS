// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
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
	
};


