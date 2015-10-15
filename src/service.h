/********************************************************************

  Filename:   NetService

  Description:NetService

  Version:  1.0
  Created:  12:10:2015   12:52
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_NetService_H__
#define __FANNY_NetService_H__
#include "fannynet.h"
#include "io_pool.h"
namespace FannyNet {
  class NetService
  {
  public:
    explicit NetService(size_t t);
    virtual ~NetService();
    //开始服务
    bool start();

    //关闭网络
    void stop();

    //增加网络
    bool add(NetPropertyPointer p);

    //移除一个网络
    bool remove(const NetName& name);

    //获取消息和状态改变
    bool poll();

    //发送消息
    bool send( BlockPtr msg);

    //发送消息
    bool send( const NetName& name,  BlockPtr msg);

    //kick
    bool kick(const SessionId& id);

    //是否在run
    bool isRun() const;
  protected:
    IoObjectPoolPtr m_ioPool;
    std::map<NetName, NetObjectPtr> m_nets;
  private:
    NetService(const NetService&) = delete;
    NetService& operator = (const NetService&) = delete;
    NetService& operator = (NetService&&) = delete;
  };
}
#endif