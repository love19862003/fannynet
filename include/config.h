/********************************************************************

  Filename:   config

  Description:config

  Version:  1.0
  Created:  14:10:2015   16:24
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_config_H__
#define __FANNY_config_H__
#include <assert.h>
#include <string>
#ifdef _MSC_VER
#define _WIN32_WINNT 0x0501
#endif

#ifndef FINNY_NET_STATIC_LIB
#ifdef WIN32
#ifdef FINNY_NET_EXPORTS
#define FINNY_NET_API __declspec(dllexport)
#else
#define FINNY_NET_API __declspec(dllimport)

#endif
#else
#define FINNY_NET_API
#endif
#else
#define FINNY_NET_API
#endif

#define MYASSERT(condition, ...) do {  \
      if( !(condition) ){\
        assert(false);\
      }\
     } while(0)  


namespace FannyNet {
  typedef unsigned long long SessionId;
  typedef std::string NetName;
  enum { INVALID_SESSION_ID = 0, };

  struct FINNY_NET_API Config {
    enum ServiceFlag {
      _SERVER_FLAG_,
      _CLIENT_FLAG_,
    };

    Config(const std::string& name, const std::string& addr, unsigned int port, int time, int maxConn) 
    : m_name(name)
    , m_address(addr)
    , m_port(port)
    , m_timeOut(time)
    , m_maxConnect(maxConn)
    , m_serviceType(_SERVER_FLAG_)
    , m_autoReconnect(false)
    {

    }
    Config(const std::string& name, const std::string& addr, unsigned int port, bool autoConn) 
      : m_name(name)
      , m_address(addr)
      , m_port(port)
      , m_timeOut(0)
      , m_maxConnect(1)
      , m_serviceType(_CLIENT_FLAG_)
      , m_autoReconnect(autoConn)
    {

    }
    std::string  m_name;
    std::string  m_address;
    unsigned int m_port;
    int          m_timeOut;
    int          m_maxConnect;
    ServiceFlag  m_serviceType;
    bool         m_autoReconnect;
  };
}
#endif