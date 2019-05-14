#include "web.h"
#include <functional>


#define WEB_DBG
#ifdef WEB_DBG
  #define WEB_DbgMsg DbgMsg("Web: "); DbgMsg
#else
  #define WEB_DbgMsg
#endif

static String Str;


void class_web::handleRoot(void)
{
  if(m_WebServ.args())
  {
    parseArgs();
  }
  else
  {
    sendIndexHtml();
  }
}

//this will append the menu in jquery
void class_web::appendMenu(void)
{
  // home  ++  status/info  ++  configuration  ++  system tools



}

void class_web::sendIndexHtml(void)
{
  char str_hostname[20] = {0};
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  (**mPtr_param).getId(MEMID_HOSTNAME, (uint8_t*)str_hostname);
  String hostname = String(str_hostname);

//<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>


  Str =    
"<!DOCTYPE html>\
<html>\
<head>\
<link rel=\"stylesheet\" href=\"https://code.jquery.com/ui/1.12.1/themes/base/jquery-ui.css\">\
<script src=\"https://code.jquery.com/jquery-1.12.4.js\"></script>\
<script src=\"https://code.jquery.com/ui/1.12.1/jquery-ui.js\"></script>\
<title>"+ hostname +"</title>\
</head>\
<body>\
<h1>"+ hostname +"</h1>\
<div>Uptime: "+hr+":"+(min%60)+":"+(sec%60)+"</div>\
<div>NTPtime: (w"+(**mPtr_dt).GetTime().tm_wday+") "+(**mPtr_dt).GetTime().tm_hour+":"+(**mPtr_dt).GetTime().tm_min+":"+(**mPtr_dt).GetTime().tm_sec+" </div>\
<div>SW ver: "+(SW_VERSION/100)+"."+(SW_VERSION%100)+"</div>\
<div>Build time: "+String(BUILD_DATE)+" - "+String(BUILD_TIME)+"</div>\
<br>\
<div id=\"motpos\">Mot pos: "+p_blinds->GetPos()+"</div>\
<div id=\"usrpos\">Usr pos: NONE</div>\
<br><br>\
<button type=\"button\" onclick=\"cmd('open')\" style=\"font-size:50px; height:100px; width:330px\">open</button>\
<br><br>\
<button type=\"button\" onclick=\"cmd('close')\" style=\"font-size:50px; height:100px; width:330px\">close</button>\
<br><br>\
<button type=\"button\" onclick=\"cmd('stop')\" style=\"font-size:50px; height:100px; width:330px\">stop</button>\
<br><br>\
<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>\
<script>\
function cmd_open()\
{\
var xhttp = new XMLHttpRequest();\
xhttp.open(\"GET\", \"?getdata=uptime\", true);\
xhttp.onreadystatechange = function(){\
if (this.readyState == 4 && this.status == 200) {\
alert(this.responseText);\
}};\
xhttp.send();\
}\
function cmd(c)\
{\
var xhttp = new XMLHttpRequest();\
xhttp.open(\"GET\", \"?cmd=\"+c, true);\
xhttp.send();\
}\
</script>\
</body>\
</html>";

  m_WebServ.send(200, "text/html", Str);
}

void class_web::handleNotFound(void)
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += m_WebServ.uri();
  message += "\nMethod: ";
  message += (m_WebServ.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += m_WebServ.args();
  message += "\n";
  for (uint8_t i=0; i<m_WebServ.args(); i++){
    message += " " + m_WebServ.argName(i) + ": " + m_WebServ.arg(i) + "\n";
  }
  m_WebServ.send(404, "text/plain", message);
}


void class_web::update1(void)
{
  m_WebServ.sendHeader("Connection", "close");
  m_WebServ.sendHeader("Access-Control-Allow-Origin", "*");
  m_WebServ.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
  ESP.restart();
}

