#include "User_CAN.h"
#include "can.h"


static BOOL CAN1_RX_Manage (CanFrame RXCanFrame);//CAN���մ�����

//CAN��ʼ������
void User_CAN1_Init(void)
{
    CAN_FilterTypeDef FilterConfig;

    /*����CAN������*/
    FilterConfig.FilterBank = 0;                      //���������
    FilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;  //����ģʽ������λģʽ--CAN_FILTERMODE_IDMASK���б�ģʽ--CAN_FILTERMODE_IDLIST
    FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; //������λ��32λ
    FilterConfig.FilterIdHigh = 0x0000;               //32λID
    FilterConfig.FilterIdLow = 0x0000;
    FilterConfig.FilterMaskIdHigh = 0x0000;           //32λMASK
    FilterConfig.FilterMaskIdLow = 0x0000;
    FilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; //������0������FIFO0
    FilterConfig.FilterActivation = ENABLE;           //�����˲���0
    FilterConfig.SlaveStartFilterBank = 14;           //��CAN�˲���������
    //����������
    if (HAL_CAN_ConfigFilter(&hcan1, &FilterConfig) != HAL_OK)
    {
    Error_Handler();
    }

    //����CAN��Χ�豸
    if(HAL_CAN_Start(&hcan1) != HAL_OK) 
    {
    Error_Handler();
    }
    //����CAN�����ж�
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
    Error_Handler();
    }
}


//CAN���ջص�����
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *CanHandle)
{
    CAN_RxHeaderTypeDef     RxHeader;      //����
    CanFrame                RXCanFrame;

    if(HAL_CAN_GetRxMessage(CanHandle, CAN_RX_FIFO0, &RxHeader, RXCanFrame.Data) == HAL_OK)
    {
        RXCanFrame.IDE = RxHeader.IDE;      //��ʶ�����ͣ�CAN_ID_STD��׼֡,CAN_ID_EXT��չ֡
        if( RXCanFrame.IDE == CAN_ID_EXT) 
        {
            RXCanFrame.ID = RxHeader.ExtId; //��չ֡ID (29λ:0x00000000~0x1FFFFFFF)
        }
        else
        {
            RXCanFrame.ID = RxHeader.StdId; //��׼֡ID (11λ:0x00~0x7FF)
        }
        RXCanFrame.RTR = RxHeader.RTR;      //֡���ͣ�CAN_RTR_DATA����֡,CAN_RTR_REMOTEԶ��֡
        RXCanFrame.DLC = RxHeader.DLC;      //CAN�������ݳ���
        
        CAN1_RX_Manage (RXCanFrame);        //CAN���մ�����
    }
    else
    {
        Error_Handler();
    }
}

//CAN1�������ݺ���
BOOL CAN1_TX_Data (CanFrame TXCanFrame)
{
    BOOL result = TRUE;
    
    CAN_TxHeaderTypeDef  TXHeader;  //can������Ϣ�Ľṹ��
    uint32_t    mailbox;       
    
    if(TXCanFrame.IDE == CAN_ID_EXT)  //CAN����ID
    {
        TXHeader.StdId = 0;
        TXHeader.ExtId = TXCanFrame.ID;
    }
    else
    {
        TXHeader.StdId = TXCanFrame.ID;
        TXHeader.ExtId = 0;
    }
    
    TXHeader.IDE = TXCanFrame.IDE;    //��ʶ�����ͣ�CAN_ID_STD��׼֡,CAN_ID_EXT��չ֡
    TXHeader.RTR = TXCanFrame.RTR;    //֡���ͣ�CAN_RTR_DATA����֡,CAN_RTR_REMOTEԶ��֡
    TXHeader.DLC = TXCanFrame.DLC;    //CAN�������ݳ���
    TXHeader.TransmitGlobalTime = DISABLE;

    if (HAL_CAN_AddTxMessage(&hcan1, &TXHeader, TXCanFrame.Data, &mailbox) != HAL_OK) 
    {
        result = FALSE;
    }

    return result;
}




//CAN1���մ�����
static BOOL CAN1_RX_Manage (CanFrame RXCanFrame)
{
    BOOL result = TRUE;
    
    CanFrame  TXCanFrame;
    TXCanFrame.ID = 0x18888888;     //֡ID
    TXCanFrame.IDE = CAN_ID_EXT;    //��չ֡
    TXCanFrame.RTR = CAN_RTR_DATA;  //����֡
    TXCanFrame.DLC = 4;             //���ݳ���
    
    TXCanFrame.Data[0] = RXCanFrame.ID>>24;
    TXCanFrame.Data[1] = RXCanFrame.ID>>16;
    TXCanFrame.Data[2] = RXCanFrame.ID>>8;
    TXCanFrame.Data[3] = RXCanFrame.ID;
    
    
    result = CAN1_TX_Data (TXCanFrame);
    return result;
}



void CAN1_TX_0x12345678(void)
{
    CanFrame  TXCanFrame;
    TXCanFrame.ID = 0x12345678;     //֡ID
    TXCanFrame.IDE = CAN_ID_EXT;    //��չ֡
    TXCanFrame.RTR = CAN_RTR_DATA;  //����֡
    TXCanFrame.DLC = 8;             //���ݳ���
    
    TXCanFrame.Data[0] = 1;
    TXCanFrame.Data[1] = 2;
    TXCanFrame.Data[2] = 3;
    TXCanFrame.Data[3] = 4;
    TXCanFrame.Data[4] = 5;
    TXCanFrame.Data[5] = 6;
    TXCanFrame.Data[6] = 7;
    TXCanFrame.Data[7] = 8;
    
    CAN1_TX_Data (TXCanFrame);
}








