/********************************************************************

  Filename:   connetion

  Description:connetion

  Version:  1.0
  Created:  14:10:2015   16:47
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#include "connection.h"
namespace FannyNet {
  Connection::Connection(SessionId s, const ConnectionProperty& pro, size_t size)
  : m_session(s)
  , m_bufferSend(new NetBuffer(size))
  , m_bufferRecv(new NetBuffer(size))
  , m_waitMessage(nullptr)
  , m_connect(false)
  , m_totalRevc(0)
  , m_totalSend(0)
  , m_isSend(false)
  , m_property(pro)
  { 
    m_socket = std::move(SocketPtr(new NetSocket(m_property._ioService)));
  }
  Connection::~Connection() {
    //close();
    m_socket.reset();
  }
  NetSocket& Connection::socket() {
    return *m_socket;
  }

  void Connection::send(BlockPtr p) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sendList.push_back(std::move(p));
    m_property._ioService.post(boost::bind(&Connection::writeMessage,  shared_from_this(), 0));

  }

  void Connection::handleClose() {
    boost::system::error_code ignored_ec;
    m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    try {
      m_socket->close();
    } catch(const boost::system::error_code&) {

    }
    m_connect = false;
  }
  void Connection::beginRead() {
    m_socket->async_read_some(boost::asio::buffer(m_bufferRecv->writeData(), m_bufferRecv->maxLength() - m_bufferRecv->length()),
                              boost::bind(&Connection::handleReadSome, this,
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred));
  }

  void Connection::handleReadSome(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if(error) { onError(error); return; }
    m_bufferRecv->writeData(bytes_transferred);
    m_totalRevc += bytes_transferred;
    auto readMessage = [this] ()->BlockPtr {
      if(nullptr == m_waitMessage) { m_waitMessage = std::move(m_property._blockMakerFun(m_session, 0)); }
      m_waitMessage->recv(m_bufferRecv);
      if(m_waitMessage->bodyDone()) {
        auto p = std::move(m_waitMessage);
        m_waitMessage = nullptr;
        return std::move(p);
      }
      return nullptr;
    };

    while(m_bufferRecv->hasWrite()) {
      auto p = std::move(readMessage());
      if(nullptr == p) { break; } else {
        m_property._eventPostRecv(std::move(p));
      }
    }

    //¼ÌÐø¶ÁÈ¡ 
    m_socket->async_read_some(boost::asio::buffer(m_bufferRecv->writeData(), m_bufferRecv->maxLength() - m_bufferRecv->length()),
                              boost::bind(&Connection::handleReadSome, this,
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred));
  }

  void Connection::handlAccept() {
    m_connect = true;
    m_property._eventConnectFun(shared_from_this());
    beginRead();
  }
  void Connection::writeMessage(size_t size) {
    if(!m_connect) { return; }
    if(m_isSend) { return; }
    std::lock_guard<std::mutex> lock(m_mutex);
    m_bufferSend->readData(size);
    m_bufferSend->clearReadBuffer();
    m_totalSend += size;
    while(!m_sendList.empty()) {
      auto& p = m_sendList.front();
      p->data()->readBuffer(*m_bufferSend);
      if(p->data()->hasRead()) { break; } else { m_sendList.pop_front(); }
    }

    if(m_bufferSend->hasRead()) {
      m_isSend = true;
      boost::asio::async_write(socket(),
                               boost::asio::buffer(m_bufferSend->readData(), m_bufferSend->needReadLength()),
                               boost::bind(&Connection::handleWrite, this, boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));
    }
  }
  void Connection::handleWrite(const boost::system::error_code& error, std::size_t size) {
    m_isSend = false;
    if(error) { onError(error); return; }
    if(m_connect) { writeMessage(size); }
  }

  void Connection::handleConnect(const boost::system::error_code& error) {
    if(m_connect) { return; }
    if(error) {
      m_connect = false;
      m_property._eventConnectFailedFun();
      return;
    } else {
      m_connect = true;
      m_property._eventConnectFun(shared_from_this());
      beginRead();
      return;
    }
  }
  void Connection::postAccept() {
    m_property._ioService.post(boost::bind(&Connection::handlAccept, shared_from_this()));
  }
  void Connection::postConnect() {

  }
  void Connection::connect(EndPointType ep) {
    m_socket->async_connect(ep, boost::bind(&Connection::handleConnect, this, boost::asio::placeholders::error));
  }
  void Connection::close() {
    m_property._ioService.post(boost::bind(&Connection::handleClose, shared_from_this()));
  }
}