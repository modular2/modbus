#include "mqtt.h"
mqtt::mqtt()
{
conn_opts = MQTTClient_connectOptions_initializer;
pubmsg = MQTTClient_message_initializer;

}
/*int mqtt::msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void mqtt::connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
    MQTTClient_connect();
}*/
int mqtt::connect()
{ int rc;
      MQTTClient_create(&client, ADDRESS, CLIENTID,
      MQTTCLIENT_PERSISTENCE_NONE, NULL);
      conn_opts.keepAliveInterval = 100;
       conn_opts.cleansession = 1;  
    // MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, NULL);  
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        return EXIT_FAILURE;
    }
    return 0;
}
int  mqtt::publish( const char * topic, char * message,int length) 
{   int rc;
    pubmsg.payload = message;
    pubmsg.payloadlen =length;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    if (!MQTTClient_isConnected(client))
    {
       if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
            {
                printf("Failed to connect, return code %d\n", rc);
                return EXIT_FAILURE;
            }
    }
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    printf("publish topic: %s message: %s\n", topic,message);
    return  MQTTClient_waitForCompletion(client, token, TIMEOUT);
}
 

 
    
