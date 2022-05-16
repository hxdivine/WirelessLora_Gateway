
#ifndef _SMS_LANGUAGE_H_
#define _SMS_LANGUAGE_H_


#define C_FORMATERR  		(EngMode? "SMS Format Error, Please check Caps Lock in Command!":"指令格式错误,请确认!" )


#define C_SETPASSOK  		(EngMode? ",This is the New Password, please remember it carefully.":",这是新密码,请牢记" )
	
#define C_YEAR  				(EngMode? "(Y)":"年")		
#define C_MON  					(EngMode? "(M)":"月")	
#define C_DAY  					(EngMode? "(D)":"日")	
#define C_HOUR  				(EngMode? "(H)":"时")	
#define C_MIN  					(EngMode? "(M)":"分")				
#define C_UTC						(EngMode? "(UTC time)":"(UTC 时间)")
	
#define C_MODEL  				(EngMode? "model:":"型号:")	
#define C_VERSION  			(EngMode? "Version:":"版本:")	
#define C_IMEI 					("IMEI:")		
#define C_GSMVAL  			(EngMode? "Cellular Network Signal Strength:":"蜂窝网信号强度:")	
#define C_PWR2OFF  			(EngMode? "External DC Power Goes OFF":"外部电源掉电")
#define C_PWR2ON  			(EngMode? "External DC Power Goes ON":"外部电源恢复")
#define C_PWRISOK  			(EngMode? "External DC Power Goes ON":"外部电源正常")	
	
#define C_NETPRIORITY		(EngMode? "Net Priority:":"网络优先级:")
#define C_SAMETIM				(EngMode? "At the same time":"同时")
#define C_CELL_PRI			(EngMode? "Cellular network priority":"蜂窝网优先")
#define C_ETH_PRI				(EngMode? "Ethernet first":"以太网优先")

#define C_IP  					(EngMode? 		"Server IP:":" 服务器地址:")
#define C_PORT  				(EngMode? 		"Port:":"端口:")

#define C_APN  					(EngMode? 		"APN:":"APN:")
#define C_USR  					(EngMode? 		"User ID:":"用户名:")
#define C_PWD  					(EngMode? 		"Password:":"密码:")

#define C_ETHIP					(EngMode? "local IP:" :"本地IP:")

#define C_RECONT				(EngMode? "Reconnect Count:" :"掉线后,再次自动连接次数:")
#define C_COUNT					(EngMode? "(freq)":"次")

#define C_SMSREBOOT			(EngMode? "Reboot Successfully" : "重启成功")

#define C_DELDEV1				(EngMode? "Device " :"从机:")
#define C_DELHIS				(EngMode? "Delete successfully" :"删除成功")
#define C_DEVNOR				(EngMode? "Master-slave communicate normally:" :	"通讯正常:")	
#define C_DEVABNOR			(EngMode? "communicate abnormally:" :"通讯异常:")
#define C_DEVNO					(EngMode? "No related terminal equipment" :"无相关终端设备")

#define C_DEVID					(EngMode? "Device ID:" :	"设备ID号:")
#define REFACTOK   			(EngMode? "Reset Successfully!":"工厂化成功")

#define C_GPRS			  	(EngMode? 	"GPRS/3G:":"GPRS/3G:")
#define C_RS232			  	(EngMode? 	"RS485/RS232:":"RS485/RS232:")
#define C_ETH			   		(EngMode? 	"ETH:":"网口:")

#define C_DORMAIN				(EngMode? 		"Domain:":" 域名:")

#define C_GPRSONLINE  	(EngMode? "GPRS Online":"GPRS一直在线")

#define C_GPRSOPEN0			(EngMode?"GPRS Open MOBUS":"GPRS开启MODBUS")
#define C_GPRSOPEN1			(EngMode?"GPRS Open MQTT":"GPRS开启MQTT")
#define C_GPRSCLOSE			(EngMode?"GPRS Close":"GPRS关闭")

#define C_ETHOPEN0			(EngMode?"ETH Open MODBUS":"以太网开启MODBUS")
#define C_ETHOPEN1			(EngMode?"ETH Open MQTT":"以太网开启MQTT")
#define C_ETHCLOSE			(EngMode?"ETH Close":"以太网关闭")

#endif
