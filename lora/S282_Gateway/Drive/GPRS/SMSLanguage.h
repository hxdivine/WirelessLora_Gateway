
#ifndef _SMS_LANGUAGE_H_
#define _SMS_LANGUAGE_H_


#define C_FORMATERR  		(EngMode? "SMS Format Error, Please check Caps Lock in Command!":"ָ���ʽ����,��ȷ��!" )


#define C_SETPASSOK  		(EngMode? ",This is the New Password, please remember it carefully.":",����������,���μ�" )
	
#define C_YEAR  				(EngMode? "(Y)":"��")		
#define C_MON  					(EngMode? "(M)":"��")	
#define C_DAY  					(EngMode? "(D)":"��")	
#define C_HOUR  				(EngMode? "(H)":"ʱ")	
#define C_MIN  					(EngMode? "(M)":"��")				
#define C_UTC						(EngMode? "(UTC time)":"(UTC ʱ��)")
	
#define C_MODEL  				(EngMode? "model:":"�ͺ�:")	
#define C_VERSION  			(EngMode? "Version:":"�汾:")	
#define C_IMEI 					("IMEI:")		
#define C_GSMVAL  			(EngMode? "Cellular Network Signal Strength:":"�������ź�ǿ��:")	
#define C_PWR2OFF  			(EngMode? "External DC Power Goes OFF":"�ⲿ��Դ����")
#define C_PWR2ON  			(EngMode? "External DC Power Goes ON":"�ⲿ��Դ�ָ�")
#define C_PWRISOK  			(EngMode? "External DC Power Goes ON":"�ⲿ��Դ����")	
	
#define C_NETPRIORITY		(EngMode? "Net Priority:":"�������ȼ�:")
#define C_SAMETIM				(EngMode? "At the same time":"ͬʱ")
#define C_CELL_PRI			(EngMode? "Cellular network priority":"����������")
#define C_ETH_PRI				(EngMode? "Ethernet first":"��̫������")

#define C_IP  					(EngMode? 		"Server IP:":" ��������ַ:")
#define C_PORT  				(EngMode? 		"Port:":"�˿�:")

#define C_APN  					(EngMode? 		"APN:":"APN:")
#define C_USR  					(EngMode? 		"User ID:":"�û���:")
#define C_PWD  					(EngMode? 		"Password:":"����:")

#define C_ETHIP					(EngMode? "local IP:" :"����IP:")

#define C_RECONT				(EngMode? "Reconnect Count:" :"���ߺ�,�ٴ��Զ����Ӵ���:")
#define C_COUNT					(EngMode? "(freq)":"��")

#define C_SMSREBOOT			(EngMode? "Reboot Successfully" : "�����ɹ�")

#define C_DELDEV1				(EngMode? "Device " :"�ӻ�:")
#define C_DELHIS				(EngMode? "Delete successfully" :"ɾ���ɹ�")
#define C_DEVNOR				(EngMode? "Master-slave communicate normally:" :	"ͨѶ����:")	
#define C_DEVABNOR			(EngMode? "communicate abnormally:" :"ͨѶ�쳣:")
#define C_DEVNO					(EngMode? "No related terminal equipment" :"������ն��豸")

#define C_DEVID					(EngMode? "Device ID:" :	"�豸ID��:")
#define REFACTOK   			(EngMode? "Reset Successfully!":"�������ɹ�")

#define C_GPRS			  	(EngMode? 	"GPRS/3G:":"GPRS/3G:")
#define C_RS232			  	(EngMode? 	"RS485/RS232:":"RS485/RS232:")
#define C_ETH			   		(EngMode? 	"ETH:":"����:")

#define C_DORMAIN				(EngMode? 		"Domain:":" ����:")

#define C_GPRSONLINE  	(EngMode? "GPRS Online":"GPRSһֱ����")

#define C_GPRSOPEN0			(EngMode?"GPRS Open MOBUS":"GPRS����MODBUS")
#define C_GPRSOPEN1			(EngMode?"GPRS Open MQTT":"GPRS����MQTT")
#define C_GPRSCLOSE			(EngMode?"GPRS Close":"GPRS�ر�")

#define C_ETHOPEN0			(EngMode?"ETH Open MODBUS":"��̫������MODBUS")
#define C_ETHOPEN1			(EngMode?"ETH Open MQTT":"��̫������MQTT")
#define C_ETHCLOSE			(EngMode?"ETH Close":"��̫���ر�")

#endif
