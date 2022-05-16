#include "systick.h"
#include "stdio.h"
#include "stdlib.h"

#include "GSM234G.h"
#include "SMSInstruct.h"
#include "hardware.h"
#include "config.h"
#include "iwdg.h"
#include "pub.h"
#include "dealDat.h"

GPRS_DATA_Frame gprs_data_Frame;
//���� ��������
SMS_Data_Frame sms_data_Frame;
//��������ȷ�ϱ�ʶ
int32_t g_flag = 0;
//���� �������ݱ�ʶ
int32_t g_Smsflag = 0;
//GSMģ������Ӧ����
int32_t g_NoRespCnt = 0;
//���յ�IP���ݳ���
int32_t g_netLen = 0;
//��������
u8 g_SmsCnt;
//��ǰ��������
u8 g_SmsTempCnt;
//��ǰ�������
u8 g_CmeErr = 0;
//GSM��������״̬
u8 GSMBusy = 0;
//�汾��� 2 3 4G
u8 ModuleVer = 0;
//IMEI���к�
u8_t g_IMEI[15] = {0};
//ȡCCID�ĺ��λ
u8_t g_SIMCode[8] = {0};

//����GSM �Ƿ���ñ�ʶ
//u8 GsmSetInvalid = 0;
//����APN
u8 NeedSetAPN = 0;
//�ź�ǿ��
u8 g_SigLevel = 0;
//�����ض��ַ� �� >
u8 g_UartDat = 0;
//���ڽ�����һ���ݰ�
//u16 g_GprsPtr_W = 0;
//�Ƿ���Ҫ����
u8 NeedOffline = 0;
u8 SMSFlag = 0;

u16 g_Recptr = 0;
/*
	��ȡ�汾�� GSM
*/
void PutModuleVer(void)
{
	if(ModuleVer == 2)
	{
		version[0] = '2';
	}
	else if(ModuleVer == 3)
	{
		version[0] = '3';
	}
	else if(ModuleVer == 4)
	{
		version[0] = '4';
	}
	S282_Log(version);
}

/*
	��ȡ����״̬
*/
void GSMGetReg(void)
{
	static u8 netFlag = 0;
	//����ע��״̬
	SendCmd("AT+CREG?\r\n",RECOK,1,1000);
	//���總��״̬
	SendCmd("AT+CGATT?\r\n",RECOK,1,1000);
	if(g_flag & (REGOK | CGATT))
	{
		netFlag = 1;
		S282_Log("Gprs network registry OK");
	}
	else
	{
		if(netFlag)
		{
			S282_Log("Gprs network registry Err");
			netFlag = 0;
		}
	}
}

/*
	��ȡSIM�����ź�����
*/
void GSMGetCsq(void)
{
	SendCmd("AT+CSQ\r\n",RECOK,1,500);
}
/*
	��ȡSIM���ж�����Ϣ����
*/
void GSMGetMsgCap(void)
{
	if(ModuleVer == 2)
	{
		WriteComm("AT+CPMS=\"SM\"\r\n");
	}
	else
	{
		WriteComm("AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n");
	}
}
/*
	APN����
*/
u8 GSMSetGprs(void)
{
	u8 ptr = 0;

	//��ʼ�� û�н���APN����
	if(!ISVALIDCHAR(SysInfo.GprsApn[0]))
	{
		return 0;
	}
	u8 *buf = malloc(GSM_LEN);
	//ATָ�� ����APN
	if(ModuleVer == 2)
	{
		sprintf((char *)buf,"%s","AT+QICSGP=1,\"");
		ptr = 13;
	}
	else
	{
		sprintf((char *)buf,"%s","AT+QICSGP=1,1,\"");
		ptr = 15;
	}
	
	ptr += CopyStr(&buf[ptr],&SysInfo.GprsApn[0],MAXGPRSAPN);
	buf[ptr++] = '"';
	buf[ptr++] = ',';
	buf[ptr++] = '"';
	ptr += CopyStr(&buf[ptr],&SysInfo.GprsUsr[0],MAXGPRSUSR);
	buf[ptr++] = '"';
	buf[ptr++] = ',';
	buf[ptr++] = '"';
	ptr += CopyStr(&buf[ptr],&SysInfo.GprsPwd[0],MAXGPRSUSR);
	buf[ptr++] = '"';
	buf[ptr++] = '\r';
	buf[ptr++] = '\n';
	buf[ptr++] = 0;
	if(SendCmd(buf,RECOK,2,3000) == 0)
	{
		free(buf);
		return 0;
	}
	free(buf);
	return 1;
}

