/********************************************************************

  Filename:   TcpObject

  Description:TcpObject

  Version:  1.0
  Created:  13:10:2015   14:33
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#include "TcpObject.h"
#include "Connection.h"
namespace FannyNet {
  class TcpServer : public  TcpObj {
  public:
    explicit TcpServer(const IOPoolPtr& io, NetPropertyPointer p) : TcpObj(io, std::move(p)) {
     
    }
    virtual bool start() override {
      try {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), property()->config().m_port);
        m_acceptor = std::move(AcceptorPtr(new boost::asio::ip::tcp::acceptor(m_refIOPool->getIoService())));
        m_acceptor->open(ep.protocol());
        boost::asio::socket_base::reuse_address option(true);
        boost::asio::socket_base::linger optiontemp;
        boost::asio::socket_base::linger option2(true, 0);
        m_acceptor->get_option(optiontemp);
        m_acceptor->set_option(option);
        m_acceptor->set_option(option2);
        m_acceptor->bind(ep);
        m_acceptor->listen();
      } catch(boost::system::system_error& e) {
        LOGERROR("init server failed with error:", e.what());
      }

      beginAccept();
    }
    virtual bool stop() override { return true; }
  private:
    void beginAccept() {
      m_current = std::move(SessionPtr(new NetConnection));
    
      m_acceptor->async_accept(m_current->socket(), std::bind(&TcpServer::acceptSocket, this, boost::asio::placeholders::error));
    }
    void acceptSocket(SessionPtr s, const boost::system::error_code& error) {
      if (!error){
        m_current->shared_from_this();
        auto fun = [this] (PostFun call) {  push(call); }
        m_current->getIo().post(&NetConnection::connect, m_current.get(), std::move(fun));
      } else {
        m_current->onError(error);
      }
      beginAccept();
    }
  protected:
    typedef std::unique_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;
    AcceptorPtr m_acceptor;
    SessionPtr m_current;
  private:
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator = (const TcpServer&) = delete;
    TcpServer& operator = (TcpServer&&) = delete;
  };
  class TcpClient : public TcpObj {
  public:
    explicit TcpClient(const IOPoolPtr& io, NetPropertyPointer p) : TcpObj(io, std::move(p)) {

    };
    virtual bool start() override { return true; }
    virtual bool stop() override { return true; }
  protected:
    SessionPtr m_session;
  private:
  };

  TcpObjPtr TcpObj::create(const IOPoolPtr& io, NetPropertyPointer p) {
    if(nullptr == p) { return nullptr; }
    if(Config::_CLIENT_FLAG_ == p->config().m_serviceType) { return  std::move(TcpObjPtr(new TcpClient(io, std::move(p)))); }
    if(Config::_SERVER_FLAG_ == p->config().m_serviceType) { return  std::move(TcpObjPtr(new TcpServer(io, std::move(p)))); }
    return nullptr;
  }
}