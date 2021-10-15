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
const char *FileSattingsName = "/Settings.txt";  // Назва файлу в якому лежить json
const char *EGGList[] = {
  "Chicken",
  "Quail",
};

int numlist = sizeof(EGGList)/2;
#include <LiquidCrystal.h>            //  Подключаем библиотеку LiquidCrystal для работы с LCD дисплеем
LiquidCrystal lcd(16,17,23,25,27,29); //  Объявляем объект библиотеки, указывая выводы дисплея (RS,E,D4,D5,D6,D7)

#include "GyverEncoder.h"  //  Подключаем библиотеку для работы с енкодером

Encoder enc1(CLK, DT, SW);
int value = 0;
int val = 0;
boolean Edirection; //Напрвалення енкодера

#include <ArduinoJson.h>  //  Подключаем библиотеку для работы с json
#include <SD.h>
#include <SPI.h>
const int PIN_CHIP_SELECT = 53; // Ініціалізація бібліотек 
String input; // Строка в якій буде зберігатись файл json
String inputJson; // Строка в якій буде зберігатись файл json
String inputData; // Строка в якій буде зберігатись файл jsonData
String inputSett;

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
float termis;

#include "SparkFunHTU21D.h"   // Подключаем библиотеку SparkFunHTU21D
HTU21D myHumidity;  // Создаем объект для работы с датчиком
int temp;  //Змінна для температури
int humd;  //Змінна для вологості

#include <buildTime.h>
#include <microDS3231.h>
MicroDS3231 rtc;

long last_time;
boolean time_flag;
int dopp_tine_flag;
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
boolean flag; //Змінна яка використовується в показі меню
int Pflag;
int error;    //Змінна яка використовується в показі меню
int val_m = 0;
int dopp;


int egg; //0-Chicken   1-Quail
float STemp;
int SHumd;
int SFan;
int SAiring;
boolean TMode;
boolean HMode;
boolean FMode;
boolean AMode;
boolean SBuzzer;
boolean SServo;


