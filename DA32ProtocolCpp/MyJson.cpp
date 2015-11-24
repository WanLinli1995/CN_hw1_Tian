#include "include/MyJson.h"
#include"include/md5.h"
//�������������̶���ʽ���ַ���������ʱ��ṹtm
tm convert_string_to_time_t(const std::string & time_string)  
{  
    struct tm tm1;  
    time_t time1;  
	int i = sscanf(time_string.c_str(), "%d.%d.%d %d:%d:%d" , &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday),  &(tm1.tm_hour),&(tm1.tm_min),&(tm1.tm_sec));              
    return tm1;  
}  

MyJson::MyJson(void)
{
}

MyJson::~MyJson(void)
{
}

bool MyJson::getJson(std::string charflow)
{
	Json::Reader reader;
    Json::Value root;
	std::string md5_check;
	char *p;
    if (!reader.parse(charflow, root, false))
    {
		return false;
    }
 
    name = root["data"]["name"].asString();
	id=root["id"].asInt();
	type_s=root["type"].asString();
	time_s=root["time"].asString();
	text=root["data"]["text"].asString();
	_else=root["else"];
	md5_s=root["md5"].asString();
	time=convert_string_to_time_t(time_s);
	p=(char*)&id;
	md5_check=*p;
	md5_check=md5_check+*(p+1)+*(p+2)+*(p+3);
	md5_check=md5_check+type_s+time_s+name+text;
	MD5 md5(md5_check);
	string result = md5.md5();
	if (result!=md5_s)
	{
		//У�����
		cout<<"md5 should be:"<<result<<endl;
		cout<<"now md5 is:"<<md5_s<<endl;
		cout<<"md5У��ʧ�ܣ���"<<endl;
		return false;
	}
	return true;
}

bool MyJson::getJson(istream& charflow)
{
	Json::Reader reader;
    Json::Value root;
	std::string md5_check;
	char *p;
    if (!reader.parse(charflow, root, false))
    {
		return false;
    }
 
    name = root["data"]["name"].asString();
	id=root["id"].asInt();
	type_s=root["type"].asString();
	time_s=root["time"].asString();
	text=root["data"]["text"].asString();
	_else=root["else"];
	md5_s=root["md5"].asString();
	time=convert_string_to_time_t(time_s);
	p=(char*)&id;
	md5_check=*p;
	md5_check=md5_check+*(p+1)+*(p+2)+*(p+3);
	md5_check=md5_check+type_s+time_s+name+text;
	MD5 md5(md5_check);
	string result = md5.md5();
	if (result!=md5_s)
	{
		//У�����
		cout<<"md5 should be:"<<result<<endl;
		cout<<"now md5 is:"<<md5_s<<endl;
		cout<<"md5У��ʧ�ܣ���"<<endl;
		return false;
	}
	return true;
}

void MyJson::showJson_in_console()
{
	std::cout<<this->name<<std::endl;
    std::cout<<this->id<<std::endl;
	std::cout<<this->time_s<<std::endl;
	std::cout<<this->text<<std::endl;

	system("pause");
}
char* MyJson::PackJson(std::string input)
{
	std::string root;
	std::string md5_check;
	char*p;
	int a;
	//this->showJson_in_console();
	root="{\"id\":1,\"type\":\""+this->type_s+"\",\"time\":\""+this->time_s+"\",\"else\":{},\"data\":{\"name\":\""+this->name+"\",\"text\":\"";
	this->text=input;
	p=(char*)&id;
	md5_check=*p;
	md5_check=md5_check+*(p+1)+*(p+2)+*(p+3);
	md5_check=md5_check+type_s+time_s+name+text;
	MD5 md5(md5_check);
	md5_s=md5.md5();
	this->showJson_in_console();
	/*�������봦��
	*/
	int pos=0;
	while(pos=this->text.find("\\",pos)+1)
	{	
		text.replace(pos-1,1,"\\\\");
		pos++;
	}
	pos=0;
	while(pos=this->text.find("\r",pos)+1)
	{	
		text.replace(pos-1,1,"\\\r");
		pos++;
	}
	pos=0;
	while(pos=this->text.find("\n",pos)+1)
	{	
		text.replace(pos-1,1,"\\\n");
		pos++;
	}
	pos=0;
	while(pos=this->text.find("\'",pos)+1)
	{	
		text.replace(pos-1,1,"\\\'");
		pos++;
	}
	pos=0;
	while(pos=this->text.find("\"",pos)+1)
	{	
		text.replace(pos-1,1,"\\\"");
		pos++;
	}
	this->showJson_in_console();
	root=root+text+"\"},\"md5\":\""+md5_s+"\"}";
	p=new char[root.length()];
	strcpy(p,root.c_str());
	return p;
}