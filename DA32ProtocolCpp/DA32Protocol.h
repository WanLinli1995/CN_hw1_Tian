/*
�뽫��Ҫ��Ԥ�����ļ���������ļ��ڡ�
*/
#pragma comment(lib,"lib_json_d.lib") 
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
#include<mutex>
//TODO:������ͷ�ļ�
//eg:#include "xx.h"
   
using namespace std; 

extern mutex mtx_server_acceptlist;//��������acceptlist�Ļ�����
extern mutex mtx_cout;//��������cout�Ļ�����
extern mutex mtx_clientlist;
extern mutex mtx_handle;