void Time (){   //Функція обрахунку часу
  
  if((rtc.getMonth()-Month)>0){
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
      lcd.setCursor(2, 0);
      lcd.print("Stop incubation?");
      lcd.setCursor(6, 2);
      lcd.print("NO   YES");
  
      if (Edirection == 0){
        lcd.setCursor(5, 2);
        lcd.print(">");
        lcd.setCursor(10, 2);
        lcd.print(" ");
      }else{
        lcd.setCursor(5, 2);
        lcd.print(" ");
        lcd.setCursor(10, 2);
        lcd.print(">");
      }
    break;

    case 2:
      if(number >= 0 && number <= 3){
        if(flag == 1){
          lcd.clear();
          flag = 0;
        }
        lcd.setCursor(2, 0);
        lcd.print("Exit   Settings");
        if(mode == 1){
         lcd.setCursor(1, 1);
         lcd.write(7);
        }
        lcd.setCursor(2, 1);
        lcd.print("Egg:");
        lcd.setCursor(2, 2);
        lcd.print("Temp:       M:");
        lcd.setCursor(2, 3);
        lcd.print("Humd:       M:");
      }else{
        if(flag == 0){
          lcd.clear();
          flag = 1;
        }
        lcd.setCursor(2, 0);
        lcd.print("Fan:        M:");
        lcd.setCursor(2, 1);
        lcd.print("Vent:       M:");
        lcd.setCursor(2, 2);
        lcd.print("Buzzer:");
        lcd.setCursor(2, 3);
        lcd.print("Servo:");
      }
      //----------- вивід інформації --------------
      if(flag == 0){
        lcd.setCursor(7, 1);
        lcd.print(EGGList[egg] + String("    "));
        lcd.setCursor(8, 2);
        lcd.print(STemp,1);
        lcd.write(3);
        if (TMode == 0){
          lcd.setCursor(16, 2);
          lcd.print("Auto");
        }else{
          lcd.setCursor(16, 2);
          lcd.print("On  ");
        }
        lcd.setCursor(8, 3);
        lcd.print(SHumd);
        lcd.print("%");
        if (HMode == 0){
          lcd.setCursor(16, 3);
          lcd.print("Auto");
        }else{
          lcd.setCursor(16, 3);
          lcd.print("On  ");
        }
      }else{
        lcd.setCursor(7, 0);
        lcd.print(SFan);
        lcd.print("%");
        if (FMode == 0){
          lcd.setCursor(16, 0);
          lcd.print("Auto");
        }else{
          lcd.setCursor(16, 0);
          lcd.print("On  ");
        }
        lcd.setCursor(8, 1);
        lcd.print(SAiring);
        lcd.print("%");
        if (AMode == 0){
          lcd.setCursor(16, 1);
          lcd.print("Auto");
        }else{
          lcd.setCursor(16, 1);
          lcd.print("On  ");
        }
        lcd.setCursor(10,2);
        if(SBuzzer == 0){
          lcd.print("Off");
        }else{
          lcd.print("On ");
        }
        lcd.setCursor(9  ,3);
        if(SServo == 0){
          lcd.print("Auto      ");
        }else{
          lcd.print("Horizontal");
        }
      }
      if (Pflag == 1){
        if (val == 0){
          lcd.setCursor(0, 1);
          lcd.print(" >");
          lcd.setCursor(6, 1);
          lcd.print(" ");
        }
        if (val == 1){
          lcd.setCursor(6, 1);
          lcd.print(">");
          lcd.setCursor(1, 1);
          lcd.print(" ");
        }
      }else if (Pflag == 2){
        if (val == 0){
          lcd.setCursor(0, 2);
          lcd.print(" >");
          lcd.setCursor(7, 2);
          lcd.print(" ");
          lcd.setCursor(13, 2);
          lcd.print(" ");
        }
        if (val == 1){
          lcd.setCursor(7, 2);
          lcd.print(">");
          lcd.setCursor(13, 2);
          lcd.print(" ");
          lcd.setCursor(1, 2);
          lcd.print(" ");
        }
        if (val == 2){
          lcd.setCursor(13, 2);
          lcd.print(">");
          lcd.setCursor(7, 2);
          lcd.print(" ");
          lcd.setCursor(1, 2);
          lcd.print(" ");
        }
      }else if (Pflag == 3){
        if (val == 0){
          lcd.setCursor(0, 3);
          lcd.print(" >");
          lcd.setCursor(7, 3);
          lcd.print(" ");
          lcd.setCursor(13, 3);
          lcd.print(" ");
        }
        if (val == 1){
          lcd.setCursor(7, 3);
          lcd.print(">");
          lcd.setCursor(13, 3);
          lcd.print(" ");
          lcd.setCursor(1, 3);
          lcd.print(" ");
        }
        if (val == 2){
          lcd.setCursor(13, 3);
          lcd.print(">");
          lcd.setCursor(7, 3);
          lcd.print(" ");
          lcd.setCursor(1, 3);
          lcd.print(" ");
        }
      }else if (Pflag == 4){
        if (val == 0){
          lcd.setCursor(0, 0);
          lcd.print(" >");
          lcd.setCursor(6, 0);
          lcd.print(" ");
          lcd.setCursor(13, 0);
          lcd.print(" ");
        }
        if (val == 1){
          lcd.setCursor(6, 0);
          lcd.print(">");
          lcd.setCursor(13, 0);
          lcd.print(" ");
          lcd.setCursor(1, 0);
          lcd.print(" ");
        }
        if (val == 2){
          lcd.setCursor(13, 0);
          lcd.print(">");
          lcd.setCursor(6, 0);
          lcd.print(" ");
          lcd.setCursor(1, 0);
          lcd.print(" ");
        }
      }else if (Pflag == 5){
        if (val == 0){
          lcd.setCursor(0, 1);
          lcd.print(" >");
          lcd.setCursor(7, 1);
          lcd.print(" ");
          lcd.setCursor(13, 1);
          lcd.print(" ");
        }
        if (val == 1){
          lcd.setCursor(7, 1);
          lcd.print(">");
          lcd.setCursor(13, 1);
          lcd.print(" ");
          lcd.setCursor(1, 1);
          lcd.print(" ");
        }
        if (val == 2){
          lcd.setCursor(13, 1);
          lcd.print(">");
          lcd.setCursor(7, 1);
          lcd.print(" ");
          lcd.setCursor(1, 1);
          lcd.print(" ");
        }
      }else if (Pflag == 6){
        if (val == 0){
          lcd.setCursor(0, 2);
          lcd.print(" >");
          lcd.setCursor(9, 2);
          lcd.print(" ");
        }
        if (val == 1){
          lcd.setCursor(9, 2);
          lcd.print(">");
          lcd.setCursor(1, 2);
          lcd.print(" ");
        }
      }else if (Pflag == 7){
        if (val == 0){
          lcd.setCursor(0, 3);
          lcd.print(" >");
          lcd.setCursor(8, 3);
          lcd.print(" ");
        }
        if (val == 1){
          lcd.setCursor(8, 3);
          lcd.print(">");
          lcd.setCursor(1, 3);
          lcd.print(" ");
        }
      }else{
        if(number >= 0 && number <= 3){
          lcd.setCursor(0, number);
          if(mode == 1&&number == 1){
            lcd.print(">");
          }else{
            lcd.print("> ");
          }
        }
        if(number == 0){
          lcd.setCursor(0, number+1);
          lcd.print(" ");
        }if(number == 1 or number == 2){
          lcd.setCursor(0, number+1);
          lcd.print(" ");
          lcd.setCursor(0, number-1);
          lcd.print(" ");
        }if(number == 3){
          lcd.setCursor(0, number-1);
          lcd.print("  ");
          lcd.setCursor(0, number-3);
          lcd.print("  ");
        }
        if(number >= 4 && number <= 7){
          lcd.setCursor(0, number-4);
        lcd.print("> ");
        }
        if(number == 4){
          lcd.setCursor(0, number+1-4);
          lcd.print("  ");
          lcd.setCursor(0, number+3-4);
          lcd.print("  ");
        }if(number == 5 or number == 6){
          lcd.setCursor(0, number+1-4);
          lcd.print("  ");
          lcd.setCursor(0, number-1-4);
          lcd.print("  ");
        }if(number == 7){
          lcd.setCursor(0, number-1-4);
          lcd.print("  ");
        }
      }
    break;

    default:
      if (error == 0){
        if (mode==1){
        lcd.setCursor(2, 0);
        lcd.print("Incubation  Day:");
        }else{
        lcd.setCursor(2, 0);
        lcd.print("START           ");
        }
      }else if (error == 1){
        lcd.setCursor(2, 0);
        lcd.print("Insert the sd card");
      }else if (error == 2){
        lcd.setCursor(2, 0);
        lcd.print("Check humd sensor ");
      }else if (error == 3){
        lcd.setCursor(2, 0);
        lcd.print("Add water Inc.D:  ");
        
      }
      lcd.setCursor(4, 1);
      lcd.print("TBed:     T :");
      lcd.setCursor(15, 1);       
      lcd.write(byte(0));

      lcd.setCursor(2, 2);
      lcd.print("TAir:    HAir:"); 
           
      //----------- вивід інформації --------------
      
      if (error == 0 or error == 3 && mode == 1){
        lcd.setCursor(18, 0);
        if (IDay <= 9){
          lcd.print(IDay);
          lcd.print(" ");
        }else if(IDay >= 10&&mode == 1){
          lcd.print(IDay);
        }
      }
      switch (pos) {
        case 0:
          lcd.setCursor(2, 1);
          lcd.write(byte(0));
        break;
        case 1:
          lcd.setCursor(2, 1);
          lcd.write(1);
        break;
        case 2:
          lcd.setCursor(2, 1);
          lcd.write(2);
        break;
      }

      lcd.setCursor(9, 1);
      lcd.print(termis,0);
      lcd.write(3);
      lcd.setCursor(17, 1);
      if(error != 1){lcd.print(String(temperature[2]));
      }else{lcd.print("--");}
      lcd.write(3);
      
      lcd.setCursor(7, 2);    
      if (temp <= 99){
          lcd.print(temp);
          lcd.write(3);
          lcd.print(" ");
      }else{
          lcd.print(temp);
          lcd.write(3);
      }
      lcd.setCursor(16, 2);
      if (humd <= 99){
          lcd.print(humd);
          lcd.print("%");
          lcd.print(" ");
      }else{
          lcd.print(humd);
          lcd.print("%");
      }

      lcd.setCursor(2, 3);
      lcd.write(4);
      if (Water == 1){
        lcd.setCursor(4, 3);
        lcd.write(5);
      }else{
        lcd.setCursor(4, 3);
        lcd.write(6);
      }
      if (rtc.getDate() <= 9){
          lcd.setCursor(13, 3);
          lcd.print(" ");
      }if (rtc.getMonth() <= 9){
          lcd.setCursor(12, 3);
          lcd.print(" ");
      }


      
      lcd.setCursor(6, 3);
      lcd.print(rtc.getDate());
      lcd.print(".");
      lcd.print(rtc.getMonth());
      lcd.print(".");
      lcd.print(rtc.getYear()-2000);
      
      lcd.setCursor(15, 3);
      if (rtc.getHours() <= 9){
          lcd.print(" ");
          lcd.print(rtc.getHours());
      }else{
          lcd.print(rtc.getHours());
      }
      lcd.print(":");
      if (rtc.getMinutes() <= 9){
          lcd.print(rtc.getMinutes());
          lcd.print(" ");
      }else{
          lcd.print(rtc.getMinutes());
      }
      lcd.setCursor(0, number);
      lcd.print(">");
      if(number == 0){
        lcd.setCursor(0, number+1);
        lcd.print(" ");
        lcd.setCursor(0, 3);
        lcd.print(" ");
      }if(number == 1 or number == 2){
        lcd.setCursor(0, number+1);
        lcd.print(" ");
        lcd.setCursor(0, number-1);
        lcd.print(" ");
      }if(number == 3){
        lcd.setCursor(0, number-1);
        lcd.print(" ");
        lcd.setCursor(0, 0);
        lcd.print(" ");
      }
      
      

      
    break;
  }
}

