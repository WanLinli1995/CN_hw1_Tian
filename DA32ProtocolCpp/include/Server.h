#pragma once

#include <Winsock2.h>
#include<vector>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
class Server
{
private:
	SOCKET sockSrv;	
	SOCKADDR_IN addrSrv;
	WORD wVersionRequested;
	WSADATA wsaData;

	vector<DWORD> threadId;//�߳�id
	vector<HANDLE> hThread;//�߳�
	vector<SOCKET> sockAccept;//Ӧ��socket�б�ʵ���ϲ�û���õ�

	
	int max_binner;//��������������,Ĭ�ϻ���20
public:
	bool Init(int port_id=3232);
	Server(void);//������Ĭ�ϵĹ���
	~Server(void);
	//Server(int port_id=3232);
	void set_max_binner(int n){max_binner=n;}//�������������
	int begin();//�����ؿ�ʼ��������̣߳���hThread�е�������
	vector<SOCKET> get_AcceptList(){return sockAccept;}
	SOCKET get_SockSrv(){return sockSrv;}
	vector<HANDLE> get_hThread(){return hThread;}
	vector<DWORD> get_threadId(){return threadId;}
	int isSocketAlreadyInVector(SOCKADDR_IN addr);
	string receiveData(SOCKET torcv);
	void exitSocket(SOCKET &toclose);
	void addSocket(SOCKET &toadd);
};

