#ifndef __User_CAN_H
#define	__User_CAN_H

#include "main.h"



typedef struct _CanFrame_
{
    uint32_t    ID;         //CAN����ID
    uint8_t     IDE;        //��ʶ�����ͣ�CAN_ID_STD��׼֡,CAN_ID_EXT��չ֡
    uint8_t     RTR;        //֡���ͣ�CAN_RTR_DATA����֡,CAN_RTR_REMOTEԶ��֡
    uint8_t     DLC;        //CAN�������ݳ���
    uint8_t     Data[8];    //CAN������������
}CanFrame;




void User_CAN1_Init(void);
BOOL CAN1_TX_Data (CanFrame TXCanFrame);
void CAN1_TX_0x12345678(void);







#endif





