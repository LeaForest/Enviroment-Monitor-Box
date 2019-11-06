#ifndef DISPLAY_H
#define DISPLAY_H
/**************室内温度显示***************
 *********************************************
 ********************************************/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <DHT.h>
#include <Time.h>
#include <Timezone.h>
#include "NTP.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>


/********************oled与DHT22初始化定义*********************************/
#define OLED_RESET   4     // define SSD1306 OLED reset at ESP8266 GPIO5 (NodeMCU D1)
Adafruit_SSD1306 display(OLED_RESET);
#define DHTPIN  13         // DHT22 data pin is connected to ESP8266 GPIO14 (NodeMCU D5)
#define DHTTYPE DHT22       // DHT22 sensor is used
DHT dht(DHTPIN, DHTTYPE);   // configure DHT library
char temperature[] = "000.0 C";
char humidity[]    = "000.0 %";
/*****************************网络初始化与时间定义****************************/
#define WIFI_SSID "HUAWEI-yuhan"       // 使用时请修改为当前你的 wifi ssid
#define WIFI_PASS "yuhan123456"   // 使用时请修改为当前你的 wifi 密码
#define ledPin 14                          // 定义ledPin连接到GPIO14
#define STD_TIMEZONE_OFFSET +8    // 时区更改为北京时间时区标准时间偏移 (-7 is mountain time)
TimeChangeRule mySTD = {"", First,  Sun, Jan, 0, STD_TIMEZONE_OFFSET * 60};// 中国制定的夏令时规则
Timezone myTZ(mySTD, mySTD);
time_t previousSecond = 0;// Previous seconds value
WiFiClient espClient;
WiFiClient client;
int weekdays;
int days;
int months;
int years;
int seconds;
int minutes;
int hours;
/********************天气API初始化与定义*********************************/
const char* host  ="api.seniverse.com";
const char* APIKEY  ="wcmquevztdy1jpca";       //API KEY
const char* city = "nanjing";
const char* language ="en";//zh-Hans 简体中文  会显示乱码
const unsigned long HTTP_TIMEOUT = 3900;               // max respone time from server---------------------------------------------
const size_t MAX_CONTENT_SIZE = 2000;                   // max size of the HTTP response

struct UserData {// 我们要从此网页中提取的数据的类型
  char city[16];//城市名称
  char weather[32];//天气介绍（多云...）
  char temp[16];//温度
  char udate[32];//更新时间
};

char response[MAX_CONTENT_SIZE];
char endOfHeaders[] = "\r\n\r\n";



void dhtDisplay() {
   int humidity = dht.readHumidity();
   float temperature = dht.readTemperature();
   Serial.print("Temp:");
   Serial.print(temperature);
   Serial.print(",    Humi:");
   Serial.println(humidity);
  display.setTextSize(1);
  display.setCursor(50, 0);
  display.print("Indoor");
  
  display.setCursor(5, 13);
  display.print("TEMP:(C)");
  display.setTextSize(2);
  display.setCursor(3, 26);
  display.print(temperature,1);


  display.setTextSize(1);
  display.setCursor(78, 13);
  display.print("HUMI:(%)");
  display.setTextSize(2);
  display.setCursor(86, 26);
  display.print(int(humidity));

  
  display.setTextSize(1);
  display.setCursor(0, 46);
  display.print("---------------------");
  display.setCursor(108, 54);
  display.print("2/3");
  display.setCursor(0, 54);
  display.print("DHT22");
  display.display();
  display.clearDisplay();
}
/**************NTP时间显示***************
 *********************************************
 ********************************************/
