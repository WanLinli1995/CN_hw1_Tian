#pragma once
#include <string>
#include<fstream>
#include "include/json/json.h"  
using namespace std;
//��Ϣ����MyJson�������˽�Json����ȡ������Ϣ�Ĳ��������г�Ա����Ϊ��Ϣ������ݡ�
class MyJson
{
public:
	std::string name,type_s,md5_s,time_s,text;
	tm time; 
	int id;
	Json::Value _else;

public:
	MyJson(void);
	~MyJson(void);
	//����Json��Ϣ������������ʾ
	void showJson_in_console();
	//����Json��Ϣ�ķ�ʽһ����һ���ַ����л�ȡ
	void getJson(std::string charflow);
	//����Json��Ϣ�ķ�ʽ�������ļ��������л�ȡ
	void getJson(istream& charflow);

	//����Json��Ϣ��һ��Json���С�Ϊ���ʵ�壡
	void PackJson(std::string input);
};

