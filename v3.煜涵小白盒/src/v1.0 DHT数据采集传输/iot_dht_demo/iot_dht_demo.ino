 /* 第三方开发板下载配置方法https://blog.csdn.net/u013810296/article/details/54862447?utm_source=blogxgwz3
   硬件：将0.96寸7针OLED，进行改造焊接R3置于R1，并短路R8；
   软定义：Wire.h进行SDA、SCL引脚的软定义
   引脚接线：CS置空、DC连3.3V、D1连SDA、D0连SCL,不采用默认IIC引脚（GPIO4、GPIO5），RES连软定义的复位引脚（便于封装焊接）
   本案列采用Adafruit_SSD1306.h库文件，不能直接使用，需要进行文件修改(本例OLED是SSD1306_128_64）)，解决方法:
        1.进入Arduino库文件夹，打开Adafruit_SSD1306.h (使用任意文本编辑器打开)
        2.找到行：#define SSD1306_128_32
        3.取消改行的注释: #define SSD1306_128_64
        4.在Arduino中打开实例文件： ssd1306_128x64_i2c
        5.在setup函数中将地址从0x3D更改为0x3C
为避免取模后出现错误显示，取模时进行设置：1.横向取模；2.保留最后一个逗号；3.只在字母前加0；
*/
/********************库文件引用*****************************************/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
/*****************************网络初始化****************************/
#define WIFI_SSID "TCGW"       // 使用时请修改为当前你的 wifi ssid
#define WIFI_PASS "tcgw1234"   // 使用时请修改为当前你的 wifi 密码                        
const char* mqtt_server = "139.224.9.212";
WiFiClient espClient;
PubSubClient client(espClient);
/********************oled与DHT22初始化定义*********************************/
#define OLED_RESET   4    
Adafruit_SSD1306 display(OLED_RESET);
#define DHTPIN  13       
#define DHTTYPE DHT22     
DHT dht(DHTPIN, DHTTYPE);  
/********************变量初始化*********************************/
float humidity,humiditybuf;
float temperature,temperaturebuf;
long lastmsg = 0;
long lastpub = 0;
char msg[50];


static const unsigned char PROGMEM Gateway[] = {
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFE,0xFB,0xFF,0xFF,0xC0,0x1C,0x00,0x7B,0xBF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x40,0x30,0x11,0xDA,0xDD,0xFF,0x7B,0x87,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFB,0xFD,0x7D,0xC0,0x1D,0xFF,0x77,0x77,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xE0,0x70,0x1D,0xFD,0xFC,0xEE,0x6C,0xAF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFE,0x7B,0xF5,0x5D,0x80,0x0D,0x55,0x63,0xDF,0xCF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x40,0x35,0x51,0xFD,0xFD,0xBB,0x7B,0xAF,0xCF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x6F,0x73,0x97,0xE0,0x3D,0xBB,0x76,0x73,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x60,0x77,0xD7,0xEF,0xBD,0x55,0x61,0x07,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x6F,0x70,0x17,0xEF,0xBC,0xEE,0x7F,0x77,0xCF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x60,0x77,0xD5,0xED,0xBD,0xFF,0x79,0x77,0xCF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x2F,0x70,0x15,0xED,0xBD,0xFF,0x67,0x07,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0x6E,0x77,0xD9,0x80,0x0D,0xFC,0x7F,0x77,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  };
  static const unsigned char PROGMEM Kuang[] = {
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,};\
static const unsigned char PROGMEM Kuang2[] = {
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};


/***********************************************************/
void setup(void) {
  Serial.begin(9600);
  dht.begin();
  pinMode(16,OUTPUT);

  Wire.begin(4, 5);           // set I2C pins [SDA = GPIO0 (D2), SCL = GPIO2 (D3)], default clock is 100kHz
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  
  display.clearDisplay(); // Clear the display buffer.
  display.drawFastHLine(0, 64, SSD1306_LCDWIDTH, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);

  WiFi.begin(WIFI_SSID, WIFI_PASS);     
  client.setServer(mqtt_server,1883); 
  delay(2000);
  Serial.println(F("[Debug]:初始化完成..."));
}

