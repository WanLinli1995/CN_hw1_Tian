/*
�뽫��Ҫ��Ԥ�����ļ���������ļ��ڡ�
*/
#pragma once
#pragma comment(lib,"lib_json.lib") 
#include<iostream>
#include "include/json/json.h"  
#include <fstream>  
#include <string>  
#include <cassert>
#include<time.h>
#include"include/md5.h"
#include"include/Client.h"
#include"include/Message.h"
#include"include/MyJson.h"
#include"include/Server.h"
#include<vector>
//TODO:������ͷ�ļ�
//eg:#include "xx.h"
   
using namespace std; 
vector<Client>clientList;//��Ҫά����Client�б�
Server *domainServer;//��Ҫά����Server