/*
	������ ����AT����
*/
void InitGsmBasic(void)
{	
	//����������ʾ
	if(SendCmd("AT+CLIP=1\r\n",RECOK,2,300) == 0)
	{
		S282_Log("clip err");
	}		
	if(ModuleVer == 2)
	{
		//������ȷ��
		if(SendCmd("AT+COLP=1\r\n",RECOK,2,1000) == 0)
		{
			return;
		}		
		//���ý�����ʾIP����ͷ
		SendCmd("AT+QIHEAD=1\r\n",RECOK,2,1000); 
	}
	else	
	{//����URCָʾѡ��
		SendCmd("AT+QURCCFG=\"urcport\",\"uart1\"\r\n",RECOK,2,800);
	}	
	if(SendCmd("AT+CNMI=2,1\r\n",RECOK,2,400) == 0)
	{
		return; //��������	
	}
	if(SendCmd("AT+CMGF=0\r\n",RECOK,2,400) == 0)
	{
		return;		//���Ÿ�ʽ����		
	}
	
	//���ڲ��� = 0
	SendCmd("AT+QSIDET=0\r\n",RECOK,2,400); 
}

/*
 ��ʼ��GSMģ��
*/
u8 GSMInit(void)
{
	u8 i;
	overAllData_Fram.recLen = 0;
	g_flag = 0;
	g_Smsflag = 0;
	g_NoRespCnt = 0;
	
	for(u8 i = 0; i < 3; i++)
	{
		//�汾��ȡ 2G 3G 4G ���� R4A021      CXC1122528 
		SendCmd("AT+CGMR\r\n",RECOK,1,300);
		if(ModuleVer > 0)
		{
			break;
		}
	}
	PutModuleVer();
	//��ȡIMEI���к�
	SendCmd("AT+CGSN\r\n",RECID,3,1000);
	if(SendCmd("AT+CREG=2\r\n",RECOK,2,1000) == 0)
	{//����ע��״̬
		S282_Log("net status err");
		return 1;
	}
	
	
	for(i = 0; i < 5; i++)
	{
		g_flag |= READYSIMID;
		//��ȡSIM����ICCID
		SendCmd("AT+QCCID\r\n",RECOK,1,1000);
		if(g_flag & SIMID)
		{
			if(DB_Flag)
				printf("SIM+CCID = %s\r\n",g_SIMCode);
			break;
		}
	}
	
	if(i >= 5)
	{
		S282_Log("get SIM CCID err");
		return 1;
	}
	SMSFlag = 1;
	//����AT����80
	InitGsmBasic();
	g_flag = 0;
	for(i = 0; i < 5; i++)
	{//��ȡ�ź�
		GSMGetCsq();
		delay_1ms(500);
		GSMGetReg();
		delay_1ms(500);
		if((g_flag & REGOK) || (g_flag & CGATT))
		{
			break;
		}
	}
	if(i >= 5)
	{
		S282_Log("GSM NO NET REG");
	}
	//��ʼ��APN
	GSMSetGprs();
	g_flag |= INITOK;
	GPRSLEDShow();
	
	return 0;
}
/*
	���ڷ���
*/
void WriteComm(u8 *dat)
{
	while(*dat != '\0')
	{
		PutInUart1(*dat++);
	}
}
/*
	ָ���
*/
u8 SendCmd(u8 *cmd,u32 flag,u8 cnt,u16 ms)
{
	if(GSMBusy)
	{
		return 0;
	}
	GSMBusy = 1;
	for(u8 i = 0; i < cnt; i++)
	{
		g_flag &= ~flag;
		WriteComm(cmd);
		for(u16 tims = 0; tims < ms; tims += 2)
		{
			if(g_flag & flag)
			{
				GSMBusy = 0;
				return 1;
			}
			delay_1ms(2);
		}
	}
	GSMBusy = 0;
	return 0;
}
/*
	�ȴ����ݷ��� 
	�жϽ����Ƿ���ȷ
*/
u8 GSMRecOK(u16 ms)
{
	if(GSMBusy == 1)
	{
		return 0;
	}
	GSMBusy = 1;
	g_flag &= ~(RECOK | RECERR);
	for(u16 i = 0; i < ms; i += 2)
	{
		if(g_flag & RECOK)
		{
			GSMBusy = 0;
			return 1;
		}
		else if(g_flag & RECERR)
		{
			GSMBusy = 0;
			return 2;
		}
		delay_1ms(2);
	}
	GSMBusy = 0;
	return 0;
}
/*
	����ATָ��
*/
u8 CheckAt(void)
{
	u8 rec = 0;
	rec = SendCmd("AT\r\n",RECOK,1,200);
	return rec;
}

