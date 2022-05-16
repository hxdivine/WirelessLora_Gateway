
#ifndef _GPRS_NET_H_
#define _GPRS_NET_H_

#include "def.h"

//网络状态枚举类
enum GPRSSTATE {	
	
	S_FREE = 0,	  	//0	空闲，没上线
	S_READYCONNECT,	//1	准备连接服务
	S_REGGING,			//2	注册到服务 
	S_WAITREGACK,		//3 等待注册完成		
	S_REGOK,	  		//4 注册成功
	S_WAITHBACK, 		//5 等待心跳响应
	S_WAITHISTACK,	//6 等待
	S_WAITDATAACK, 	//7 等待数据包
	
};

enum SMSSTATE {
	S_SMS_FREE = 0,	//0	空闲
	S_SMS_READCAP,	//1 读取短信容量
	S_SMS_READ,			//2	读短信
	S_SMS_READWAIT,	//3 等待读
	S_SMS_DEL,			//4	删除短信
	S_SMS_ANA				//5 分析短信
};

void GprsRunning(void);

void GprsFun(void);

void SmsFun(void);

void GprsGetServerTime(void);

#endif
