#include "include/Server.h"
#include<iostream>
#include"include\Client.h"
#include"include\Message.h"
#include"include\MyJson.h"
#include<mutex>

extern DWORD WINAPI ReceiveThread(LPVOID lparam);
extern DWORD WINAPI ListenerThread(LPVOID lparam);
//�߳�֮�䴫�ݵĲ���
//�ر�server����������������ر�ʱ��
Server::~Server(void)
{
	//��û�к����е�����˵�ټ���!���ﲻ˵�ˣ���ΪServer�����˳����˳������������˳���ֱ�Ӷ�����clientList˵�ټ��ͺã�����Ҫ�������ķ�����д��
	closesocket(sockSrv);//�رռ���
	for(int i=0;i<hThread.size();i++) CloseHandle(hThread[i]);//ֹͣ�������ӵ��߳�
	for(int i=0;i<sockAccept.size();i++) closesocket(sockAccept[i]);//�ر�����ʹ���е�socket
	hThread.clear();
	sockAccept.clear();
	threadId.clear();
	WSACleanup();
}
//�½�һ��server
Server::Server()
{
}
//��ʼ���������ֿ�
bool Server::Init(int port_id)//�����ֿ�Ȳ���
{
	int err;
	///////////////////�����ֿ�////////////////
	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		cout<<"faild when WSAStartup!"<<endl;
		system("pause");
		return false;
	}
	if ( LOBYTE( wsaData.wVersion ) != 1 ||HIBYTE( wsaData.wVersion ) != 1 )
	{ 
		WSACleanup( );
		cout<<"faild when WSAStartup!"<<endl;
		system("pause");
		return false;
	}
	max_binner=20;
	sockSrv=socket(AF_INET,SOCK_STREAM,0);//���������ڶ���������ѡ�������׽��֣�UDPѡ��SOCK_DGRAM���ݱ��׽���
	addrSrv.sin_family=AF_INET;//��������
	addrSrv.sin_port=htons(port_id);//����������Ķ˿�
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//���������ip��ַɸѡ,��Ϊ�Ƿ�����������ʱlisten�����Բ�����ANY�����Զ�ѡ��Ĭ�ϵı�������������ǿͻ��������ʱconnet����Ҫ��������ΪĿ��ip�� 
	
	if(::bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(addrSrv)) == -1)
	{
		cout<<"bind() failed."<<endl;
		closesocket(sockSrv);//�����ڴ򿪣�ʧ���ˣ�Ӧ���ں����ڹر�
		//system("pause");
		this->~Server();
		return false;
	}
	if(listen(sockSrv, max_binner) == SOCKET_ERROR)//���Կ�ʼ����,����2�ǵȴ�������󳤶ȣ�Ҳ���Ǽ���ʱ����������
	{
		cout<<"Error listening on socket."<<endl;
		system("pause");
		return false;
	}
	cout<<"Server Created!"<<endl<<"Now,Listening``````"<<endl;
	return true;
}
//�����������̣߳��ü���socket��ʼaccpet����Ҫ�ⲿ��
int Server::begin()
{
	DWORD *newId= new DWORD;//�µ��߳�id
	HANDLE *newThread=new HANDLE;//�µ��̱߳���

	*newThread=CreateThread(NULL, NULL, ListenerThread, (LPVOID)this, 0, newId);
	hThread.push_back(*newThread);
	threadId.push_back(*newId);
	return hThread.size();
}
//TODO����ѯ���е�Vector�����е�socket��add�Ƿ��кͲ�����ͬ��
int Server::isSocketAlreadyInVector(SOCKADDR_IN addr)
{
	return 0;
}
//��ʼ�������ݣ���ʱsocket�����ȴ�����״̬��ֱ�����ս���
string Server::receiveData(SOCKET torcv)
{
	int bytesRecv=-1;
	char *recvbuf=new char[8];
	char rare[2];
	while(bytesRecv == SOCKET_ERROR || bytesRecv==0)
	{
		bytesRecv = recv(torcv, recvbuf, 2, 0);
		try
		{
			if (recvbuf[0]!='\x32' || recvbuf[1]!='\xA0') bytesRecv=-1;//server���е��˴���������ֱ��������Ϣ�򴥷��쳣
		}
		catch(exception e)
		{
			//TODO:���ܻ��ж����쳣��
			break;
		}
	}
	bytesRecv=-1;
	while(bytesRecv == SOCKET_ERROR || bytesRecv==0)
	{
		bytesRecv = recv(torcv, recvbuf, 8, 0);
	}
	bytesRecv=-1;
	UINT64 length=0;
	for (int i=0;i<8;i++)
	{
		int x=pow(256,i);
		x=(unsigned char)(recvbuf[i])*x;
		length+=x;
	}
	recvbuf=new char[length];
	while(bytesRecv == SOCKET_ERROR || bytesRecv==0)
	{
		bytesRecv = recv(torcv, recvbuf, length, 0);
	}
	bytesRecv=-1;
	while(bytesRecv == SOCKET_ERROR || bytesRecv==0)
	{
		bytesRecv = recv(torcv, rare, 2, 0);
	}
	return recvbuf;
}
//�ر�һ��socket����ע������һ����Ϣ����Ҫ�ⲿ��
void Server::exitSocket(SOCKET &toclose)
{
	vector<SOCKET>::iterator itr = find(sockAccept.begin(),sockAccept.end(),toclose);
	closesocket(toclose);
	sockAccept.erase(itr);
}
//���һ��socket���б��У�������ӵĶ��Ǻ�һ��Client���ӵ�socket����Ҫ�ⲿ��
void Server::addSocket(SOCKET &toadd)
{
	sockAccept.push_back(toadd);
}