/*
	���ӶϿ�
*/
void GprsDisConnect(void)
{
	GprsDisConRemote();
	WatchDogFeed();
	delayMs(100);
	//2Gģ��  �ر�����
	if(ModuleVer == 2)
	{
		SendCmd("AT+QIDEACT\r\n",RECOK | RECERR,1,10000);
	}
	else
	{
		SendCmd("AT+QIDEACT=1\r\n",RECOK | RECERR,1,10000);
	}
	g_flag &= ~(GPRSCONNECTED | GPRSSENDOK | TCPCONNECTED | GPRSSENDFAIL);
}
/*
	�ر��������������
*/
void GprsDisConRemote(void)
{
	if(ModuleVer == 2) 
	{
		SendCmd("AT+QICLOSE\r\n",RECOK | RECERR,1,1000);
	}
	else
	{
		SendCmd("AT+QICLOSE=0\r\n",RECOK | RECERR,1,1000);
	}
	g_flag &= ~TCPCONNECTED;
}
/*
	GPRS ���ӷ�����
*/
u8 GprsConnectServer(u8 *TarIP,u16 port)
{
	u8 len;
	u8 *sendBuf = malloc(100);
	if(ModuleVer == 2)
	{//2G
		if(IsIPaVailable(TarIP))
		{
			SendCmd("AT+QIDNSIP=0\r\n",RECOK,1,800);
		}
		else
		{
			SendCmd("AT+QIDNSIP=1\r\n",RECOK,1,800);
		}	
		if((SysInfo.GprsMode & 0x80) == 0)
		{//TCP
			len = CopyStr(sendBuf,"AT+QIOPEN=\"TCP\",\"",50);
		}
		else
		{//UDP
			len = CopyStr(sendBuf,"AT+QIOPEN=\"UDP\",\"",50);
		}
	}
	else
	{//3G 4G
		if((SysInfo.GprsMode & 0x80) == 0)
		{//TCP
			len = CopyStr(sendBuf,"AT+QIOPEN=1,0,\"TCP\",\"",50);
		}
		else
		{//UDP
			len = CopyStr(sendBuf,"AT+QIOPEN=1,0,\"UDP\",\"",50);
		}
	}
	
	len += CopyStr(&sendBuf[len],TarIP,MAXGPRSIP);
	sendBuf[len++] = '"';
	sendBuf[len++] = ',';
	
	if(ModuleVer == 2)
	{
		sendBuf[len++] = '"';
		len += NumToStr(&sendBuf[len],port);
		sendBuf[len++] = '"';
	}
	else
	{
		len += NumToStr(&sendBuf[len],port);
		len += CopyStr(&sendBuf[len],",0,1",5);
	}
	sendBuf[len++] = '\r';
	sendBuf[len++] = '\n';
	sendBuf[len++] = '\0';
	
	g_flag &= ~TCPCONNECTED;
	SendCmd(sendBuf,TCPCONNECTED | GPRSCONNFAIL,1,5000);
	free(sendBuf);
	if(g_flag & TCPCONNECTED)
	{//���ӳɹ�
		g_flag |= GPRSCONNECTED;
		return 0;
	}
	S282_Log("GPRS Connect Fail");
	return 1;
}
/*
	����2Gģ���� �Ƿ������ݷ���
	����Ϊ�������� ��ߴ�������Ч��
*/
__inline void GprsAnalysisCmd2G(void)
{
	//SEND OK
	if(overAllData_Fram.recBuf[0] == 'S' && overAllData_Fram.recBuf[1] == 'E' && overAllData_Fram.recBuf[2] == 'N'
		&& overAllData_Fram.recBuf[3] == 'D' && overAllData_Fram.recBuf[5] == 'O')
	{
		g_flag |= GPRSSENDOK;
	}
	//SEND FAIL
	else if(overAllData_Fram.recBuf[0] == 'S' && overAllData_Fram.recBuf[1] == 'E' && overAllData_Fram.recBuf[2] == 'N'
		&& overAllData_Fram.recBuf[3] == 'D' && overAllData_Fram.recBuf[5] == 'F')
	{
		g_flag |= GPRSSENDFAIL;
	}
	//CLOSED
	//CLOSE OK
	else if(CompStr(overAllData_Fram.recBuf,"CLOSED",6))
	{
		g_flag &= ~TCPCONNECTED;
		g_flag |= RECOK;
	}
	//+PDP DEACT
	else if(CompStr(overAllData_Fram.recBuf,"+PDP DEACT",10))	
	{
		g_flag &= ~TCPCONNECTED;		
	}
	//DEACT OK	
	else if(CompStr(overAllData_Fram.recBuf,"DEACT OK",8))
	{
		g_flag &= ~(GPRSCONNECTED | TCPCONNECTED);	
		g_flag |= RECOK;	

	}
	//ALREADY CONNECT
	else if(CompStr(overAllData_Fram.recBuf,"ALREADY C",9))	
	{		
		g_flag |= TCPCONNECTED;

	}
	//CONNECT OK
	else if(CompStr(overAllData_Fram.recBuf,"CONNECT OK",10))
	{
		g_flag |= TCPCONNECTED;
	}
		//CONNECT FAIL
	else if(CompStr(overAllData_Fram.recBuf,"CONNECT FA",10))
	{
		g_flag |=GPRSCONNFAIL;		
	}
}

