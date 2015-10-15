/********************************************************************

  Filename:   connetion

  Description:connetion

  Version:  1.0
  Created:  12:10:2015   17:34
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_connetion_H__
#define __FANNY_connetion_H__
#include <atomic>
#include <memory>
#include <mutex>
#include <list>
#include "fannynet.h"
#include "net_type.h"
namespace FannyNet {
  class Connection : public std::enable_shared_from_this<Connection>
  {
  public:
    explicit Connection(SessionId s, const ConnectionProperty& pro, size_t size = 65535);
    virtual ~Connection();
    NetSocket& socket();
    void writeMessage(size_t size);
    void send(BlockPtr p);
    void handleClose();
    void beginRead();
    void handleReadSome(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handlAccept();
    void handleWrite(const boost::system::error_code& error, std::size_t size);
    void handleConnect(const boost::system::error_code& error);
    void onError(const NetErrorType& er) { close();}
    void close();
    void connect(EndPointType ep);
    SessionId id()const { return m_session; }
    void postAccept();
    void postConnect();
    const ConnectionProperty& property() const { return m_property; }
  private:
    SessionId m_session;
    BufferPtr m_bufferSend;
    BufferPtr m_bufferRecv;
    BlockPtr  m_waitMessage;
    bool      m_connect;
    std::atomic<size_t>    m_totalRevc;
    std::atomic<size_t>    m_totalSend;
    std::atomic<bool>      m_isSend;
    std::mutex             m_mutex;
    std::list<BlockPtr>    m_sendList;
    SocketPtr m_socket;
    ConnectionProperty m_property;
  private:
    Connection(const Connection&) = delete;
    Connection& operator = (const Connection&) = delete;
    Connection& operator = (Connection&&) = delete;
  };
}
#endif