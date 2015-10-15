/********************************************************************

  Filename:   NetManager

  Description:NetManager

  Version:  1.0
  Created:  31:3:2015   19:14
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __NetManager_H__
#define __NetManager_H__
#include <memory>
#include "fannynet.h"
namespace FannyNet {
    class NetService;
    class FINNY_NET_API NetManager {
    public:

      explicit NetManager(unsigned int t);
      virtual ~NetManager();
      //��ʼ����
      bool start();

      //���ùر�����
      void setStop();

      //��������
      bool add(NetPropertyPointer p);

      //�Ƴ�һ������
      bool remove(const NetName& name);

      //��ȡ��Ϣ��״̬�ı�
      bool poll();

      //������Ϣ
      bool send( BlockPtr msg);

      //������Ϣ
      bool send(const NetName& name,  BlockPtr msg);

      //kick
      bool kick(const SessionId& id);

      //�Ƿ���run
      bool isRun() const;

    public:
    protected:
    private:
      NetManager(const NetManager&) = delete;
      NetManager& operator = (const NetManager&) = delete;
      NetManager& operator = (NetManager&&) = delete;
      std::unique_ptr<NetService> m_service;
    };
  }
#endif