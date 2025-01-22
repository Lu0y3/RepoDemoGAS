// YanWei.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; //此类在服务器运行，然后复制到每个客户端
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile); //设置碰撞类型 自定义的
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //设置其只用作查询使用
	Sphere->SetCollisionResponseToChannels(ECR_Ignore); //设置其忽略所有碰撞检测
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); //设置其与世界动态物体产生重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap); //设置其与世界静态物体产生重叠事件
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); //设置其与Pawn类型物体产生重叠事件
	
	//创建发射组件
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f; //设置初始速度
	ProjectileMovement->MaxSpeed = 550.f; //设置最大速度
	ProjectileMovement->ProjectileGravityScale = 0.f; //设置重力影响因子，0为不受影响
	
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	//设置此物体的存在时间
	SetLifeSpan(LifeSpan);
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraProjectile::OnSphereOverlap);

	//添加一个音效，并附加到根组件上面，在技能移动时，声音也会跟随移动
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound,GetRootComponent());
}

void AAuraProjectile::Destroyed()
{
	//如果没有权威性且未触发重叠事件时， 在销毁之前播放击中特效
	if (!bHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(),FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		LoopingSoundComponent->Stop();  //将音乐停止后会自动销毁
	}
	
	//Destroyed()当此 Actor 在游戏过程中或在编辑器中被明确销毁时调用，而不是在关卡流式传输或游戏结束期间调用
	Super::Destroyed(); //  调用父类的销毁逻辑
}

void AAuraProjectile::ServerDestroyProjectile_Implementation()
{
	// 确保在服务器上销毁
	Destroy();
}

bool AAuraProjectile::ServerDestroyProjectile_Validate()
{
	// 可以在这里添加验证逻辑，检查销毁请求是否合理
	return true;
}


void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 播放 Sound 和 Niagara 特效
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(),FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	LoopingSoundComponent->Stop();
	
	
	//如果拥有权威性，在触发重叠后，销毁它，如果没有，将bHit设置为true，代表它已经触发了重叠事件，并且已经播放了几种特效，
	//但是没有对火球的控制权，无法自身直接销毁。 需添加销毁函数
	if (HasAuthority())
	{
		//Apply GE to do
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		//拥有权限时销毁（服务器端）
		Destroy(); // 服务器端控制，直接销毁
	}else
	{
		//无权威时销毁（客户端）
		bHit = true;  // 客户端设置 bHit 为 true，等待服务器处理
		
		ServerDestroyProjectile();// 客户端请求服务器
	}
	
}

