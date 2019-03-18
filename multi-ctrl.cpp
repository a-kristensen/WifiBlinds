#include "multi-ctrl.h"
#include "main.h"
#include <ESP8266WiFi.h>


#define MULTI_DBG
#ifdef MULTI_DBG
  #define MULTI_DbgMsg(...) DbgMsg("Multi: "); DbgMsg(__VA_ARGS__)
#else
  #define MULTI_DbgMsg
#endif


typedef enum {
  HERE_I_AM = 10,
  MOTOR_CMD = 20
} MsgIdentifier_t;

typedef enum {
  STOP = 20,
  RUN_CLOSE = 30,
  RUN_OPEN = 40,
  RUN_TO_POS = 50
} Command_t;

typedef struct {
  uint8_t   Cmd;
  uint8_t   Param;
} CmdMsg_t;

typedef struct {
  uint8_t   group;
  uint8_t   pos;
} HereIamMsg_t;


typedef struct {
  uint16_t        SOT;
  uint8_t         MsgId;        // MsgIdentifier_t
  union {
    HereIamMsg_t  HereIAmMsg;   //2 bytes: group, pos
    CmdMsg_t      CmdMsg;       //2 bytes: cmd, param
  };
  uint8_t        crc8;
} udpMessage_t;

const char* class_multi::mcast_addr = "239.255.255.250";
const uint8_t class_multi::multicast_addr[4] = {239, 255, 255, 250};
const uint16_t class_multi::multicast_port = 56997;




uint8_t class_multi::crc8(uint8_t *data, uint8_t len)
{
  uint8_t crc = 0xff;
  uint8_t i, j;

  for (i = 0; i < len; i++)
  {
    crc ^= data[i];
    for (j = 0; j < 8; j++)
    {
      if ((crc & 0x80) != 0)
        crc = (uint8_t)((crc << 1) ^ 0x31);
      else
        crc <<= 1;
    }
  }

  return crc;
}


//stop, run_close, run_open, run_to_pos()


class_multi::class_multi()
{
  m_group = 3;  
  m_secSincePwrOn = 0;
  m_CleanupInterval = 0;
}

void class_multi::Tick1sec(void)
{
  m_secSincePwrOn++;
  cntSec++;
  m_CleanupInterval++;
}

void class_multi::sendHereIAm(void)
{
  //multicast use class D: 224.0.0.0 through 239.255.255.255
  //addresses 224.0.0.0 and 224.0.0.255 are reserved
  //SSDP uses 1900

  udpMessage_t msg;
  
  msg.SOT =   const_SOT;
  msg.MsgId = (uint8_t)HERE_I_AM;
  msg.HereIAmMsg.group = m_group;
  msg.HereIAmMsg.pos = p_blinds->GetPos();
  msg.crc8 = crc8((uint8_t*)&msg, sizeof(msg)-1);
  

  m_udp.beginPacketMulticast( multicast_addr, multicast_port, WiFi.localIP());
  //m_udp.write("Here I am, you fucker !!!");
  m_udp.write((uint8_t*)&msg, sizeof(msg));
  m_udp.endPacket();

  //MULTI_DbgMsg("HereIAm packet sent\n");
}

void class_multi::parsePacket(void)
{
  uint8_t rxBuf[255];
  uint16_t rxSize = m_udp.parsePacket();

  if (rxSize)
  {
    int len = m_udp.read(rxBuf, 255);
    if (len > 0)
    {
      // valid length of message ?
      if(len >= (int)sizeof(udpMessage_t))
      {
        udpMessage_t* p_msg = (udpMessage_t*)rxBuf;

        // start of transmission ?
        if(const_SOT == p_msg->SOT)
        {
          // valid crc ?
          if(p_msg->crc8 == crc8(rxBuf, sizeof(udpMessage_t)-1));
          {
            //valid message - handle it
            switch(p_msg->MsgId)
            {
              case HERE_I_AM:
                // update external devices list
                updateDeviceList(m_udp.remoteIP(), p_msg->HereIAmMsg.group, p_msg->HereIAmMsg.pos);
              break;
              case MOTOR_CMD:
                MULTI_DbgMsg("Received UDP command\n");
                switch(p_msg->CmdMsg.Cmd)
                {
                  case STOP:
                    p_blinds->Stop();
                  break;
                  case RUN_CLOSE:
                    p_blinds->Close();
                  break;
                  case RUN_OPEN:
                    p_blinds->Open();
                  break;
                  case RUN_TO_POS:
                    p_blinds->RunToPos(p_msg->CmdMsg.Param);
                  break;
                }
              break;
            }
          }
        }
      }

      rxBuf[len] = 0; //zero-terminate string
    }

    /*MULTI_DbgMsg("Received %d bytes from %s, port %d, dest ip: %s\nUDP packet contents: %s\n",
      rxSize,
      m_udp.remoteIP().toString().c_str(),
      m_udp.remotePort(),
      m_udp.destinationIP().toString().c_str(),
      rxBuf );*/
  }
}

