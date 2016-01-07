/********************************************************************

  Filename:   net_buffer

  Description:net_buffer

  Version:  1.0
  Created:  14:10:2015   16:26
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_net_buffer_H__
#define __FANNY_net_buffer_H__
#include <algorithm>
#include <memory>
#include <functional>
#include "config.h"
namespace FannyNet {
  class FINNY_NET_API NetBuffer {
  public:
    explicit NetBuffer(size_t len);
    virtual ~NetBuffer();
    NetBuffer& operator = (NetBuffer&& buffer);

    bool isLock() const { return m_lock; }
    bool lock() { m_lock = true; return m_lock; }
    bool hasWrite() const { return length() > 0; }
    bool hasRead() const { return needReadLength() > 0; }
    bool isFull() const { return m_writePos >= m_maxLen; }
    bool canRead(size_t len) const { return len <= needReadLength(); }

    size_t length() const { return m_writePos; }
    size_t maxLength() const { return m_maxLen; }
    size_t needReadLength() const { return m_writePos - m_readPos; }

    const char* data() const { return m_buffer; }
    char* data() { return m_buffer; }

    const char* writeData() const { return m_buffer + m_writePos; }
    char* writeData() { return m_buffer + m_writePos; }
    char* writeData(size_t len);  // set  m_writePos += len && return the write pointer before add 

    const char* readData() const { return m_buffer + m_readPos; }
    char* readData() { return m_buffer + m_readPos; }
    char* readData(size_t len);   // set  m_readPos += len && return the read pointer before add 

    void clearReadBuffer();                             // clear read data
    void reset();                                       // reset buffer
    bool tailNetBuffer(const NetBuffer& buffer);        // add buffer to this tail && if space is not enough, resize it
    bool tailData(const char* base, size_t len);        // add buffer to this tail && if space is not enough, resize it

    //add a pod data to this tail && if space is not enough, resize it
    template <typename PODTYPE>
    bool tailPod(const PODTYPE& t) {
      return tailData((const char*)(&t), sizeof(PODTYPE));
    }

    bool headNetBuffer(const NetBuffer& buffer);      // add buffer to this head && if space is not enough, resize it
    bool headData(const char* base, size_t len);      // add buffer to this head && if space is not enough, resize it

    //add a pod data to this head && if space is not enough, resize it
    template <typename PODTYPE>
    bool headPod(const PODTYPE& t) {
      return headData((const char*)(&t), sizeof(PODTYPE));
    }

    // add buffer to this head, you must ensure the head is has enough space .. 
    template <typename PODTYPE>
    bool setHeadPod(const PODTYPE& t) {
      return setHead(static_cast<void*>(const_cast<PODTYPE*>(&t)), sizeof(PODTYPE));
    }

    // read data to buffer . the read len is std::min<m_writePos - m_readPos, buffer.m_maxLen - buffer.m_writePos >
    bool readBuffer(NetBuffer& buffer);

    // read a pod data 
    template <typename PODTYPE>
    bool readPod(PODTYPE& t) {
      char* p = readData(sizeof(PODTYPE));
      if(!p) { return false; }
      t = *((PODTYPE*)(p));
      return true;
    }
  protected:
  private:
    // resize  a new len if len > m_maxLen;
    bool resize(size_t len);
    // add base to this tail . you must ensure has enough space 
    bool tail(void* base, size_t len);
    // add base to this head . you must ensure has enough space
    bool head(void* base, size_t len);
    // add base to this head . you must ensure len < m_writePos
    bool setHead(void* base, size_t len);
  private:
    char* m_buffer;
    size_t m_maxLen;       // buffer space len
    size_t m_writePos;     // buffer write len
    size_t m_readPos;      // buffer read len
    bool m_lock;           // buffer is lock . if lock you can only read
    NetBuffer& operator = (const NetBuffer&) = delete;
    NetBuffer(const NetBuffer&) = delete;
  };

  class FINNY_NET_API NetBlockBase {
  public:
    typedef std::unique_ptr<NetBuffer> BufferPtr;
    typedef std::unique_ptr<NetBlockBase> BlockPtr;
    explicit NetBlockBase(SessionId s, size_t size) :m_data(nullptr), m_state(_STATE_INIT_), m_session(s) { 
      if(size > 0) { m_data = std::move(BufferPtr(new NetBuffer(s))); }
    }
    virtual ~NetBlockBase() { m_data.reset(); }

    static BlockPtr sMakeDefaultBlock(SessionId s, size_t size);

    enum State {
      _STATE_INIT_ = 1,
      _STATE_BODY_ = 2,
      _STATE_DONE_ = 3,
    };
    const BufferPtr& data() const { return m_data; }
    BufferPtr& data() { return m_data; }
    bool headDone() const { return m_state == _STATE_BODY_; }
    bool bodyDone() const { return m_state == _STATE_DONE_; }
    const SessionId& session() const { return m_session; }
    virtual bool push(const char* data, size_t len) = 0;
    virtual BlockPtr clone(SessionId) = 0;
    virtual bool recv(BufferPtr& buf) = 0;
    virtual void debugStr() const { }
  protected:
   
  protected:
    BufferPtr m_data;
    State m_state;
    SessionId m_session;
  };

  
}
#endif