#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <FirebaseESP32.h>
#include "FS.h"
#include "SPIFFS.h"

#define FIREBASE_HOST "automation-aa27f-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "vJ7ggGytnsU9JUHK5IdJH9G3SQlc0DNFczSn5HbQ"
#define URL_fw_Version "https://raw.githubusercontent.com/Darshnik253/Firmware_Update/master/version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/Darshnik253/Firmware_Update/master/Firmware_Update.bin"
#define ledPin 3
#define FORMAT_SPIFFS_IF_FAILED true
volatile byte touch = 0;
static const int snormal = 6;
static const int sdim = 0;
volatile byte fq = 0, l = 0, lf = 0, le = 0, fa = 0, por = 0, c = 0, lef = 0, pf = 0, sigd1 = 0, sigd2 = 0, sig1 = 0, sig2 = 0,fail = 0;
volatile int rep = 0, wup = 0, makeb = 0; // mqttconnect,74disable
int fi = 0, ff = 0;
void IRAM_ATTR handleInterrupt1();
void IRAM_ATTR handleInterrupt2();
void IRAM_ATTR handleInterrupt3();
void IRAM_ATTR handleInterrupt4();
void IRAM_ATTR handleInterrupt5();
void IRAM_ATTR handleInterrupt6();
void printResult(FirebaseData &data);
void printResult(StreamData &data);
void connect_wifi();
void firmwareUpdate();
int FirmwareVersionCheck();

static const int lightin = 26;//D2
static const int fanin = 25;//D1
static const int ledin = 15;//D6
static const int portin = 14;//d5
static const int signalin1 = 13;//rx
static const int signalin2 = 4;//tx

static const int lightout = 23;//D2
static const int fanout = 22;//D1
static const int ledout = 21;//D6
static const int portout = 19;//d5
static const int signalout1 = 18;//rx
static const int signalout2 = 5;//tx

const char * ssid = "Realme 7";
const char * password = "Darshnik";

FirebaseData firebaseData;

String path = "/device";
String path1 = "/Dhruval";

unsigned long sendDataPrevMillis = 0;
uint16_t count = 0;

#ifndef CERT_H

#define CERT_H

const char * rootCACertificate = \
  "-----BEGIN CERTIFICATE-----\n"
"MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
"ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL\n"
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
"LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug\n"
"RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm\n"
"+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW\n"
"PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM\n"
"xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB\n"
"Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3\n"
"hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg\n"
"EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF\n"
"MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA\n"
"FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec\n"
"nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z\n"
"eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF\n"
"hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2\n"
"Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe\n"
"vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep\n"
"+OkuE6N36B9K\n"
"-----END CERTIFICATE-----\n";

#endif

String FirmwareVer = {
  "2.2"
};

//#define URL_fw_Version "http://cade-make.000webhostapp.com/version.txt"
//#define URL_fw_Bin "http://cade-make.000webhostapp.com/firmware.bin"

unsigned long previousMillis = 0; // will store last time LED was updated
unsigned long previousMillis_2 = 0;
const long interval = 5000;
const long mini_interval = 1000;
void repeatedCall() 
{
  static int num=0;
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (FirmwareVersionCheck()) {
      firmwareUpdate();
    }
  }
  if ((currentMillis - previousMillis_2) >= mini_interval) {
    previousMillis_2 = currentMillis;
    Serial.print("idle loop...");
    Serial.print(num++);
    Serial.print(" Active fw version:");
    Serial.println(FirmwareVer);
   if(WiFi.status() == WL_CONNECTED) 
   {
       Serial.println("wifi connected");
   }
   else
   {
    connect_wifi();
   }
  }
}

