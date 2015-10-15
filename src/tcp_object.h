/********************************************************************

  Filename:   tcp_object

  Description:tcp_object

  Version:  1.0
  Created:  14:10:2015   16:38
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_tcp_object_H__
#define __FANNY_tcp_object_H__
#include <list>
#include <mutex>
#include "fannynet.h"
#include "io_pool.h"
#include "block.h"
namespace FannyNet {
  class TcpObj {
  public:
    explicit TcpObj(const IoObjectPoolPtr& io, NetPropertyPointer p) :m_netProperty(std::move(p)), m_refIOPool(io){;}
    virtual ~TcpObj() {
      m_call.clear();
      m_onlines.clear();
      m_netProperty.reset(nullptr);
    }
    const NetPropertyPointer& property() const { return m_netProperty; }
    static NetObjectPtr create(const IoObjectPoolPtr& io, NetPropertyPointer p);

    void poll() {
      std::list<TcpEvent> recv;
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        recv = m_call;
        m_call.clear();
      }
      for(auto& b : recv) { b(); }
    }
    void push(const TcpEvent& ev) {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_call.push_back(std::move(ev));
    }

    void eventRecv(BlockPtr& p) {
      auto pp = p.release();
      TcpEvent ev = [this, pp] ()->void { 
        BlockPtr up(pp);
        m_netProperty->callFun()(m_netProperty->config().m_name, boost::cref(up));
      };
      push(ev);
    }

    void eventConnect(ConnectPtr s) {
      TcpEvent fun = [=] () {
        m_refIOPool->addSession(s);
        m_onlines.insert(s);
        m_netProperty->connectFun()(m_netProperty->config().m_name, s->id());
      };
      push(fun);
    }

    void eventCloseFun(ConnectPtr s) {
      TcpEvent fun = [=] () {
        m_netProperty->closeFun()(m_netProperty->config().m_name, s->id());
        m_refIOPool->delSession(s);
        m_onlines.erase(s);
        printf("del session");
      };
      push(fun);
    };

    ConnectPtr createConnection() {
      ConnectionProperty pro = std::move(createProperty());
      SessionId id = m_refIOPool->nextSession();
      ConnectPtr s(new Connection(id, pro));
      return std::move(s);
    }
    void createPropertyBase(ConnectionProperty& pro) {
      pro._blockMakerFun = m_netProperty->makeBlockFun();
      pro._eventPostRecv = boost::bind(&TcpObj::eventRecv, this, _1);
      pro._eventConnectFun = boost::bind(&TcpObj::eventConnect, this, _1);
      pro._eventCloseFun = boost::bind(&TcpObj::eventCloseFun, this, _1);
    }
    virtual ConnectionProperty createProperty() = 0;
    bool start() { m_stop = false; return doStart(); }
    bool stop() {
      m_stop = true;
      for (auto& v: m_onlines){ v->close(); }
      return doStop();
    }

    const std::set<ConnectPtr>& onlines() const {
      return m_onlines;
    }
  protected:
    virtual bool doStart() = 0;
    virtual bool doStop() = 0;
  protected:
    std::mutex m_mutex;
    std::list<TcpEvent> m_call;
    std::set<ConnectPtr> m_onlines;
    NetPropertyPointer m_netProperty;
    const IoObjectPoolPtr& m_refIOPool;
    bool m_stop = true;
  private:
  };

}
#endif