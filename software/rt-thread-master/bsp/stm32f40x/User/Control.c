/*
 * Control.c
 *
 *  Created on: 2019��3��20��
 *      Author: zengwangfa
 *      Notes:  ��λ�ǿ��ơ���ȿ���
 */
#include "Control.h"
#include "PID.h"
#include <rtthread.h>
#include <stdlib.h>
#include "RC_Data.h"
#include "focus.h"
#include "led.h"
#include "servo.h"
#include "PropellerControl.h"
#include "propeller.h"


float Yaw_Control = 0.0f;//Yaw���� ƫ������ 
float Yaw = 0.0f;

/* ԭ��
 *  N���� 0��/-0��
 *  W���� 90��
 *  S���� 180��/-180
 *  E���� -90��
*/
/** �涨   ��ʱ��
 *  N���� 0��
 *  W���� 90��
 *  S���� 180��
 *  E���� 270��
 */

void control_thread_entry(void *parameter)
{

		rt_thread_mdelay(1500);//�ȴ������豸��ʼ���ɹ�
		while(1)
		{
				Devices_Control();
				rt_thread_mdelay(10);
		}
}



/**
  * @brief  Devices_Control(�豸����)
  * @param  None
  * @retval None
  * @notice 
  */
void Devices_Control(void)
{
	
		Light_Control(&Control.Light);  //̽�յƿ���
		Focus_Zoom_Camera(&Control.Focus);//�佹�۽�����ͷ����

		YunTai_Control(&Control.Yuntai); //��̨����
		RoboticArm_Control(&Control.Arm);//��е�ۿ���

		Propeller_Control();
			/*
				Control.Depth_Lock     = Control_Data[3]; //�������
				Control.Direction_Lock = Control_Data[4]; //��������
				Control.Move					 = Control_Data[5]; //ǰ���˶�
				Control.Translation		 = Control_Data[6]; //�����ƶ�
				Control.Vertical 			 = Control_Data[7]; //��ֱ�˶�
				Control.Rotate 				 = Control_Data[8]; //��ת�˶�
				
				Control.Power 				 = Control_Data[9];  //��������
				Control.Light 				 = Control_Data[10]; //�ƹ����
				
				Control.Focus 				 = Control_Data[11]; //�佹����ͷ����
	
				Control.Yuntai 				 = Control_Data[12]; //��̨����
				Control.Arm						 = Control_Data[13]; //��е�ۿ���*/
	
}



int control_thread_init(void)
{
		rt_thread_t control_tid;
		/*������̬�߳�*/
    control_tid = rt_thread_create("control",//�߳�����
                    control_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    15,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (control_tid != RT_NULL){
				rt_thread_startup(control_tid);
		}
		return 0;
}
INIT_APP_EXPORT(control_thread_init);


void Angle_Control(void)
{
	
		if(Sensor.JY901.Euler.Yaw < 0) Yaw = (180+Sensor.JY901.Euler.Yaw) + 180;//�ǶȲ���
		if(Sensor.JY901.Euler.Yaw > 0) Yaw = Sensor.JY901.Euler.Yaw;            //�ǶȲ���
		Total_Controller.Yaw_Angle_Control.Expect = Yaw_Control;//ƫ�����ٶȻ�������ֱ����Դ��ң���������
		Total_Controller.Yaw_Angle_Control.FeedBack = Yaw;//ƫ���Ƿ���
		PID_Control_Yaw(&Total_Controller.Yaw_Angle_Control);//ƫ���Ƕȿ���
	

		//ƫ�����ٶȻ���������Դ��ƫ���Ƕȿ��������
		//Total_Controller.Yaw_Gyro_Control.Expect = Total_Controller.Yaw_Angle_Control.Control_OutPut;
}



void Depth_Control(void)
{
	
		PID_Control(&Total_Controller.High_Position_Control);//�߶�λ�ÿ�����
		Propeller_upDown(Total_Controller.High_Position_Control.Control_OutPut);		//��ֱ�ƽ�������
}
void Gyro_Control(void)//���ٶȻ�
{

//  	ƫ����ǰ������
//  	Total_Controller.Yaw_Gyro_Control.FeedBack=Yaw_Gyro;


//		PID_Control_Div_LPF(&Total_Controller.Yaw_Gyro_Control);
//		Yaw_Gyro_Control_Expect_Delta=1000*(Total_Controller.Yaw_Gyro_Control.Expect-Last_Yaw_Gyro_Control_Expect)
//			/Total_Controller.Yaw_Gyro_Control.PID_Controller_Dt.Time_Delta;
//		//**************************ƫ����ǰ������**********************************
//		Total_Controller.Yaw_Gyro_Control.Control_OutPut+=Yaw_Feedforward_Kp*Total_Controller.Yaw_Gyro_Control.Expect
//			+Yaw_Feedforward_Kd*Yaw_Gyro_Control_Expect_Delta;//ƫ����ǰ������
//		Total_Controller.Yaw_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Yaw_Gyro_Control.Control_OutPut,
//																																		 -Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit,
//																																		 Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit);
//		Last_Yaw_Gyro_Control_Expect=Total_Controller.Yaw_Gyro_Control.Expect;
//		

}
/*����е�ۡ���� ����yaw MSH���� */
static int yaw(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        rt_kprintf("Error! Proper Usage: RoboticArm_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }

		Yaw_Control = atoi(argv[1]);

		
_exit:
    return result;
}
MSH_CMD_EXPORT(yaw,ag: yaw 100);
