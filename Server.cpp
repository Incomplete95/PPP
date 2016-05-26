#include "stdio.h"
#include "winsock2.h"//socketͨ�ţ�ϵͳͷ�ļ�
#include "windows.h"
#include "iostream"
#pragma comment(lib,"ws2_32.lib")//��̬����һ��lib�ļ�Ҳ���ǿ��ļ�ws2_32.lib�ļ�
#pragma comment(lib,"netapi32.lib")   //����Netapi32.lib�⣬MAC��ȡ���õ���NetApi32.DLL�Ĺ���

using namespace std;

int id_num ;

int getMAC(char * mac , int id)     //��NetAPI����ȡ����MAC��ַ
{    
NCB ncb;     //����һ��NCB(������ƿ�)���͵Ľṹ�����ncb
typedef struct _ASTAT_     //�Զ���һ���ṹ��_ASTAT_
{
  ADAPTER_STATUS   adapt;
  NAME_BUFFER   NameBuff   [30];    
}ASTAT, *PASTAT;
ASTAT Adapter; 

typedef struct _LANA_ENUM     //�Զ���һ���ṹ��_ASTAT_
{
  UCHAR length;
  UCHAR lana[MAX_LANA];     //�������MAC��ַ
}LANA_ENUM;    
LANA_ENUM lana_enum; 

//   ȡ��������Ϣ�б�    
UCHAR uRetCode;    
memset(&ncb, 0, sizeof(ncb));     //���ѿ����ڴ�ռ�ncb ��ֵ����Ϊֵ 0
memset(&lana_enum, 0, sizeof(lana_enum));     //���һ���ṹ���͵ı���lana_enum����ֵΪ0
//�Խṹ�����ncb��ֵ
ncb.ncb_command = NCBENUM;     //ͳ��ϵͳ������������
ncb.ncb_buffer = (unsigned char *)&lana_enum; //ncb_buffer��Աָ����LANA_ENUM�ṹ���Ļ�����
ncb.ncb_length = sizeof(LANA_ENUM);  
//����������NCBENUM����Ի�ȡ��ǰ������������Ϣ�����ж��ٸ�������ÿ�������ı�ţ�MAC��ַ��
uRetCode = Netbios(&ncb); //����netbois(ncb)��ȡ�������к�   
if(uRetCode != NRC_GOODRET)    
return uRetCode;   

//��ÿһ�������������������Ϊ�����ţ���ȡ��MAC��ַ  
id_num = 1;
for(int lana=0; lana<lana_enum.length; lana++)    
{
  ncb.ncb_command = NCBRESET;   //����������NCBRESET������г�ʼ��
  ncb.ncb_lana_num = lana_enum.lana[lana];
  uRetCode = Netbios(&ncb);  
}
if(uRetCode != NRC_GOODRET)
return uRetCode;    
//   ׼��ȡ�ýӿڿ���״̬��ȡ��MAC��ַ
memset(&ncb, 0, sizeof(ncb));
ncb.ncb_command = NCBASTAT;    //����������NCBSTAT�����ȡ������Ϣ
ncb.ncb_lana_num = lana_enum.lana[id];     //ָ�������ţ��������ָ����һ��������ͨ��Ϊ��������
strcpy((char*)ncb.ncb_callname, "*");     //Զ��ϵͳ����ֵΪ*
ncb.ncb_buffer = (unsigned char *)&Adapter; //ָ�����ص���Ϣ��ŵı���
ncb.ncb_length = sizeof(Adapter);
//���ŷ���NCBASTAT�����Ի�ȡ��������Ϣ
uRetCode = Netbios(&ncb);
//   ȡ����������Ϣ����������������������Ļ������ر�׼��ð�ŷָ���ʽ��  
if(uRetCode != NRC_GOODRET)  
return uRetCode;  
//������MAC��ַ��ʽת��Ϊ���õ�16������ʽ,������ַ���mac��
sprintf(mac,"%02X-%02X-%02X-%02X-%02X-%02X",    
Adapter.adapt.adapter_address[0],    
Adapter.adapt.adapter_address[1],    
Adapter.adapt.adapter_address[2],    
Adapter.adapt.adapter_address[3],    
Adapter.adapt.adapter_address[4],    
Adapter.adapt.adapter_address[5]
);
return 0;  
}


