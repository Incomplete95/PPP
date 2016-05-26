// Client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdio.h"
#include "winsock2.h"
#include "windows.h"
#include "iostream"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"netapi32.lib")   //����Netapi32.lib�⣬MAC��ȡ���õ���NetApi32.DLL�Ĺ���

using namespace std;
	//ͨ��WindowsNT/Win2000�����õ�NetApi32.DLL�Ĺ�����ʵ�ֵġ�����ͨ������NCBENUM����,��ȡ������
   //��Ŀ��ÿ���������ڲ����,Ȼ���ÿ��������ŷ���NCBASTAT�����ȡ��MAC��ַ��


int id_num =1; 
int getMAC(char * mac, int id)     //��NetAPI����ȡ����MAC��ַ
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
//id_num = lana_enum.length; 
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
ncb.ncb_command = NCBASTAT;   
//for (int i=0;i<lana_enum.length;++i) 
 //����������NCBSTAT�����ȡ������Ϣ
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



DWORD WINAPI RecevData(LPVOID lpParameter);

SOCKET ClientSocket;
SOCKADDR_IN SverAddr;
int SverLen = sizeof(SverAddr);
char host_name[225];

HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);//ϵͳ�������ƾ֤ 

int main( )
{
	
SOCKET clientSocket;
SOCKADDR_IN clientsock_in;
     char ip_addr[16]=
{"127.0.0.1"};
	WSADATA wsadate;
	int Ret;
	if ((Ret =WSAStartup(MAKEWORD(2,1),&wsadate)) != 0 )
	{
		cout << "WSAStartup error " << Ret << endl;
	}
	ClientSocket =  socket(AF_INET,SOCK_DGRAM,0);//ipv4����Э���ʹ�ò��������ɿ������ݰ�����

	SverAddr.sin_family = AF_INET;//TCP/ipЭ����
	SverAddr.sin_port = htons(8000);//����˶˿�
	 clientSocket=socket(AF_INET,SOCK_STREAM,0);
cout << "����������ip:" << endl;
cin >> ip_addr;
//���ӷ�����
SverAddr.sin_addr.S_un.S_addr=inet_addr(ip_addr);      //����˵�ַ

	SOCKADDR_IN ClientAddr;//ָ�벢��ʼ��
	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_port = htons(7000);//�˿ں�
	ClientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//ռ�����е�ip���м���

	bind(ClientSocket,(LPSOCKADDR)&ClientAddr,sizeof(ClientAddr));//����
 
	HANDLE handl =CreateThread(NULL,NULL,RecevData,NULL,NULL,0);

    struct hostent *pHost = gethostbyname(host_name); // //////////// host name
	char mac[200];
	getMAC(mac, 0) ;
	for(int id=0;id<id_num ;id++){
		cout << "����ip:";
		cout << inet_ntoa(*((struct in_addr *)pHost->h_addr_list[id])) << endl;

	}

	{
 
		char   mac[200];  
		for(int id=0; id<id_num; id++) {
			getMAC(mac, id);   //����getMAC()������ã����MAC��ַ
			cout <<"����MAC:";
			cout << mac <<endl;  
		}

	}

	 
	while(1)
	{
		//cout<< "Client:" ;
		char buffer[1024];
		memset(buffer,0,1024);//��һ���ڴ�����������ֵ
		SetConsoleTextAttribute(hStdout,FOREGROUND_RED|FOREGROUND_INTENSITY);//����̨��ɫ
		cin.getline(buffer,1024);//cin >> buffer;
		if(strlen(buffer) == 0)
		{
			continue;
		}
		if (buffer[0] == 'q')
		{
			cout << "������ֹ����" <<endl;
			strcpy(buffer,"�Է�����ֹ����");
			int Ret;
			Ret = sendto(ClientSocket,buffer,strlen(buffer),0,(LPSOCKADDR)&SverAddr,sizeof(SverAddr));
			if (Ret == SOCKET_ERROR)
			{
				cout << "sendto error " << GetLastError() <<endl;
			}
			system("pause");
			break;
		}
		int Ret;
		Ret = sendto(ClientSocket,buffer,strlen(buffer),0,(LPSOCKADDR)&SverAddr,sizeof(SverAddr));//����
		if (Ret == SOCKET_ERROR)
		{
			cout << "sendto error " << GetLastError() <<endl;
			continue;
		}
		 Sleep(10);
	}
	closesocket(ClientSocket);
	CloseHandle(handl);
	WSACleanup();//���ú����������
	return 0;
}

DWORD WINAPI RecevData(LPVOID lpParameter)
{
	while(1)
	{
		char buffer[1024];
		memset(buffer,0,1024);
		int Ret;
		Ret = recvfrom(ClientSocket,buffer,1024,0,(LPSOCKADDR)&SverAddr,&SverLen);//����
		if (Ret == SOCKET_ERROR)
		{
			cout << "recvfrom error " << GetLastError() <<endl;
			continue;
		}
		SetConsoleTextAttribute(hStdout,FOREGROUND_GREEN|FOREGROUND_INTENSITY);
		cout << inet_ntoa(SverAddr.sin_addr) << endl;
		cout << "�Է�˵:" << buffer << endl;
		SetConsoleTextAttribute(hStdout,FOREGROUND_RED|FOREGROUND_INTENSITY);
		Sleep(10);
	}
               system("pause");
	return 0;
}
  
