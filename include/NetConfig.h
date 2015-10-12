/********************************************************************

  Filename:   FannyNet

  Description:FannyNet

  Version:  1.0
  Created:  12:10:2015   10:29
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_NETCONFIG_H__
#define __FANNY_NETCONFIG_H__

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

#include <string>
namespace FannyNet {
 
  // net config
  typedef unsigned long long SessionId;
  typedef std::string NetName;
  enum { INVALID_SESSION_ID = 0,};
  struct FINNY_NET_API Config{
    enum ServiceFlag {
      _SERVER_FLAG_,
      _CLIENT_FLAG_,
    };

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