DWORD WINAPI RecevData(LPVOID lpParameter);// ˫�֣�32λ

SOCKET SerSocket;
SOCKADDR_IN ClientAddr;
int ClientLen =  sizeof(SOCKADDR);//�Զ�ת��
char host_name[225];
HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 

int main()
{
	  
	WSADATA wsadate;
	int Ret;
	if ((Ret =WSAStartup(MAKEWORD(2,1),&wsadate)) != 0 )//����winsock����
	{
		cout << "WSAStartup error " << Ret << endl;
	}
	SerSocket =  socket(AF_INET,SOCK_DGRAM,0);
	memset(&ClientAddr,0,sizeof(ClientAddr));//����ĳ�ʼ��

	SOCKADDR_IN SverAddr;
	SverAddr.sin_family = AF_INET;
	SverAddr.sin_port =htons(8000);
	SverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	bind(SerSocket,(LPSOCKADDR)&SverAddr,sizeof(SOCKADDR));
    
    struct hostent *pHost = gethostbyname(host_name); // //////////// host name
    
	char mac[200];
	getMAC(mac, 0);
//	printf("id_num = %d\n", id_num) ;


	for(int id=0; id<id_num; id++) {
		cout << "����ip:";
		cout << inet_ntoa(*((struct in_addr *)pHost->h_addr_list[id])) << endl;
	}

	{
 
		char   mac[200];  
		for(int id=0; id<id_num; id++)
		{
			getMAC(mac, id);   //����getMAC()������ã����MAC��ַ
			cout << "����MAC:";
			cout << mac <<endl; 
		}

	}


	HANDLE handl = CreateThread(NULL,NULL,RecevData,NULL,NULL,0);
	 
	while (1)
	{
		//cout<< "Server:" ;
		char buffer[1024];
		memset(buffer,0,1024);
	    //cin >> buffer;
		SetConsoleTextAttribute(hStdout,FOREGROUND_RED|FOREGROUND_INTENSITY);
		cin.getline(buffer,1024);
		if(strlen(buffer) == 0)
		{
			continue;
		}
		if (buffer[0] == 'q')
		{
			cout << "������ֹ����" <<endl;
			strcpy(buffer,"�Է�����ֹ����");
			int Ret;
			Ret = sendto(SerSocket,buffer,strlen(buffer),0,(LPSOCKADDR)&ClientAddr,ClientLen);
			if (Ret == SOCKET_ERROR)
			{
				cout << "sendto error " << GetLastError() << endl;
			}
			system("pause");
			break;
		}
		int Ret;
		Ret = sendto(SerSocket,buffer,strlen(buffer),0,(LPSOCKADDR)&ClientAddr,ClientLen);
		if (Ret == SOCKET_ERROR)
		{
			cout << "sendto error " << GetLastError() << endl;
			continue;
		}
		Sleep(10);
	}
	closesocket(SerSocket);//�Ͽ�
	CloseHandle(handl);
	WSACleanup();
	return 0;
}


DWORD WINAPI RecevData(LPVOID lpParameter)
{
	while(1)
	{
		char buffer[1024];     
		memset(buffer,0,1024);
		int Ret;
		Ret = recvfrom(SerSocket,buffer,1024,0,(LPSOCKADDR)&ClientAddr,&ClientLen);
		if (Ret == SOCKET_ERROR)
		{
			cout << "recvfrom error" << GetLastError() << endl;
			continue;
		}
		SetConsoleTextAttribute(hStdout,FOREGROUND_GREEN|FOREGROUND_INTENSITY);
		cout << inet_ntoa(ClientAddr.sin_addr) << endl;
		cout << "�Է�˵:" << buffer << endl;
		SetConsoleTextAttribute(hStdout,FOREGROUND_RED|FOREGROUND_INTENSITY);
		Sleep(10);
	}
	            system("pause");
	return 0;
}


