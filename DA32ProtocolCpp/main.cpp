#include"DA32Protocol.h"


vector<Client>clientList;
Server domainServer;

int main()
{
	string input;
	Client *newClient;
	if(domainServer.Init(3232))
	{
		domainServer.begin();
		cout<<"������Է���IP�ţ�"<<endl;
		cin>>input;
		try
		{
			newClient= new Client(input,3232);
		}
		catch(exception e)
		{

		}
		clientList.push_back(*newClient);
		while(1)
		{
			while(mtx_cout.try_lock()!=1);
			cout<<"������һ�������ı�������exit�˳�"<<endl;
			mtx_cout.unlock();
			cin>>input;
			if(input=="exit") break;
			newClient->sendData(input);	
		}
	}
	system("pause");
	clientList.clear();	
	return 0;
}