void servo(int angle) {
  if(angle==89)pos = 0;
  if(angle==106)pos = 1;
  if(angle==69)pos = 2;
  Lservo.write(angle);
}

void beginAll() {
    Serial.begin(9600);  //швидкість Serial
    lcd.begin(20, 4);  //  Инициируем работу с LCD дисплеем, указывая количество (столбцов, строк)
    //enc1.setType(TYPE1);  //Инициализация енкодера
    enc1.setTickMode(AUTO);
    sensors.begin();  //Инициализация DS18B20
    myHumidity.begin();  // Инициализация HTU21D
    DynamicJsonBuffer jsonBuffer;
    if (!SD.begin()&&error == 0) {      // При помилці ініціалізувати SD бібліотеку
      error = 1;
    }
    //Iicons();  //Функція ініціалізації іконок
    Iicons();
    delay(100);
    SetinJSON(FileSattingsName);
    DataJSON(FileDataName);  // Функція читання jsonData
    Time ();  //Функція обрахунку часу (працює після читання jsonData)
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

   // if(temperature[0]<=-10 or temperature[1]<=-10 or temperature[2]<=-10&&error == 0){error=2;
   // }else if(error == 2){/*error = 0;*/}
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
  termis = steinhart;
}



void setup() {
  beginAll();  //Функція ініціалізації всього
  //  rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);  //Розкоментувати для задання часу вручну (потім закоментувати і перезаписати код)

  Lservo.attach(Pservo);  // Пін серво
  pinMode(BStop, INPUT_PULLUP);  //Встановити пін як вхід з підтяжкою до +
  pinMode(BWater, INPUT_PULLUP);  //Встановити пін як вхід з підтяжкою до +
  pinMode( Termistor, INPUT );  //Встановити пін як вхід

  DravMenu(menu,value,mode,Edirection);  //Функція показу меню
}