void class_multi::sendCmd(IPAddress ip, uint8_t cmd, uint8_t param)
{
  udpMessage_t msg;
  
  msg.SOT =   const_SOT;
  msg.MsgId = (uint8_t)MOTOR_CMD;
  msg.CmdMsg.Cmd = cmd;
  msg.CmdMsg.Param = param;
  msg.crc8 = crc8((uint8_t*)&msg, sizeof(msg)-1);

  m_udp.beginPacket(ip, multicast_port);
  m_udp.write((uint8_t*)&msg, sizeof(msg));
  m_udp.endPacket();
}

void class_multi::updateDeviceList(IPAddress ip, uint8_t grp, uint8_t pos)
{
  bool DeviceAlreadyInList = false;
  //search through if the device is already existing in list
  for (auto it=m_deviceList.begin(); it != m_deviceList.end(); ++it)
  {
    if(it->ip == ip)
    {
      // device already in list - update position, group and timeout
      it->pos = pos;
      it->group = grp;
      it->timestamp = m_secSincePwrOn;
      DeviceAlreadyInList = true;
      return;
    }
  }

  //add device, if it is not in the list
  if(DeviceAlreadyInList == false)
  {
    if(m_deviceList.size() < const_MAX_DEVICES)
    {
      m_deviceList.push_back( {ip, grp, pos, m_secSincePwrOn} );
      MULTI_DbgMsg("Device joined (%s). Total devices: %i\n", ip.toString().c_str(), m_deviceList.size());
    }
  }
}


void class_multi::TaskRun(void)
{
  // send HereIAm every 5 second
  if(cntSec > 5)
  {
    cntSec = 0;
    sendHereIAm();
  }

  // parse incoming packets
  parsePacket();

  // housekeeping of deviceList
  if(m_CleanupInterval > 8)
  {
    cleanupDeviceList();
    m_CleanupInterval = 0;
  }
}

void class_multi::cleanupDeviceList(void)
{
  for (auto it=m_deviceList.begin(); it != m_deviceList.end(); ++it)
  {
    // has timeout exceed ?
    if(it->timestamp + 10 < m_secSincePwrOn )
    {
      m_deviceList.erase(it);
      MULTI_DbgMsg("Device left. Total devices: %i\n", m_deviceList.size());
      return;
    }
  }
}

void class_multi::TaskInit(void)
{
  bool initState = true;

  // start listen for udp multicasts
  if( false == m_udp.beginMulticast(WiFi.localIP(), multicast_addr, multicast_port) )
  {
    MULTI_DbgMsg("Multcast join - failed\n");
  }
  
  // start listen for normal udp packets
  if( false == m_udp.begin(multicast_port) )
  {
    MULTI_DbgMsg("failed udp start listen !!\n");
  }
  
  if(true == initState)
  {
    MULTI_DbgMsg("Initiated\n");
  }
}

void class_multi::CmdGroup(uint8_t grp, uint8_t Cmd, uint8_t param)
{
  for (auto it=m_deviceList.begin(); it != m_deviceList.end(); ++it)
  {
    if(it->group == grp)
    {
      sendCmd(it->ip, Cmd, param);
    }
  }
}

uint8_t class_multi::GetNumberOfDevices(void)
{
  return (uint8_t)m_deviceList.size();
}
