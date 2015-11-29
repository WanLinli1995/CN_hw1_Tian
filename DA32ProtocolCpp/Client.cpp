#include "include/Client.h"
#include"include\Message.h"
#include"include\MyJson.h"
#include<mutex>

extern mutex mtx;
extern string myName;
char Client::HEAD_CHAR[2] = {'\x32','\xA0'};
char Client::RARE_CHAR[2] = {'\x42','\xF0'};

byte* get_data_length(string data)
{
	UINT64 len=data.length();
	char *pointer=(char*)&len;
	byte *temp=new byte[8];
	for(int i=0;i<8;i++)
		temp[i]=*(pointer+i);
	return temp;
}

Client::Client()
{
	Init();	
}
bool Client::newClient(string dest_ip,int dest_port)
{
	sockClient=socket(AF_INET,SOCK_STREAM,0); 
	addrClient.sin_addr.S_un.S_addr=inet_addr(dest_ip.c_str());
    addrClient.sin_family=AF_INET;
    addrClient.sin_port=htons(dest_port);
	try
	{
	  int isok=connect(sockClient,(SOCKADDR*)&addrClient,sizeof(SOCKADDR)); 
	  if(isok==-1) return false;
	}
	catch(exception e)
	{
		cout<<e.what();
		return false;
	}
	return true;
	//TODO�����û�����ӳɹ��Ĵ���
}

Client::~Client(void)
{

	//WSACleanup();
}

void Client::Init(void)//�����ֿ�Ȳ���
{
	int err;
	///////////////////�����ֿ�////////////////
	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		cout<<"faild when WSAStartup!"<<endl;
		system("pause");
		return ;
	}
	if ( LOBYTE( wsaData.wVersion ) != 1 ||HIBYTE( wsaData.wVersion ) != 1 )
	{ 
		WSACleanup( );
		cout<<"faild when WSAStartup!"<<endl;
		system("pause");
		return ;
	}
}
//����
void Client::sendData(string input)
{
	Message mess;
	MyJson info;
	info.name=myName;
	info.id=1;
	info.type_s="text";
	string tosend;
	char *ch;
	tosend=info.PackJson(input);
	ch=new char[tosend.length()];
	strcpy(ch,tosend.c_str());
	tosend=mess.getWrap(ch);
	ch=new char[tosend.length()+HEAD_LENTH+RARE_LENTH+WIEDTH_LENTH];
	for(int i=0;i<HEAD_LENTH;i++)
	{
		ch[i]=Client::HEAD_CHAR[i];
	}
	byte *lenth=get_data_length(tosend);
	for(int i=0;i<WIEDTH_LENTH;i++)
	{
		ch[i+HEAD_LENTH]=lenth[i];
	}
	for(int i=0;i<tosend.length();i++)
	{
		ch[i+HEAD_LENTH+WIEDTH_LENTH]=tosend.c_str()[i];
	}
	for(int i=0;i<RARE_LENTH;i++)
	{
		ch[i+HEAD_LENTH+WIEDTH_LENTH+tosend.length()]=RARE_CHAR[i];
	}
	try
	{
		send(sockClient,ch,tosend.length()+HEAD_LENTH+RARE_LENTH+WIEDTH_LENTH+1,0);  
	}
	catch(exception e)
	{
		cout<<e.what();
	}
	//while(mtx_cout.try_lock()!=1);
	//cout<<"Client send:"<<endl;
	//info.showJson_in_console();
	//cout<<endl;
	//mtx_cout.unlock();
	delete[] ch;
	delete[] lenth;
}
//�����˳���Ϣ��������һ�����ӽ���ʱ
void Client::exit()
{
	Message mess;
	MyJson info;
	info.name="";
	info.id=1;
	info.type_s="exit";
	string tosend;
	char *ch;
	tosend=info.PackJson("");
	ch=new char[tosend.length()];
	strcpy(ch,tosend.c_str());
	tosend=mess.getWrap(ch);
	ch=new char[tosend.length()+HEAD_LENTH+RARE_LENTH+WIEDTH_LENTH];
	for(int i=0;i<HEAD_LENTH;i++)
	{
		ch[i]=Client::HEAD_CHAR[i];
	}
	byte *lenth=get_data_length(tosend);
	for(int i=0;i<WIEDTH_LENTH;i++)
	{
		ch[i+HEAD_LENTH]=lenth[i];
	}
	for(int i=0;i<tosend.length();i++)
	{
		ch[i+HEAD_LENTH+WIEDTH_LENTH]=tosend.c_str()[i];
	}
	for(int i=0;i<RARE_LENTH;i++)
	{
		ch[i+HEAD_LENTH+WIEDTH_LENTH+tosend.length()]=RARE_CHAR[i];
	}
	send(sockClient,ch,tosend.length()+HEAD_LENTH+RARE_LENTH+WIEDTH_LENTH+1,0);  
	delete[] ch;
	delete[] lenth;
	closesocket(this->sockClient);
}
//���ͻذ��������ڱ����ķ������յ���һ��text֮��
void Client::respend()
{
	Message mess;
	MyJson info;
	info.name="";
	info.id=1;
	info.type_s="respond";
	string tosend;
	char *ch;
	tosend=info.PackJson("");
	ch=new char[tosend.length()];
	strcpy(ch,tosend.c_str());
	tosend=mess.getWrap(ch);
	ch=new char[tosend.length()+HEAD_LENTH+RARE_LENTH+WIEDTH_LENTH];
	for(int i=0;i<HEAD_LENTH;i++)
	{
		ch[i]=Client::HEAD_CHAR[i];
	}
	byte *lenth=get_data_length(tosend);
	for(int i=0;i<WIEDTH_LENTH;i++)
	{
		ch[i+HEAD_LENTH]=lenth[i];
	}
	for(int i=0;i<tosend.length();i++)
	{
		ch[i+HEAD_LENTH+WIEDTH_LENTH]=tosend.c_str()[i];
	}
	for(int i=0;i<RARE_LENTH;i++)
	{
		ch[i+HEAD_LENTH+WIEDTH_LENTH+tosend.length()]=RARE_CHAR[i];
	}
	send(sockClient,ch,tosend.length()+HEAD_LENTH+RARE_LENTH+WIEDTH_LENTH+1,0);  
	delete[] ch;
	delete[] lenth;
}