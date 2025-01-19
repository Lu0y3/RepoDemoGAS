// YanWei.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h" 
#include "MyGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	//对服务器上数据更新的响应
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRun();
}

void AAuraPlayerController::AutoRun()
{
	
	if (APawn* ControlledPawn = GetPawn())
	{
		if (!bAutoRunning) return;
		//Spline上离受控Pawn最近的位置和方向  smoothcurve
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(),ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline,ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::CursorTrace()
{
	//FHitResult CursorHit; //命中结果—一个结构体，可以了解
	//检查在光标下命中结果物体的可见性，如果是阻挡命中则返回
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());//检查被hit的目标是否实现了IEnemyInterface

	if(CurrentActor != LastActor)
	{
		if(CurrentActor) CurrentActor->HighlightActor();
		if(LastActor) LastActor->UnHighlightActor();
	}
	
	/**
 * 射线拾取后，会出现的几种情况
 * 1. LastActor is null   CurrentActor is null 不需要任何操作
 * 2. LastActor is null   CurrentActor is valid 高亮CurrentActor
 * 3. LastActor is valid   CurrentActor is null 取消高亮LastActor
 * 4. LastActor is valid   CurrentActor is valid LastActor ！= CurrentActor 取消高亮LastActor 高亮CurrentActor
 * 5. LastActor is valid   CurrentActor is valid LastActor == CurrentActor 不需要任何操作
 */
	/*
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
	*/
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());

	//鼠标左键输入
	if (InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		//TODO::Click toMove
		bTargeting = CurrentActor ? true : false; //是否瞄准了某个目标，这里是高亮目标
		bAutoRunning = false; //这里触发的是PressFunc 无法确认是否是短按直至释放鼠标
		FollowTime = 0.f; //重置统计的时间
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, *InputTag.ToString());
	if (!InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	
	if (bTargeting)
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	}else
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn) //判断是否不是点按
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this,ControlledPawn->GetActorLocation(),CachedDestination))
			{
				//NavPath->PathPoints  获取计算得到的路径的具体坐标序列。路径的每个“转折点”或“中间点”。
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					//将路径点添加到样条中
					Spline->AddSplinePoint(PointLoc,ESplineCoordinateSpace::World);
					DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green,false,5.f);
				}
				if (NavPath->PathPoints.Num() >= 2)
				{
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1]; //自动寻路的最终点。为了检查是否为可达点
				}
				bAutoRunning = true;
			}
		}
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Green, *InputTag.ToString());
	//如果按住的不是LMB ,如1 2 3之类的，则代表对应能力处于活跃状态
	if (!InputTag.MatchesTagExact(FMyGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	//如果按住的是LMB 并且瞄准了某个目标 同上
	if (bTargeting)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}else //未瞄准目标
	{
		FollowTime += GetWorld()->GetDeltaSeconds();////统计悬停时间来判断是否为点击
		//增量秒 GetDeltaSeconds() 返回当前帧与上一帧之间的时间差，也就是游戏的帧率。

		//FHitResult Hit;  //鼠标下的目标
		if(CursorHit.bBlockingHit/*GetHitResultUnderCursor(ECC_Visibility, false, Hit)*/)
		{
			CachedDestination = CursorHit.ImpactPoint; //冲击点
		}

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal(); //向量归一化处理
			ControlledPawn->AddMovementInput(WorldDirection); //控制Pawn移动
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
		//考虑多人游戏不放置断言，，即如果没有增强输入子系统则不会拥有InpuContext 即输入？
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
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(LookZoomAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::LookZoom);
	/*AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);*/
	//内含委托 传InputConfig的Tag 做参数 给func函数
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
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
	/*const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);*/

	const FVector ForwardDirection = YawRotation.RotateVector(FVector::ForwardVector);
	const FVector RightDirection = YawRotation.RotateVector(FVector::RightVector);
	
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		//InputAxisVector.Y/X 根据BPIA_IMC的设置
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::LookZoom(const FInputActionValue& InputActionValue)
{
	const float Zoom = InputActionValue.Get<float>();
	const APawn* ControlledPawn = GetPawn(); //获得当前控制的玩家
	if (!ControlledPawn) return;
	USpringArmComponent* SpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>(); //搜索其实现的Components
	if (SpringArm)
	{
		float CurrentArmLength = SpringArm->TargetArmLength;
		// 调整相机臂长度
		CurrentArmLength = FMath::Clamp(CurrentArmLength + Zoom * 25.0f, 500.0f, 800.0f);
		SpringArm->TargetArmLength = CurrentArmLength;
	}
}
