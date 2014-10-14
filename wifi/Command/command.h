#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "sys.h"


//输入数据型设备的获得数据更新
void updateData();

	
//检查接受数据真确性	
int checkCmd( u8* c,int len);


//解码接受的数据帧
int Decode(u8*c,int len);


#endif

