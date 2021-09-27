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
  //Serial.print(input);
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