/*
	����3G 4Gģ���� �Ƿ������ݷ���
*/
__inline void GprsAnalysisCmd3GOr4G(void)
{
		if(CompStr(&overAllData_Fram.recBuf[1],"QIU",3))
		{
			//+QIURC: ��recv��,0,4
			//*QIURC: "recv",0,12
			//+QIURC: "recv",0,177 
			if(overAllData_Fram.recBuf[9] == 'r' && overAllData_Fram.recBuf[10] == 'e' 
				&& overAllData_Fram.recBuf[15] == '0')
			{
				g_netLen = 0;
				if(overAllData_Fram.recBuf[18] == 0x0d) 
				{//��λ
					g_netLen = overAllData_Fram.recBuf[17] - '0';
					g_flag |= GPRSIPDDATA;
				}
				else if(overAllData_Fram.recBuf[19] == 0x0d)
				{//�ڶ������ ��λ��
					g_netLen = (overAllData_Fram.recBuf[17] - '0') * 10 + (overAllData_Fram.recBuf[18] - '0');
					g_flag |= GPRSIPDDATA;
				}
				else if(overAllData_Fram.recBuf[20] == 0x0d){
					g_netLen = (overAllData_Fram.recBuf[17] - '0') * 100 + (overAllData_Fram.recBuf[18] - '0') * 10+(overAllData_Fram.recBuf[19] - '0');
					g_flag |= GPRSIPDDATA;
				}
				return;
			}
			//+QIURC: "closed",0
			if(overAllData_Fram.recBuf[9] == 'c' && overAllData_Fram.recBuf[10] == 'l'){
				g_flag &= ~TCPCONNECTED;				
				return;
			}
		}	
		//+QIOPEN: 0,0
		if(overAllData_Fram.recBuf[1] == 'Q' && overAllData_Fram.recBuf[2] == 'I' && overAllData_Fram.recBuf[3] == 'O'
			&& overAllData_Fram.recBuf[4] == 'P' && overAllData_Fram.recBuf[9] == '0')
		{			
			if(overAllData_Fram.recBuf[11] == '0')
			{
				g_flag |= TCPCONNECTED;
				return;
			}
			else
			{
				g_flag &= ~TCPCONNECTED;
			}
		}
	//SEND OK
	if(CompStr(overAllData_Fram.recBuf,"SEND OK",7))	
	{
		g_flag |= GPRSSENDOK;
		return;
	}
	//SEND FAIL
	if(CompStr(overAllData_Fram.recBuf,"SEND FA",7))
	{
		g_flag |= GPRSSENDFAIL;
		return;
	}
	
	//CLOSED
	//CLOSE OK
	if(CompStr(overAllData_Fram.recBuf,"CLOSED",6))
	{
		g_flag &= ~TCPCONNECTED;
		g_flag |= RECOK;	
		return;
	}
	//+PDP DEACT
	if(CompStr(overAllData_Fram.recBuf,"+PDP DEACT",10))	
	{
		g_flag &= ~TCPCONNECTED;		
		return;
	}
	//DEACT OK	
	if(CompStr(overAllData_Fram.recBuf,"DEACT OK",8))
	{
		g_flag &= ~(GPRSCONNECTED | TCPCONNECTED);	
		g_flag |= RECOK;	
		return;
	}
}		

