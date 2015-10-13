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
#ifndef __FANNY_NetIo_H__
#define __FANNY_NetIo_H__
#include <mutex>
#include <set>
#include <map>
#include <thread>
#include <list>
#include <memory>
#include <boost/asio/io_service.hpp>
#include "FannyNet.h"
#include "TcpObject.h"
namespace FannyNet {
  class NetIo {
  public:
    explicit NetIo() :m_ios(), m_work(m_ios){
      auto fun = [this] () { m_ios.run(); };
      m_thread = std::move(std::thread(fun));
    }
    ~NetIo() {
      m_ios.stop();
      m_thread.join();
    }
    const boost::asio::io_service& io()const { return m_ios; }
    boost::asio::io_service& io() { return m_ios; }
  protected:
    boost::asio::io_service m_ios;
    boost::asio::io_service::work m_work;
    std::thread m_thread;
  private:
    NetIo(const NetIo&) = delete;
    NetIo& operator = (const NetIo&) = delete;
    NetIo& operator = (NetIo&&) = delete;
  };
  class IOPool {
  public:
    typedef std::unique_ptr<NetIo> NetIoPtr;
    typedef TcpObj::TcpObjPtr TcpObjPtr;
    explicit IOPool(size_t size) :m_curIndex(0) {
      MYASSERT(size > 0, "handle io service size is failed");
      for(size_t i = 0; i < size; ++i) {
        NetIoPtr io(new NetIo());
        m_netIo.push_back(std::move(io));
      }
    }

    NetIoPtr& getIoService() {
      if(m_curIndex >= m_netIo.size()) { m_curIndex = 0; }
      return m_netIo[m_curIndex++];
    }

    void stop() {
      for(auto& io : m_netIo) { io.reset(); }
      m_netIo.clear();
    }
    void poll() {
      for(auto& obj : m_nets) { obj.second->poll(); }
    }

    bool add(NetPropertyPointer p) {
      if(!p) { return false; }
      auto name = p->config().m_name;
      if(m_nets.count(name)) { return false; }
      m_nets[name] = std::move(TcpObj::create(getIoService(), std::move(p)));
      return true;
    }
  protected:
    size_t m_curIndex = 0;
    std::vector<NetIoPtr> m_netIo;
    std::map<NetName, TcpObjPtr> m_nets;
  private:
    IOPool(const IOPool&) = delete;
    IOPool& operator = (const IOPool&) = delete;
    IOPool& operator = (IOPool&&) = delete;
  };
}
#endif