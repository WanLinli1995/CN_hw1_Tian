/*
�뽫��Ҫ��Ԥ�����ļ���������ļ��ڡ�
*/
#pragma comment(lib,"lib_json.lib") 
#include<iostream>
#include "include/json/json.h"  
#include <fstream>  
#include <string>  
#include <cassert>
#include<time.h>
#include"include/md5.h"
#include"include/Client.h"
#include"include/Message.h"
#include"include/MyJson.h"
#include"include/Server.h"
#include<vector>
#include<mutex>
//TODO:������ͷ�ļ�
//eg:#include "xx.h"
   
using namespace std; 

//TODO���������
vector<Client>clientList;//��Ҫά����Client�б�
Server domainServer;//��Ҫά����Server
mutex mtx;//��������ֻ�ڷ��̡߳����߳���ʹ�ã�����������ʹ��
///�̴߳��ݲ���
struct ThreadParament
{
	SOCKET *rec_socket;
	Server *server;
};


//TODO�����庯��
///�����Ƿ���Դ�ClientList���ҵ�һ��Client
bool findClient(Client *tofind)
{
	vector<Client>::iterator itr = find(clientList.begin(),clientList.end(),*tofind);
	if (itr!=clientList.end())
	{
		return true;
	}
	return false;
}
///�����߳�
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
		mtx.lock();
		server_p->addSocket(*ReceiveSocket);//ά��acceptsocket
		mtx.unlock();
		if(::getpeername(*ReceiveSocket, (SOCKADDR*)&dest_add, &nAddrLen) != 0)
		{
			mtx.lock();
			std::cout<<"Get IP address by socket failed!"<<endl;
			server_p->exitSocket(*ReceiveSocket);//��server��ɾ�����socket
			mtx.unlock();
			return 0;
		}
		mtx.lock();
		cout<<"IP: "<<::inet_ntoa(dest_add.sin_addr)<<"  PORT: "<<ntohs(dest_add.sin_port)<<"��ʼ����ͨ����"<<endl;
		mtx.unlock();
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
			respond_client=new Client();
			mtx.lock();
			while(respond_client->newClient(inet_ntoa(dest_add.sin_addr),3232)==false);
			clientList.push_back(*respond_client);//���뵽List��
			mtx.unlock();
		}
		//��ѭ��
		while(1)
		{
			try
			{
				Message mess=Message();
				MyJson infomation=MyJson();
				rcv=server_p->receiveData(*ReceiveSocket);//������Ϣ���������̻߳�ȴ������յ�Ϊֹ
				infomation=mess.getContent(rcv);//��������˴��ֽڲ㵽��Ϣ��Ľ��
				if(infomation.type_s=="exit")
				{	
					mtx.lock();
					//ע������ͻ���			
					vector<Client>::iterator itr = find(clientList.begin(),clientList.end(),*respond_client);
					if (itr!=clientList.end())//����ҵ������Client�����ţ���ɱ����
					{
						itr->exit();
						clientList.erase(itr);
					}
					server_p->exitSocket(*ReceiveSocket);//��acceptsocket��ɾ�����socket
					mtx.unlock();
					cout<<inet_ntoa(dest_add.sin_addr)<<"�����ˣ�"<<endl;
					respond_client=NULL;
					return 0;
				}
				else
				{
					if(infomation.type_s=="text")
					{
						mtx.lock();
						respond_client->respend();
						infomation.showJson_in_console();
						cout<<endl;
						mtx.unlock();
					}
					else
					{
						//TODO:�������İ����ͽ��д���
					}
				}
			}
			catch(exception e)//TODO��Ӧ����������쳣��
			{
				cout<<e.what();
				mtx.lock();
				server_p->exitSocket(*ReceiveSocket);
				mtx.unlock();
				delete respond_client;
				return -1;
			}
		}
		mtx.lock();
		server_p->exitSocket(*ReceiveSocket);
		mtx.unlock();
		delete respond_client;
		respond_client=NULL;
		return 1;
	}
///�����߳�
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
				mtx.lock();
				server->get_AcceptList().push_back(*AcceptSocket);
				mtx.unlock();
				DWORD *newId= new DWORD;//�µ��߳�id
				HANDLE *newThread=new HANDLE;//�µ��̱߳���
				ThreadParament pa;//�̼߳�ͨ�ŵĲ���
				pa.rec_socket=AcceptSocket;
				pa.server=server;
				mtx.lock();
				*newThread=CreateThread(NULL, NULL, ReceiveThread, (LPVOID)&pa, 0, newId);
				server->get_hThread().push_back(*newThread);
				server->get_threadId().push_back(*newId);
				mtx.unlock();
			}
			catch(exception e)
			{
				cout<<e.what()<<endl;
				closesocket(sr);
			}
		}
		return 1;
	}
///���̣߳���ʵ�ϲ���һ���̣߳�����һ��������
void mainThread()
{
	string input,context;
	Client *newClient=new Client();
	int pass=0;

	while(1)
		{
			pass=0;//���Client�Ƿ���ڵ�flag
			cout<<"�����硰�Է�IP ����(�ÿո�ֿ�)����"<<endl;
			cin>>input;//TODO:�����������ƾ����ˣ��㲻�������˿ո�֮���ֺ�����롤backspace������ip������
			cin>>context;
			for(int i=0;i<clientList.size();i++)
			{
				string temp=inet_ntoa(clientList[i].getAddr().sin_addr);
				if(temp==input)
				{
					pass=1;
					newClient=&clientList[i];
					break;
				}
			}
			if(pass==0)//�����ھ��½�֮��
			{
				while(newClient->newClient(input)==false)
				{
					cout<<"����ʧ�ܣ���������ȷ��IP�ţ�"<<endl;
					cin>>input;
					for(int i=0;i<clientList.size();i++)
					{
						string temp=inet_ntoa(clientList[i].getAddr().sin_addr);
						if(temp==input)
						{
							pass=1;
							newClient=&clientList[i];
							break;
						}
					}
					if(pass==1) break;
				}
				if(pass==0) clientList.push_back(*newClient);
			}
			if(context=="exit") break;
			mtx.lock();
			if(findClient(newClient)) newClient->sendData(context);	
			else
			{
				mtx.unlock();
				break;
			}
			mtx.unlock();
		}
}
///��ȫ�˳�����Ҫ�Ĳ���
void quitFunction()
{
	mtx.lock();
	for(int i=clientList.size()-1;i>=0;i--)
	{
		clientList[i].exit();
		clientList.pop_back();
	}
	mtx.unlock();
	_sleep(1000);
}

