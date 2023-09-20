#include "control.h"	
#include "usart2.h"
/**************************************************************************
 ��  �� ���������
 �Ա���ַ��https://shop119207236.taobao.com
 ΢�Ź��ںš���������ˡ�
 ��̨�ظ���ƽ��С��������ȡƽ��С��ȫ��DIY����
 ��̨�ظ������ӿ������ߡ�����ȡ���ӹ���ʦ�ر���������
 ��̨�ظ�������������ϡ�����ȡ����������ϰ�
 ֪���������� 
**************************************************************************/
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         Timer3 10ms��ʱ�ж�
**************************************************************************/
//�����Լ��ĵ��ѡ��ֻ����һ���������define��Ч
#define GA12N20
//#define GM25310
//#define TT130
//#define GB37520

#ifdef GB37520
#define E_DIR 1
#define K_DIR -1
#endif

#ifdef TT130
#define E_DIR -1
#define K_DIR -1
#endif

#ifdef GA12N20
#define E_DIR -1
#define K_DIR -1
#endif

#ifdef GM25310
#define E_DIR 1
#define K_DIR 1
#endif

#define SPEED_Y 40 //ǰ������趨�ٶ�
#define SPEED_Z 100//��������趨�ٶ� 
int   TargetVelocity=20, TargetCircle=0, CurrentPosition = 0, PWM = 0;  //Ŀ���ٶȡ�Ŀ��Ȧ��(λ��)��������������PWM���Ʊ���
float Position_Kp=8, Position_Ki=0.23, Position_Kd=3; //���λ��PID����
int delay_flag = 0;
int count,TimeCount;
extern int mode;

float velocity_KP= 360;     // С���ٶȻ�PI����
float velocity_KI= 1.5;
int i = 0;
int Position_FeedbackControl(float Circle, int CurrentPosition);
int flag = 1;
int flag_ = 1,flag__ = 1;
void TIM4_IRQHandler(void)   //TIM4�ж�
{
	static u8 Voltage_Counter=0;
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		  Led_Flash(100);
			
				Encoder_Left=-Read_Encoder(3);
			
		  Encoder_Right=-Read_Encoder(2);                           //===��ȡ��������ֵ����Ϊ�����������ת��180�ȵģ����Զ�����һ��ȡ������֤�������һ��
			usart2_send(Encoder_Left);
			                           //===��ȡ��������ֵ
		  CurrentPosition+=Encoder_Left;
			Voltage_Counter++;
			if(Voltage_Counter==10)									 //===100ms��ȡһ�γ������������Լ���ѹ
			{
				Voltage_Counter=0;
				Voltage=Get_battery_volt();		                         //===��ȡ��ص�ѹ		
			}
			if(delay_flag == 1) // ������������ȷ�ӳ�
			{
				count++;
				if(count == 3)
				{
					delay_flag = 0;
					count = 0;
				}
			}
			if(mode <= 5)
			{
				Moto2=Incremental_PI_B(Encoder_Right,Target_B);                 //===�������ֵ������PWM
				Xianfu_Pwm();  																					 //===PWM�޷�
				Turn_Off(Voltage);																 //===���Ƕ��Լ���ѹ�Ƿ�����
				Set_Pwm(0,Moto2);                                    //===��ֵ��PWM�Ĵ���  
			}
			else if(mode == 7 || mode == 8 ||mode == 9){
				PWM=Position_FeedbackControl(TargetCircle, CurrentPosition); //λ�ñջ�����
				Set_Pwm(PWM,0); 
				
			}
			else if(mode == 11 || mode == 12 || mode ==13)
			{
				PWM=Position_FeedbackControl(TargetCircle, CurrentPosition); //λ�ñջ�����
				PWM=Velocity_Restrict(PWM,TargetVelocity);
				PWM = PWM/76;
				Moto1=Incremental_PI_A(Encoder_Left,PWM);
				Moto1 = -Moto1;
				
					Set_Pwm(Moto1,0);
					
				
			}
			else if(mode == 15)
			{
				Moto1 = Incremental_PI_A(-Encoder_Left,Encoder_Right);
				Xianfu_Pwm();  																					 //===PWM�޷�
				Turn_Off(Voltage);																 //===���Ƕ��Լ���ѹ�Ƿ�����
				Set_Pwm(Moto1,0);
			}
			else
			{
				Target_A = 0;
				Target_B = 0;
				Set_Pwm(0,0);
			}
	}
}

/**************************************************************************
�������ܣ�PI������
��ڲ���������������ֵ��Ŀ���ٶ�
����  ֵ�����PWM
**************************************************************************/
int Incremental_PI_A (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //����ƫ��
	 Pwm+=velocity_KP*(Bias-Last_bias)+velocity_KI*Bias;   //����ʽPI������
	 Last_bias=Bias;	                   //������һ��ƫ�� 
	 return Pwm;                         //�������
}
int Incremental_PI_B (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //����ƫ��
	 Pwm+=velocity_KP*(Bias-Last_bias)+velocity_KI*Bias;   //����ʽPI������
	 Last_bias=Bias;	                   //������һ��ƫ�� 
	 return Pwm;                         //�������
}
/**************************************************************************
�������ܣ�λ��ʽPID������
��ڲ�����Ŀ��Ȧ��(λ��) ��ǰλ��
����  ֵ���ٶȿ���ֵ
����λ��ʽ��ɢPID��ʽ 
pwm=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ�� 
e(k-1)������һ�ε�ƫ��  
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
pwm�������
**************************************************************************/
int Position_FeedbackControl(float Circle, int CurrentPosition)
{
		float TargetPosition,Bias, ControlVelocity;     //������ر���
		static float Last_bias, Integral_Bias;          //��̬�������������ý�������ֵ��Ȼ����
		
			TargetPosition=Circle*1560;
		
		
	   //Ŀ��λ��=Ŀ��Ȧ��*1040
	                                   //10ms��ȡһ�α�����(��100HZ)��������ٱ�Ϊ20����������������13��AB˫����ϵõ�4��Ƶ��
	                                   //��ת1Ȧ����������Ϊ20*13*4=1040�����ת��=Encoder*100/1040r/s ʹ�ö�ʱ��2
	                                   //1.04�����ϵ�����������������ɸ���ʵ�����������ϵ������߿��ƾ���
		Bias=TargetPosition-CurrentPosition; //��λ��ƫ��
		
	  Integral_Bias+=Bias;
    if(Integral_Bias> 750) Integral_Bias= 750;	//�����޷� ��ֹ����Ŀ��λ�ú����
	  if(Integral_Bias<-750) Integral_Bias=-750;	//�����޷� ��ֹ����Ŀ��λ�ú����
	
		ControlVelocity=Position_Kp*Bias+Position_Ki*Integral_Bias+Position_Kd*(Bias-Last_bias);  //����ʽPI������
	                                                                                            //Position_Kp*Bias ƫ��Խ���ٶ�Խ��
	                                                                                            //Position_Ki*Integral_Bias ��С��̬���
	                                                                                            //Position_Kd*(Bias-Last_bias) �����ٶ�
		Last_bias=Bias;	
		return ControlVelocity;    //�����ٶȿ���ֵ 
}

int Velocity_Restrict(int PWM_P, int TargetVelocity)
{
    if     (PWM_P>+TargetVelocity*76) PWM_P=+TargetVelocity*76;
	  else if(PWM_P<-TargetVelocity*76) PWM_P=-TargetVelocity*76;
	  else PWM_P=PWM_P;
	  return PWM_P;
}


