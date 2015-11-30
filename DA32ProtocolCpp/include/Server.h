#pragma once
#include <Winsock2.h>
#include<vector>
#include<thread>
#include<mutex>
#include"Client.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;

namespace allarea
{
	//TODO���������
	///�̴߳��ݲ���
	struct ThreadParament
	{
		function<Client* (SOCKADDR_IN)> checkClientList;
		function<void (Client*)> deleteClient;
		function<void (Client*)> respendClient;
		function<void (Client*)> elsefunction;
	};
}



class Server
{
private:
	SOCKET sockSrv;	
	SOCKADDR_IN addrSrv;
	WORD wVersionRequested;
	WSADATA wsaData;
	vector<SOCKET> sockAccept;//Ӧ��socket�б�ʵ���ϲ�û���õ�
	std::vector<std::thread *> mThread;

private:	
	int max_binner;//��������������,Ĭ�ϻ���20
	function <Client* (SOCKADDR_IN)> checkClientList;
	function <void (Client*)> deleteClient;
	function<void (Client*)> respendClient;
	function<void (Client*)> elsefunction;
	std::mutex *mtx;//��������ֻ�ڷ��̡߳����߳���ʹ�ã�����������ʹ��

public:
	bool Init(int port_id=3232);
	Server(void);
	Server(allarea::ThreadParament tp);
	~Server(void);
	//Server(int port_id=3232);
	void set_max_binner(int n){max_binner=n;}//�������������
	int begin();//�����ؿ�ʼ��������̣߳���hThread�е�������
	vector<SOCKET> get_AcceptList(){return sockAccept;}
	SOCKET get_SockSrv(){return sockSrv;}
	vector<thread*> get_hThread(){return mThread;}
	int isSocketAlreadyInVector(SOCKADDR_IN addr);
	string receiveData(SOCKET torcv);
	void exitSocket(SOCKET &toclose);
	void addSocket(SOCKET &toadd);
	void ListenerThread();
	void ReceiveThread(LPVOID lparam);	
};