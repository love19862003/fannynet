/********************************************************************

  Filename:   block

  Description:block

  Version:  1.0
  Created:  14:10:2015   16:30
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#include "block.h"
#include "config.h"
namespace FannyNet {
  NetBlockBase::BlockPtr NetBlock::clone(SessionId s) {
    auto* p = new NetBlock(s);
    p->m_head = m_head;
    p->m_state = m_state;
    p->m_data = std::move(BufferPtr(new NetBuffer(m_data->maxLength())));
    p->m_data->tailData(m_data->data(), m_data->length());
    return std::move(BlockPtr(p));
  }
  NetBlock::NetBlock(SessionId s) : NetBlockBase(s, 0) {
    memset(&m_head, 0, sizeof(head));
  }
  NetBlock::NetBlock(SessionId s, size_t size) : NetBlockBase(s, size > 0 ? size + sizeof(head) : 0) {
    memset(&m_head, 0, sizeof(head));
    if(m_data) { m_data->tailPod<head>(m_head); }
  }

  NetBlock::~NetBlock() { }

  bool NetBlock::push(const char* data, size_t len) {
    if(m_data) {
      m_data->tailData(data, len);
      encrypt(nullptr);
    }
    return true;
  }
  bool NetBlock::recv(BufferPtr& buf) {
    MYASSERT(nullptr != buf, "the recv buffer is null");
    if(!buf) { return false; }
    return recvHead(buf, nullptr);
  }

  void NetBlock::encrypt(const DecryptEncryptFunType& fun) {
    m_head._len = static_cast<head::head_len_type>(m_data->length());
    m_head._mask = 0;
    m_head._check = 0;/*Utility::makeCRC32(m_data->data() + sizeof(head), m_head._len - sizeof(head))*/;
    if(fun) { fun(this); }
    m_data->setHeadPod(m_head);
    //m_data->lock();
  }

  bool NetBlock::recvHead(BufferPtr& buf, const DecryptEncryptFunType& fun) {
    if(_STATE_INIT_ == m_state) {
      if(buf->readPod<head>(m_head)) {
        buf->clearReadBuffer();
        m_state = _STATE_BODY_;
        return recvBody(buf, fun);
      } else {
        return false;
      }
    }
    return recvBody(buf, fun);
  }

  bool NetBlock::recvBody(BufferPtr& buf, const DecryptEncryptFunType& fun) {
    if(_STATE_BODY_ == m_state) {
      size_t len = m_head._len - sizeof(head);
      if(!m_data) { m_data = BufferPtr(new NetBuffer(len)); }
      buf->readBuffer(*m_data);
      buf->clearReadBuffer();
      if(m_data->isFull()) {
        m_state = _STATE_DONE_;
        if(fun) { fun(this); }
        m_data->lock();
        return  m_head._check == 0;/* Utility::makeCRC32(m_data->data(), m_data->length());*/
      }
    }
    return false;
  }

  NetBlockBase::BlockPtr NetBlockBase::sMakeDefaultBlock(SessionId s, size_t size) {
    return std::move(BlockPtr(new NetBlock(s, size)));
  }
}