void class_web::update2(void)
{
  HTTPUpload& upload = m_WebServ.upload();
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

class_web::class_web(class_datetime*& ptr_dt, class_param*& ref_param)
{
  mPtr_dt = &ptr_dt;
  mPtr_param = &ref_param;
}


void class_web::TaskRun(void)
{
  m_WebServ.handleClient();
}


void class_web::TaskInit(void)
{ 
  m_WebServ.on("/", std::bind(&class_web::handleRoot, this) );

  m_WebServ.on("/script.js", std::bind(&class_web::sendJsFile, this) );
  m_WebServ.on("/style.js", std::bind(&class_web::sendCssFile, this) );

  m_WebServ.on("/update", HTTP_POST, std::bind(&class_web::update1, this), std::bind(&class_web::update2, this));
  m_WebServ.onNotFound( std::bind(&class_web::handleNotFound, this) );

  m_WebServ.begin();
  WEB_DbgMsg("Initialized\n");
}



void class_web::parseArgs(void)
{
  String replyStr = "";
  char str_hostname[20] = {0};

  (**mPtr_param).getId(MEMID_HOSTNAME, (uint8_t*)str_hostname);
  String hostname = String(str_hostname);

  if(m_WebServ.args())
  {
    WEB_DbgMsg("Query args: %u\n", m_WebServ.args());
    for(int i =0; i<m_WebServ.args(); i++)
    {
      // format: arg (number) arg_string : value
      WEB_DbgMsg("arg(%u) %s : %s\n", i, m_WebServ.argName(i).c_str(), m_WebServ.arg(i).c_str() );
    }


    // was a command received ?
    if( m_WebServ.hasArg("cmd") )
    {
      if(m_WebServ.arg("cmd") == "open")
      {
        p_blinds->Open();
        WEB_DbgMsg("cmd_open\n");
      }
      else if(m_WebServ.arg("cmd") == "close")
      {
        p_blinds->Close();
        WEB_DbgMsg("cmd_close\n");
      }
      else if(m_WebServ.arg("cmd") == "stop")
      {
        p_blinds->Stop();
        WEB_DbgMsg("cmd_stop\n");
      }
    }

    // was a request for data received ?
    if( m_WebServ.hasArg("getdata") )
    {
      if(m_WebServ.arg("getdata") == "pos")
      {
        replyStr = String(p_blinds->GetPos());
        WEB_DbgMsg("get_pos\n");
      }

      else if(m_WebServ.arg("getdata") == "uptime")
      {
        int sec = millis() / 1000;
        int min = sec / 60;
        int hr = min / 60;

        replyStr = String(hr) + ":" + String(min%60) + ":" + String(sec%60);
        WEB_DbgMsg("get_pos\n");
      }
    }
  }

  replyStr = makeJSONMsg({
    {"pos",       String(p_blinds->GetPos())},
    {"uptime",    String(millis())},
    {"buildtime", String(BUILD_DATE)+" - "+String(BUILD_TIME)},
    {"ntptime",   "(w" +String((**mPtr_dt).GetTime().tm_wday)+") "+(**mPtr_dt).GetTime().tm_hour+":"+(**mPtr_dt).GetTime().tm_min+":"+(**mPtr_dt).GetTime().tm_sec},
    {"swver",     String(SW_VERSION/100)+"."+String(SW_VERSION%100)},
    {"name",      hostname}
  });


  // send reply back
  m_WebServ.send(200, "text/plain", replyStr);

  //text/html             .html
  //text/plain            .txt
  //text/css              .css format
  //text/javascript       .js files
  //application/json      .json format
}


String class_web::makeJSONMsg(vector< pair<String, String> > v_str)
{
  String r = "{";

/*{
  "pos": "22",
  "uptime": "19:53:21",
  "buildtime": "Apr 15 2019 - 16:52:35",
  "ntptime": "0:1:2"
}*/

  for (auto it = v_str.begin(); it != v_str.end(); ++it)
  {
    r += "\"" + it->first + "\": \"" + it->second + "\"";

    if(it != v_str.end()-1)
      r += ", ";
  }
  r += "}";

  return r;
}





void class_web::sendJsFile(void)
{
  String replyStr = "the javascript file";
  m_WebServ.send(200, "text/javascript", replyStr);
}

void class_web::sendCssFile(void)
{
  String replyStr = "the css file";
  m_WebServ.send(200, "text/css", replyStr);
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
