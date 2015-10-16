/********************************************************************

  Filename:   test

  Description:test

  Version:  1.0
  Created:  15:10:2015   10:39
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#include "manager.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <signal.h>
#include <time.h>
#include <chrono>
#include <memory>
#include "redisblock.h"
using namespace  FannyNet;

static std::atomic<bool> isRunning(true);
static std::function<void()> quitFunction = nullptr;
void signalHandler(int /*sig*/) {
  isRunning = false;
  if(quitFunction) { quitFunction(); }
}

int main() {
  NetManager net(3);
  quitFunction = [&]()->void{
    net.setStop();
  };
  FunCall fc = [&] (const NetName& name, const BlockPtr& ptr) { 
//     std::cout << "handle net:" << name << 
//               " session:" << ptr->session() 
//               << " msg:" << std::string(ptr->data()->data(), ptr->data()->length()) << std::endl;

    RedisBlock* p = static_cast<RedisBlock*>(ptr.get());
    for(auto& vv : p->debug()) { std::cout <<"redis command recv:" << vv << std::endl; }
     std::string msg = RedisCommand::makeCommand("get player");
     BlockPtr pp(new RedisBlock(ptr->session(),msg.length()));
     pp->push(msg.data(), msg.length());
     net.send(std::move(pp));
  };

  NetCall nc = [&] (const NetName& name, const SessionId& s) {
    std::cout << "add net:" << name << " session:" << s << std::endl;
    std::string msg = RedisCommand::makeCommand("set player a1");
    BlockPtr p(new RedisBlock(s, msg.length()));
    p->push(msg.data(), msg.length());
    net.send(std::move(p));
  };

  NetCall ncc = [] (const NetName& name, const SessionId& s) {
    std::cout << "remove net:" << name << " session:" << s << std::endl;
  };

  signal(SIGABRT, signalHandler);
  signal(SIGINT, signalHandler);

  auto fun = [] (SessionId s, size_t len)->BlockPtr {
    return std::move(BlockPtr(new RedisBlock(s, len)));
  };
  //NetPropertyPointer s1(new NetProperty(Config("s1", "127.0.0.1", 9812, 1000, 2000), fc, nc, ncc));
  NetPropertyPointer c1(new NetProperty(Config("c1", "127.0.0.1", 6379, true), fc, nc, ncc, fun));
 // NetPropertyPointer c2(new NetProperty(Config("c2", "127.0.0.1", 9812, true), fc, nc, ncc));
  //NetPropertyPointer c3(new NetProperty(Config("c3", "127.0.0.1", 9812, true), fc, nc, ncc));
  //net.add(std::move(s1));
  net.add(std::move(c1));
  //net.add(std::move(c2));
  //net.add(std::move(c3));
  net.start();
  
  while(isRunning.load()) {
    net.poll();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  net.stop(5);

  printf("exit");
  return 1;
}