void loop() {
  //enc1.tick();
  Water = digitalRead(BWater);
  termistor();
  
  if (millis() - last_time > 1000){
    DravMenu(menu,value,mode,Edirection);
    last_time = millis();
    if (time_flag == 1) {
      dopp_tine_flag ++;
      humd = myHumidity.readHumidity();    // Считываем влажность
      if (!SD.begin()&&error == 0) {      // При помилці ініціалізувати SD бібліотеку
        error = 1;
        DravMenu(menu,value,mode,Edirection);  //Функція показу меню
      }
      if(humd <800&&error==2){
        error = 0;
      }
      if(humd >=800 or temp >= 800&&error == 0){
        error = 2;
      }
      if(Water == 0&&error==3){
        error = 0;
      }
      if(Water == 1&&error == 0){
        error = 3;
      }
      if(error==1){
        tone(37, 700, 100);
        if (SD.begin()) {      // При ініціалізації SD бібліотекі
          error = 0;
          beginAll();
        }
      }else if(error>1){
        if(SBuzzer = 1){
          tone(37, 700, 100);
        }
      }
    }
    time_flag = !time_flag;
  }
  if(dopp_tine_flag==5){
    temp = myHumidity.readTemperature(); // Считываем температуру
    ds18b20();
    dopp_tine_flag=0;
  }
  
  if (enc1.isLeft()) {
    if (Pflag == 0){
      value++;      // если был поворот направо, увеличиваем на 1
      Edirection = 1;
    }else{
      val++;}}
  if (enc1.isRight()) {
    if (Pflag == 0){
      value--;
      Edirection = 0;
    }else{
      val--;}}
   if (enc1.isTurn()) {       // если был совершён поворот (индикатор поворота в любую сторону)
    if(value <= -1&&menu == 0)value=0;
    if(value >= 4&&menu == 0)value=3;
    if(value <= -1&&menu == 2)value=0;
    if(value >= 8&&menu == 2)value=7;
    DravMenu(menu,value,mode,Edirection);
    Serial.print("val: ");
    Serial.println(val);
    Serial.print("Pflag: ");
    Serial.println(Pflag);
    if(val_m == 0){
      if(val <= -1&&menu == 2)val=0;
      if(val >= 2&&menu == 2)val=1;
    }else if(val_m == 1){
      if(val <= -1&&menu == 2)val=0;
      if(val >= 3&&menu == 2)val=2;
    }
    if(egg <= -1&&menu == 2)egg=0;
    if(egg >= numlist&&menu == 2)egg=(numlist-1);
  }
   if (enc1.isRightH()){
     if (menu == 2&&Pflag == 1&&val == 1){
       egg --;
       if(egg <= -1&&menu == 2)egg=0;
     }
     if (menu == 2&&Pflag == 2){
       if(val == 1)STemp = STemp-0.1;
       if(val == 2)TMode = 0;
       if(STemp <= 29.9&&menu == 2)STemp=30;
     }
     if (menu == 2&&Pflag == 3){
       if(val == 1)SHumd = SHumd-1;
       if(val == 2)HMode = 0;
       if(SHumd <= 19&&menu == 2)SHumd=20;
     }
     if (menu == 2&&Pflag == 4){
       if(val == 1)SFan = SFan-5;
       if(val == 2)FMode = 0;
       if(SFan <= -1&&menu == 2)SFan=0;
     }
     if (menu == 2&&Pflag == 5){
       if(val == 1)SAiring = SAiring-5;
       if(val == 2)AMode = 0;
       if(SAiring <= -1&&menu == 2)SAiring=0;
     }
     if (menu == 2&&Pflag == 6&&val == 1){
       SBuzzer = 0;
     }
     if (menu == 2&&Pflag == 7&&val == 1){
       SServo = 0;
     }
   DravMenu(menu,value,mode,Edirection);
   }
   if (enc1.isLeftH()){
     if (menu == 2&&Pflag == 1&&val == 1){
       egg ++;
       if(egg >= numlist&&menu == 2)egg=(numlist-1);
     }
     if (menu == 2&&Pflag == 2){
       if(val == 1)STemp = STemp+0.1;
       if(val == 2)TMode = 1;
       if(STemp >= 40.1&&menu == 2)STemp=40;
     }
     if (menu == 2&&Pflag == 3){
       if(val == 1)SHumd = SHumd+1;
       if(val == 2)HMode = 1;
       if(SHumd >= 81&&menu == 2)SHumd=80;
     }
     if (menu == 2&&Pflag == 4){
       if(val == 1)SFan = SFan+5;
       if(val == 2)FMode = 1;
       if(SFan >= 101&&menu == 2)SFan=100;
     }
     if (menu == 2&&Pflag == 5){
       if(val == 1)SAiring = SAiring+5;
       if(val == 2)AMode = 1;
       if(SAiring >= 101&&menu == 2)SAiring=100;
     }
     if (menu == 2&&Pflag == 6&&val == 1){
       SBuzzer = 1;
     }
     if (menu == 2&&Pflag == 7&&val == 1){
       SServo = 1;
     }
   DravMenu(menu,value,mode,Edirection);
   }
 if (enc1.isRelease()) {
    dopp = 0;
    if (menu == 2){
      if (flag == 0 && value == 1 && menu == 2){
        if (Pflag == 0&&mode == 0){
          Pflag = 1;
          val == 0;
          dopp = 1;
          val_m = 0;
          lcd.setCursor(0, value);
          lcd.print(" ");
      }}
      if (flag == 0 && value == 2 && menu == 2){
        if (Pflag == 0){
          Pflag = 2;
          val == 0;
          dopp = 1;
          val_m = 1;
          lcd.setCursor(0, value);
          lcd.print(" ");
      }}
      if (flag == 0 && value == 3 && menu == 2){
        if (Pflag == 0){
          Pflag = 3;
          val == 0;
          dopp = 1;
          val_m = 1;
          lcd.setCursor(0, value);
          lcd.print(" ");
      }}
      if (flag == 1 && value == 4 && menu == 2){
        if (Pflag == 0){
          Pflag = 4;
          val == 0;
          dopp = 1;
          val_m = 1;
          lcd.setCursor(0, value);
          lcd.print(" ");
      }}
      if (flag == 1 && value == 5 && menu == 2){
        if (Pflag == 0){
          Pflag = 5;
          val == 0;
          dopp = 1;
          val_m = 1;
          lcd.setCursor(0, value);
          lcd.print(" ");
      }}
      if (flag == 1 && value == 6 && menu == 2){
        if (Pflag == 0){
          Pflag = 6;
          val == 0;
          dopp = 1;
          val_m = 0;
          lcd.setCursor(0, value);
          lcd.print(" ");
        }}
      if (flag == 1 && value == 7 && menu == 2){
        if (Pflag == 0){
          Pflag = 7;
          val == 0;
          dopp = 1;
          val_m = 0;
          lcd.setCursor(0, value);
          lcd.print(" ");
        }}
      if (Pflag >= 1&&val == 0&&dopp == 0){
          Pflag = 0;
      }
    }
    if (menu == 0&&mode == 0&&value==0 ){
      DataWrite(FileDataName,1);
      inputData = "";
      DataJSON(FileDataName);
      Time ();
      DravMenu(menu,value,mode,Edirection);
    }
    if (menu == 0&&value == 3){
      menu = 2;
      value = 1;
      lcd.clear();
      DravMenu(menu,value,mode,Edirection);
    }
    if (menu == 2&&value == 0){
      menu = 0;
      value = 0;
      SetinWrite(FileSattingsName);
      lcd.clear();
      DravMenu(menu,value,mode,Edirection);
    }
    if (menu == 1&&Edirection == 0){
      menu = 0;
      value = 0;  
      lcd.clear();
      DravMenu(menu,value,mode,Edirection);
    }
    if (menu == 1&&Edirection == 1){
      menu = 0;
      value = 0;
      DataWrite(FileDataName,0);
      inputData = "";
      DataJSON(FileDataName);
      lcd.clear();
      DravMenu(menu,value,mode,Edirection);
    }

  }
  //Serial.println(SHumd);
  //Serial.println(value);
  //DravMenu(menu,value,mode,Edirection);
  if (digitalRead(BStop)==0&&mode==1){
    menu = 1;
    Edirection = 0;
    lcd.clear();
    DravMenu(menu,value,mode,Edirection);
    Serial.println("stop");
  }
}
