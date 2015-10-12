/********************************************************************

  Filename:   NetThread

  Description:NetThread

  Version:  1.0
  Created:  12:10:2015   12:55
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_NetThread_H__
#define __FANNY_NetThread_H__
#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>
#include <mutex>
namespace FannyNet {
  class NetThread
  {
  public:
    enum state {
      _init_ = 0,
      _run_ = 1,
      _stop_ = 2,
    };


  protected:
  private:
    std::mutex m_sendMutex;
    std::mutex m_recvMutex;
    state m_state;
    unsigned int m_onlineCount;
    
  };
}
#endif