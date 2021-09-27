#define BWater 3 // Пін датчика наявності води
#define BStop 41 // Пін кнопки СТОП

#define Pservo 5  //Пін на якій підключена серво

#define CLK 31  //Пін на якій підключено CLK від енкодера
#define DT 33  //Пін на якій підключено DT від енкодера
#define SW 35  //Пін на якій підключено SW від енкодера

#define Ds18b20 4  //Пін на якій підключено ds18b20
#define Termistor A13  //Пін на якій підключено термістор

const char *filename = "/Test.txt";  // Назва файлу в якому лежить json
const char *FileDataName = "/Data.txt";  // Назва файлу в якому лежить json з змінними

#include <LiquidCrystal.h>            //  Подключаем библиотеку LiquidCrystal для работы с LCD дисплеем
LiquidCrystal lcd(16,17,23,25,27,29); //  Объявляем объект библиотеки, указывая выводы дисплея (RS,E,D4,D5,D6,D7)

#include "GyverEncoder.h"  //  Подключаем библиотеку для работы с енкодером
Encoder enc1(CLK, DT, SW);
int value = 0;
boolean Edirection; //Напрвалення енкодера

#include <ArduinoJson.h>  //  Подключаем библиотеку для работы с json
#include <SD.h>
#include <SPI.h>
const int PIN_CHIP_SELECT = 53; // Ініціалізація бібліотек 
String input; // Строка в якій буде зберігатись файл json
String inputJson; // Строка в якій буде зберігатись файл json
String inputData; // Строка в якій буде зберігатись файл jsonData

#include <Servo.h> //  Подключаем библиотеку для работы с серво
Servo Lservo;
int pos;//Позиція серво

#include <OneWire.h>  //  Подключаем библиотеку для работы по одному піну
#include <DallasTemperature.h>  //  Подключаем библиотеку для работы с ds18b20
OneWire oneWire(Ds18b20);
DallasTemperature sensors(&oneWire);
DeviceAddress *sensorsUnique;
int countSensors;
int temperature[10];

#define B 3950 // B-коэффициент
#define SERIAL_R 4700 // сопротивление последовательного резистора
#define THERMISTOR_R 100000 // номинальное сопротивления термистора, 100 кОм
#define NOMINAL_T 25 // номинальная температура (при которой TR = 100 кОм)
int termis;

#include "SparkFunHTU21D.h"   // Подключаем библиотеку SparkFunHTU21D
HTU21D myHumidity;  // Создаем объект для работы с датчиком
int temp;  //Змінна для температури
int humd;  //Змінна для вологості

#include <buildTime.h>
#include <microDS3231.h>
MicroDS3231 rtc;

long last_time;
boolean time_flag;
boolean Water;
int YMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; //Вказано скільки днів в кожному місяці 
boolean mode = 0;//Змінна для запису прочитаного з json
int Month = 0;//Змінна для запису прочитаного з json
int Day = 0;//Змінна для запису прочитаного з json
int Hours = 0;//Змінна для запису прочитаного з json
int Min = 0;//Змінна для запису прочитаного з json
int IDay = 0;//Змінна для запису днів які пройшли після запуску інкубації (звертатись після обрахунку)
int IHours = 0;//Змінна для запису годин які пройшли після запуску інкубації (звертатись після обрахунку)
int IMin = 0;//Змінна для запису хвилин які пройшли після запуску інкубації (звертатись після обрахунку)

int menu = 0; //Змінна яка використовується в показі меню
int error;    //Змінна яка використовується в показі меню


void Time (){   //Функція обрахунку часу
  
  if((rtc.getMonth()-Month)<0){
    IDay = (YMonth[Month-1]-Day) + rtc.getDate();
  }else{
    IDay = rtc.getDate() - Day;
  }
  if((rtc.getHours()-Hours)<0){
    IDay--;
    IHours = (24-Hours)+rtc.getHours();
  }else{
    IHours = rtc.getHours()-Hours;
  }
  if((rtc.getMinutes()-Min)<0){
    IHours--;
    IMin = (60-Min)+rtc.getMinutes();
  }else{
    IMin = rtc.getMinutes()-Min;
  }
  Serial.println();
  Serial.print("IDay ");
  Serial.println(IDay);
  Serial.print("IHours ");
  Serial.println(IHours);
  Serial.print("IMin ");
  Serial.println(IMin);
}

void DravMenu(int menu,int number,boolean mode,boolean Edirection){  //Функція показу меню
  switch (menu) {
    case 1:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Stop incubation?");
      lcd.setCursor(6, 2);
      lcd.print("NO   YES");
  
      if (Edirection == 0){
      lcd.setCursor(5, 2);
      lcd.print(">");
      }else{
      lcd.setCursor(10, 2);
      lcd.print(">");
      }
      break;
    default:
      lcd.clear();
      if (error == 0){
        if (mode==1){
        lcd.setCursor(2, 0);
        lcd.print("Incubation");
        lcd.setCursor(14, 0);
        lcd.print("Day:");
        lcd.setCursor(18, 0);
        lcd.print(IDay);
        }else{
        lcd.setCursor(2, 0);
        lcd.print("START");}
        } if (error == 1){
        lcd.setCursor(2, 0);
        lcd.print("Check all sensors");
      }
      if (pos == 0){
        lcd.setCursor(2, 1);
        lcd.write(byte(0));
      }else if(pos == -1){
        lcd.setCursor(2, 1);
        lcd.write(1);
      }else if(pos == 1){
        lcd.setCursor(2, 1);
        lcd.write(2);
      }

      lcd.setCursor(4, 1);
      lcd.print("TBed:");
      lcd.print(termis);
      lcd.write(3);
      
      lcd.setCursor(14, 1);
      lcd.print("T :");
      lcd.setCursor(15, 1);       
      lcd.write(byte(0));
      lcd.setCursor(17, 1);
      if(error == 0){lcd.print(String(temperature[2]));
      }else{lcd.print("--");}
      lcd.write(3);
      


      lcd.setCursor(2, 2);
      lcd.print("TAir:");      
      lcd.print(temp);
      lcd.write(3);
      lcd.setCursor(11, 2);
      lcd.print("HAir:");      
      lcd.print(humd);
      lcd.print("%");




      
  lcd.setCursor(0, number);
  lcd.print(">");

  
  lcd.setCursor(6, 3);
  lcd.print(rtc.getDate());
  lcd.print(".");
  lcd.print(rtc.getMonth());
  lcd.print(".");
  lcd.print(rtc.getYear()-2000);
  lcd.setCursor(15, 3);
  lcd.print(rtc.getHours());
  lcd.print(":");
  lcd.print(rtc.getMinutes());

  if (Water == 1){
    lcd.setCursor(4, 3);
  lcd.write(5);
  }else{
    lcd.setCursor(4, 3);
  lcd.write(6);
  }
  
  lcd.setCursor(2, 3);
  lcd.write(4);
  break;
  }

}