/*
	����Sim���Ƿ���յ��Ķ���
*/
__inline void SimAnalysisCmd(void)
{
	u16 i,j;
	if(overAllData_Fram.recBuf[0] == '+' && overAllData_Fram.recBuf[1] == 'C')
	{	
		if(overAllData_Fram.recBuf[3] == 'T' && overAllData_Fram.recBuf[4] == 'I')
		{
			g_Smsflag |= NEWSMSCOME;
			return;
		}
		//+CMGR: ������
		if(overAllData_Fram.recBuf[2] == 'M' && overAllData_Fram.recBuf[3] == 'G' 
			&& overAllData_Fram.recBuf[4] == 'R')
		{
			g_Smsflag |= READYRECSMSPDU;
		}
			//+CPMS: 7,40,7,40,7,40 �����ڴ�
		if(overAllData_Fram.recBuf[2] == 'P' && overAllData_Fram.recBuf[3] == 'M' 
			&& overAllData_Fram.recBuf[4] == 'S')
		{
			g_SmsCnt = 0;
			g_SmsTempCnt = 0;
			
			for(i = 7; i < overAllData_Fram.recLen; i++)
			{
				j = overAllData_Fram.recBuf[i];
				if(j >= 0x30 && j <= 0x39)
				{
					g_SmsTempCnt = g_SmsTempCnt * 10 + j - 0x30;
				}
				else
				{
					break;
				}
			}
			for( ++i; i < overAllData_Fram.recLen; i++)
			{
				j = overAllData_Fram.recBuf[i];
				if(j >= 0x30 && j <= 0x39)
				{
					g_SmsCnt = g_SmsCnt * 10 + j - 0x30;
				}
				else
				{
					break;
				}
			}
			g_Smsflag |= RECSMSCAP;				
		}  	
	}
}

