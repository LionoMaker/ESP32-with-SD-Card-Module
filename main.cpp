//@LionoMaker
/*
Hardware connection
ESP32:          SD_CARD Module:
MOSI------------------------D23
MISO------------------------D19
SCK-------------------------D18
GND-------------------------GND
VCC-------------------------VCC
CS/SS-----------------------D5
*/

#include <Arduino.h>
// Libraries for SD card
#include "FS.h"
#include "SD.h"
#include <SPI.h>

// Libraries to get time from NTP Server
#include <WiFi.h>
#include "time.h"

#include <fstream>


// Replace with your network credentials
const char* ssid     = "EiG";
const char* password = "12344321";
// ifstream fileName;
// // fileName.eof();

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 18000;
const int   daylightOffset_sec = 0;
char timeStringBuff[50]; //50 chars should be enough
int i=0; //sr no.
int yr,mon,dy,hr,sc,mn;

//// Variables to hold sensor readings
String dataMessage,Strng;

// Variable to save current epoch time
unsigned long epochTime; 

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}
void printLocalTime()
{
  time_t rawtime;
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
   return;
  }
  // //strftime(timeStringBuff, sizeof(timeStringBuff), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  // strftime(timeStringBuff, sizeof(timeStringBuff), " %Y %B %d %H:%M:%S", &timeinfo);
  // //print like "const char*"
  // Serial.println(timeStringBuff);
   yr=timeinfo.tm_year+1900; // year=1900+122=2022
   mon=timeinfo.tm_mon+1; //month+1 
   dy=timeinfo.tm_mday;
   hr=timeinfo.tm_hour;
   mn=timeinfo.tm_min;
   sc=timeinfo.tm_sec;
   Strng=String(yr)+ "" +String(mon)+ "" +String(dy)+ "  " +String(hr)+ ":" +String(mn)+ ":" +String(sc)+"\r\n";
}

// void HelloDate_Time(){
//   //delay(1000);
//   printLocalTime(); //call and print local date and time
// }

//Serial no.
void Srno(){
// for(int i=1;true;i++)
// {
//   delay(1000);
//   Serial.println(i);
//     //i++;
// }
// while (true)
// {
//   i++;
//   delay(1000);
//   Serial.println(i);
// }

  i++;
  //delay(1000);
  Serial.println(i);

}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

// Initialize SD card
void initSDCard(){
   if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

// Write to the SD card
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

// void setTime(int yr, int month, int mday, int hr, int minute, int sec, int isDst){
//   struct tm tm;

//   tm.tm_year = yr - 1900;   // Set date
//   tm.tm_mon = month-1;
//   tm.tm_mday = mday;
//   tm.tm_hour = hr;      // Set time
//   tm.tm_min = minute;
//   tm.tm_sec = sec;
//   tm.tm_isdst = isDst;  // 1 or 0
//   time_t t = mktime(&tm);
//   Serial.printf("Setting time: %s", asctime(&tm));
//   struct timeval now = { .tv_sec = t };
//   settimeofday(&now, NULL);
// }

void setup() {
  Serial.begin(115200);
  
  initWiFi();
  initSDCard();
  // configTime(18000, 0, ntpServer);
    // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  // Create a file on the SD card and write the data labels
  File file = SD.open("/let.txt");
  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/let.txt", "Sr, Unix Time, YYYYMMDD HH:MM:SS\r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
}

void loop() {
//setTime(2022,8,24,5,28,50,1);

  if ((millis() - lastTime) > timerDelay) {
  //   //Get epoch time
    epochTime = getTime();
    //HelloDate_Time();
    printLocalTime();
    Srno();

    //Concatenate all info separated by commas
    dataMessage = String(i)+ "," +String(epochTime)+ "," +String(Strng)+"\r\n";
    Serial.print("Saving data: ");
    Serial.println(dataMessage);

    //Append the data to file
    appendFile(SD, "/let.txt", dataMessage.c_str());

    lastTime = millis();
  }
  
}