void timeDisplay() {
     display.setTextSize(1);
    if (years == NULL && hours == NULL) {
      display.setCursor(30, 0);
      display.print("NTP disconnect yet!");
    }else {
      display.setCursor(35, 0);
      display.print(years);
      display.setCursor(60, 0);
      display.print("/");
    if (months < 10) {
      display.setCursor(67, 0);
      display.print("0");
      display.setCursor(73, 0);// print data on the SSD1306 display
      display.print(months);
    } else {
      display.setCursor(67, 0);// print data on the SSD1306 display
      display.print(months);
    }
      display.setCursor(80, 0);
      display.print("/");
    if (days < 10) {
      display.setCursor(87, 0);
      display.print("0");
      display.setCursor(93, 0);// print data on the SSD1306 display
      display.print(days);
    } else {
      display.setCursor(87, 0);
      display.print(days);
    }
    
      display.setCursor(39, 12);
      display.print(dayStr(weekdays));


      
    display.setTextSize(2); 
    if (hours < 10) {
      display.setCursor(14, 27);
      display.print("0");
      display.setCursor(28, 27);
      display.print(hours);
    } else {
      display.setCursor(14, 27);
      display.print(hours);
    }
      display.setCursor(38, 27);
      display.print(":");
   if (minutes < 10) {
      display.setCursor(50, 27);
      display.print("0");
      display.setCursor(62, 27);
      display.print(minutes);
    } else {
      display.setCursor(50, 27);
      display.print(minutes);
    }
      display.setCursor(76, 27);
      display.print(":");
   if (seconds < 10) {
      display.setCursor(88, 27);
      display.print("0");
      display.setCursor(100, 27);
      display.print(seconds);
    } else {
      display.setCursor(88, 27);
      display.print(seconds);
    }
    
    display.setTextSize(1);
    display.setCursor(0, 46);
    display.print("---------------------");
    display.setCursor(108, 54);
    display.print("1/3");
    display.setCursor(0, 54);
    display.print("Clock");
    display.display();
   }
    display.clearDisplay();
}
/**************天气API显示***************
 *********************************************
 ********************************************/
void weatherDisplay(const struct UserData* userData){
  digitalWrite(16,HIGH);
  if(!strcmp(userData->weather,"Sunny")  ){
     display.drawBitmap(5, 3, Sunny1, 40, 40, 1);
  } else if(!strcmp(userData->weather,"Overcast")){
     display.drawBitmap(5, 3,Overcast1, 40, 40, 1);
   }  else if(!strcmp(userData->weather,"Partly cloudy")){
     display.drawBitmap(5, 3,PartlyCloudy1, 40, 40, 1);
   }else if(!strcmp(userData->weather,"Cloudy")){
     display.drawBitmap(5, 3, Cloudy1, 40, 40, 1);
   }else if(!strcmp(userData->weather,"Light rain")){
      display.drawBitmap(5, 3, LightRain1, 40, 40, 1);
   }else if(!strcmp(userData->weather,"Heavy rain")){
      display.drawBitmap(5, 3, HeavyRain1, 40, 40, 1);
   }else if(!strcmp(userData->weather,"Storm")){
      display.drawBitmap(5, 3, Storm1, 40, 40, 1);
   }else if(!strcmp(userData->weather,"Light snow")){
      display.drawBitmap(5, 3, LightSnow1, 40, 40, 1);
   }else if(!strcmp(userData->weather,"Heavy snow")){
      display.drawBitmap(5, 3, HeavySnow1, 40, 40, 1);
   }else if(!strcmp(userData->weather,"Sleet")){
      display.drawBitmap(5, 3, Sleet1, 40, 40, 1);
   }else{
      display.drawBitmap(5,3,Unknown1,40,40,1);
    }
   
    display.setTextSize(0);
    display.setCursor(70, 0);
    display.print(userData->city);

    display.drawBitmap(60, 10, wenduji, 24, 22, 1);
    display.setTextSize(2);
    display.setCursor(80, 15);
    display.print(userData->temp);
     display.setTextSize(1);
    display.drawBitmap(105, 18, sheshidu, 16, 12, 1);
    
    display.setTextSize(1);
    display.setCursor(60, 36);
    display.print(userData->weather);

    display.setTextSize(1);
    display.setCursor(0, 46);
    display.print("---------------------");
    display.setCursor(108, 54);
    display.print("3/3");
    display.setCursor(0, 54);
    display.print("Weather");
  
   display.display();
   display.clearDisplay();
  }  


/**************NTP时间获取*****************
 *********************************************
 ********************************************/
void updateDisplay(void) {// 这个函数每秒调用一次
  TimeChangeRule *tcr;        // 指向时间更改规则的指针
  time_t utc = now();// 从NTP提供程序读取当前UTC时间
  time_t localTime = myTZ.toLocal(utc, &tcr);// 考虑到DST，转换到本地时间

  weekdays =   weekday(localTime); // 将时间映射到像素位置
  days    =   day(localTime);
  months  =   month(localTime);
  years   =   year(localTime);
  seconds =   second(localTime);
  minutes =   minute(localTime);
  hours  =   hour(localTime) ;   //12小时使用格式: 12(localTime)  isPM()/isAM()

  Serial.print("{\"Date:");
  Serial.print(days);
  Serial.print("/");
  Serial.print(months);
  Serial.print("/");
  Serial.print(years);
  Serial.print("-");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.print("-");
  Serial.print(dayStr(weekdays));
  Serial.println("}");
}
  /********************天气获取*****************
 *********************************************
 ********************************************/
