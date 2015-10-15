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
    //��ʼ����
    bool start();

    //�ر�����
    void stop();

    //��������
    bool add(NetPropertyPointer p);

    //�Ƴ�һ������
    bool remove(const NetName& name);

    //��ȡ��Ϣ��״̬�ı�
    bool poll();

    //������Ϣ
    bool send( BlockPtr msg);

    //������Ϣ
    bool send( const NetName& name,  BlockPtr msg);

    //kick
    bool kick(const SessionId& id);

    //�Ƿ���run
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