#pragma O3 Otime
/*
	�������ڽ��յ�������
	����buf: overAllData_Frame.recBuf
	����len: overAllData_Frame.recLen
*/
static void GSMToAnalysisData(void)
{
	//����Ӧ  ����Ӧ��������
	g_NoRespCnt = 0;
	//2Gģ��  ���ָ���ж�
	if(ModuleVer == 2)
	{
		GprsAnalysisCmd2G();
	}
	else
	{
		GprsAnalysisCmd3GOr4G();
	}
	//���ŷ���
	SimAnalysisCmd();
	
	if(overAllData_Fram.recBuf[0] == 'O' && overAllData_Fram.recBuf[1] == 'K')
	{
		g_flag |= RECOK;
		return;
	}
	else if(CompStr(overAllData_Fram.recBuf,"ERRO",4))
	{
		g_flag |= RECERR;
		return;
	}
	else if(CompStr(overAllData_Fram.recBuf,"BUSY",4))
	{
		g_flag |= REMOTEEND;
		return;
	}
	else if(CompStr(overAllData_Fram.recBuf,"RING",4))
	{
		g_flag |= RINGCOME;
	}
	//NORMAL POWER DOWN
	else if(overAllData_Fram.recBuf[0] == 'N' && overAllData_Fram.recBuf[1] == 'O' && 
		overAllData_Fram.recBuf[2] == 'R' && overAllData_Fram.recBuf[7] == 'P' && overAllData_Fram.recBuf[8] == 'O' && 
	overAllData_Fram.recBuf[13] == 'D' && overAllData_Fram.recBuf[14] == 'O')
	{		
		g_flag |= POWERDOWN;
	}
	//UNDER_VOLTAGE POWER DOWN	
	else if(overAllData_Fram.recBuf[0] == 'U' && overAllData_Fram.recBuf[1] == 'N' && 
		overAllData_Fram.recBuf[2] == 'D' && overAllData_Fram.recBuf[6] == 'V' && 
	overAllData_Fram.recBuf[14] == 'P' && overAllData_Fram.recBuf[15] == 'O')
	{		
		g_flag |= POWERDOWN;
	}
	else if(CompStr(overAllData_Fram.recBuf,"AT+",3))
	{
		if(overAllData_Fram.recBuf[3] == 'C' && overAllData_Fram.recBuf[4] == 'G' && 
			overAllData_Fram.recBuf[5] == 'S')
		{//�ٴν��յ������к�  �����Ƚ����˳�  �ȴ����ڽ���
			g_flag |= READYID;
			return;
		}
		if(overAllData_Fram.recBuf[3] == 'C' && overAllData_Fram.recBuf[4] == 'G' &&
			overAllData_Fram.recBuf[5] == 'M' && overAllData_Fram.recBuf[6] == 'R')
		{
			ModuleVer = 1;
			return;
		}
	}
	
	if(ModuleVer == 1)
	{
		for(u8 i = 0; i < 15; i++)
		{//ֱ����ѯ����Ӧ�İ汾
			if(CompStr(&overAllData_Fram.recBuf[i],"M35",3)){
				ModuleVer = 2;
				break;
			}
			else if(CompStr(&overAllData_Fram.recBuf[i],"UC15",4)){
				ModuleVer = 3;
				break;
			}
			else if(CompStr(&overAllData_Fram.recBuf[i],"EC2",3)){
				ModuleVer = 4;
				break;
			}
		}
	}
	
	if(g_flag & READYID)
	{
		if(overAllData_Fram.recBuf[0] >= '0' && overAllData_Fram.recBuf[0] <= '9')
		{//imei
			g_flag &= ~READYID;
			for(u8 i = 0; i < overAllData_Fram.recLen && i < 15; i++)
			{ //IMEI���к� ��15λ
				g_IMEI[i] = overAllData_Fram.recBuf[i];
			}
			g_flag |= RECID;
			return;
		}
	}
	
	if(g_flag & READYSIMID)
	{
		if(overAllData_Fram.recBuf[0] >= '0' && overAllData_Fram.recBuf[0] <= '9')
		{//imei
			g_flag &= ~READYID;
			for(u8 i = 0; i < 8; i++)
			{ //CCID���к� ��20λ
				//ȡ�û���ʶ
				g_SIMCode[i] = overAllData_Fram.recBuf[overAllData_Fram.recLen - 9 + i];
			}
			g_flag |= SIMID;
			return;
		}	
	}
	
	if(overAllData_Fram.recBuf[0] == '+')
	{
		if(overAllData_Fram.recBuf[1] == 'C')
		{
			if(CompStr(&overAllData_Fram.recBuf[1],"CSQ",3))
			{
				g_SigLevel = 0;
				//ֻ��һλ�����ź�
				if(overAllData_Fram.recBuf[7] == ',')
				{
					g_SigLevel = overAllData_Fram.recBuf[6] - '0';
				}
				else
				{
					g_SigLevel = (overAllData_Fram.recBuf[6] - '0') * 10 + overAllData_Fram.recBuf[7] - '0';
					if(g_SigLevel == 99)
					{
						g_SigLevel = 0;
					}
				}
				return;
			}
			else if(CompStr(&overAllData_Fram.recBuf[1],"CLCC",4))
			{
				//+CLCC: 1,1,0,0,0,"0242  //����ͨ��ֱ�ӷ���OK
				//+CLCC: 1,0,3,0,0,"13504032056",129,""������
				if(overAllData_Fram.recBuf[9] == '0')
				{ //
					if(overAllData_Fram.recBuf[11]=='3')
					{
						g_flag |= ACTIVCALL;	
					}
					if(overAllData_Fram.recBuf[11] == '0')
					{						
						g_flag |= CALLOK;
					}					
				}
			}
			else if(overAllData_Fram.recBuf[2] == 'M' && overAllData_Fram.recBuf[5] == 'E')
			{
				g_flag |= RECERR;
				if(overAllData_Fram.recBuf[12] == '1' && overAllData_Fram.recBuf[13] == '0' && 
					overAllData_Fram.recBuf[14] < 0x20)
				{
					g_flag |= NOSIM;
				}
				else if(overAllData_Fram.recBuf[13] < 0x20)
				{
					g_CmeErr = (overAllData_Fram.recBuf[12] - '0');
				}
				else
				{
					g_CmeErr = (overAllData_Fram.recBuf[12] - '0') * 10 + (overAllData_Fram.recBuf[13] - '0');
				}
				return;
			}
			//+CPIN: READY
			else if(CompStr(&overAllData_Fram.recBuf[2],"PIN: RE",7))
			{
				g_flag |= PINOK;
			}
	
			else if(CompStr(&overAllData_Fram.recBuf[2],"PIN: SI",7)){
				g_flag |= WAITPIN;
			}
			else if(CompStr(&overAllData_Fram.recBuf[2],"OLP",3))
			{
				g_flag |= CALLOK;
			}
			//+CLIP: "02486806939",129,,,,0
			else if(CompStr(&overAllData_Fram.recBuf[2],"LIP",3))
			{
				overAllData_Fram.recLen = 0;
			}
			//+CGATT: 1   //20200321
			else if(CompStr(&overAllData_Fram.recBuf[2],"GATT",4))
			{
				if(overAllData_Fram.recBuf[8] == '1')
				{
					g_flag |= CGATT;		
				}
				else 
				{
					g_flag &= ~CGATT;
				}
			}	
			//+CREG: 2,5,"4104","5DF3"//��ѯ�õ�
			//+CREG: 0,5//��ѯ�õ�
			//+CREG: 5,"4104","A65F"	�����ͳ�	
			//+CREG: 1  +CREG: 3  +CREG: 5  �����ͳ�
			//AT+CREG?			
			else if(CompStr(&overAllData_Fram.recBuf[2],"REG",3))
			{
				if((overAllData_Fram.recBuf[10] == ',' && (overAllData_Fram.recBuf[9] == '1' || overAllData_Fram.recBuf[9] == '5'))
					|| (overAllData_Fram.recBuf[8] == ',' && (overAllData_Fram.recBuf[7] == '1' || overAllData_Fram.recBuf[7] == '5')))
				{
					g_flag |= REGOK;												
				}
				else
				{
					g_flag &= ~REGOK;
				}
			}	
		}
		else if(overAllData_Fram.recBuf[1] == 'Q')
		{
			if(CompStr(&overAllData_Fram.recBuf[2],"CCID",4))
			{
				u8 j = 0;
				for(u8 i = 8; i< overAllData_Fram.recLen && j < 8;i++)
				{
					g_SIMCode[j] = overAllData_Fram.recBuf[i];
					if(overAllData_Fram.recBuf[i] < 0x30)
					{
						break;
					}
					j++;
				}
				g_SIMCode[j]=0;
				g_flag|=SIMID;
			}
		}
	}
}

