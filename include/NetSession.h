/********************************************************************

  Filename:   NetSession

  Description:NetSession

  Version:  1.0
  Created:  12:10:2015   17:34
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_NetSession_H__
#define __FANNY_NetSession_H__
#include <boost/asio.hpp>
#include <boost/logic/tribool.hpp>
#include "FannyNet.h"
namespace FannyNet {
  class NetSession
  {
  public:
    typedef boost::asio::ip::tcp::socket NetSocket;
    typedef std::unique_ptr<NetSocket> SocketPtr;
    explicit NetSession(SocketPtr );
  protected:
  private:
    SessionId m_session;
    BufferPtr m_bufferSend;
    BufferPtr m_bufferRecv;
    BlockPtr  m_waitMessage;
    boost::tribool m_connect;
    bool      m_reconn;
    size_t    m_totalRevc = 0;
    size_t    m_totalSend = 0;
    SocketPtr m_socket;
  private:
    NetSession(const NetSession&) = delete;
    NetSession& operator = (const NetSession&) = delete;
    NetSession& operator = (NetSession&&) = delete;
  };
}
#endif