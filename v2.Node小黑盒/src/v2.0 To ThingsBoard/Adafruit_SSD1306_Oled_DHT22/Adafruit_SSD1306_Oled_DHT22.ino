/* 第三方开发板下载配置方法https://blog.csdn.net/u013810296/article/details/54862447?utm_source=blogxgwz3
 *  api文档地址：https://www.seniverse.com/doc
   此案列采用的是Adafruit_SSD1306.h库文件；
   oled进行了IIC的焊接更改，将背部电阻R3换至R1，再将R8短路；
   同样采用了Wire.h进行SDA、SCL引脚的软定义,CS引脚置空、DC置3.3V、D1连SDA（GPIO4）、D0连SCL(GPIO0),所以未采用默认IIC引脚（GPIO4、GPIO5）
   采用了引脚定义复位按钮（也可以直接连接RST引脚）
   本案列存在着显示的问题(下载下来的库不能直接使用，有几处需要修改(本例OLED是SSD1306_128_64）)，解决方法:
        1.进入Arduino库文件夹，修改 Adafruit_SSD1306.h (使用任意文本编辑器打开)
        2.找到并注释掉改行: #define SSD1306_128_32
        3.取消改行的注释: #define SSD1306_128_64
        4.在Arduino中打开实例文件： ssd1306_128x64_i2c
        5.在setup函数中将地址从0x3D更改为0x3C
  NTP选用阿里服务器：ntp1.aliyun.com,地址：203,107,6,88
  为避免取模后出现错误显示，取模时进行设置：1.横向取模；2.保留最后一个逗号；3.只在字母前加0；
*/
/********************库文件引用*************************
 *********************************************/
#include "icon.h"
#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Wire.h>
#include <DHT.h>

#include <Time.h>
#include <Timezone.h>
#include "NTP.h"

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>


const char* mqtt_server = "139.224.9.212";
PubsubClient client(espClient);
long lastMsg = 0;
char msg[50];




/*****************初始化函数******************
 ********************************************/
void setup(void) {
  Serial.begin(9600);
  pinMode(16,OUTPUT);
  dht.begin(); 
  delay(100);
  initNTP(WIFI_SSID, WIFI_PASS);
  Wire.begin(0, 2);           // set I2C pins [SDA = GPIO0 (D2), SCL = GPIO2 (D3)], default clock is 100kHz
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay(); // Clear the display buffer.
  display.drawFastHLine(0, 64, SSD1306_LCDWIDTH, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  WiFi.begin(WIFI_SSID, WIFI_PASS);   //连接wifi  
  while (WiFi.status() != WL_CONNECTED) {//这个函数是wifi连接状态，返回wifi链接状态
  delay(100);}
client.setTimeout(HTTP_TIMEOUT);
}
/**************定义变量与初始化***************
 ********************************************/
void loop() {
  digitalWrite(16,LOW); 
  if (timeStatus() != timeNotSet) {//  只有在时间发生变化时才更新显示
    if (second() != previousSecond) {
      previousSecond = second();
      updateDisplay(); // 更新显示
      //delay(450);//-------------------------------------------------------
      }
    }   timeDisplay();               
        delay(3000);
        dhtDisplay();
  while (!client.connected()){
     if (!client.connect(host, 80)){
         Serial.println("connection....");
         }
     }
  if(sendRequest(host, city, APIKEY) && skipResponseHeaders()) {
     memset(response, 0, MAX_CONTENT_SIZE);      //清空
     readReponseContent(response, sizeof(response));
     UserData userData;
     if (parseUserData(response, &userData)) {
        printUserData(&userData);
        weatherDisplay(&userData);
        }
     }
mqttsend();
}




void printUserData(const struct UserData* userData) {// 打印从JSON中提取的数据
    Serial.print("City : ");
    Serial.print(userData->city);
    Serial.print(", \t");
    Serial.print("Weather : ");
    Serial.print(userData->weather);
    Serial.print(",\t");
    Serial.print("Temp : ");
    Serial.print(userData->temp);
    Serial.print(" C");
    Serial.print(",\t");
    Serial.print("Last Updata : ");
    Serial.println(userData->udate);
}
