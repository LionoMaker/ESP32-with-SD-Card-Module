# ESP32-with-SD-Card-Module
In this project ESP32 interfaces with SD Card module, getting NTPC real time and storing in SD card via creating new file .txt. 
The way to store data in .txt file is:
Sr No.  Unix Time  YYYYMMDD HH:MM:SS
Created a function named as String_lastRead(), which is called to store the last write string in .txt.
function read the file to jump on last string, saves into the second variable and to restart from there.
