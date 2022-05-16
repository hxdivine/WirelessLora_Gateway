void ProcRfCmd(void);
void SendPack(void);
void ReSendPack(void);
void SendPackDat(u8 IsAlarm);
extern u8 PackSeq;
extern  u16 LastSendStamp;
extern u8 AckOk;

typedef struct
{
    u8 len;
    u8 src;//源地址
    u8 tar;//目标地址
    u8 PackSeq;//命令序号   
    
}PACKHEAD;  

u8 SetFreq(u8 ch);//ch=0-99
void InitRf433(void);