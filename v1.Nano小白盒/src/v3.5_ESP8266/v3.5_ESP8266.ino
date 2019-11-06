#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "HUAWEI-yuhan";
const char* password = "yuhan123456";
const char* mqtt_server = "139.224.9.212";

WiFiClient espClient;
PubSubClient client(espClient);

String _databuffer="";
long lastMsg = 0;
char msg[70];
int value = 0;

void setup_wifi() {
      delay(10);
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
           delay(500);
           }
      randomSeed(micros());
    }

void callback(char* topic, byte* payload, unsigned int length) {
      for (int i = 0; i < length; i++) {
          }
      if ((char)payload[0] == '1') {   //如果收到的数据是1
          digitalWrite(BUILTIN_LED, LOW);   
      } else {
          digitalWrite(BUILTIN_LED, HIGH); 
      }
}
 
void reconnect() {        
     while (!client.connected()) {                                             
           String clientId = "ESP8266-v3.4-";
           clientId += String(random(0xffff), HEX);// Create a random client ID
           if (client.connect("cfCpXuQ4TKfPLpBlMOcP","cfCpXuQ4TKfPLpBlMOcP","")) {
               client.subscribe("mySub2");
               delay(20);
           } else {
               delay(5000);
               }
           }
     }

void setup() {
     pinMode(BUILTIN_LED, OUTPUT);    
     Serial.begin(9600);
     setup_wifi();
     client.setServer(mqtt_server, 1883);
     client.setCallback(callback);
      //while(Serial.read()>=0){};其存在与否无影响
      digitalWrite(BUILTIN_LED, LOW); 
     }

void loop() {
       if (!client.connected()) {
           reconnect();//确保连上服务器，否则一直等待
           }  
       client.loop();//MUC接收数据的主循环函数
       long now = millis();
       if (now - lastMsg >1000) {   //每隔1秒发一次信号
           lastMsg = now;     //刷新上一次发送数据的时间
           ++value; 
     while(Serial.available()){
        char dataread=(char)Serial.read();
        _databuffer+=dataread;
        }
     snprintf (msg, 75, "%s", _databuffer.c_str()); //string转char型/*此处函数变量的_databuffer.c_str()为字符串转字串类型，输出使用"%s"与"%c"的影响：
     client.publish("v1/devices/me/telemetry", msg);
      _databuffer=""; 
       digitalWrite(BUILTIN_LED, HIGH);
           }      
      }
