


#define ENSENSOR        {GPIO_SetBits(GPIOC, GPIO_Pin_4);}
#define DISSENSOR       GPIO_ResetBits(GPIOC, GPIO_Pin_4);


#define ALARMLEDON	    GPIO_SetBits(GPIOD, GPIO_Pin_6); 
#define ALARMLEDOFF	    GPIO_ResetBits(GPIOD, GPIO_Pin_6);

#define WORKLEDON	    GPIO_SetBits(GPIOD, GPIO_Pin_7); 
#define WORKLEDOFF	    GPIO_ResetBits(GPIOD, GPIO_Pin_7);


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

void StopAdc(void);
void InitAdc();

extern u16 CommCnt,AlarmCnt;;

  
   // u8 *data;
   // u8 sum;
    

typedef struct
{
    u8 id;//设备通讯编号1-100	
	u8 TranMinute;//上发间隔时间 分为单位	取值为5,10,30,60,240
    s16 TempHigh;
    u16 PT100_0C;   
    s16 TempLow;
    u16 PT100_97C;   
    u8 freq;
}WTSYSINFO;

extern WTSYSINFO SysInfo;
extern s16 g_Temp;
extern s16 g_humid;;

extern u8 InSetMode;
//#define HALFSEC 20