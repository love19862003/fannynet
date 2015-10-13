/********************************************************************

  Filename:   TcpServer

  Description:TcpServer

  Version:  1.0
  Created:  13:10:2015   13:18
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_TcpServer_H__
#define __FANNY_TcpServer_H__
#include "FannyNet.h"
#include <list>
#include <mutex>
#include <boost/asio/ip/tcp.hpp>
namespace FannyNet {
  class TcpObj
  {
  public:
    typedef std::unique_ptr<TcpObj> TcpObjPtr;
    typedef std::function<void()> PostFun;
    explicit TcpObj(IoPtr io, NetPropertyPointer p);
    const NetPropertyPointer& property() const { return m_netProperty; }
    static TcpObjPtr create(IoPtr io, NetPropertyPointer p);

    IoPtr getIo() {
      if(!m_io.expired()) {
        return m_io.lock();
      }
      return nullptr;
    }

    void poll() {
      std::list<PostFun> recv;
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        recv = m_call;
        m_call.clear();
      }
      for (auto& b : recv){ b();}
    }
    void addRecv(BlockPtr b) {
      PostFun call = std::bind(m_netProperty->callFun(), m_netProperty->config().m_name,  std::move(b));
      push(std::move(call));
    }
    void push(PostFun fun) {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_call.push_back(std::move(fun));
    }
    virtual bool start() = 0;
    virtual bool stop() = 0;
  protected:
    std::mutex m_mutex;
    std::list<PostFun> m_call;
    IoWeakPtr m_io;
    NetPropertyPointer m_netProperty;
  private:
  };
  
}
#endif