void streamCallback(StreamData data)
{

  Serial.println("Stream Data1 available...");
  Serial.println("STREAM PATH: " + data.streamPath());
  Serial.println("EVENT PATH: " + data.dataPath());
  Serial.println("DATA TYPE: " + data.dataType());
  Serial.println("EVENT TYPE: " + data.eventType());
  String value = String(data.intData());
  String key =  String(data.dataPath());
  Serial.print("VALUE: "  + value);
  
  printResult(data);
  if (key == "/t") {
    for (int iq = 0; iq < value.toInt(); iq++)
    {
      //Also can use Firebase.push instead of Firebase.pushInt
      if (Firebase.setInt(firebaseData, path1 + "/n" + iq, iq))
      {
        Serial.println("PASSED");
        Serial.println("PATH: " + firebaseData.dataPath());
        //      Serial.print("PUSH NAME: ");
        //      Serial.println(firebaseData.pushName());
        //      Serial.println("ETag: " + firebaseData.ETag());
        Serial.println("------------------------------------");
        Serial.println();
      }
      else
      {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
    }
  } else if (key == "/x") {
    for (int ii = 0; ii < value.toInt(); ii++)
    {
      //Also can use Firebase.push instead of Firebase.pushInt
      if (Firebase.setInt(firebaseData, path1 + "/d" + ii, ii))
      {
        Serial.println("PASSED");
        Serial.println("PATH: " + firebaseData.dataPath());
        //      Serial.print("PUSH NAME: ");
        //      Serial.println(firebaseData.pushName());
        //      Serial.println("ETag: " + firebaseData.ETag());
        Serial.println("------------------------------------");
        Serial.println();
      }
      else
      {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
      }
    }
  }

  Serial.println();
}

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button_boot = {
  0,
  0,
  false
};
/*void IRAM_ATTR isr(void* arg) {
    Button* s = static_cast<Button*>(arg);
    s->numberKeyPresses += 1;
    s->pressed = true;
}*/

void IRAM_ATTR isr() {
  button_boot.numberKeyPresses += 1;
  button_boot.pressed = true;
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
  }
}
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
String readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);
  String ret = "";
  int ss[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int cou = 0;
  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return ret;
  }

  //Serial.println("- read from file:");
  while (file.available()) {
    char c = file.read();
    //    if (cou < 6) {
//     Serial.println(c);
    ss[cou] = c- '0';;
    Serial.println("s" + String(cou) + " - " + String(ss[cou]) );
    cou = cou + 1;
    
    //    }
    //    //Serial.write(c);
    ret += c;


  }
  //Serial.println("");
  file.close();
  digitalWrite(lightout, ss[0]);
  digitalWrite(fanout, ss[1]);
  digitalWrite(portout, ss[2]);
  digitalWrite(ledout, ss[3]);
  digitalWrite(signalout1, ss[4]);
  digitalWrite(signalout2, ss[5]);
  return ret;
}
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
    }
}

void IRAM_ATTR handleInterrupt1() {
//  rep = 0;
//  fail = 0;
  c = 1;
  //  if ((millis() - last_pressed1) > 300) {
  //    last_pressed1 = millis();
  lf = !(digitalRead(lightin));
  digitalWrite(lightout,lf);
   Serial.println(("switch "+String(c) + " out -> " +String(lf)));
   
//  if (touch == 0) {
//    if (lf != l) {
//      l = lf;
//      //        registerWrite(8, lf);
//      //digitalWrite(pin,lf);///////////////example
//    }
//  } else {
//    if (lf != l) {
//      l = lf;
//      //        registerWrite(8, !(bitRead (data, 7)));
//      //digitalWrite(pin,!(digitalRead(pin)));///////////////example
//    }
//  }
  //  }

}
void IRAM_ATTR handleInterrupt2() {
  c = 2;
  //  if ((millis() - last_pressed1) > 300) {
  //    last_pressed1 = millis();
  if (sdim > 0) {
      fq = !(digitalRead(fanin));
      Serial.println(("switch "+String(c) + " out -> " +String(lf)));
      fa = fq;
      if (fa == LOW) {

      } else {
        //          registerWrite(1, 1);
        if (ff == 2) {

        } else if (ff == 3) {

        } else if (ff == 4) {

        } else if (ff == 5) {

        }
      }
  } else {
    fq = !(digitalRead(fanin));
    digitalWrite(fanout,fq);
    Serial.println(("switch "+String(c) + " out -> " +String(lf)));
    }
}

