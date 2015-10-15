/********************************************************************

  Filename:   block

  Description:block

  Version:  1.0
  Created:  14:10:2015   16:27
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_block_H__
#define __FANNY_block_H__
#include "net_buffer.h"
#include "fannynet.h"
namespace FannyNet {
  class NetBlock : public NetBlockBase {
  public:
    struct head {
      typedef unsigned int head_len_type;
      typedef unsigned short head_mask_type;
      typedef unsigned int  head_check_type;

      head_len_type   _len;     //message stream len
      head_mask_type  _mask;    //message mask
      head_check_type _check;   //check msg valid
    };
    typedef std::function<void(NetBlock*)> DecryptEncryptFunType; // decrypt or encrypt function . you can use nullptr or you self
    // constructor for send 
    explicit NetBlock(SessionId s, size_t size);
    explicit NetBlock(SessionId s);
    virtual ~NetBlock();
    virtual BlockPtr clone(SessionId) override;
    virtual bool recv(BufferPtr& buf) override;
    virtual bool push(const char* data, size_t len) override;
  private:
    // encrypt send message
    void encrypt(const DecryptEncryptFunType& fun);
    // read recv head
    bool recvHead(BufferPtr& buf, const DecryptEncryptFunType& fun);
    // read recv body
    bool recvBody(BufferPtr& buf, const DecryptEncryptFunType& fun);
  private:
    NetBlock(const NetBlock&) = delete;
    NetBlock& operator = (const NetBlock&) = delete;
    NetBlock& operator = (NetBlock&&) = delete;
    head m_head;
  };
}
#endif