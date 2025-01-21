// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;


UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();

	//设置为公共public  因为之后可能会用它改变projectile的参数
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	// 声明一个 Server RPC，用于在服务器上销毁对象
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDestroyProjectile();

	//TODO:: Projectile Overlap
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	//此物体的存在时间
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
	bool bHit = false; //用于判断当前火球是否和其它物体产生了碰撞
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound; //移动循环音效
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent; //储存循环音效的变量，后续用于删除

};
