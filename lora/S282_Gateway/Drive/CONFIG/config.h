 
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "def.h"
/*
	W25x60 8MBYTE 8*16Block 8*16*16扇区 每个扇区4k
	每页地址 0x1000
	最大地址 0x7fffff
*/
#define LOCAL_IPDEF						0xC0A801F9
#define LOCAL_PORT						502
#define	DEFAULT_MASK					0xFFFFFF00
#define	DEFAULT_GW						0xC0A80101
#define	DEFAULT_DNS						0xCA608685

#define MAXDEVICENAME					60

#define PHONELEN							11
#define MAXGPRSAPN 						40
#define MAXGPRSUSR 						40
#define MAXGPRSIP 						60
#define MAXREGDAT							52
//数据长度
#define SYS_TXT_DATLEN				50
#define MQT_TXT_DATLEN				70

#define MQTT_PWD							72
#define MQTT_TOPICLEN_MAX 		30
#define MQTT_DEVICEIDLEN_MAX	30
#define MQTT_USERNAMELEN_MAX 	20
#define MQTT_PWDLEN_MAX				20
#define HW_DEVSECRET_LEN			34

//终端设备 第一个存放在第一页  
#define TERMINAL_ADDR					0
//每个设备存储信息在256byte
#define DEV_ADDR(ptr)					(TERMINAL_ADDR + ptr * 4096)		
#define DEV_ADDR_1(ptr)				(0x10000 + ptr * 4096)	

//历史记录最大计数值
#define MAXRECORDNO						2000
//历史记录存储基地址
#define HISRECORDBASIC				0x20000
//历史记录存储地址 一次性读写擦除
#define HISRECORD_ADDR(ptr)		(HISRECORDBASIC + (ptr) * 32)

//报警记录存储地址 一次性读写擦除
//报警记录最大计数值
#define MAXALARM						500
#define ALARM_BASIC				0x30000
#define ALARM_ADDR(ptr)		(ALARM_BASIC + ptr * 32)

//MQTT补传上报记录
#define HISTORY_REPCORDBASIC	0x40000
#define HISTORY_REPDATCORD_ADDR(ptr)	(ptr * 32+ HISTORY_REPCORDBASIC)

//通道名称最大长度
#define DESCRIBE_NAME_LEN			20
#define DESCRIBE_DEVICE				DESCRIBE_NAME_LEN + 10
//终端设备数
#define	DEVICE_MAX 						50
//寄存器最大数量
#define	MAXREG								(DEVICE_MAX * 7)

//网络开关枚举类
enum ConnType
{
	CT_DISABLE = 0,
	CT_MODBUS,
	CT_MQTT
};

enum DevType
{
	DEV_WT107 = 1, //7个参数
	DEV_WT108,  //2
	DEV_WT109,  //2
	DEV_WT102,   
	DEV_WT100,
	//3 - 6 是温湿度
	DEV_WT103,   //3
	DEV_WT104,
	DEV_WT105,
	DEV_WT106,
	
	DEV_WT101,	//4
	DEV_WT110,	//7个参数
	DEV_WT111,
};

//本机设备基础信息
// 964 BYTE
typedef struct
{	
	u8 PassCode[4];															//设备密码
	u16 DeviceId; 															//设备Id
	u8 DeviceName[MAXDEVICENAME];								//设备名称或描述

	u8 GprsMode;																//GPRS模式 0 关闭 1 ModBus 2 MQTT	0x8x UPD协议  0x0x TCP协议
	u8 GprsApn[MAXGPRSAPN]; 										//运营商设置APN
	u8 GprsUsr[MAXGPRSUSR];											//用户名
	u8 GprsPwd[MAXGPRSUSR];											//密码
	u8 GprsServerIP[MAXGPRSIP];									//服务器IP地址
	u16	GprsServerPort;													//端口号

	u16 GprsBeatHeart;													//心跳
	u16 GprsFreeOffline;												//空闲下线重连	
	u16 GprsRetryCnt;														//重连次数				
//	u16 GprsReconnSec;												//断线重连时间

	u8 GprsRegDat[MAXREGDAT];										//注册包 50位为数据长度 51位为数据类型
	u8 GprsServerAckReg[MAXREGDAT];							//注册响应包
	u8 GprsServerKickOff[MAXREGDAT];						//下线包
	u8 GprsHeartCont[MAXREGDAT];								//心跳包
	u8 GprsServerAckHeart[MAXREGDAT];						//心跳响应包
	u8 GprsDatType;															//用于存储上述五个包是HEX还是ASCII
	
	u8 GprsRegMode;															//注册策略包 0每次连接 1每数据包 2同时  

	u8 Rj45Mode;																//以太网模式 0 禁止 1 ModBus 2 MQTT  0x8x UPD协议  0x0x TCP协议
	u8 Dhcp;																	//动态或静态
	u8 LocalIp[4];															//本机IP							
	u16 LocalPort;															//本机端口号
	u8 SubNetMask[4];														//子网掩码
	u8 GateIp[4];																//网关IP
	u8 DnsIp1[4];																//主DNS1
	u8 DnsIp2[4];																//副DNS2
	u8 EthTarIP[MAXGPRSIP]; 										//目标IP地址
	u16	EthTarTcpPort;													//端口号

	u16 EthBeatHeart;														//心跳	
	u16 EthFreeOffline;													//空闲重连时间
	u8 EthRetryCnt;														//重连次数

	u8 EthRegDat[MAXREGDAT];										//注册包
	u8 EthServerAckReg[MAXREGDAT];							//注册响应包
	u8 EthServerKickOff[MAXREGDAT];							//下线包
	u8 EthHeartCont[MAXREGDAT];									//心跳包
	u8 EthServerAckHeart[MAXREGDAT];						//心跳响应包
	u8 EthDatType;															//用于存储上述五个包是HEX还是ASCII
	
	u8 EthRegMode;															//注册策略包 0每次连接 1每数据包 2同时

	u8 MqttSubTopic[MAXREGDAT];	 								//mqtt订阅主题	52
	u8 MqttPubTopic[MAXREGDAT]; 								//mqtt发布主题	52
	u8 MqttDeviceID[MQTT_PWD];									//mqtt设备序列号 70
	u8 MqttUserName[MQTT_PWD];	 								//mqtt用户名			70
	u8 MqttPwd[MQTT_PWD];											//mqtt用户名密码		70
	u32 MqttRepTim; 														//mqtt上报时间		4
	u8 MqttRepData; 														//mqtt数据掉线重传  0禁止 1开启
	u8 ProductKey[HW_DEVSECRET_LEN];						//阿里云密钥
	u8 DeviceName_Could[HW_DEVSECRET_LEN];			//***设备名称  4-32
	u8 DeviceSecret[MAXGPRSUSR];								//***设备密钥
	u8 HWDevId[MAXGPRSIP];											//华为云设备ID 60
	u8 HWDevSecret[HW_DEVSECRET_LEN];						//***设备密钥  40
	u8 HWSerId[MQTT_PWDLEN_MAX];								//***服务ID    20
	u8 HWServer[MAXGPRSIP];											//***服务器地址   60
	u8 AliyunSet;																//0x01 阿里云 0x10 华为云			
	
	u8 Workfreq;																//工作频率
	u8 TerminalRepCycle; 												//终端上报周期
	
	u8 NetLinkPriority;														//连接优先级

} SYSINFO;


