// YanWei.


#include "Character/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true; //人物朝向移动方向
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	//平面约束 功能，用于限制角色的移动，使其只能在一个指定平面上运动。
	GetCharacterMovement()->bConstrainToPlane = true; //启用平面约束
	GetCharacterMovement()->bSnapToPlaneAtStart = true; //开始时对齐平面

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}