bool sendRequest(const char* host, const char* cityid, const char* apiKey) {
  String GetUrl = "/v3/weather/now.json?key=";
  GetUrl += apiKey;
  GetUrl += "&location=";
  GetUrl += city;
  GetUrl += "&language=";
  GetUrl += language;
  client.print(String("GET ") + GetUrl + " HTTP/1.1\r\n" +"Host: " + host + "\r\n" +"Connection: close\r\n\r\n");   
  return true;
}

bool skipResponseHeaders() {// HTTP headers end with an empty line
  bool ok = client.find(endOfHeaders);
  if (!ok) {
    Serial.println("No response or invalid response!");
    }
  return ok;
}
  

void readReponseContent(char* content, size_t maxSize) { //从HTTP服务器响应中读取正文
    size_t length = client.peekBytes(content, maxSize);
    delay(100);//----------------------------------------------------------------------------------------------------------
    //Serial.println("Get the data from Internet!");
    content[length] = 0;
    //Serial.println(content);
   //Serial.println("Read data Over!");
   client.flush();//这句代码需要加上  不然会发现每隔一次client.find会失败
}

bool parseUserData(char* content, struct UserData* userData) {
    //根据我们需要解析的数据来计算JSON缓冲区最佳大小,
    //const size_t BUFFER_SIZE = 1024;//如果你使用StaticJsonBuffer时才需要
    //StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;//在堆栈上分配一个临时内存池.如果堆栈的内存池太大，使用 DynamicJsonBuffer jsonBuffer 代替
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(content);
    if (!root.success()) {
        Serial.println("JSON parsing failed!");
        return false;
        }   
    strcpy(userData->city, root["results"][0]["location"]["name"]);
    strcpy(userData->weather, root["results"][0]["now"]["text"]);
    strcpy(userData->temp, root["results"][0]["now"]["temperature"]);    
    //strcpy(userData->pressur, root["results"][0]["now"]["pressure"]);
   //    strcpy(userData->visibi, root["results"][0]["now"]["visibility"]);
   // strcpy(userData->wind_direct, root["results"][0]["now"]["wind_direction"]);
   //  strcpy(userData->wind_s, root["results"][0]["now"]["wind_speed"]);
    strcpy(userData->udate, root["results"][0]["last_update"]);
    return true; //  -- 这不是强制复制，你可以使用指针，因为他们是指向“内容”缓冲区内，所以你需要确保当你读取字符串时它仍在内存中
}

void setup_wifi() { 
      delay(10);
      WiFi.begin(WIFI_SSID, WIFI_PASS);                //进行连接
      while (WiFi.status() != WL_CONNECTED) {    //串口显示状态，直至连接至网络
           delay(500);
           Serial.print(".");
           }
      randomSeed(micros());                       //micros()函数返回以微秒表示的时间并作为随机数
}

void reconnect() {                                                //MQTT服务重连函数
     while (!client.connected()) {                                //当未连接到MQTT服务时                                               
           String clientId = "nodemcu-soil";                          //设置客户端ID（自行更改）
           clientId += String(random(0xffff), HEX);                // 生成10进制的随机数作为客户端ID后缀
           if (client.connect("WLhr4IMdvHBKxPAFMGXg","WLhr4IMdvHBKxPAFMGXg","")) { //当客户端连至当前指定的代理时                 
               delay(20);
           } else {                                              
               delay(5000);                                         //延时5秒=5000ms
               }
           }
     }
void mqttsend(){
         delay(3000);//----------------------------------------------------------------------
       if (!client.connected()) {                                         //如果MQTT客户端断线
                 reconnect();                                                   //调用MQTT重连函数，确保连上服务器
                 }  
       client.loop();                                                     //MCU接收数据的主循环函数
       long now = millis();                                               //millis()函数从程序开始处返回毫秒
       if (now - lastMsg >1000) {                                         //每隔1秒发一次信号
                 lastMsg = now;                                                  //刷新上一次发送数据的时间
                 String payload = "{";                       //构建一个 JSON 格式的payload的字符串     
                 payload += "\"HUM\":"; 
                 payload += humidity;                             //引入温度变量
                 payload += ","; 
                 payload += "\"TEM\":"; 
                 payload += temperature;                             //引入温度变量
                 payload += "}";
                 char attributes[300];                         //发布主题的内容长度
                 payload.toCharArray( attributes, 100 );       //将pyload字串指向变量attributes
                 client.publish( "v1/devices/me/telemetry", attributes );      //发布的主题名称及存储在json格式payload下的字串（主题名可更改）
                 Serial.print("Pub Message: ");                                  //串口打印发布的信息
                 Serial.println(attributes);       
                 }   }
  
#endif