//本机用户配置信息
typedef struct
{
	u16 isReboot;															//重启标识
	u8 isChinese;															//是否为中文  用在短信发送中
	u8 isWriteFlag;														//写入默认配置标志 0x11不用写 0需要写
	u8 version[15];														//版本设置
	u8 SmsTelNo[PHONELEN];										//电话号码
	u8 SmsTelNoIn;														//短信电话号码格式
	u8 SmsTelNoLen;														//短信电话号码长度
	

	u8 year;
	u8 month;
	u8 day;
}USERINFO;

//终端设备 存储信息	WT107 WT102 WT108 WT109
typedef struct
{
	//设备是否有效
	u8 isExist;
	//终端类型 0 WT107 1 WT108 2 WT109 WT102 WT100 WT102 
	u8 Type;
	//设备ID
	u8 deviceId;
	u8 devName[DESCRIBE_DEVICE];
	//光照度 温度 湿度 CO2 土壤水分
	u8 name[7][DESCRIBE_NAME_LEN];
	//数据高低限
	u16 HighLim[7];
	u16 LowLim[7];
	
	u8 temp[2]; 				//lowLim[7] 会影响后面数据需要间隔两个字节
	u8 uniId[6];
}TERMINALINFO_WT2;

//相关数据记录
typedef struct
{
	u16 seq;		//序号
	//时间
	u8	year,mon,day,hour,min,sec;
	
	u16	BatVal;		//主机电池电压
	u8	GsmVal;		//GSM信号
	
	u8	DeviceId;	//终端ID
	u8 	DevType;	//终端类型 0WT107 1WT108 2WT109
	u16 Temp;			//温度
	u16	Humi;			//湿度
	
	u16 Illu;			//光照度
	u16	Co2;			//CO2浓度
	u16 Tvoc;			//环境质量
	
	u16 SoilTemp; //土壤温度
	u16	SoilHumi;	//土壤水分 土壤湿度
}HISTORY;

//flash读写的地址模式  用去区别读写类型
enum FLASH_OPE_MODE{
	SYSINFO_MODE = 0,
	USERINFO_MODE,
	TERMINALWT2_MODE,
	HISTORYCORD_MODE,
	HISTORY_REPORTCORD_MODE,
	ALARM_MODE
};

enum NETLINK_PRIORITY
{
	ETHER_NETWORK = 0,
	CELLULAR_NETWORK,
	ALL_NETWORK,
	
};

extern SYSINFO SysInfo;

extern USERINFO UserInfo;

extern TERMINALINFO_WT2 DevInfo[DEVICE_MAX];

extern HISTORY History;

void ClearSysInfo(void);

void SetSysInfoDef(void);


void SysInfoInit(void);

void SaveSysInfo(void);

void ReadSysInfo(void);

void ReadUserInfo(void);

void SaveUserInfo(void);


void DevInfoInit(void);

void SaveDevWT2Config(u16 ptr);

u8 ReadDevWT2Config(u8 Index);

void DelDevWT2Config(u16 ptr);

void ClearHistory(u8 mode);

void ReadHistroy(u8 *InBuy,u16 ptr,u8 mode);

void SaveHistory(u8 Index,u8 type,u8 mode);

void ReadHistroyRep(u8 *InBuy,u16 ptr);

void SaveHistoryRep(u8 Index,u8 type,u8 mode);

void EraseChipOrSector(u32 addr,u8 mode);

void ReadConfig(u8 mode,u8 *InBuf,u8 ptr);

void ReadHistory(u8 mode,u8 *InBuf,u16 ptr);

void WriteConfig(u8 mode,u8 *InBuf,u8 ptr);

void WriteHistory(u8 mode,u8 Index,u8 type,u16 ptr);

void SetHistory(HISTORY *his,u8 Index,u8 type,u16 ptr);

extern u16 historyCnt;													//历史记录数量
extern u16 historyRepCnt;												//历史数据上报记录数量
extern u16 alarmCnt;														//报警记录数量
extern u16 historyPtr;
extern u16 historyRepPtr;							
extern u16 alarmPtr;	

#endif
