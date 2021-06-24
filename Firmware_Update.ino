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
volatile byte fq = 0, l = 0, lf = 0, le = 0, fa = 0, por = 0, c = 0, lef = 0, pf = 0, sigd1 = 0, sigd2 = 0, sig1 = 0, sig2 = 0, fail = 0,val1 = 0,val2 = 0,val3 = 0,val4 = 0,val5 = 0,val6 = 0;
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

const char * ssid = "Home Jio";
const char * password = "123456789";

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
const long interval = 3000;
const long mini_interval = 1000;
void repeatedCall()
{
  static int num = 0;
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
    if (WiFi.status() == WL_CONNECTED)
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
void streamTimeoutCallback(bool timeout)
{
  if (timeout)
  {
    Serial.println();
    Serial.println("Stream timeout, resume streaming...");
    Serial.println();
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
    ss[cou] = c - '0';;
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
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
}

void IRAM_ATTR handleInterrupt1() {
  c = 1;
  //  if ((millis() - last_pressed1) > 300) {
  //    last_pressed1 = millis();
  if (touch == 0) {
    lf = digitalRead(lightin);
    if (lf != l) 
    {
      l = lf;
      digitalWrite(lightout, lf);
      Firebase.setInt(firebaseData, "/light", l);
      Serial.println(("switch " + String(c) + " out -> " + String(l)));
    }
  } 
  else {
    digitalWrite(lightout, !(digitalRead(lightout)));
    Firebase.setInt(firebaseData, "/light", (!(digitalRead(lightout))));
  }
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
//    if (touch == 0) {
//      fq = digitalRead(fanin);
//    } else {
//      fq = !(bitRead (data, 0));
//    }
//    if (fq != fa || touch == 1) {
//      fa = fq;
//      if (fa == LOW) {
//        registerWrite(1, 0);
//        registerWrite(3, 0);
//        registerWrite(4, 0);
//        registerWrite(5, 0);
//      } else {
//        registerWrite(1, 1);
//        if (ff == 2) {
//          registerWrite(3, 1);
//          registerWrite(4, 0);
//          registerWrite(5, 0);
//        } else if (ff == 3) {
//          registerWrite(3, 0);
//          registerWrite(4, 1);
//          registerWrite(5, 0);
//        } else if (ff == 4) {
//          registerWrite(3, 1);
//          registerWrite(4, 1);
//          registerWrite(5, 0);
//        } else if (ff == 5) {
//          registerWrite(3, 0);
//          registerWrite(4, 0);
//          registerWrite(5, 1);
//        }
//      }
//    }
  } 
  else {
    if (touch == 0) 
    {
      fq = digitalRead(fanin);
      if (fq != fa) {
        fa = fq;
        digitalWrite(fanout, fq);
        Firebase.setInt(firebaseData, "/fan", fa);
        Serial.println(("switch " + String(c) + " out -> " + String(fa)));
      }
    else {
      digitalWrite(fanout, !(digitalRead(fanout)));
       Firebase.setInt(firebaseData, "/fan", (!(digitalRead(fanout))));
    }
  }
}
}
void IRAM_ATTR handleInterrupt3() {
  c = 3;
  //  if ((millis() - last_pressed1) > 300) {
  //    last_pressed1 = millis();
  if (touch == 0) {
    lef =  digitalRead(ledin);
    if (lef != le ) 
    {
      le = lef;
      digitalWrite(ledout, lef);
      Firebase.setInt(firebaseData, "/led", le);
      Serial.println(("switch " + String(c) + " out -> " + String(le)));
    }
    } 
  else {
    digitalWrite(ledout, !(digitalRead(ledout)));
    Firebase.setInt(firebaseData, "/led", (!(digitalRead(ledout))));
  }
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
  if (touch == 0) 
  {
    pf =  digitalRead(portin);
    if (pf != por) 
    {
      por = pf;
      digitalWrite(portout, pf);
      Firebase.setInt(firebaseData, "/por", por);
      Serial.println(("switch " + String(c) + " out -> " + String(por)));
    }
  } 
  else {
    digitalWrite(portout, !(digitalRead(portout)));
    Firebase.setInt(firebaseData, "/port", (!(digitalRead(portout))));
  }
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
  if (touch == 0) 
  {
    sigd1 =  digitalRead(signalin1);
    if (sigd1 != sig1) 
    {
      sig1 = sigd1;
      digitalWrite(signalout1, sig1);
      Firebase.setInt(firebaseData, "/signal1", sigd1);
      Serial.println(("switch " + String(c) + " out -> " + String(sigd1)));
    }
  } 
  else {
   digitalWrite(signalout1, !(digitalRead(signalout1)));
    Firebase.setInt(firebaseData, "/signal1", (!(digitalRead(signalout1))));
  }
  
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
  c = 6;
  if (touch == 0) {
    sigd2 =  digitalRead(signalin2);
    if (sigd2 != sig2) {
      sig2 = sigd2;
      digitalWrite(signalout2, sig2);
      Firebase.setInt(firebaseData, "/signal2", sig2);
      Serial.println(("switch " + String(c) + " out -> " + String(sigd2)));}
    }
    else {
    digitalWrite(signalout2, !(digitalRead(signalout2)));
    Firebase.setInt(firebaseData, "/signal2", (!(digitalRead(signalout2))));
  }
  
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
  Serial.begin(115200);
  Serial.print("Active firmware version:");
  Serial.println(FirmwareVer);
  pinMode(ledPin, OUTPUT);
  connect_wifi();

  pinMode(lightin, INPUT_PULLUP);//_PULLUP
  pinMode(fanin, INPUT_PULLUP);
  pinMode(ledin, INPUT_PULLUP);
  pinMode(portin, INPUT_PULLUP);
  pinMode(signalin1, INPUT_PULLUP);
  pinMode(signalin2, INPUT_PULLUP);

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
  
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
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
  readFile(SPIFFS, "/hello.txt");

}
void loop() {

  repeatedCall();
  delay(5000);
  if (c > 0)
  {
    String sstate = String(digitalRead(ledout)) + String(digitalRead(fanout)) + String(digitalRead(portout)) + String(digitalRead(signalout1)) + String(digitalRead(signalout2)) + String(digitalRead(lightout));
    int len = sstate.length();
    const char* state1 = sstate.c_str();
    writeFile(SPIFFS, "/hello.txt", state1);
    Serial.println( "State updated!" );
  }
  if (Firebase.getInt(firebaseData, "/light")) {
      Serial.println((firebaseData.intData()));
      if ( (firebaseData.intData()) != l ) {
      digitalWrite(lightout, (firebaseData.intData()));
      Serial.println(("switch " + String(c) + " out -> " + String((firebaseData.intData()))));}}
  
  if (Firebase.getInt(firebaseData, "/fan")) {
      Serial.println((firebaseData.intData()));
      if ( (firebaseData.intData()) != fq ) {
      digitalWrite(fanout, (firebaseData.intData()));
      Serial.println(("switch " + String(c) + " out -> " + String((firebaseData.intData()))));}}

   if (Firebase.getInt(firebaseData, "/led")) {
      Serial.println((firebaseData.intData()));
      if ( (firebaseData.intData()) != le ) {
      digitalWrite(ledout, (firebaseData.intData()));
      Serial.println(("switch " + String(c) + " out -> " + String((firebaseData.intData()))));}}

   if (Firebase.getInt(firebaseData, "/port")) {
      Serial.println((firebaseData.intData()));
      if ( (firebaseData.intData()) != por ) {
      digitalWrite(portout, (firebaseData.intData()));
      Serial.println(("switch " + String(c) + " out -> " + String((firebaseData.intData()))));}}
      
   if (Firebase.getInt(firebaseData, "/signal1")) {
      Serial.println((firebaseData.intData()));
      if ( (firebaseData.intData()) != sig1 ) {
      digitalWrite(signalout1, (firebaseData.intData()));
      Serial.println(("switch " + String(c) + " out -> " + String((firebaseData.intData()))));}}

   if (Firebase.getInt(firebaseData, "/signal2")) {
      Serial.println((firebaseData.intData()));
      if ( (firebaseData.intData()) != sig2 ) {
      digitalWrite(signalout2, (firebaseData.intData()));
      Serial.println(("switch " + String(c) + " out -> " + String((firebaseData.intData()))));}}
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
