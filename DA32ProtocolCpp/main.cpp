#include"DA32Protocol.h"
#include"usinginmain.h"
int main()
{
	usinginmain::initServer();
	string myName="New User";
	cout<<"�����������ǳƣ�";
	cin>>myName;
	Client::USERNAME=new char[myName.size()];
	strcpy(Client::USERNAME,myName.c_str());
	if(domainServer->Init(3232))
	{
		domainServer->begin();
		usinginmain::mainThread();//��ʼ��ѭ����
	}
	usinginmain::quitFunction();
	return 0;
}
