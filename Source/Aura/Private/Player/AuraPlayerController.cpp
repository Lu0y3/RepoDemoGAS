// YanWei.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	//对服务器上数据更新的响应
	bReplicates = true;
	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit; //命中结果—一个结构体，可以了解
	//检查在光标下命中结果物体的可见性，如果是阻挡命中则返回
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());//检查被hit的目标是否实现了IEnemyInterface
	/**
 * 射线拾取后，会出现的几种情况
 * 1. LastActor is null   CurrentActor is null 不需要任何操作
 * 2. LastActor is null   CurrentActor is valid 高亮CurrentActor
 * 3. LastActor is valid   CurrentActor is null 取消高亮LastActor
 * 4. LastActor is valid   CurrentActor is valid LastActor ！= CurrentActor 取消高亮LastActor 高亮CurrentActor
 * 5. LastActor is valid   CurrentActor is valid LastActor == CurrentActor 不需要任何操作
 */
	if (LastActor == nullptr)
	{
		if (CurrentActor != nullptr)
		{
			//2
			CurrentActor->HighlightActor();
		}
		else
		{
			//1
		}
	}
	else
	{
		if (CurrentActor == nullptr)
		{
			//3
			LastActor->UnHighlightActor();
		}else
		{
			if (CurrentActor != LastActor)
			{
				//4
				LastActor->UnHighlightActor();
				CurrentActor->HighlightActor();
			}
			else
			{
				//5
			}
		}
	}
	
	
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);
	//增强输入子系统是单例的，在整个游戏中只存在一个
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	//鼠标光标设置
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); //鼠标光标是否锁定到视口
	InputModeData.SetHideCursorDuringCapture(false); //鼠标光标被捕获是否隐藏
	SetInputMode(InputModeData); //设置控制器输入模式
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* AuraInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	/*AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);*/
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	
	/*if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}*/
	//InputAxisVector矢量取值 +1/-1
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	//Matrix矩阵  UnitAxis单位轴
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		//InputAxisVector.Y/X 根据BPIA_IMC的设置
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}