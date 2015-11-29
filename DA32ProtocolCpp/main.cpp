#include"DA32Protocol.h"


vector<Client>clientList;
Server domainServer;


bool findClient(Client *tofind)
{
	vector<Client>::iterator itr = find(clientList.begin(),clientList.end(),*tofind);
	if (itr!=clientList.end())
	{
		return true;
	}
	return false;
}

int main()
{
	string input,context;
	Client *newClient=new Client();
	int pass=0;

	cout<<"�����������ǳƣ�";
	cin>>myName;

	if(domainServer.Init(3232))
	{
		domainServer.begin();
		while(1)
		{
			pass=0;//���Client�Ƿ���ڵ�flag
			cout<<"�����硰�Է�IP ����(�ÿո�ֿ�)����"<<endl;
			cin>>input;//TODO:�����������ƾ����ˣ��㲻�������˿ո�֮���ֺ�����롤����backspace������ip������
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
				}
				clientList.push_back(*newClient);
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
	mtx.lock();
	for(int i=clientList.size()-1;i>=0;i--)
	{
		//clientList[i].~Client();
		clientList[i].exit();
		clientList.pop_back();
	}
	mtx.unlock();
	_sleep(1000);
	//system("pause");
	/*mtx.lock();
	domainServer.~Server();
	mtx.unlock();*/
	newClient=NULL;
	return 0;
}
