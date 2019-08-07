#include <streambuf>
#include <sstream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/base64/base64.hpp>
#include "main.hpp"
#include <iostream>
#include <vector>
using namespace std;
using namespace rapidjson;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
typedef void ( *BaseMessageCallback)(string request, string* response);
typedef void ( *HtmlCallback)(string request, string* response);
typedef void ( *ServerMessageCallback)(string request, string* response);
int sendMessage(string message);
void start_web(int port);
string uri;
client c;
server s;
bool connected;
int app_port;
websocketpp::connection_hdl base_hd; 
BaseMessageCallback baseMessageCallback;
HtmlCallback htmlCallback;
ServerMessageCallback serverMessageCallback;  
// 类定义
class splitstring : public string {
    vector<string> flds;
public:
    splitstring(char *s) : string(s) { };
    vector<string>& split(char delim, int rep=0);
};
vector<string>& splitstring::split(char delim, int rep) {
    if (!flds.empty()) flds.clear();  // empty vector if necessary
    string work = data();
    string buf = "";
    int i = 0;
    while (i < work.length()) {
        if (work[i] != delim)
            buf += work[i];
        else if (rep == 1) {
            flds.push_back(buf);
            buf = "";
        } else if (buf.length() > 0) {
            flds.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        flds.push_back(buf);
    return flds;
}
 void * sender_thread(void *arg)
 {
     while(true)
    { 
       if (connected)
       {   
           string msg_payload;
           msg_payload="{\"method\":\"ping\",\"id\":0}";
         //  cout<<"ping to: "<<msg_payload<<endl;
        sendMessage(msg_payload); 
       }
       sleep(2);
    }   
 }
 void * web_thread(void *arg)
 {
     start_web(app_port);
     return 0;
 }
bool ReadFileContent(string filename,string * buffer)
{
      streampos size;
  char * memblock;
ifstream rfile;
string line;
//cout<<"open file :"<<filename<<endl;
rfile.open(filename.c_str(),ios::binary|ios::ate);
if (rfile.is_open()) {
  size = rfile.tellg();
 // cout<<"file size :"<<size<<endl;
    memblock = new char [size];
    rfile.seekg (0, ios::beg);
    rfile.read (memblock, size);
    rfile.close();
    string ss(memblock,size);// char * to string Convert
     *buffer= ss;
     return true;
} else
    return false;
 
 
}
void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg)
{
 int i,p;
    string response;
    base_hd=hdl;
    connected=true;
   baseOnMessage(msg->get_payload());
}
 bool s_validate(server *, websocketpp::connection_hdl) {
	//sleep(6);
	return true;
}
 void s_on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) 
 {
     std::cout << "Close handler" << std::endl;
 }

  void s_on_http(server* ser, websocketpp::connection_hdl hdl)
  {
     server::connection_ptr con = ser->get_con_from_hdl(hdl);
 
	    websocketpp::http::parser::request rt = con->get_request();
		const string& strUri = rt.get_uri();
		const string& strMethod = rt.get_method();
		const string& strBody = rt.get_body();	//只针对post时有数据
		const string& strHost = rt.get_header("host");
		const string& strVersion = rt.get_version();
      cout<<"receive an request: "<<strUri.c_str()<<endl;
 if (strUri.compare("/home.html") == 0)
 { 
    static const string strHtmlPath = "./views/index.html";
     std::ifstream file;
				string strBuffer;
				int code = websocketpp::http::status_code::ok;
				if( ReadFileContent(strHtmlPath,&strBuffer))
					con->set_body(strBuffer);
				 
 }  else
 { 
       char * writable = new char[strUri.size() + 1];
       std::copy(strUri.begin(), strUri.end(), writable);
       writable[strUri.size()] = '\0';
       splitstring uri(writable);
       vector<string> detail= uri.split('/') ;
        delete[] writable;
    cout<<"class :"<<detail[0]<<endl;
       if (detail[0].compare("views")==0)
     {
          string strBuffer;
     if(ReadFileContent("./views/"+detail[1],&strBuffer))  
                con->set_body(strBuffer); 
     }
     else
     if (detail[0].compare("css")==0)
     {
     //cout<<"file name: "<<detail[1]<<endl;
   
     string strBuffer;
     if(ReadFileContent("./css/"+detail[1],&strBuffer))  
                con->set_body(strBuffer);
           con->replace_header("content-type","text/css");        
     } else
      if (detail[0].compare("js")==0)
     {
     //cout<<"file name: "<<detail[1]<<endl;
   
     string strBuffer;
     if(ReadFileContent("./js/"+detail[1],&strBuffer))
                con->set_body(strBuffer);
            con->replace_header("content-type","text/js");  
     } else
      if (detail[0].compare("images")==0) 
      {
     static const string strIcoPath ="./images/"+detail[1];
            string strBuffer;
            if (ReadFileContent("./images/"+detail[1],&strBuffer))      
			 con->set_body(strBuffer);
     }  if (detail[0].compare("font")==0) 
     {
         string strBuffer;
           char * writable = new char[detail[1].size() + 1];
        std::copy(detail[1].begin(), detail[1].end(), writable);
        writable[detail[1].size()] = '\0';
        splitstring fontfile(writable);
        vector<string> fn= fontfile.split('?') ;
        delete[] writable;
        cout<<"font: "<<fn[0]<<endl;
            if (ReadFileContent("./font/"+fn[0],&strBuffer))      
			 con->set_body(strBuffer);
     }
      else
       if (detail[0].compare("api")==0) 
       {   string strBuffer;
          webAPI(strBody,&strBuffer);
         con->set_body(strBuffer);  
       }                 
 }
	con->set_status(websocketpp::http::status_code::ok);
  }
