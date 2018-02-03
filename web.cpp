#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "web.h"
#include "main.h"
#include "motor.h"
#include "datetime.h"
#include "debug.h"
#include "schedule.h"

#define WEB_DBG
#ifdef WEB_DBG
  #define WEB_DbgMsg DbgMsg
#else
  #define WEB_DbgMsg
#endif

const char* ssid = "Deathstar";
const char* password = "hailtothekingbaby";

ESP8266WebServer server(80);

//prototypes
void handleRoot();
void handleNotFound();
void handleCmdOpen();
void handleCmdClose();
void handleCmdResetPos();
void handleGetPos();
void handleSettings();


void update1();
void update2();

void InitWeb()
{
  WiFi.begin(ssid, password);

  /*if(MDNS.begin("akr_ww"))
    Serial.println("MDNS responder started");*/

  server.on("/", handleRoot);
  server.on("/update", HTTP_POST, update1, update2);
  server.on("/cmd_open", handleCmdOpen);
  server.on("/cmd_close", handleCmdClose);
  server.on("/cmd_reset_pos", handleCmdResetPos);
  server.on("/get_pos", handleGetPos);
  server.on("/settings", handleSettings);
  server.onNotFound(handleNotFound);

  server.begin();
  WEB_DbgMsg("HTTP server started");

  //wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    WEB_DbgMsg(".");
  }
  WEB_DbgMsg("connected");
}

void WebTask(void)
{
  server.handleClient();
}

void handleSettings()
{

  String Str =
"<!DOCTYPE html>\
<head>\
</head>\
<body scroll=no>\
<h1>Settings</h1>\
<br><br><br>\
<form action=\"settings\" method=\"POST\">\
<input type=\"checkbox\" name=\"ena\" value=\"1\">\
<input type=\"text\" name=\"rule_no\"> \
<input type=\"text\" name=\"wd\"> \
<input type=\"text\" name=\"hour\"> \
<input type=\"text\" name=\"min\"> \
<input type=\"text\" name=\"cmd\">\
</select>\
<select name=\"cmd\">\
<option value=\"1\">close</option>\
<option value=\"2\">open</option></select><br>\
<input type=\"submit\" value=\"save to RAM\">\
</form>\
<form action=\"settings\" method=\"POST\">\
<input type=\"submit\" value=\"commit to ROM\">\
</form>\
<br><br><br>\
<a href=\"/\">main</a>\
</body>\
</html>";

  server.send(200, "text/html", Str);


  for(int i=0; i<server.args(); i++)
  {
    Serial.println(server.argName(i));
    WEB_DbgMsg(" = ");
    Serial.println(server.arg(i));
    WEB_DbgMsg("\r\n");    
  }

}



void handleRoot()
{
  char str[1200];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int days = hr / 24;

  snprintf (str, 1200,
  
"<!DOCTYPE html>\
<head>\
<title>WiFiBlinds</title>\
</head>\
<body scroll=no>\
<h1>WifiBlinds</h1>\
<div>Uptime: %02d:%02d:%02d:%02d</div>\
<div>NTP time: %02u:%02u:%02u </div>\
<div>NTP date: %s d. %2u. %s - %2u </div>\
<div>SW ver: %01d.%02d</div>\
<br>\
<div id=\"motpos\">Mot pos: %d</div>\
<div id=\"usrpos\">Usr pos: %d</div>\
<br><br>\
<button type=\"button\" onclick=\"cmd_open()\" style=\"font-size:50px; height:100px; width:330px\">open</button>\
<br><br>\
<button type=\"button\" onclick=\"cmd_close()\" style=\"font-size:50px; height:100px; width:330px\">close</button>\
<br><br><br><br>\
<button type=\"button\" onclick=\"cmd_reset_pos()\">Reset Pos</button>\
<br><br><br><br>\
<a href=\"settings\">Settings</a>\
<br><br><br><br>\
<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>\
<script>\
function cmd_open()\
{\
var xhttp = new XMLHttpRequest();\
xhttp.open(\"GET\", \"cmd_open\", true);\
xhttp.send();\
}\
function cmd_close()\
{\
var xhttp = new XMLHttpRequest();\
xhttp.open(\"GET\", \"cmd_close\", true);\
xhttp.send();\
}\
function cmd_reset_pos()\
{\
var xhttp = new XMLHttpRequest();\
xhttp.open(\"GET\", \"cmd_reset_pos\", true);\
xhttp.send();\
}\
</script>\
</body>\
</html>",

days, hr % 24, min % 60, sec % 60,
GetTime().hour, GetTime().min, GetTime().sec,
StrWeekDay[GetTime().wday], GetTime().day+1, StrMonth[GetTime().month], GetTime().year,
SW_VERSION/100,SW_VERSION%100, MotPosTck, UserPosTck);

  server.send(200, "text/html", str);
}


void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleCmdOpen()
{
  CmdOpen();
  server.send(200, "text/plain", "");
  WEB_DbgMsg("http: cmd_open");
}

void handleCmdClose()
{
  CmdClose();
  server.send(200, "text/plain", "");
  WEB_DbgMsg("http: cmd_close");
}

void handleCmdResetPos()
{
  CmdResetPos();
  server.send(200, "text/plain", "");
  WEB_DbgMsg("http: cmd_reset_pos");
}

void handleGetPos()
{
  server.send(200, "text/plain", String(MotPosTck));
  WEB_DbgMsg("http: get_pos");
}


void update1()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
  ESP.restart();
}
    
void update2()
{
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START)
  {
    Serial.setDebugOutput(true);
    WiFiUDP::stopAll();
    WEB_DbgMsg("Update: %s\n", upload.filename.c_str());
    
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if(!Update.begin(maxSketchSpace))
    {//start with max available size
      Update.printError(Serial);
    }
  }
  else if(upload.status == UPLOAD_FILE_WRITE)
  {
    if(Update.write(upload.buf, upload.currentSize) != upload.currentSize)
    {
      Update.printError(Serial);
    }
  }
  else if(upload.status == UPLOAD_FILE_END)
  {
    if(Update.end(true))
    {//true to set the size to the current progress
      WEB_DbgMsg("Update Success: %u\nRebooting...\n", upload.totalSize);
    }
    else
    {
      Update.printError(Serial);
    }
    
    Serial.setDebugOutput(false);
  }
  yield();
}











/*
void handleInline()
{
  if(server.hasArg("USERNAME") && server.hasArg("PASSWORD"))
  {
    if(server.arg("USERNAME")=="admin" &&  server.arg("PASSWORD")=="admin" )
    {
    }
  }

  server.send(200, "text/plain", "this works as well");
}*/

