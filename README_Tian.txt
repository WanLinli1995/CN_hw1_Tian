1��json�ļ����е�.h�ļ���Ϊϵͳ�Դ���ֱ�ӵ��ü���
2��server��client�շ�
3��md5.cpp��md5У���
4��MyJson.cpp��������Ϣ����Jason��
5��DA32Protocol.h������Ҫ��Ԥ�����ļ�����Ҫά����Client�б��Server
6��MyTson.h��������Ϣ����MyJson
7��message.cpp��װJson����ͷβ
8��usinginmain.h��д���������еĸ��ֹ���
9��main.cpp������
**********************************************************************
usinginmain.h����main����ʵ�����������ܣ�
bool findClient(Client *tofind)�����Ƿ�����ҵ�һ��Client��1Ϊ�ҵ���
void mianThread()�����̣߳����롢��֤�ʹ���Client
void quitFunction()ʵ�ְ�ȫ�˳�
Client* checkClientList(SOCKADDR_IN add)���ͻ������ӱ����ص�ǰ�ͻ���
void deleteClient(Client* c)ɾ���ͻ���
void initServer()���ֳ�ʼ��������
**********************************************************************
MyJson.cpp
tm convert_string_to_time_t(const std::string & time_string)���̶���ʽ���ַ���������ʱ��ṹtm
Json�ຯ���Ķ���
bool MyJson::getJson(std::string charflow)���ַ����л��һ��Json����1Ϊ�ɹ�
bool MyJson::getJson(istream& charflow)���ļ��ж���һ��Json����1�ɹ�
void MyJson::showJson_in_console()���Json
string MyJson::PackJson(std::string input)���
**********************************************************************
Message.cpp
GBKToUTF8��UTF8ToGBK:GBK�����UTF8����ת��
MyJson Message::getContent(string dataflow)��������Json
string Message::getWrap(string tosend)��װ������UTF8
**********************************************************************