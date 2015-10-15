/********************************************************************

  Filename:   fannynet

  Description:fannynet

  Version:  1.0
  Created:  14:10:2015   16:23
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_fannynet_H__
#define __FANNY_fannynet_H__
#include "config.h"
#include <string>
#include "net_buffer.h"
namespace FannyNet {
  typedef std::shared_ptr<NetBlockBase> BlockPtr;
  typedef NetBlockBase::BufferPtr BufferPtr;
  typedef std::function<void(const NetName&, const BlockPtr&)> FunCall;
  typedef std::function<void(const NetName&, const SessionId&)> NetCall;
  typedef std::function<BlockPtr(const SessionId&, size_t)> FunMakeBlock;

  class FINNY_NET_API NetProperty {
  public:
    explicit NetProperty(const Config& c, FunCall fc, NetCall fct, NetCall fcl, FunMakeBlock fb = nullptr)
      : _config(c)
      , _funCall(fc)
      , _funConnect(fct)
      , _funClose(fcl)
      , _funBlock(fb) {
      if (nullptr == _funBlock){
        _funBlock = &NetBlockBase::sMakeDefaultBlock;
      }
    }
    const Config& config() const { return _config; }
    const NetCall& connectFun() const { return _funConnect; }
    const NetCall& closeFun() const { return _funClose; }
    const FunMakeBlock& makeBlockFun() const { return _funBlock; }
    const FunCall& callFun()const { return _funCall; }
  protected:
    Config _config;
    FunCall _funCall;
    NetCall _funConnect;
    NetCall _funClose;
    FunMakeBlock _funBlock;
  };
  typedef std::unique_ptr<NetProperty> NetPropertyPointer;
}

#endif