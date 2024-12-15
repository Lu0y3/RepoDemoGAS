// YanWei.


#include "Character/AuraEnemy.h"

#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	//设置碰撞响应  --用于高亮
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); //设置可视为阻挡
	
}

void AAuraEnemy::HighlightActor()
{
	//bIsHighlighted = true; 用于Debug
	
	//渲染深度自定义渠道 模板值
	GetMesh()->SetRenderCustomDepth(true); 
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED); //材质PP_Highlight
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED); //CUSTOM_DEPTH_RED在Aura自定义变量
	
}

void AAuraEnemy::UnHighlightActor()
{
	//bIsHighlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}
