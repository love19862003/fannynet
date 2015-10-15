/********************************************************************

  Filename:   NetIo

  Description:NetIo

  Version:  1.0
  Created:  13:10:2015   13:34
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_io_pool_H__
#define __FANNY_io_pool_H__
#include <mutex>
#include <set>
#include <map>
#include <thread>
#include <list>
#include <memory>
#include <atomic>
#include "fannynet.h"
#include "net_type.h"
#include "connection.h"
namespace FannyNet {
  class IoObject {
  public:
    explicit IoObject() :m_ios(), m_work(m_ios){
      auto fun = [this] () { m_ios.run(); };
      m_thread = std::move(std::thread(fun));
    }
    ~IoObject() {
       if (m_thread.joinable()){
         stop();
       }
    }

    void stop() {
      m_ios.stop();
      m_thread.join();
    }
    const NetIOService& io()const { return m_ios; }
    NetIOService& io() { return m_ios; }
  protected:
    NetIOService m_ios;
    NetWorkType m_work;
    std::thread m_thread;
  private:
    IoObject(const IoObject&) = delete;
    IoObject& operator = (const IoObject&) = delete;
    IoObject& operator = (IoObject&&) = delete;
  };
  class IoObjectPool {
  public:
    explicit IoObjectPool(size_t size) :m_curIndex(0), m_curSessionId(INVALID_SESSION_ID) {
      assert(size > 0);
      for(size_t i = 0; i < size; ++i) {
        IoObjectPtr io(new IoObject());
        m_netIo.push_back(std::move(io));
      }
    }
    virtual ~IoObjectPool() { stop(); }

    NetIOService& getIoService() {
      if(m_curIndex >= m_netIo.size()) { m_curIndex = 0; }
      return m_netIo[m_curIndex++]->io();
    }
   

    SessionId nextSession() {
      return ++m_curSessionId;
    }

    void addSession(ConnectPtr s) {
      ConnectWeakPtr wp(s);
      m_onlines[s->id()] = wp;
    }
    void delSession(ConnectPtr s) { 
      auto it = m_onlines.find(s->id());
      if(it != m_onlines.end()) { m_onlines.erase(it); }
    }
    ConnectPtr session(SessionId s) {
      auto it = m_onlines.find(s);
      if(it != m_onlines.end()) { 
        ConnectWeakPtr wp = it->second;
        if(!wp.expired()) { return wp.lock(); }
      }
      return nullptr;
    }
  protected:
    void stop() {
      for(auto& io : m_netIo) { io->stop(); io.reset(); }
      m_netIo.clear();
      m_onlines.clear();
    }
  protected:
    size_t m_curIndex = 0;
    std::atomic<SessionId> m_curSessionId;
    std::vector<IoObjectPtr> m_netIo;
    std::map<SessionId, ConnectWeakPtr> m_onlines;
  private:
    IoObjectPool(const IoObjectPool&) = delete;
    IoObjectPool& operator = (const IoObjectPool&) = delete;
    IoObjectPool& operator = (IoObjectPool&&) = delete;
  };
}
#endif