 
#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "def.h"
/*
	W25x60 8MBYTE 8*16Block 8*16*16���� ÿ������4k
	ÿҳ��ַ 0x1000
	����ַ 0x7fffff
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
//���ݳ���
#define SYS_TXT_DATLEN				50
#define MQT_TXT_DATLEN				70

#define MQTT_PWD							72
#define MQTT_TOPICLEN_MAX 		30
#define MQTT_DEVICEIDLEN_MAX	30
#define MQTT_USERNAMELEN_MAX 	20
#define MQTT_PWDLEN_MAX				20
#define HW_DEVSECRET_LEN			34

//�ն��豸 ��һ������ڵ�һҳ  
#define TERMINAL_ADDR					0
//ÿ���豸�洢��Ϣ��256byte
#define DEV_ADDR(ptr)					(TERMINAL_ADDR + ptr * 4096)		
#define DEV_ADDR_1(ptr)				(0x10000 + ptr * 4096)	

//��ʷ��¼������ֵ
#define MAXRECORDNO						2000
//��ʷ��¼�洢����ַ
#define HISRECORDBASIC				0x20000
//��ʷ��¼�洢��ַ һ���Զ�д����
#define HISRECORD_ADDR(ptr)		(HISRECORDBASIC + (ptr) * 32)

//������¼�洢��ַ һ���Զ�д����
//������¼������ֵ
#define MAXALARM						500
#define ALARM_BASIC				0x30000
#define ALARM_ADDR(ptr)		(ALARM_BASIC + ptr * 32)

//MQTT�����ϱ���¼
#define HISTORY_REPCORDBASIC	0x40000
#define HISTORY_REPDATCORD_ADDR(ptr)	(ptr * 32+ HISTORY_REPCORDBASIC)

//ͨ��������󳤶�
#define DESCRIBE_NAME_LEN			20
#define DESCRIBE_DEVICE				DESCRIBE_NAME_LEN + 10
//�ն��豸��
#define	DEVICE_MAX 						50
//�Ĵ����������
#define	MAXREG								(DEVICE_MAX * 7)

//���翪��ö����
enum ConnType
{
	CT_DISABLE = 0,
	CT_MODBUS,
	CT_MQTT
};

enum DevType
{
	DEV_WT107 = 1, //7������
	DEV_WT108,  //2
	DEV_WT109,  //2
	DEV_WT102,   
	DEV_WT100,
	//3 - 6 ����ʪ��
	DEV_WT103,   //3
	DEV_WT104,
	DEV_WT105,
	DEV_WT106,
	
	DEV_WT101,	//4
	DEV_WT110,	//7������
	DEV_WT111,
};

//�����豸������Ϣ
// 964 BYTE
typedef struct
{	
	u8 PassCode[4];															//�豸����
	u16 DeviceId; 															//�豸Id
	u8 DeviceName[MAXDEVICENAME];								//�豸���ƻ�����

	u8 GprsMode;																//GPRSģʽ 0 �ر� 1 ModBus 2 MQTT	0x8x UPDЭ��  0x0x TCPЭ��
	u8 GprsApn[MAXGPRSAPN]; 										//��Ӫ������APN
	u8 GprsUsr[MAXGPRSUSR];											//�û���
	u8 GprsPwd[MAXGPRSUSR];											//����
	u8 GprsServerIP[MAXGPRSIP];									//������IP��ַ
	u16	GprsServerPort;													//�˿ں�

	u16 GprsBeatHeart;													//����
	u16 GprsFreeOffline;												//������������	
	u16 GprsRetryCnt;														//��������				
//	u16 GprsReconnSec;												//��������ʱ��

	u8 GprsRegDat[MAXREGDAT];										//ע��� 50λΪ���ݳ��� 51λΪ��������
	u8 GprsServerAckReg[MAXREGDAT];							//ע����Ӧ��
	u8 GprsServerKickOff[MAXREGDAT];						//���߰�
	u8 GprsHeartCont[MAXREGDAT];								//������
	u8 GprsServerAckHeart[MAXREGDAT];						//������Ӧ��
	u8 GprsDatType;															//���ڴ洢�����������HEX����ASCII
	
	u8 GprsRegMode;															//ע����԰� 0ÿ������ 1ÿ���ݰ� 2ͬʱ  

	u8 Rj45Mode;																//��̫��ģʽ 0 ��ֹ 1 ModBus 2 MQTT  0x8x UPDЭ��  0x0x TCPЭ��
	u8 Dhcp;																	//��̬��̬
	u8 LocalIp[4];															//����IP							
	u16 LocalPort;															//�����˿ں�
	u8 SubNetMask[4];														//��������
	u8 GateIp[4];																//����IP
	u8 DnsIp1[4];																//��DNS1
	u8 DnsIp2[4];																//��DNS2
	u8 EthTarIP[MAXGPRSIP]; 										//Ŀ��IP��ַ
	u16	EthTarTcpPort;													//�˿ں�

	u16 EthBeatHeart;														//����	
	u16 EthFreeOffline;													//��������ʱ��
	u8 EthRetryCnt;														//��������

	u8 EthRegDat[MAXREGDAT];										//ע���
	u8 EthServerAckReg[MAXREGDAT];							//ע����Ӧ��
	u8 EthServerKickOff[MAXREGDAT];							//���߰�
	u8 EthHeartCont[MAXREGDAT];									//������
	u8 EthServerAckHeart[MAXREGDAT];						//������Ӧ��
	u8 EthDatType;															//���ڴ洢�����������HEX����ASCII
	
	u8 EthRegMode;															//ע����԰� 0ÿ������ 1ÿ���ݰ� 2ͬʱ

	u8 MqttSubTopic[MAXREGDAT];	 								//mqtt��������	52
	u8 MqttPubTopic[MAXREGDAT]; 								//mqtt��������	52
	u8 MqttDeviceID[MQTT_PWD];									//mqtt�豸���к� 70
	u8 MqttUserName[MQTT_PWD];	 								//mqtt�û���			70
	u8 MqttPwd[MQTT_PWD];											//mqtt�û�������		70
	u32 MqttRepTim; 														//mqtt�ϱ�ʱ��		4
	u8 MqttRepData; 														//mqtt���ݵ����ش�  0��ֹ 1����
	u8 ProductKey[HW_DEVSECRET_LEN];						//��������Կ
	u8 DeviceName_Could[HW_DEVSECRET_LEN];			//***�豸����  4-32
	u8 DeviceSecret[MAXGPRSUSR];								//***�豸��Կ
	u8 HWDevId[MAXGPRSIP];											//��Ϊ���豸ID 60
	u8 HWDevSecret[HW_DEVSECRET_LEN];						//***�豸��Կ  40
	u8 HWSerId[MQTT_PWDLEN_MAX];								//***����ID    20
	u8 HWServer[MAXGPRSIP];											//***��������ַ   60
	u8 AliyunSet;																//0x01 ������ 0x10 ��Ϊ��			
	
	u8 Workfreq;																//����Ƶ��
	u8 TerminalRepCycle; 												//�ն��ϱ�����
	
	u8 NetLinkPriority;														//�������ȼ�

} SYSINFO;


//�����û�������Ϣ
typedef struct
{
	u16 isReboot;															//������ʶ
	u8 isChinese;															//�Ƿ�Ϊ����  ���ڶ��ŷ�����
	u8 isWriteFlag;														//д��Ĭ�����ñ�־ 0x11����д 0��Ҫд
	u8 version[15];														//�汾����
	u8 SmsTelNo[PHONELEN];										//�绰����
	u8 SmsTelNoIn;														//���ŵ绰�����ʽ
	u8 SmsTelNoLen;														//���ŵ绰���볤��
	

	u8 year;
	u8 month;
	u8 day;
}USERINFO;

//�ն��豸 �洢��Ϣ	WT107 WT102 WT108 WT109
typedef struct
{
	//�豸�Ƿ���Ч
	u8 isExist;
	//�ն����� 0 WT107 1 WT108 2 WT109 WT102 WT100 WT102 
	u8 Type;
	//�豸ID
	u8 deviceId;
	u8 devName[DESCRIBE_DEVICE];
	//���ն� �¶� ʪ�� CO2 ����ˮ��
	u8 name[7][DESCRIBE_NAME_LEN];
	//���ݸߵ���
	u16 HighLim[7];
	u16 LowLim[7];
	
	u8 temp[2]; 				//lowLim[7] ��Ӱ�����������Ҫ��������ֽ�
	u8 uniId[6];
}TERMINALINFO_WT2;

//������ݼ�¼
typedef struct
{
	u16 seq;		//���
	//ʱ��
	u8	year,mon,day,hour,min,sec;
	
	u16	BatVal;		//������ص�ѹ
	u8	GsmVal;		//GSM�ź�
	
	u8	DeviceId;	//�ն�ID
	u8 	DevType;	//�ն����� 0WT107 1WT108 2WT109
	u16 Temp;			//�¶�
	u16	Humi;			//ʪ��
	
	u16 Illu;			//���ն�
	u16	Co2;			//CO2Ũ��
	u16 Tvoc;			//��������
	
	u16 SoilTemp; //�����¶�
	u16	SoilHumi;	//����ˮ�� ����ʪ��
}HISTORY;

//flash��д�ĵ�ַģʽ  ��ȥ�����д����
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

extern u16 historyCnt;													//��ʷ��¼����
extern u16 historyRepCnt;												//��ʷ�����ϱ���¼����
extern u16 alarmCnt;														//������¼����
extern u16 historyPtr;
extern u16 historyRepPtr;							
extern u16 alarmPtr;	

#endif
