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
#include <boost/timer.hpp>
#include "service.h"
#include "block.h"
#include "tcp_object.h"
namespace FannyNet {
  

  NetService::NetService(size_t t) :m_ioPool(new IoObjectPool(t)), m_state(_INIT_){

  }
  NetService::~NetService() {
    m_nets.clear();
    m_ioPool.reset();
  }
  bool NetService::start() { 
    m_state = _RUN_;
    for (auto& pair : m_nets){
      if(!pair.second->start()) { return false; }
    }
    return true; 
  }
  void NetService::setStop() {
    m_state = _STOP_;
    for(auto& pair : m_nets) {
      pair.second->stop();
    }
  }
  bool NetService::stop(unsigned int sec) {
    if(sec <= 15) { sec = 20; }
    boost::timer  t;
    if(_STOP_ !=  m_state) { return false; }
    do 
    {
      poll();
    } while(t.elapsed() < sec && m_ioPool->hasOnline());
    return true;
  }
  bool NetService::add(NetPropertyPointer p) {
    auto it = m_nets.find(p->config().m_name);
    if(it != m_nets.end()) { return false; }
    auto name = p->config().m_name;
    m_nets.insert(std::make_pair(name, std::move(TcpObj::create(m_ioPool, std::move(p)))));
    return true;
  }
  bool NetService::remove(const NetName& name) {
    auto it = m_nets.find(name);
    if(it != m_nets.end()) { return false; }
    auto& n = it->second;
    n->stop();
    n.reset();
    m_nets.erase(it);
    return true;
  }
  bool NetService::poll() {
    for (auto& pair : m_nets){
      auto&  n = pair.second;
      n->poll();
    }
    return true;
  }
  bool NetService::send(BlockPtr msg) {
    if(!isRun()) { return false; }
    ConnectPtr c = m_ioPool->session(msg->session());
    if (c){
      c->send(std::move(msg));
      return true;
    }
    return false;
  }
  bool NetService::send(const NetName& name,  BlockPtr msg) {
    if(!isRun()) { return false; }
    auto it = m_nets.find(name);
    if(it == m_nets.end()) { return false; }
    auto& net = it->second;
    for (const ConnectPtr& c : net->onlines()){
      c->send(std::move(msg->clone(c->id())));
    }
    return true;
  }
  bool NetService::kick(const SessionId& id) {
    ConnectPtr c = m_ioPool->session(id);
    if(c) { c->close(); }
    return true;
  }
  bool NetService::isRun() const {
    return _RUN_ == m_state;
  }

}