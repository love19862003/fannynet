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
#include "FannyNet.h"
namespace FannyNet {
  class NetService
  {
  public:
    //��ʼ����
    bool start();

    //�ر�����
    void stop();

    //��������
    bool add(NetPointer p);

    //�Ƴ�һ������
    bool remove(const NetName& name);

    //��ȡ��Ϣ��״̬�ı�
    bool poll();

    //������Ϣ
    bool send(const BlockPtr& msg);

    //������Ϣ
    bool send(const NetName& name, const BlockPtr& msg);

    //kick
    bool kick(const SessionId& id);

    //�Ƿ���run
    bool isRun() const;
  protected:
  private:
    NetService(const NetService&) = delete;
    NetService& operator = (const NetService&) = delete;
    NetService& operator = (NetService&&) = delete;
  };
}
#endif