void s_on_open(server* s, websocketpp::connection_hdl hdl)
{
    cout << "open handler" << std::endl;
}
void s_on_close(websocketpp::connection_hdl hdl)
{
std::cout << "Close handler" << std::endl;
} 
 void s_on_fail(server* s, websocketpp::connection_hdl hdl) {
	server::connection_ptr con = s->get_con_from_hdl(hdl);
 
	std::cout << "Fail handler: " << con->get_ec() << " " << con->get_ec().message() << std::endl;
}
int sendMessage(string message)
{
     websocketpp::lib::error_code ec;
   c.send(base_hd, message,websocketpp::frame::opcode::TEXT, ec);

    if (ec) {
         std::cout << "Echo failed because: " << ec.message() << std::endl;
         return -1;
     }
     return 0;
}
void start_web(int port)
{
    try {
         s.clear_access_channels(websocketpp::log::alevel::all);
		s.set_message_handler(bind(&s_on_message, &s, ::_1, ::_2));
		s.set_http_handler(bind(&s_on_http, &s, ::_1));
		s.set_fail_handler(bind(&s_on_fail, &s, ::_1));
		s.set_open_handler(bind(&s_on_open, &s, ::_1));
		s.set_close_handler(bind(&s_on_close, ::_1));
		s.set_validate_handler(bind(&s_validate, &s, ::_1));
 
		// Initialize ASIO
		s.init_asio();
		s.set_reuse_addr(true);
 
		// Listen on port 9100
		s.listen(port);
		// Start the server accept loop
		s.start_accept();
 
		// Start the ASIO io_service run loop
		s.run();
 
		//stop
		s.stop();
	}
	catch (websocketpp::exception const & e) {
		std::cout << e.what() << std::endl;
        }
}

void init(int port)
{  
     uri = "ws://localhost:2019/iosocket/0123";
    connected=false;
    app_port=port;
     pthread_t a_thread;
    pthread_t w_thread;
   pthread_create(&a_thread, NULL, sender_thread, NULL);    
   pthread_create(&w_thread, NULL, web_thread, NULL); 
     try {
        // Set logging to be pretty verbose (everything except message payloads)
      //  c.set_access_channels(websocketpp::log::alevel::all);
       // c.clear_access_channels(websocketpp::log::alevel::frame_payload);
       // c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::all);
        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        c.set_message_handler(bind(&on_message,&c,::_1,::_2));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
           // return 0;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);
        
        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } 
    
}