/*
	��GSM�д���������
*/
void UartToGSM(u8 dat)
{
	if(dat == g_UartDat)
	{//����ָ���ַ�
		g_flag |= RECCHAR;
	}
	if(g_Smsflag & READYRECSMSPDU)	//��������
	{
		if(dat == 0x0A)
		{//������
			g_Smsflag &= ~READYRECSMSPDU;
			g_Smsflag |= RECSMSOK;
			sms_data_Frame.recLen = g_Recptr;
			g_Recptr = 0;
		}
		else
		{
			if(dat >= 0x41) //ABCDEF -> A:0x0A
			{
				dat = dat - 0x41 + 0x0A;
			}
			if(g_Recptr < SMS_RECLEN * 2 - 1)
			{
				if(g_Recptr & 0x01)
				{//���յ�������Ϊ�ߵ�λ ��Ҫ���д��� �Ƚ��ո�λ���ٽ�����
					sms_data_Frame.recbuf[g_Recptr / 2] = sms_data_Frame.recbuf[g_Recptr / 2] | (dat & 0x0F);
				}
				else 
				{
					sms_data_Frame.recbuf[g_Recptr / 2] = dat << 4;
				}
				g_Recptr++;
			}
		}
	}
	else
	{
		if((ModuleVer == 2) && (g_flag & GPRSIPDLEN))	//����IPDͷ
		{
			if(dat == ':')
			{	//������ ����������
				g_Recptr = 0;	
				g_flag &= ~GPRSIPDLEN;
				if(g_netLen > 0)
				{
					g_flag |= GPRSIPDDATA;
				}
			}
			else
			{
				if(dat >= 0x30 && dat <= 0x39)
				{//����
					g_netLen = g_netLen * 10 + (dat - 0x30);
				}
				else
				{
					//�յ�����������
					g_flag &= ~(GPRSIPDDATA | GPRSIPDLEN);
				}
				if(g_netLen > GPRSDATALEN) 
				{
					g_flag &= ~(GPRSIPDDATA | GPRSIPDLEN);//�յ�����������
				}
			}
		}
		else
		{
			if(g_flag & GPRSIPDDATA)	//IP���ݰ�
			{
				gprs_data_Frame.recBuf[g_Recptr++] = dat;
				if(g_Recptr >= g_netLen)
				{
					g_flag &= ~GPRSIPDDATA;
					g_flag |= GPRSPACKOK;
					gprs_data_Frame.recLen = g_netLen;
					g_Recptr = 0;
				}
			}
			else	//��������
			{
				if(dat == 0x0A)	//���ݽ������
				{
					if(overAllData_Fram.recLen > 2)
					{//0D 0A
						overAllData_Fram.recBuf[overAllData_Fram.recLen] = '\0';
						//���ݷ���
						GSMToAnalysisData();
					}
					overAllData_Fram.recLen = 0;
				}
				else
				{//�����������ݺ��Բ�����
					if(overAllData_Fram.recLen < SEND_DATA_MAX)
					{
						overAllData_Fram.recBuf[overAllData_Fram.recLen] = dat;
					}
					overAllData_Fram.recLen++;
				}
				
				if(ModuleVer == 2 && overAllData_Fram.recLen == 3 && (overAllData_Fram.recBuf[0] == 'I') 
						&& (overAllData_Fram.recBuf[1] == 'P') && (overAllData_Fram.recBuf[2] == 'D'))
				{//���յ�IPD���ݰ�ͷ��
						g_flag	|=	GPRSIPDDATA | GPRSIPDLEN;
						g_netLen = 0;
						overAllData_Fram.recLen = 0;
						g_Recptr = 0;
				}
			}
		}
	}
	
}
/*
	�ȴ������ض��ַ�
*/
u8 WaitGsmChar(u8 dat,u16 ms)
{
	u16 i;
	g_flag &= ~RECCHAR;
	g_UartDat = dat;
	for(i = 0; i < ms; i += 2)
	{
		if(g_flag & RECCHAR)
		{
			break;
		}
		delay_1ms(2);
	}	
	g_flag &= ~RECCHAR;
	if(i >= ms)
	{
		return 0;
	}
	return 1;
}

