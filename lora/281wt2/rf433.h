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
    u8 src;//Դ��ַ
    u8 tar;//Ŀ���ַ
    u8 PackSeq;//�������   
    
}PACKHEAD;  

u8 SetFreq(u8 ch);//ch=0-99
void InitRf433(void);