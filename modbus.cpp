#include <streambuf>
#include <sstream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <pthread.h>
#include "fft/fft.h"
#include <math.h>
#include <webservice.h>
#include "base64B.h"
#include <string>
#include <cstring>
#include <stdio.h>
#define FFTPoint 128
using namespace std;
using namespace rapidjson;
int id=0;
 int get_id()
 {
     id=id+1;
     if (id>5000) id=0;
     return id;    
 }
void webAPI(string request,string *response)
{
    Document req;
    req.Parse(request.c_str());
    cout<<"jsonstring method:"<<req["method"].GetString()<<endl;
    *response="{\"result\":\"OK\"}";
}
void registerApp(string appName,int port)
{
   string path="000/000/000";
    Document message;
    message.SetObject();
    Document::AllocatorType& allocator = message.GetAllocator();   
     message.AddMember("method","register.app",allocator);
      message.AddMember("mode",1,allocator);
     Value vstring(kStringType);
      vstring.SetString(path.c_str(),path.size(), allocator); 
      message.AddMember("path",vstring,allocator); 
     Value params(kObjectType);
      vstring.SetString(appName.c_str(),appName.size(), allocator); 
     params.AddMember("name",vstring,allocator); 
      params.AddMember("port",port,allocator); 
     message.AddMember("params",params,allocator); 
     message.AddMember("id",get_id(),allocator);
     StringBuffer buffer;
    buffer.Clear();
    Writer<StringBuffer> writer(buffer);
    message.Accept(writer);
    string msg_payload=buffer.GetString();
   // cout<<"msg payload: "<<msg_payload<<"\n"<<endl;
    sendMessage(msg_payload); 
}
void on_connect_base()
{
    registerApp("modbus",9101);
} 
void baseOnMessage(string request)
{
 int i,p;
     string path;
    int mode;
    string methodName;
    connected=true;
    Document req;
    Value result(kObjectType);
    req.Parse(request.c_str());

   if (req.HasMember("mode"))
   {
       mode=req["mode"].GetInt();
   }
   if( req.HasMember("path"))
    {
        path=req["path"].GetString();
    }
    if (req.HasMember("method"))
    {
        methodName=req["method"].GetString();
         if (methodName.compare("welcome")==0) 
         {
            on_connect_base();
         }
    }     
}
int main()
{
   cout<<"modbus control App"<<endl;
     init(9101);
}