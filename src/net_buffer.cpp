/********************************************************************

Filename:   NetMessage

Description:NetMessage

Version:  1.0
Created:  14:9:2015   10:22
Revison:  none
Compiler: gcc vc

Author:   wufan, love19862003@163.com

Organization:
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net_buffer.h"
namespace FannyNet {
  NetBuffer::NetBuffer(size_t len) {
    m_buffer = (char*)malloc(len);
    memset(m_buffer, 0, len);
    m_maxLen = len;
    m_writePos = 0;
    m_readPos = 0;
    m_lock = false;
  }
  NetBuffer::~NetBuffer() {
    free(m_buffer);
    m_buffer = nullptr;
    m_maxLen = m_writePos = m_readPos = 0;
    m_lock = false;
  }

  NetBuffer& NetBuffer::operator = (NetBuffer&& buffer) {
    free(m_buffer);
    m_buffer = buffer.m_buffer;
    m_writePos = buffer.m_writePos;
    m_readPos = buffer.m_readPos;
    m_maxLen = buffer.m_maxLen;
    m_lock = buffer.m_lock;

    buffer.m_buffer = nullptr;
    buffer.m_writePos = buffer.m_readPos = buffer.m_maxLen = 0;
    buffer.m_lock = false;
    return *this;
  }

  char*  NetBuffer::writeData(size_t len) {
    if(m_writePos + len > m_maxLen) { MYASSERT(false, "net buffer write pos error"); return nullptr; }
    char* p = writeData();
    m_writePos += len;
    return p;
  }
  char* NetBuffer::readData(size_t len) {
    MYASSERT(canRead(len), "net buffer read len error");
    if(!canRead(len)) { return nullptr; }
    char* p = readData();
    m_readPos += len;
    return p;
  }

  void NetBuffer::clearReadBuffer() {
    MYASSERT(m_readPos <= m_writePos, "net buffer m_readPos error");
    memmove(m_buffer, m_buffer + m_readPos, m_writePos - m_readPos);
    m_writePos -= m_readPos;
    m_readPos = 0;
  }
  void NetBuffer::reset() {
    if(m_lock) { MYASSERT(false, "buffer is lock"); return; }
    memset(m_buffer, 0, m_writePos);
    m_writePos = 0;
    m_readPos = 0;
  }
  bool NetBuffer::tailNetBuffer(const NetBuffer& buffer) {
    if(this == &buffer) { return false; }
    //resize(m_writePos + buffer.m_writePos);
    return tailData(buffer.data(), buffer.length());
  }
  bool NetBuffer::tailData(const char* base, size_t len) {
    resize(m_writePos + len);
    return tail(static_cast<void*>(const_cast<char*>(base)), len);
  }
  bool NetBuffer::headNetBuffer(const NetBuffer& buffer) {
    if(this == &buffer) { return false; }
    //resize(m_writePos + buffer.m_writePos);
    return headData(buffer.data(), buffer.length());
  }
  bool NetBuffer::headData(const char* base, size_t len) {
    resize(m_writePos + len);
    return head(static_cast<void*>(const_cast<char*>(base)), len);
  }
  bool NetBuffer::readBuffer(NetBuffer& buffer) {
    if(this == &buffer) { return false; }
    if(buffer.isFull()) { return false; }
    if(!hasWrite()) { return false; }
    size_t len = std::min<size_t>(m_writePos - m_readPos, buffer.maxLength() - buffer.m_writePos);
    char* p = readData(len);
    if(!p) { return false; }
    bool result = buffer.tail(p, len);
    return result;
  }

  bool NetBuffer::resize(size_t len) {
    if(m_lock) { MYASSERT(false, "buffer is lock"); return false; }
    if(len > m_maxLen) {
      m_maxLen = len * 2;
      char* newBuffer = (char*)malloc(m_maxLen);
      memset(newBuffer, 0, m_maxLen);
      memcpy(newBuffer, m_buffer, m_writePos);
      free(m_buffer);
      m_buffer = newBuffer;
    }
    return true;
  }
  bool NetBuffer::tail(void* base, size_t len) {
    if(m_lock) { MYASSERT(false, "buffer is lock"); return false; }
    if(len <= 0) { return false; }
    MYASSERT(len + m_writePos <= m_maxLen, "buffer not has enough space");
    memcpy(m_buffer + m_writePos, base, len);
    m_writePos += len;
    return true;
  }
  bool NetBuffer::head(void* base, size_t len) {
    if(m_lock) { MYASSERT(false, "buffer is lock"); return false; }
    if(len <= 0) { return false; }
    MYASSERT(len + m_writePos <= m_maxLen, "buffer not has enough space");
    memmove(m_buffer + len, m_buffer, m_writePos);
    memcpy(m_buffer, base, len);
    m_writePos += len;
    return true;
  }
  bool NetBuffer::setHead(void* base, size_t len) {
    if(m_lock) { MYASSERT(false, "buffer is lock"); return false; }
    MYASSERT(len <= m_writePos, "buffer not has enough space in head");
    if(len <= m_writePos) {
      memcpy(m_buffer, base, len);
      return true;
    }
    return false;
  }
}