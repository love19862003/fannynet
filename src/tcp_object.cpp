/********************************************************************

  Filename:   tcp_objetc

  Description:tcp_objetc

  Version:  1.0
  Created:  14:10:2015   16:40
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#include "tcp_object.h"
#include "net_type.h"
namespace FannyNet {
  class TcpServer : public  TcpObj {
  public:
    explicit TcpServer(const IoObjectPoolPtr& io, NetPropertyPointer p) : TcpObj(io, std::move(p)) {

    }
    virtual ~TcpServer() { m_acceptor.reset();  m_current.reset(); }
    virtual bool doStart() override {
      try {
        EndPointType ep(boost::asio::ip::tcp::v4(), property()->config().m_port);
        m_acceptor = std::move(AcceptorPtr(new NetAcceptor(m_refIOPool->getIoService())));
        m_acceptor->open(ep.protocol());
        boost::asio::socket_base::reuse_address option(true);
        boost::asio::socket_base::linger optiontemp;
        boost::asio::socket_base::linger option2(true, 0);
        m_acceptor->get_option(optiontemp);
        m_acceptor->set_option(option);
        m_acceptor->set_option(option2);
        m_acceptor->bind(ep);
        m_acceptor->listen();
      } catch(const NetErrorType& e) {
        //LOGERROR("init server failed with error:", e.what());
        printf(e.message().c_str());
        return false;
      }

      beginAccept();
      return true;
    }
    virtual bool doStop() override {
      m_acceptor->close();
      m_acceptor.reset();
      m_current->close();
      m_stop = true;
      return true;
    }
    virtual ConnectionProperty createProperty() override {
      ConnectionProperty pro(m_refIOPool->getIoService());
      createPropertyBase(pro);
      return std::move(pro);
    }
  private:
    void beginAccept() {
      m_current = createConnection();
      m_acceptor->async_accept(m_current->socket(), boost::bind(&TcpServer::acceptSocket, this, boost::asio::placeholders::error));
    }
    void acceptSocket(const boost::system::error_code& error) {
      if(m_stop || !m_current) { return; }
      if(!error) {
        m_current->postAccept();
      } else {
        m_current->onError(error);
      }
      beginAccept();
    }
  protected:
    typedef std::unique_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;
    AcceptorPtr m_acceptor;
    ConnectPtr m_current;
    bool m_stop = false;
  private:
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator = (const TcpServer&) = delete;
    TcpServer& operator = (TcpServer&&) = delete;
  };
  class TcpClient : public TcpObj {
  public:
    explicit TcpClient(const IoObjectPoolPtr& io, NetPropertyPointer p) : TcpObj(io, std::move(p)) {
      m_session = createConnection();
      m_endpoint.port(m_netProperty->config().m_port);
      std::string ip = m_netProperty->config().m_address.c_str();
      struct hostent *hptr = gethostbyname(ip.c_str());
      if(hptr) {
        char str[32] = {0};
        sprintf(str, "%d.%d.%d.%d",
                (hptr->h_addr_list[0][0] & 0x00ff),
                (hptr->h_addr_list[0][1] & 0x00ff),
                (hptr->h_addr_list[0][2] & 0x00ff),
                (hptr->h_addr_list[0][3] & 0x00ff));
        ip = str;
      }
      m_endpoint.address(boost::asio::ip::address_v4::from_string(ip));
    };
    virtual ~TcpClient() {
      m_session.reset();
    }
    virtual bool doStart() override {
      m_session->connect(m_endpoint);
      return true;
    }
    virtual bool doStop() override {
      
      return true;
    }
    virtual ConnectionProperty createProperty() override {
      ConnectionProperty pro(m_refIOPool->getIoService());
      createPropertyBase(pro);
      pro._eventConnectFailedFun = [this] () {
        if(m_netProperty->config().m_autoReconnect) {
          TcpEvent fun = [this] {start(); };
          push(fun);
        } else { printf("connect failed but not auto reconnect"); }
      };
      return std::move(pro);
    }
  protected:
    ConnectPtr m_session;
    EndPointType m_endpoint;
  private:
  };

  NetObjectPtr TcpObj::create(const IoObjectPoolPtr& io, NetPropertyPointer p) {
    if(nullptr == p) { return nullptr; }
    if(Config::_CLIENT_FLAG_ == p->config().m_serviceType) { return  std::move(NetObjectPtr(new TcpClient(io, std::move(p)))); }
    if(Config::_SERVER_FLAG_ == p->config().m_serviceType) { return  std::move(NetObjectPtr(new TcpServer(io, std::move(p)))); }
    return nullptr;
  }
}