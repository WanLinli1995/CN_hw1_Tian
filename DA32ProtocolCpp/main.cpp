#include"DA32Protocol.h"
int main()
{
	MyJson test;
	Json::Value _el;
	ifstream ifs;
	string input,packed;
	//һЩ��ʼ������
	test.name="Cwx";
	test.type_s="text";
	test._else=_el;
	test.id=1;

	cout<<"����Ҫ������Ϣ��";
	cin>>input;
	test.text=input;
	packed=test.PackJson(input);
	cout<<packed;//����õ�packed
	/*ifs.open("test/testjson.json");
	assert(ifs.is_open());
	if(!test.getJson(ifs)) return -1;
	cout<<"from filestream"<<endl;
	test.showJson_in_console();*/
	cout<<"from charstream"<<endl;
	test.getJson(packed);
	test.showJson_in_console();
    return 0;
}