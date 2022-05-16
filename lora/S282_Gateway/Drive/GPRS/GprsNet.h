
#ifndef _GPRS_NET_H_
#define _GPRS_NET_H_

#include "def.h"

//����״̬ö����
enum GPRSSTATE {	
	
	S_FREE = 0,	  	//0	���У�û����
	S_READYCONNECT,	//1	׼�����ӷ���
	S_REGGING,			//2	ע�ᵽ���� 
	S_WAITREGACK,		//3 �ȴ�ע�����		
	S_REGOK,	  		//4 ע��ɹ�
	S_WAITHBACK, 		//5 �ȴ�������Ӧ
	S_WAITHISTACK,	//6 �ȴ�
	S_WAITDATAACK, 	//7 �ȴ����ݰ�
	
};

enum SMSSTATE {
	S_SMS_FREE = 0,	//0	����
	S_SMS_READCAP,	//1 ��ȡ��������
	S_SMS_READ,			//2	������
	S_SMS_READWAIT,	//3 �ȴ���
	S_SMS_DEL,			//4	ɾ������
	S_SMS_ANA				//5 ��������
};

void GprsRunning(void);

void GprsFun(void);

void SmsFun(void);

void GprsGetServerTime(void);

#endif
