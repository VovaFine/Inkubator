void jsonFile(const char *filename,const char *fileinput) {  // Функція запису json в input
  input = "";
  File file = SD.open(filename);    // Відкрити файл
  if (!file) {     //При помилці читання файлу 
    Serial.println(F("Помилка читання файлу"));  // Вивести в серіал "Помилка читання файлу"
    return;
  }

  while (file.available()) {  // Якщо файл відкритий
    String inputdop = String((char)file.read());   // Читати по кусочку
    input = input + inputdop;   // Склеїти прочитані кусочки і записати в input
  }
  if(fileinput=="Data"){
    inputData = input;
  }
  if(fileinput=="Sett"){
    inputSett = input;
  }
  if(fileinput=="Json"){
    inputJson = input;
  }
  Serial.print(input);
  Serial.println();
  file.close();   // Закрити файл
}

void DataWrite(const char *FileDataName,boolean i) {  // Функція запису json в файл Data
  SD.remove("/Data.txt");
  File file = SD.open(FileDataName, FILE_WRITE);    // Відкрити файл
  if (!file) {     //При помилці читання файлу 
    Serial.println(F("Помилка читання файлу WRITE"));  // Вивести в серіал "Помилка читання файлу"
    return;
  }
  if(i == 1){
    file.println("{\"mode\":" + String(i) + ",\"Month\":" + String(rtc.getMonth()) + ",\"Day\":" + String(rtc.getDate()) + ",\"Hours\":" + String(rtc.getHours()) + ",\"Min\":" + String(rtc.getMinutes()) + "}");
    file.close();   // Закрити файл
  }else{
    file.println("{\"mode\":" + String(i) + ",\"Month\":0,\"Day\":0,\"Hours\":0,\"Min\":0}");
    file.close();   // Закрити файл
  }
    
}

void DataJSON(const char *FileDataName) {    // Функція читання jsonData
  DynamicJsonBuffer jsonBuffer;
  jsonFile(FileDataName,"Data");// Перейти до функції запису json в inputData
  JsonObject& rootData = jsonBuffer.parseObject(inputData);
  delay(50);
  mode = rootData[String("mode")];
  Month = rootData[String("Month")];
  Day = rootData[String("Day")];
  Hours = rootData[String("Hours")];
  Min = rootData[String("Min")];
}

void SetinWrite(const char *FileSattingsName) {  // Функція запису json в файл Data
  SD.remove("/Settings.txt");
  File file = SD.open(FileSattingsName, FILE_WRITE);    // Відкрити файл
  if (!file) {     //При помилці читання файлу 
    Serial.println(F("Помилка читання файлу WRITE"));  // Вивести в серіал "Помилка читання файлу"
    return;
  }
    file.println("{\"egg\":" + String(egg) + ",\"STemp\":" + String(STemp) + ",\"TMode\":" + String(TMode) + ",\"SHumd\":" + String(SHumd) + ",\"HMode\":" + String(HMode) + ",\"SFan\":" + String(SFan) + ",\"FMode\":" + String(FMode) + ",\"SAiring\":" + String(SAiring) + ",\"AMode\":" + String(AMode) + ",\"SBuzzer\":" + String(SBuzzer) + ",\"SServo\":" + String(SServo) + "}");
    file.close();   // Закрити файл 
}

void SetinJSON(const char *FileSattingsName) {    // Функція читання jsonData
  DynamicJsonBuffer jsonBuffer;
  jsonFile(FileSattingsName,"Sett");// Перейти до функції запису json в inputData
  JsonObject& rootSett = jsonBuffer.parseObject(inputSett);
  delay(50);
  egg  = rootSett[String("egg")];
  STemp = rootSett[String("STemp")];
  SHumd = rootSett[String("SHumd")];
  SFan = rootSett[String("SFan")];
  SAiring = rootSett[String("SAiring")];
  TMode = rootSett[String("TMode")];
  HMode = rootSett[String("HMode")];
  FMode = rootSett[String("FMode")];
  AMode = rootSett[String("AMode")];
  SBuzzer = rootSett[String("SBuzzer")];
  SServo = rootSett[String("SServo")];
  //Serial.println(egg);
}
void JsonJSON(const char *FileSattingsName) {    // Функція читання jsonData
  DynamicJsonBuffer jsonBuffer;
  jsonFile(FileSattingsName,"Json");// Перейти до функції запису json в inputData
  JsonObject& rootJson = jsonBuffer.parseObject(inputJson);
  delay(50);
  String request =String(IDay) + "_" + String(IHours) + "_" + String(IMin);
  int pars1 = rootJson[request][0];

  switch (pars1) {
      case 0:
        JsonTemp = rootJson[request][1];
      break;

      case 1:
        if (rootJson[request][1] == "L"){
          JsonServo = 106;
        }else if (rootJson[request][1] == "H"){
          JsonServo = 89;
        }else if (rootJson[request][1] == "R"){
          JsonServo = 69;
        }
      break;

      case 2:
        JsonHumd = rootJson[request][1];
        //JsonHumd = int(Jsondata[1]);
      break;

      case 3:
        JsonFan = rootJson[request][1];
      break;

      case 4:
        JsonAir = rootJson[request][1];
        Serial.println(JsonAir);
      break;
      
    }
}