/******************************************************************/
void loop() {
  /*监测WiFi状态，并显示、提示连接的网关*/
 if (WiFi.status() != WL_CONNECTED) {                                         
        Serial.println(F("[Debug]:请连接Wifi..."));
        display.setTextColor(BLACK,WHITE );
        display.setTextSize(1);
        display.drawBitmap(0, 0, Gateway, 128, 19, 1);
        display.setTextColor(WHITE,BLACK);
        display.setTextSize(1);
        display.setCursor(0, 31);
        display.print("     SSID: TCGW"); 
        display.setCursor(0, 46);
        display.print("   PSWD: tcgw1234"); 
        display.display();
        display.clearDisplay();  
        delay(3500);}
        digitalWrite(16,HIGH);
           
        /*数据采集，采集的过程中会出现采集失败“nan”的正常状况，所以采用缓存来避免“nan”值的显示*/
        humidity = dht.readHumidity();
        temperature = dht.readTemperature();
        if(!isnan(temperature) && !isnan(humidity)){
             temperaturebuf = temperature;
             humiditybuf = humidity;
             Serial.print(F("温度："));
             Serial.print(temperature);
             Serial.print(F("  湿度："));
             Serial.println(humidity);
        } else{
             Serial.print(F("温度(buf)："));
             Serial.print(temperaturebuf);
             Serial.print(F("  湿度(buf)："));
             Serial.println(humiditybuf);
             }
      
      
        /*屏幕第一行的显示*/
        display.setTextColor(BLACK,WHITE );
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("                     ");
        display.setCursor(0, 2);
        display.print("     DHT-DEMO 05       ");
        /*第二行标签的显示*/
        display.setTextSize(1);
        display.setTextColor( WHITE,BLACK);
        display.setCursor(0, 10);
        display.print("                     ");
        display.setCursor(5, 13);
        display.print("TEMP(c)");
        display.setCursor(84, 13);
        display.print("HUMI(%)");
        display.setTextColor(WHITE,BLACK);
        //display.setCursor(0, 20);
        display.drawBitmap(0, 10, Kuang, 128, 15, 1);
        display.drawBitmap(0, 0, Kuang2, 128, 64, 1);
        //display.print("---------------------");
        /*第三行温湿度的显示*/
        display.setTextSize(2);
        display.setCursor(5, 38);
        if(!isnan(temperature)){
            display.print(temperature,1);
        }else{
            display.print(temperaturebuf,1);}
        display.setTextSize(2);
        display.setCursor(76, 38);
        if(!isnan(humidity)){
            display.print(humidity,1);
        }else{
            display.print(humiditybuf,1);}

        display.display();
        display.clearDisplay();  
       
        client.loop();                                                    
        long now = millis();                                               
        if (now - lastmsg >15000 && !isnan(dht.readTemperature())) {                                        
               lastmsg = now; 
               client.connect("LOCBx798RQ39ByN05LVd","LOCBx798RQ39ByN05LVd",""); 
                       /*jason字串拼接*/ 
                       String payload = "{";                     
                       payload += "\"TEMP\":"; 
                       payload += temperature;                            
                       payload += ",";
                       payload += "\"HUMI\":"; 
                       payload += humidity;
                       payload += "}";
                       /*数据发布*/
                       char attributes[300];                         
                       payload.toCharArray( attributes, 100 );       
                       client.publish( "v1/devices/me/telemetry", attributes );      
                       Serial.print(F("[Debug]:发布成功,内容为: "));                                  
                       digitalWrite(16,LOW); 
                       Serial.println(attributes);    
                       } 
                 delay(1000);
   }














   

/*
static const unsigned char PROGMEM Du[] = {//24x21
0x03,0x00,0x00,0x07,0x87,0xC0,0x06,0x9E,0xE8,0x07,0x98,0x38,0x03,0x30,0x18,0x00,
0x30,0x08,0x00,0x60,0x00,0x00,0x60,0x00,0x00,0x60,0x00,0x00,0x60,0x00,0x00,0x60,
0x00,0x00,0x30,0x00,0x00,0x30,0x08,0x00,0x18,0x38,0x00,0x0F,0xF0,0x00,0x07,0xC0,
0x12,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x31,0x36,0x00,
};
static const unsigned char PROGMEM Bai[] = {
0x00,0x00,0x00,0x0E,0x00,0xC0,0x1F,0x01,0xC0,0x19,0x03,0x80,0x19,0x07,0x00,0x19,
0x0E,0x00,0x19,0x1C,0x00,0x1F,0x38,0x00,0x0E,0x73,0x80,0x00,0xE7,0xC0,0x01,0xC4,
0xC0,0x03,0x84,0xC0,0x07,0x04,0xC0,0x0E,0x04,0xC0,0x0C,0x07,0xC0,0x00,0x03,0x80,
0x08,0x62,0x39,0x02,0x08,0x62,0x39,0x02,0x7C,0x06,0x00,0x00,0x10,0x00,0x00,};

第四行显示单位
display.setTextSize(1);
display.drawBitmap(9, 49, Du, 21, 21, 1);
display.drawBitmap(91, 48, Bai, 21, 21, 1);
*/
         /*避免过多的nan值显示而引用的测试代码：
       * client.loop();                                                     //MCU接收数据的主循环函数
        long now = millis();                                               //millis()函数从程序开始处返回毫秒
        if (now - lastmsg >5000 && !isnan(dht.readTemperature())) {                                         //每隔1秒发一次信号       
              lastmsg = now; 
               
               if(WiFi.status() != WL_CONNECTED){
                      Serial.println(F("[Debug]:WiFi未连接..."));
               }else{ Serial.println(F("[Debug]:WiFi已连接，即将连接服务..."));}
               client.connect("aH1oVsTHng8LZUKTxi55","aH1oVsTHng8LZUKTxi55",""); 
               if (!client.connected()) {                                         
                      Serial.println(F("[Debug]:MQTT服务未连接..."));                                                
               }else{
                      Serial.println(F("[Debug]:MQTT连接成功..."));                       
                       String payload = "{";                       //构建一个 JSON 格式的payload的字符串   
                       payload += "\"TEMP\":"; 
                       payload += temperature;                             //引入温度变量
                       payload += ",";
                       payload += "\"HUMI\":"; 
                       payload += humidity;
                       payload += "}";
                       
                       char attributes[300];                         //发布主题的内容长度
                       payload.toCharArray( attributes, 100 );       //将pyload字串指向变量attributes
                       client.publish( "v1/devices/me/telemetry", attributes );      //发布的主题名称及存储在json格式payload下的字串（主题名可更改）
                       Serial.print(F("[Debug]:发布成功,内容为: "));                                  //串口打印发布的信息
                       digitalWrite(16,LOW); 
                       Serial.println(attributes);    
                       } 
                 } 
                 delay(1000);*/
