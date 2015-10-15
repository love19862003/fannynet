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
using namespace  FannyNet;

static std::atomic<bool> isRunning(true);
static std::function<void()> quitFunction = nullptr;
void signalHandler(int /*sig*/) {
  isRunning = false;
}

int main() {
  NetManager net(3);
  FunCall fc = [&] (const NetName& name, const BlockPtr& ptr) { 
    std::cout << "handle net:" << name << 
              " session:" << ptr->session() 
              << " msg:" << std::string(ptr->data()->data(), ptr->data()->length()) << std::endl;
    std::string msg = "hello........";
    BlockPtr p = NetBlockBase::sMakeDefaultBlock(ptr->session(), msg.length());
    p->push(msg.data(), msg.length());
    net.send(std::move(p));
  };

  NetCall nc = [&] (const NetName& name, const SessionId& s) {
    std::cout << "add net:" << name << " session:" << s << std::endl;
    std::string msg = "hello........";
    BlockPtr p = NetBlockBase::sMakeDefaultBlock(s, msg.length());
    p->push(msg.data(), msg.length());
    net.send(std::move(p));
  };

  NetCall ncc = [] (const NetName& name, const SessionId& s) {
    std::cout << "remove net:" << name << " session:" << s << std::endl;
  };

  signal(SIGABRT, signalHandler);
  signal(SIGINT, signalHandler);

  NetPropertyPointer s1(new NetProperty(Config("s1", "127.0.0.1", 9812, 1000, 2000), fc, nc, ncc));
  NetPropertyPointer c1(new NetProperty(Config("c1", "127.0.0.1", 9812, true), fc, nc, ncc));
  NetPropertyPointer c2(new NetProperty(Config("c2", "127.0.0.1", 9812, true), fc, nc, ncc));
  NetPropertyPointer c3(new NetProperty(Config("c3", "127.0.0.1", 9812, true), fc, nc, ncc));
  net.add(std::move(s1));
  net.add(std::move(c1));
  net.add(std::move(c2));
  net.add(std::move(c3));
  net.start();
  while(isRunning.load()) {
    net.poll();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  clock_t currentTick = clock();
  auto tick = [&] ()->std::chrono::milliseconds {
    auto d = (double)(clock() - currentTick) / CLOCKS_PER_SEC * 1000;
    return std::chrono::milliseconds(static_cast<long>(d));
  };

  do {
    net.poll();
  } while(tick().count()< 3000);

  net.stop();
  return 1;
}