/*
	��16��������ASC����ʽ���ͳ�ȥ
*/
void SendHexAsAsc(u8 dat)
{
	if((dat >> 4) > 9)
	{
		PutInUart1((dat >> 4) - 9 + 0x40);//abcdef
	}
	else
	{
		PutInUart1(((dat >> 4) + 0x30));
	}
	if((dat & 0x0f) > 9)
	{
		PutInUart1(((dat & 0x0f) - 9 + 0x40));//abcdef
	}
	else 
	{
		PutInUart1(((dat & 0x0f) + 0x30));
	}
}
/*
	GpRS���ݷ���
*/
u8 SendGprsDat(u8 *InBuf,u16 actionLen)
{
	if(SysInfo.GprsMode == 0)
	{
		return 1;
	}
	if(ModuleVer == 2)
	{
		WriteComm("\r\nAT\r\n");
	}
	delay_1ms(20);
	if(actionLen >= 1000)
	{
		return 1;
	}
	if(ModuleVer == 2)
	{
		WriteComm("AT+QISEND=");
	}
	else 
	{
		WriteComm("AT+QISEND=0,");
	}
	
	if(actionLen < 10)
	{//0 - 9
		PutInUart1(actionLen + '0');
	}
	else if(actionLen < 100)
	{//10 - 99
		PutInUart1((actionLen / 10) + '0');
		PutInUart1((actionLen % 10) + '0');
	}
	else if(actionLen < 1000)
	{//100
		PutInUart1((actionLen / 100) + '0');
		PutInUart1((actionLen / 10 % 10) + '0');
		PutInUart1((actionLen % 10) + '0');
	}
	WriteComm("\r\n");
	//�ȴ�����>�󣬷�������
	if(WaitGsmChar('>',2000))
	{//��Ҫ���ӳٲ��ܳ�
		delay_1ms(50);
		for(u16 i = 0; i < actionLen; i++)
		{
			PutInUart1(InBuf[i]);
		}
		g_flag &= ~(GPRSSENDOK | GPRSSENDFAIL);
		for(u16 i = 0; i < 2000; i++)
		{
			if(g_flag & GPRSSENDOK)
			{
				return 1;
			}
			else if(g_flag & GPRSSENDFAIL)
			{
				break;
			}
			delay_1ms(4);
		}
	}
	else
	{
		S282_Log("GPRS NO response");
	}
	g_flag &= ~TCPCONNECTED;
	// ����ʧ�� ��������
	g_flag |= GPRSNEEDRECONN;
	return 0;
}

