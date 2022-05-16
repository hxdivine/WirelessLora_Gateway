



#define ENDETBAT        GPIO_SetBits(GPIOC, GPIO_Pin_4);
#define DISDETBAT       GPIO_ResetBits(GPIOC, GPIO_Pin_4);

#define EN485S	    {DIS485R;GPIOD->ODR|=GPIO_Pin_7;    }  	
#define DIS485S	    GPIOD->ODR&=~GPIO_Pin_7;

#define EN485R	    {DIS485S;  GPIOD->ODR&=~GPIO_Pin_6;}   	
#define DIS485R	    GPIOD->ODR|=GPIO_Pin_6; 

#define EN485      EN485S
#define DIS485	    EN485R

#define ALARMLEDON	    GPIO_SetBits(GPIOB, GPIO_Pin_0); 
#define ALARMLEDOFF	    GPIO_ResetBits(GPIOB, GPIO_Pin_0);

#define WORKLEDON	    GPIO_SetBits(GPIOD, GPIO_Pin_3); 
#define WORKLEDOFF	    GPIO_ResetBits(GPIOD, GPIO_Pin_3);


#define ClrWdg		IWDG_ReloadCounter();//IWDG->KR = IWDG_KEY_REFRESH;


#define LORARST_H       GPIO_SetBits(GPIOD, GPIO_Pin_5);
#define LORARST_L       GPIO_ResetBits(GPIOD, GPIO_Pin_5);

#define LORACS_H       GPIO_SetBits(GPIOB, GPIO_Pin_4);
#define LORACS_L       GPIO_ResetBits(GPIOB, GPIO_Pin_4);


void Tim1_Init(void);
void Tim2_Init(void);
void Uart_Init(u8 BaudMode);
void StartAdc(void);


//hw_config.c

void SetSystem();
void UART1_SendByte(u8 data);

u16 GetBatAdc(void);
u16 GetSysTick(void);
void TranAdValue(void);
void StopAdc(void);
void InitAdc();

extern u16 CommCnt,AlarmCnt;;

  
   // u8 *data;
   // u8 sum;
    

typedef struct
{   
	u8 baud;//上发间隔时间 
    u8 freq;
}WTSYSINFO;

extern WTSYSINFO SysInfo;


extern u8 InSetMode;
//#define HALFSEC 20