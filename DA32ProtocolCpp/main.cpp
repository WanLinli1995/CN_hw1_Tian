#include"DA32Protocol.h"

int main()
{
	string myName="New User";
	cout<<"�����������ǳƣ�";
	cin>>myName;
	Client::USERNAME=new char[myName.length()];
	strcpy(Client::USERNAME,myName.c_str());
	if(domainServer.Init(3232))
	{
		domainServer.begin();
		mainThread();//��ʼ��ѭ����
	}
	quitFunction();
	return 0;
}
