     /* GND(oled)-GND(nano)
 * VCC(oled)-5v(nano)
 * D0(oled)-D13(nano)
 * D1(oled)-D11(nano)
 * RES(oled)-RST(nano)
 * DC(nano)-D9(nano)
 * CS(nano)-D10(nano)
 */
#include <dht.h> // DHT11数字传感器库
#include "U8glib.h" // OLED库
#include  "Adafruit_NeoPixel.h"

U8GLIB_SSD1306_128X64 u8g(13, 11, 10, 9);//7引脚SPI接口
dht DHT;
#define DHT22_PIN 4
int num=0;
/*********************2、数据状态显示******************************
 **************************************************************/
void NormalDisplay(void) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_8x13); //使用8x13大小的字符
    u8g.drawFrame(2, 2, 122, 60);
    u8g.setPrintPos(12, 20); // 对应 x，y 轴值
     u8g.print("Status:Normal!");
    u8g.setPrintPos(20, 40); // 对应 x，y 轴值
    u8g.print("HUM:");
    u8g.setPrintPos(60, 40);
    u8g.print((float)DHT.humidity);
    u8g.setPrintPos(105, 40);
    u8g.print("%");
    u8g.setPrintPos(20, 56);
    u8g.print("TEM:");
    u8g.setPrintPos(60, 56);
    u8g.print((float)DHT.temperature);
    u8g.setPrintPos(105, 56);
    u8g.print("C");
  } while ( u8g.nextPage() );
}
/**************************************************************
 **************************************************************/
void TempWarning(void) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_8x13); //使用8x13大小的字符
    u8g.drawFrame(2, 2, 122, 60);
    u8g.setPrintPos(30, 20); // 对应 x，y 轴值
     u8g.print("Status:Hot!");
    u8g.setPrintPos(20, 40); // 对应 x，y 轴值
    u8g.print("HUM:");
    u8g.setPrintPos(60, 40);
    u8g.print((float)DHT.humidity);
    u8g.setPrintPos(105, 40);
    u8g.print("%");
    u8g.setPrintPos(20, 56);
    u8g.print("TEM:");
    u8g.setPrintPos(60, 56);
    u8g.print((float)DHT.temperature);
    u8g.setPrintPos(105, 56);
    u8g.print("C");
  } while ( u8g.nextPage() );
}

void setup() {
  Serial.begin(9600);//串口波特率
  /***********初始动画***********/
  for (int i = 0; i < 100; i++) {
    u8g.firstPage();
    do {
      u8g.setFont(u8g_font_osb18);
      u8g.drawStr(21, 30, "Loading");
      u8g.setFont(u8g_font_8x13);
      u8g.setPrintPos(50, 44);
      u8g.print(i);
      u8g.drawStr(65, 44, "%");
      delay(1);
    } while ( u8g.nextPage());
  }
  delay(1000);
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_osb18);
    u8g.drawStr(18, 38, "welcome!");
    delay(250);
  } while ( u8g.nextPage());
  delay(1000);
}



void loop() {
  /**************温湿度获取并向串口打印***********************/
  int chk = DHT.read22(DHT22_PIN);  //读取数据
  Serial.print("{\"Humidity\":");
  Serial.print((float)DHT.humidity, 2);
  Serial.print(",\"Temperature\":");
  Serial.print((float)DHT.temperature, 2);
  Serial.print(",\"Num\":");
  Serial.print((int)num++);
  Serial.println("}");
  /*******************启动OLED数据显示*****************************/
  if ((float)DHT.temperature<35){
      NormalDisplay();
      }
  else{
      TempWarning();    
  }
  delay(1000);
}





