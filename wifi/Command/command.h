#ifndef _COMMAND_H_
#define _COMMAND_H_
#include "sys.h"


//�����������豸�Ļ�����ݸ���
void updateData();

	
//������������ȷ��	
int checkCmd( u8* c,int len);


//������ܵ�����֡
int Decode(u8*c,int len);


#endif

