/********************************************************************

  Filename:   NetService

  Description:NetService

  Version:  1.0
  Created:  13:10:2015   10:21
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/


#include <mutex>
#include <set>
#include <map>
#include <thread>
#include <list>
#include "NetService.h"
#include "Connection.h"
#include "NetIoService.h"
namespace FannyNet {
  

  NetService::NetService(size_t t) :m_pool(new IOPool(t)) {

  }
  NetService::~NetService() {

  }
  bool NetService::start() { 
    return true; 
  }
  void NetService::stop() {
    m_pool->stop();
  }
  bool NetService::add(NetPropertyPointer p) {
    return m_pool->add(std::move(p));
  }
  bool NetService::remove(const NetName& name) {
    return true;
  }
  bool NetService::poll() {
    m_pool->poll();
    return true;
  }
  bool NetService::send(const BlockPtr& msg) {
    return m_pool->send(msg);
  }
  bool NetService::send(const NetName& name, const BlockPtr& msg) {
    return m_pool->send(name, msg);
  }
  bool NetService::kick(const SessionId& id) {
    return true;
  }
  bool NetService::isRun() const {
    return true;
  }

}