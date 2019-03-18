#ifndef _MULTICTRL_h
#define _MULTICTRL_h

#include "class_defs.h"
#include "blinds.h"
#include <WiFiUdp.h>
#include <IPAddress.h>
#include <list>
#include <string>
using namespace std;







typedef struct {
  IPAddress   ip;
  uint8_t     group;
  uint8_t     pos;
  uint32_t    timestamp;
} MultiDevice_t;

class class_multi : public Task, public Timer1sec, public class_UserOfBlinds
{
  private:
    void        sendHereIAm(void);
    void        parsePacket(void);
    void        sendCmd(IPAddress ip, uint8_t cmd, uint8_t param);
    uint8_t     crc8(uint8_t *data, uint8_t len);
    void        updateDeviceList(IPAddress ip, uint8_t group, uint8_t pos);
    void        cleanupDeviceList(void);

    uint8_t                 m_group;
    list<MultiDevice_t>     m_deviceList;
    WiFiUDP                 m_udp;
    unsigned char           cntSec;
    uint32_t                m_secSincePwrOn;
    uint8_t                 m_CleanupInterval;

    static const uint8_t    multicast_addr[4];
    static const char*      mcast_addr;           //this works, using the fromString()
    static const uint16_t   multicast_port;
    static const uint16_t   const_SOT = 0x9669;
    static const uint8_t    const_MAX_DEVICES = 255;

  public:
    class_multi();

    //void CmdSingleDev(uint8_t devNo, uint8_t Cmd);
    void CmdGroup(uint8_t group, uint8_t Cmd, uint8_t param);
    //void CmdAll(uint8_t Cmd, uint8_t param);

    uint8_t GetNumberOfDevices(void);
    //list<MultiDevice_t> GetDeviceList(void);
    //void SetDeviceGroup(unsigned int, unsigned int);

    //inherited
    void Tick1sec(void);
    void TaskRun (void);
    void TaskInit (void);
};


#endif
