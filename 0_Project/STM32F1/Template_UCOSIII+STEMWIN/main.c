/***********************************************************************************************************************
* Copyright (c) Hands Free Team. All rights reserved.
* FileName: main.c
* Contact:  QQ Exchange Group -- 521037187
* Version:  V2.0
*
* LICENSING TERMS:
* The Hands Free is licensed generally under a permissive 3-clause BSD license. 
* Contributions are requiredto be made under the same license.
*
* History:
* <author>      <time>      <version>      <desc>
* mawenke      2015.10.1    V1.0          creat this file
*
* Description:   
***********************************************************************************************************************/

#include "main_includes.h"

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

#define START_TASK_PRIO				3
#define START_STK_SIZE 				256
OS_TCB StartTaskTCB;
CPU_STK START_TASK_STK[START_STK_SIZE];
void start_task(void *p_arg);

#define TOUCH_TASK_PRIO				4
#define TOUCH_STK_SIZE				128
OS_TCB TouchTaskTCB;
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
void touch_task(void *p_arg);

#define LED0_TASK_PRIO 				5
#define LED0_STK_SIZE				  64
OS_TCB Led0TaskTCB;
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);

#define EMWINDEMO_TASK_PRIO			6
#define EMWINDEMO_STK_SIZE			2048
OS_TCB EmwindemoTaskTCB;
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
void emwindemo_task(void *p_arg);

/***********************************************************************************************************************
* Function:   
*
* Scope:      
*
* Description:  
*
* Arguments:
*
* Return:
*
* Cpu_Time:  
*
* History:
* by   mawenke   2015.12.1   creat
***********************************************************************************************************************/
void System_Init(void)
{
    //SCB->VTOR = FLASH_BASE | 0x10000;  //Bootloader
    INTX_DISABLE();  //close all interruption
    HF_BSP_Init();
    INTX_ENABLE();	 //enable all interruption
}

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	System_Init();
  printf("app start \r\n");
	
	OSInit(&err);		
	OS_CRITICAL_ENTER();
	//Create start task
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		  //任务控制块
				 (CPU_CHAR	* )"start task", 		      //任务名字
                 (OS_TASK_PTR )start_task, 		//任务函数
                 (void		* )0,					      //传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,       //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	  //任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		  //任务堆栈大小
                 (OS_MSG_QTY  )0,					   //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					     //当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,				       //用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,  //任务选项
                 (OS_ERR 	* )&err);				   //存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	 
	OSStart(&err);       
								 
	while(1);
}

void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  //统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	
	OS_CRITICAL_ENTER();	

	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,		
				 (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);

	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 

	OSTaskCreate((OS_TCB*     )&Led0TaskTCB,		
				 (CPU_CHAR*   )"Led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK*    )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);					 
	OS_CRITICAL_EXIT();	
								 
}


void emwindemo_task(void *p_arg)
{
	while(1)
	{
		GUIDEMO_Main(); 
	}
}


void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//delay 5ms
	}
}


void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		HF_Set_Led_State(0,0);
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //delay 200ms
		HF_Set_Led_State(0,1);
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //delay 500ms
	}
}