void servo(int angle) {
  if(angle==89)pos = 0;
  if(angle==106)pos = -1;
  if(angle==69)pos = 1;
  Lservo.write(angle);
}

void beginAll() {
    Serial.begin(9600);  //швидкість Serial
    lcd.begin(20, 4);  //  Инициируем работу с LCD дисплеем, указывая количество (столбцов, строк)
    enc1.setType(TYPE1);  //Инициализация енкодера
    sensors.begin();  //Инициализация DS18B20
    myHumidity.begin();  // Инициализация HTU21D
    DynamicJsonBuffer jsonBuffer;
    while (!SD.begin()) {      // При помилці ініціалізувати SD бібліотеку
    Serial.println(F("Помилка ініціалізації SD бібліотеки"));  // Вивести в серіал "Помилка ініціалізації SD бібліотеки"
    Iicons();  //Функція ініціалізації іконок
    delay(100);
  }
}

void ds18b20() {
  countSensors = sensors.getDeviceCount();
  countSensors = 3;
  sensorsUnique = new DeviceAddress[countSensors];
  for (int i = 0; i < countSensors; i++) {
    sensors.getAddress(sensorsUnique[i], i);
  }
  for (int i = 0; i < countSensors; i++) {
    sensors.setResolution(sensorsUnique[i], 12);
  }
  sensors.requestTemperatures();
  for (int i = 0; i < countSensors; i++) {
    temperature[i] = sensors.getTempCByIndex(i);
  }

    if(temperature[0]<=-10 || temperature[1]<=-10 || temperature[2]<=-10){error=1;
  }else{error=0;}
}

void termistor() {
  int t = analogRead( Termistor );
  float tr = 1023.0 / t - 1;
  tr = SERIAL_R / tr;

  float steinhart;
  steinhart = tr / THERMISTOR_R; // (R/Ro)
  steinhart = log(steinhart); // ln(R/Ro)
  steinhart /= B; // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NOMINAL_T + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart; // Invert
  steinhart -= 273.15; 
  termis = int (steinhart);
}



void setup() {
  beginAll();  //Функція ініціалізації всього
  DataJSON(FileDataName);  // Функція читання jsonData
  Time ();  //Функція обрахунку часу (працює після читання jsonData)
  //  rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);  //Розкоментувати для задання часу вручну (потім закоментувати і перезаписати код)

  Lservo.attach(Pservo);  // Пін серво
  pinMode(BStop, INPUT_PULLUP);  //Встановити пін як вхід з підтяжкою до +
  pinMode(BWater, INPUT_PULLUP);  //Встановити пін як вхід з підтяжкою до +
  pinMode( Termistor, INPUT );  //Встановити пін як вхід

  DravMenu(menu,value,mode,Edirection);  //Функція показу меню
}

void loop() {
  enc1.tick();
  Water = digitalRead(BWater);
  termistor();
  if (millis() - last_time > 1000){
    DravMenu(menu,value,mode,Edirection);
    last_time = millis();
    if (time_flag == 1) {
     // ds18b20();
    }
    time_flag = !time_flag;
  }
  humd = myHumidity.readHumidity();    // Считываем влажность
  temp = myHumidity.readTemperature(); // Считываем температуру
  
  if (enc1.isRight()) {value++;      // если был поворот направо, увеличиваем на 1
  Edirection = 0;}
  if (enc1.isLeft()) {value--;
  Edirection = 1;}
   if (enc1.isTurn()) {       // если был совершён поворот (индикатор поворота в любую сторону)
   // DravMenu(menu,value,mode,Edirection);
  } 

  if (enc1.isPress()){
    if (menu == 0&&mode == 0&&value==0 ){
      DataWrite(FileDataName,1);
      inputData = "";
      DataJSON(FileDataName);
      DravMenu(menu,value,mode,Edirection);
    }
    if (menu == 1&&Edirection == 0){
      menu = 0;
      value = 0;  
      DravMenu(menu,value,mode,Edirection);
    }
    if (menu == 1&&Edirection == 1){
      menu = 0;
      value = 0;
      DataWrite(FileDataName,0);
      inputData = "";
      DataJSON(FileDataName);
      DravMenu(menu,value,mode,Edirection);
    }
  }
  //DravMenu(menu,value,mode,Edirection);
  if (digitalRead(BStop)==0&&mode==1){
    menu = 1;
    Edirection = 0;
    DravMenu(menu,value,mode,Edirection);
    Serial.println("stop");
  }
}
