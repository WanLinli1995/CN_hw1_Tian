#include "include/Server.h"
#include<iostream>
#include"include\Client.h"
#include"include\Message.h"
#include"include\MyJson.h"
#include<mutex>

//�߳�֮�䴫�ݵĲ���
mutex mtx_server_acceptlist;//��������acceptlist�Ļ�����
mutex mtx_cout;//��������cout�Ļ�����
mutex mtx_clientlist;
mutex mtx_handle;

extern vector<Client>clientList;//�����ͻ�����
//�߳�ͨ�Ų���
struct ThreadParament
{
	SOCKET *rec_socket;
	Server *server;
};
//�����߳�
DWORD WINAPI ReceiveThread(LPVOID lparam)
{
	//Ӧ���ܽ���һ���ֽ�����Ȼ�󽻸��²��Message�����õ�һ���������浽ȫ�ֱ���MyJson�С�
	//����lparam�ǽ����õ�ReceiveSocket��
	//����Ҫ���ñ��ص�Clientȥ����һ�������TCP����
	//��Ҫ���������ö�Ӧ��Clientȥ����ظ�
	ThreadParament* pa=(ThreadParament*)lparam;
	SOCKET *ReceiveSocket=pa->rec_socket;//�õ��������ӵ��Ǹ�socket
	Client *respond_client=NULL;//�����ϵĿͻ�����ָ��ȫ�ֵ�һ��Client�������ڴ��߳��д���һ��ȫ�ֵ�Client���������ָ���������Ҫ���߳̽���ʱ�ͷš�
	Server *server_p=pa->server;//�����ϵķ�����
	SOCKADDR_IN dest_add;//�ͱ�������ͨ�ŵ�Client�ĵ�ַ
	string rcv;//���յ����ֽ�����
	int x=-1;
	int nAddrLen = sizeof(dest_add);
	while (mtx_server_acceptlist.try_lock()!=1);
	server_p->addSocket(*ReceiveSocket);
	mtx_server_acceptlist.unlock();
	if(::getpeername(*ReceiveSocket, (SOCKADDR*)&dest_add, &nAddrLen) != 0)
    {
		while(mtx_cout.try_lock()!=1);
		std::cout<<"Get IP address by socket failed!n"<<endl;
		system("pause");
		mtx_cout.unlock();
		while (mtx_server_acceptlist.try_lock()!=1);
		server_p->exitSocket(*ReceiveSocket);//��server��ɾ�����socket
		mtx_server_acceptlist.unlock();
        return 0;
    }
	while(mtx_cout.try_lock()!=1);
	cout<<"IP: "<<::inet_ntoa(dest_add.sin_addr)<<"  PORT: "<<ntohs(dest_add.sin_port)<<endl;
	mtx_cout.unlock();
	for(int i=0;i<clientList.size();i++)
	{
		if(inet_ntoa(clientList[i].getAddr().sin_addr)==inet_ntoa(dest_add.sin_addr))
		{
			respond_client=&clientList[i];//��������Դ�ڱ����Ŀͻ������ӱ���
			break;
		}
	}
	if(respond_client==NULL)//������ڣ����½�һ���ͻ���
	{
		respond_client=new Client(inet_ntoa(dest_add.sin_addr),3232);
		while(!mtx_clientlist.try_lock())
		{
			clientList.push_back(*respond_client);//���뵽List��
			mtx_clientlist.unlock();
		}
	}
	//��ѭ��
	while(1)
	{
		Message mess=Message();
		MyJson infomation=MyJson();
		rcv=server_p->receiveData(*ReceiveSocket);//������Ϣ���������̻߳�ȴ������յ�Ϊֹ
		infomation=mess.getContent(rcv);//��������˴��ֽڲ㵽��Ϣ��Ľ��
		if(infomation.type_s=="exit")
		{
			infomation.showJson_in_console();
			respond_client->~Client();
			//ע������ͻ���			
			while(mtx_clientlist.try_lock()!=1);
			vector<Client>::iterator itr = find(clientList.begin(),clientList.end(),*respond_client);
			clientList.erase(itr);
			mtx_clientlist.unlock();
			//�ӿͻ���list��ɾ����
			while(mtx_server_acceptlist.try_lock()!=1);
			server_p->exitSocket(*ReceiveSocket);//��server��ɾ�����socket
			mtx_server_acceptlist.unlock();
			delete respond_client;//�ڴ����
			respond_client=NULL;
			return 0;
		}
		else
		{
			if(infomation.type_s=="text")
			{
				respond_client->respend();
				while(mtx_cout.try_lock()!=1);
				cout<<"Server Receive:"<<endl;
				infomation.showJson_in_console();
				cout<<endl;
				mtx_cout.unlock();
			}
			else
			{
				//TODO:�������İ����ͽ��д���
			}
		}
	}
	while(mtx_server_acceptlist.try_lock()!=1);
	server_p->exitSocket(*ReceiveSocket);
	mtx_server_acceptlist.unlock();
	delete respond_client;
	respond_client=NULL;
	return 1;
}
//�����߳�
DWORD WINAPI ListenerThread(LPVOID lparam)
{
	SOCKET *AcceptSocket=new SOCKET();
	Server* server=(Server*)(LPVOID)lparam;
	SOCKET sr=server->get_SockSrv();
	while(1)
	{
		try
		{
			*AcceptSocket = accept(sr,0, 0);//������һ��һֱ�ȵ���ĵ���
			while(mtx_cout.try_lock()!=1);
			cout<<"A Client connetted!"<<endl;//���������,������һ������
			mtx_cout.unlock();
			while(mtx_server_acceptlist.try_lock()!=1);
			server->get_AcceptList().push_back(*AcceptSocket);
			mtx_server_acceptlist.unlock();
			DWORD *newId= new DWORD;//�µ��߳�id
			HANDLE *newThread=new HANDLE;//�µ��̱߳���
			ThreadParament pa;//�̼߳�ͨ�ŵĲ���
			pa.rec_socket=AcceptSocket;
			pa.server=server;
			while(mtx_handle.try_lock()!=1);
			*newThread=CreateThread(NULL, NULL, ReceiveThread, (LPVOID)&pa, 0, newId);
			server->get_hThread().push_back(*newThread);
			server->get_threadId().push_back(*newId);
			mtx_handle.unlock();
		}
		catch(exception e)
		{
			cout<<e.what()<<endl;
			closesocket(sr);
		}
	}
	return 1;
}
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
	//Init();
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
//�����������̣߳��ü���socket��ʼaccpet
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
		if (recvbuf[0]!='\x32' || recvbuf[1]!='\xA0') bytesRecv=-1;//��Ϊ||
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
//�ر�һ��socket����ע������һ����Ϣ
void Server::exitSocket(SOCKET &toclose)
{
	vector<SOCKET>::iterator itr = find(sockAccept.begin(),sockAccept.end(),toclose);
	closesocket(toclose);
	sockAccept.erase(itr);
}
//���һ��socket���б��У�������ӵĶ��Ǻ�һ��Client���ӵ�socket
void Server::addSocket(SOCKET &toadd)
{
	sockAccept.push_back(toadd);
}