void IRAM_ATTR handleInterrupt3() {
  c = 3;
  //  if ((millis() - last_pressed1) > 300) {
  //    last_pressed1 = millis();
  lef =  !(digitalRead(ledin));
  digitalWrite(ledout,lef);
  Serial.println(("switch "+String(c) + " out -> " +String(lf)));
//  if (touch == 0) {
//    if (lef != le) {
//      le = lef;
//      //        registerWrite(7, lef);
//    }
//  } else {
//    if (lef != le) {
//      le = lef;
//      //        registerWrite(7, !(bitRead (data, 6)));
//    }
//  }
  //  }
}
void IRAM_ATTR handleInterrupt4() {
  c = 4;

  //  if ((millis() - last_pressed1) > 300) {
  //    last_pressed1 = millis();
  pf =  !(digitalRead(portin));
  digitalWrite(portout,pf);
  Serial.println(("switch "+String(c) + " out -> " +String(lf)));
  
//  if (touch == 0) {
//    if (pf != por) {
//      por = pf;
////      registerWrite(6, pf);
//    }
//  } else {
//    if (pf != por) {
//      por = pf;
////      registerWrite(6, !(bitRead (data, 5)));
//    }
//  }
  //  }
}
void IRAM_ATTR handleInterrupt5() {
  c = 5;
  //  if ((millis() - last_pressed1) > 300) {
  //    last_pressed1 = millis();
  sigd1 =  !(digitalRead(signalin1));
  digitalWrite(signalout1,sigd1);
  Serial.println(("switch "+String(c) + " out -> " +String(lf)));
//  if (touch == 0) {
//    if (sigd1 != sig1) {
//      sig1 = sigd1;
////      registerWrite(4, sig1);
//    }
//  } else {
//    if (sigd1 != sig1) {
//      sig1 = sigd1;
////      registerWrite(4, !(bitRead (data, 3)));
//    }
//  }
  //  }
}
void IRAM_ATTR handleInterrupt6() {
  c=6;
  sigd2 =  !(digitalRead(signalin2));
  digitalWrite(signalout2,sigd2);
  Serial.println(("switch "+String(c) + " out -> " +String(lf)));
//  if (touch == 0) {
//    if (sigd2 != sig2) {
//      sig2 = sigd2;
////      registerWrite(3, sig2);
//    }
//  } else {
//    if (sigd2 != sig2) {
//      sig2 = sigd2;
////      registerWrite(3, !(bitRead (data, 2)));
//    }
//  }
  //   }
}
void setup() 
{
  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(firebaseData, path))
  {
    Serial.println("------------------------------------");
    Serial.println("Can't begin stream connection...");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
  pinMode(lightin, INPUT_PULLUP);//_PULLUP
  pinMode(fanin, INPUT_PULLUP);
  pinMode(ledin, INPUT_PULLUP);
  pinMode(portin, INPUT_PULLUP);

  pinMode(lightout, OUTPUT);
  pinMode(fanout, OUTPUT);
  pinMode(portout, OUTPUT);
  pinMode(ledout, OUTPUT);
  pinMode(signalout1, OUTPUT);
  pinMode(signalout2, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(lightin), handleInterrupt1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(fanin), handleInterrupt2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ledin), handleInterrupt3, CHANGE);
  attachInterrupt(digitalPinToInterrupt(portin), handleInterrupt4, CHANGE);
  
  if (snormal > 4) {
    attachInterrupt(digitalPinToInterrupt(signalin1), handleInterrupt5, CHANGE);
  }  if (snormal > 5) {
    attachInterrupt(digitalPinToInterrupt(signalin2), handleInterrupt6, CHANGE);
  }
  interrupts();  
    
   listDir(SPIFFS, "/", 0);
   readFile(SPIFFS, "/hello.txt");
   if(c>0)
   {
     String sstate = String(digitalRead(ledout)) + String(digitalRead(fanout)) + String(digitalRead(portout)) + String(digitalRead(signalout1)) + String(digitalRead(signalout2)) + String(digitalRead(lightout));
     int len=sstate.length();
     const char* state1 = sstate.c_str();
     writeFile(SPIFFS, "/hello.txt", state1);
   }
  Serial.println( "Test complete" );
  pinMode(button_boot.PIN, INPUT);
  attachInterrupt(button_boot.PIN, isr, RISING);
  Serial.begin(115200);
  Serial.print("Active firmware version:");
  Serial.println(FirmwareVer);
  pinMode(ledPin, OUTPUT);
  connect_wifi();
  
  Firebase.setStreamCallback(firebaseData, streamCallback, streamTimeoutCallback);
  
}
void loop() {
  if (button_boot.pressed) { //to connect wifi via Android esp touch app 
    Serial.println("Firmware update Starting..");
    firmwareUpdate();
    button_boot.pressed = false;
  }
  repeatedCall();
}

void connect_wifi() {
  Serial.println("Waiting for WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void firmwareUpdate(void) {
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  httpUpdate.setLedPin(ledPin, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

  switch (ret) {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    break;

  case HTTP_UPDATE_NO_UPDATES:
    Serial.println("HTTP_UPDATE_NO_UPDATES");
    break;

  case HTTP_UPDATE_OK:
    Serial.println("HTTP_UPDATE_OK");
    break;
  }
}

int FirmwareVersionCheck(void) {
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  Serial.println(fwurl);
  WiFiClientSecure * client = new WiFiClientSecure;

  if (client) 
  {
    client -> setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
    HTTPClient https;

    if (https.begin( * client, fwurl)) 
    { // HTTPS      
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) // if version received
      {
        payload = https.getString(); // save received version
      } else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }
      
  if (httpCode == HTTP_CODE_OK) // if version received
  {
    payload.trim();
    if (payload.equals(FirmwareVer)) {
      Serial.printf("\nDevice already on latest firmware version:%s\n", FirmwareVer);
      return 0;
    } 
    else 
    {
      Serial.println(payload);
      Serial.println("New firmware detected");
      return 1;
    }
  } 
  return 0;  
}
void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
}

void printResult(StreamData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string" || data.dataType() == "null")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson *json = data.jsonObjectPtr();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json->toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json->iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json->iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);

      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json->iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray *arr = data.jsonArrayPtr();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr->toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();

    for (size_t i = 0; i < arr->size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData *jsonData = data.jsonDataPtr();
      //Get the result data from FirebaseJsonArray object
      arr->get(*jsonData, i);
      if (jsonData->typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData->boolValue ? "true" : "false");
      else if (jsonData->typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData->intValue);
      else if (jsonData->typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData->floatValue);
      else if (jsonData->typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData->doubleValue);
      else if (jsonData->typeNum == FirebaseJson::JSON_STRING ||
               jsonData->typeNum == FirebaseJson::JSON_NULL ||
               jsonData->typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData->typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData->stringValue);
    }
  }
}
