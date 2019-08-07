#if !defined(baseService_H)
#define baseService_H
#include <string>
using namespace std;
int   excutiveRequest(string request,string key);
string  sendEventMessage(const char * method,int pin,uint8_t *buff,int len);
#endif