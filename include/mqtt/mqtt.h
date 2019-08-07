#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pahomqtt/MQTTClient.h"
#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "baseIO"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         0
#define TIMEOUT     10000L
class mqtt {
    public:
        mqtt();
        int  connect();
        int  publish( const char * topic,  char * message,int length) ;
    private:
        MQTTClient client;
        MQTTClient_connectOptions conn_opts ;
        MQTTClient_message pubmsg;
        MQTTClient_deliveryToken token;     
};
