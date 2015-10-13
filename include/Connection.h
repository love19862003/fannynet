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
#include <atomic>
#include "FannyNet.h"
namespace FannyNet {
  class NetConnection : public std::enable_shared_from_this<NetConnection>
  {
  public:
    explicit NetConnection(SessionId s, boost::asio::io_service& io, const TcpObjPtr& obj, size_t size = 65535);
    void run();
    boost::asio::ip::tcp::socket& socket();
    void onError();
    std::string getRemotoIp();
    unsigned int getRemotoPort();
  protected:
    void handleConnect();
    void connectServer();
  private:
    SessionId m_session;
    BufferPtr m_bufferSend;
    BufferPtr m_bufferRecv;
    BlockPtr  m_waitMessage;
    bool      m_connect;
    bool      m_reconn;
    std::atomic<size_t>    m_totalRevc = 0;
    std::atomic<size_t>    m_totalSend = 0;
    std::atomic_bool       m_isSend;
    const TcpObjPtr& m_owner;
    boost::asio::io_service& m_io;
    typedef std::unique_ptr<boost::asio::ip::tcp::socket> SocketPtr;
    SocketPtr m_socket;
  private:
    NetConnection(const NetConnection&) = delete;
    NetConnection& operator = (const NetConnection&) = delete;
    NetConnection& operator = (NetConnection&&) = delete;
  };
}
#endif