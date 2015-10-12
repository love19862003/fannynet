/********************************************************************

  Filename:   NetManager

  Description:NetManager

  Version:  1.0
  Created:  31:3:2015   19:17
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#include "NetService.h"
#include "NetManager.h"
namespace FannyNet {
    NetManager::NetManager(unsigned int t)
    :m_service(new NetService(t))
    {  }
    NetManager::~NetManager() {
      m_service.reset();
    }
    bool NetManager::start() {
      return m_service->start();
    }

    bool NetManager::add(NetPointer net) {
      return m_service->add(std::move(net));
    }
    bool NetManager::remove(const NetName& name) {
      return m_service->remove(name);
    }

    bool NetManager::poll() {
      return m_service->poll();
    }

    bool NetManager::send(const BlockPtr& msg) {
      return m_service->send(msg);
    }
    bool NetManager::send(const NetName& name, const BlockPtr& msg) {
      return m_service->send(name, msg);
    }

    bool NetManager::kick(const SessionId& id) {
      return m_service->kick(id);
    }

    bool NetManager::isRun() const {
      return m_service->isRun();
    }
    void NetManager::stop() {
      return m_service->stop();
    }
}