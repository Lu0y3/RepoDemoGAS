// YanWei.


#include "Player/AuraPlayerState.h"

AAuraPlayerState::AAuraPlayerState()
{
	//网络更新频率  //每秒和服务器更新客户端频率 同步  任何需要复制的变量都会更新，使用GAS后可以设置的高一些
	NetUpdateFrequency = 144.f;
}
