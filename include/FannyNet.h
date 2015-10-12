/********************************************************************

  Filename:   FannyNet

  Description:FannyNet

  Version:  1.0
  Created:  12:10:2015   11:05
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_FannyNet_H__
#define __FANNY_FannyNet_H__
#include "NetConfig.h"
#include "NetMessage.h"
namespace FannyNet {

  typedef std::unique_ptr<NetBlockBase> BlockPtr;
  typedef NetBlockBase::BufferPtr BufferPtr;
  typedef std::function<void(const NetName&, const BlockPtr&)> FunCall;
  typedef std::function<void(const NetName&, const SessionId&)> NetCall;
  typedef std::function<BlockPtr(const SessionId&)> FunMakeBlock;
  class NetThread;
  class NetSession;

  typedef std::shared_ptr<NetThread> ThreadPtr;
  typedef std::shared_ptr<NetSession> SessionPtr;
  typedef std::weak_ptr<NetThread> ThreadWeakPtr;

  class FINNY_NET_API ServiceData {
    explicit ServiceData(const Config& c, FunCall fc, NetCall fct, NetCall fcl, FunMakeBlock fb = nullptr)
    : _config(c)
    , _funCall(fc)
    , _funConnect(fct)
    , _funClose(fcl)
    , _funBlock(fb){
      // use default net block call
      if(nullptr == _funBlock) {
        _funBlock = [] (const SessionId& id)-> BlockPtr {
          return std::move(BlockPtr(new NetBlock(id)));
        }; 
      }
    }
    const Config& config() const { return _config; }
    const FunCall& connectFun() const{return _funConnect;}
    const FunCall& closeFun() const { return _funClose; }
    const FunMakeBlock& makeBlockFun() const { return _funBlock; }
  protected:
    Config _config;
    FunCall _funCall;
    NetCall _funConnect;
    NetCall _funClose;
    FunMakeBlock _funBlock;
  };
  typedef std::unique_ptr<ServiceData> NetPointer;
}
#endif