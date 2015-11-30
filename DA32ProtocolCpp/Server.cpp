#include "include\Server.h"
#include<iostream>
#include"include\Client.h"
#include"include\Message.h"
#include"include\MyJson.h"
#include<mutex>

Server::~Server(void)
{
	//��û�к����е�����˵�ټ���!���ﲻ˵�ˣ���ΪServer�����˳����˳������������˳���ֱ�Ӷ�����clientList˵�ټ��ͺã�����Ҫ�������ķ�����д��
	closesocket(sockSrv);//�رռ���
	for(int i=0;i<mThread.size();i++) mThread[i]->~thread();//ֹͣ�������ӵ��߳�
	for(int i=0;i<sockAccept.size();i++) closesocket(sockAccept[i]);//�ر�����ʹ���е�socket
	mThread.clear();
	sockAccept.clear();
	WSACleanup();
	delete mtx;
}
//�½�һ��server
Server::Server()
{
	mtx=new mutex();
}

Server::Server(allarea::ThreadParament tp)
{
	checkClientList= tp.checkClientList;
	deleteClient=tp.deleteClient;
	elsefunction=tp.elsefunction;
	respendClient=tp.respendClient;
	mtx=new mutex();
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
	//thread newThread(mem_fun(&Server::ListenerThread),this);//��ʼ�߳�
	thread *newThread=new thread(&Server::ListenerThread,this);
	newThread->detach();
	mtx->lock();
	mThread.push_back(newThread);
	int x=mThread.size();
	mtx->unlock();
	return x;
	//return 1;
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
		bytesRecv = recv(torcv, recvbuf, Client::HEAD_LENTH, 0);
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
		bytesRecv = recv(torcv, recvbuf, Client::WIEDTH_LENTH, 0);
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
		bytesRecv = recv(torcv, rare, Client::RARE_LENTH, 0);
	}
	return recvbuf;
}
//�ر�һ��socket����ע������һ����Ϣ����Ҫ�ⲿ��
void Server::exitSocket(SOCKET &toclose)
{
	vector<SOCKET>::iterator itr = find(sockAccept.begin(),sockAccept.end(),toclose);
	closesocket(toclose);
	if(itr!=sockAccept.end())sockAccept.erase(itr);
}
//���һ��socket���б��У�������ӵĶ��Ǻ�һ��Client���ӵ�socket����Ҫ�ⲿ��
void Server::addSocket(SOCKET &toadd)
{
	sockAccept.push_back(toadd);
}

void Server::ReceiveThread(LPVOID lparam)
{
	//Ӧ���ܽ���һ���ֽ�����Ȼ�󽻸��²��Message�����õ�һ���������浽ȫ�ֱ���MyJson�С�
	//����lparam�ǽ����õ�ReceiveSocket��
	//����Ҫ���ñ��ص�Clientȥ����һ�������TCP����
	//��Ҫ���������ö�Ӧ��Clientȥ����ظ�
	SOCKET *ReceiveSocket=(SOCKET*)lparam;//�õ��������ӵ��Ǹ�socket
	Client *respond_client=NULL;//�����ϵĿͻ�����ָ��ȫ�ֵ�һ��Client�������ڴ��߳��д���һ��ȫ�ֵ�Client���������ָ���������Ҫ���߳̽���ʱ�ͷš�
	SOCKADDR_IN dest_add;//�ͱ�������ͨ�ŵ�Client�ĵ�ַ
	string rcv;//���յ����ֽ�����
	int x=-1;
	int nAddrLen = sizeof(dest_add);
	//ͨ��RCVSocket��öԷ��ĵ�ַ
	if(::getpeername(*ReceiveSocket, (SOCKADDR*)&dest_add, &nAddrLen) != 0)
	{
		mtx->lock();
		std::cout<<"Get IP address by socket failed!"<<endl;
		this->exitSocket(*ReceiveSocket);//��server��ɾ�����socket
		mtx->unlock();
		return;
	}

	mtx->lock();
	cout<<"IP: "<<::inet_ntoa(dest_add.sin_addr)<<"  PORT: "<<ntohs(dest_add.sin_port)<<"��ʼ����ͨ����"<<endl;
	mtx->unlock();

	mtx->lock();
	respond_client=checkClientList(dest_add);//���ûص�������checkClientList������ɻָ�Client�ĸ�ֵ
	mtx->unlock();
	//��ѭ��

	while(1)
	{
		try
		{
			Message mess=Message();
			MyJson infomation=MyJson();
			rcv=this->receiveData(*ReceiveSocket);//������Ϣ���������̻߳�ȴ������յ�Ϊֹ
			infomation=mess.getContent(rcv);//��������˴��ֽڲ㵽��Ϣ��Ľ��
			if(infomation.type_s=="exit")
			{	
				mtx->lock();
				//ע������ͻ��ˣ�Ӧ���ɻص��������
				this->exitSocket(*ReceiveSocket);//��acceptsocket��ɾ�����socket
				deleteClient(respond_client);
				mtx->unlock();
				cout<<inet_ntoa(dest_add.sin_addr)<<"�����ˣ�"<<endl;
				respond_client=NULL;//��ֻ��һ���޹���ָ�룬������������������벻Ҫ�ٱ������ڲ����������ݡ�
				return ;
			}
			else
			{
				if(infomation.type_s=="text")
				{
					mtx->lock();
					respendClient(respond_client);
					infomation.showJson_in_console();
					cout<<endl;
					mtx->unlock();
				}
				else
				{
					//TODO:�������İ����ͽ��д��������Ҫ����Client��������ʹ�ûص�������
					elsefunction(respond_client);
				}
			}
		}
		catch(exception e)//TODO��Ӧ����������쳣��
		{
			cout<<e.what();
			mtx->lock();
			this->exitSocket(*ReceiveSocket);
			mtx->unlock();
			deleteClient(respond_client);
			respond_client=NULL;
			return;
		}
	}
	mtx->lock();
	this->exitSocket(*ReceiveSocket);
	mtx->unlock();
	delete respond_client;
	respond_client=NULL;
	return ;
}
	///�����߳�
void Server::ListenerThread()
{
	SOCKET *AcceptSocket=new SOCKET();
	SOCKET sr=this->get_SockSrv();
	while(1)
	{
		try
		{
			*AcceptSocket = accept(sr,0, 0);//������һ��һֱ�ȵ���ĵ���
			mtx->lock();
			this->get_AcceptList().push_back(*AcceptSocket);//�����յ���Ϣ��socket�����б�				
			std::thread *RcvThread=new std::thread(&Server::ReceiveThread,this,AcceptSocket);
			RcvThread->detach();
			this->get_hThread().push_back(RcvThread);
			mtx->unlock();
		}
		catch(exception e)
		{
			cout<<e.what()<<endl;
			closesocket(sr);
		}
	}
}