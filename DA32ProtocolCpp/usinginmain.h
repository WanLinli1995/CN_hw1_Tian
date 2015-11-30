#pragma once
#include"DA32Protocol.h"
extern string GBKToUTF8(const std::string& strGBK);
namespace usinginmain
{
	mutex c_mtx;
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
	///���̣߳���ʵ�ϲ���һ���̣߳�����һ��������
	void mainThread()
	{
		string input,context;
		Client *newClient;
		int pass=0;
		while(1)
		{
			newClient=new Client();
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
			//context=context+"\n"+"��һ�س���";
			c_mtx.lock();
			if(findClient(newClient)) newClient->sendData(context);	
			else
			{
				c_mtx.unlock();
				break;
			}
			c_mtx.unlock();
		}
	}
	///��ȫ�˳�����Ҫ�Ĳ���
	void quitFunction()
	{
		for(int i=clientList.size()-1;i>=0;i--)
		{
			clientList[i].exit();
			c_mtx.lock();
			clientList.pop_back();
			c_mtx.unlock();
		}
		_sleep(1000);
	}

	Client* checkClientList(SOCKADDR_IN add)
	{
		Client * respond_client=NULL;
		for(int i=0;i<clientList.size();i++)
		{
			if(inet_ntoa(clientList[i].getAddr().sin_addr)==inet_ntoa(add.sin_addr))
			{
				respond_client=&clientList[i];//��������Դ�ڱ����Ŀͻ������ӱ���
				break;
			}
		}
		//�˲���Ӧ�ɻص��������
		if(respond_client==NULL)//������ڣ����½�һ���ͻ���
		{
			respond_client=new Client();
			while(respond_client->newClient(inet_ntoa(add.sin_addr),3232)==false);
			c_mtx.lock();
			clientList.push_back(*respond_client);//���뵽List��
			c_mtx.unlock();
		}
		return respond_client;
	}

	void deleteClient(Client* c)
	{
		c_mtx.lock();
		vector<Client>::iterator itr = find(clientList.begin(),clientList.end(),*c);
		if (itr!=clientList.end())//����ҵ������Client�����ţ���ɱ����
		{
			itr->exit();
			clientList.erase(itr);
		}
		c_mtx.unlock();
	}

	void respendClient(Client* c)
	{
		c->respend();
	}

	void elsefunction(Client* c)
	{

	}

	void initServer()
	{
		allarea::ThreadParament tp;
		tp.checkClientList=(function<Client* (SOCKADDR_IN)>)(usinginmain::checkClientList);
		tp.deleteClient=(function<void (Client*)>)(usinginmain::deleteClient);
		tp.respendClient=(function<void (Client*)>)(usinginmain::respendClient);
		tp.elsefunction=(function<void (Client*)>)(usinginmain::elsefunction);
		domainServer=new Server(tp);
	}
}