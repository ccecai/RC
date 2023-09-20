 /**************************************************************************
 ��  �� ���������
 �Ա���ַ��https://shop119207236.taobao.com
 
 ΢�Ź��ںš���������ˡ�
 ��̨�ظ���ƽ��С��������ȡƽ��С��ȫ��DIY����
 ��̨�ظ������ӿ������ߡ�����ȡ���ӹ���ʦ�ر���������
 ��̨�ظ�������������ϡ�����ȡ����������ϰ�
 
 ֪���������� 
**************************************************************************/
#include "sys.h"
/****************************ȫ�ֱ���*************************************/    
float Voltage;  															 //��ص�ѹ������صı���
float SR04_Distance;                 //���������
int   Encoder_Left,Encoder_Right;         		 //���ұ��������������
int 	Moto1=0,Moto2=0,Target_A=0,Target_B=0;												 //������������ո��������PWM

u8 		CTRL_MODE=98,Mode_Change=0;   //�ڴ�ѡ��С��ģʽ  //97���������ϣ�98������99����ѭ����100 PS2��101�Զ�����  

u8 		Flag_Direction=0;
int PS2_LX,PS2_LY,PS2_RX,PS2_RY,PS2_KEY; //PS2��ر���
u8 PS2_Plugin=0;

int Velocity=0,Turn=0;

int Uart_Receive=0;
u8 TkSensor=0;
/***********************************************************************/
int mode = 5;
extern int CurrentPosition,TargetCircle,flag,flag_,flag__,TargetVelocity;
extern float velocity_KP,velocity_KI;
int flag___ = 1,flag____ = 1;
int j = 1,arr = 0;
void Main_Init(void)
{
	LED_Init();                    //=====��ʼ���� LED ���ӵ�IO
	KEY_Init();                    //=====��ʼ���밴�����ӵ�IO
	delay_init();	    	           //=====��ʱ������ʼ��	
	uart1_init(115200);	           //=====����1��ʼ��
//	uart3_init(9600);            //=====��ʼ������3
	uart2_init(1200);            //=====��ʼ������2
	
	NVIC_Configuration();					 //=====�ж����ȼ�����,���а��������е��ж����ȼ�������,��������һ�����޸ġ�
	Adc_Init();                    //=====��ʼ��ADC
	Encoder_Init_TIM2();           //=====��ʼ��������2
	Encoder_Init_TIM3();           //=====��ʼ��������4
	
	OLED_Init();
	OLED_Clear();
	oled_first_show();
	Motor_Init();									 //=====��ʼ���������ӵ�Ӳ��IO�ӿ� 
	
	Timer4_Init(100,7199);           //=====10MS��һ���жϷ��������жϷ�������control.c
	TIM1_PWM_Init(7199,0);   			 //=====��ʼ��PWM 10KHZ,������������� 
	
	PWMA=0;
	PWMB=0;
	
	delay_ms(2000);
	
}

void Judge(void)
{
	if(mode == 1)
	{

		Target_A = 10;
		Target_B = 10;
	}
	else if(mode == 2)
	{
		Target_A = 50;
		Target_B = 50;
	}
	else if(mode == 3)
	{
		Target_A = 0;
		Target_B = 0;
		Set_Pwm(0,0);
	}
	else if(mode == 4)
	{
		Target_A = -10;
		Target_B = -10;
	}
	else if(mode == 5)
	{
		Target_A = -50;
		Target_B = -50;
	}
	else if(mode == 6)
	{
		Target_A = 0;
		Target_B = 0;
		Set_Pwm(0,0);
	}
	else if(mode == 7)
	{
		TargetCircle = -1;
	}
	else if(mode == 8)
	{
		if(flag_ == 1)
		{
			CurrentPosition = 0;
			flag_ = 0;
		}
		TargetCircle = 10;
	}
	else if(mode == 9)
	{
		if(flag____ == 1)
		{
			CurrentPosition = 0;
			flag____ = 0;
		}
		TargetCircle = 100;
	}
	else if(mode == 10)
	{
		
		Target_A = 0;
		Target_B = 0;
		Set_Pwm(0,0);
	}
	else if(mode == 11)
	{
		if(flag == 1)
		{
			CurrentPosition = 0;
			flag = 0;
		}
		
		TargetCircle = 1;
	}
	else if(mode == 12)
	{
		if(flag__ == 1)
		{
			CurrentPosition = 0;
			flag__ = 0;
		}
		TargetCircle = 20;
	}
	else if(mode == 13)
	{
		if(flag___ == 1)
		{
			CurrentPosition = 0;
			flag___ = 0;
			TargetVelocity = 0;
		}
		TargetCircle = 20;
		j++;
		if(j == 1000 && TargetVelocity < 20)
		{
			j = 0;
			TargetVelocity++;
		}
		
	}
	else if(mode == 14)
	{
		
		Target_A = 0;
		Target_B = 0;
		Set_Pwm(0,0);
	}
}


int main(void)
{
	
	Main_Init();

  while(1)	
	{
		
		printf("%d,%d\n",Target_B,Encoder_Right);
		oled_show();
		Judge();
		KEY_Press(1);
	}
}



