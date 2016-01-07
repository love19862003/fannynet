/********************************************************************

  Filename:   redisblock

  Description:redisblock

  Version:  1.0
  Created:  16:10:2015   9:56
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_redisblock_H__
#define __FANNY_redisblock_H__
#include "net_buffer.h"
#include <boost/optional.hpp>
#include <string>
#include <vector>
#include "redisparser.h"
namespace FannyNet {

  enum reply_t : unsigned int {
    no_reply,
    status_code_reply,
    error_reply,
    int_reply,
    bulk_reply,
    multi_bulk_reply
  };
  struct RedisCommand {
    static std::string makeCommand(const std::string& cmd, const std::vector<std::string>& args = { });
  };

  class RedisBlock : public NetBlockBase {
  public:
    RedisBlock(SessionId s, size_t size);
    virtual ~RedisBlock();

    
    virtual bool push(const char* data, size_t len) override;
    virtual BlockPtr clone(SessionId) override;
    virtual bool recv(BufferPtr& buf) override;

    reply_t  type() const { return m_type; }
    void check_reply_t(reply_t reply_type) const;
    const std::string & get_status_code_reply() const;
    const std::string & get_error_reply() const;
    int get_int_reply() const;
    const std::string & get_bulk_reply() const;
    const std::vector<std::string> & get_multi_bulk_reply() const;
    const std::vector<std::string> & debug() const ;
    void makeRedisBuffer();
  protected:
    boost::optional<std::string> readline(BufferPtr& buf);
    boost::optional<std::string> readLen(BufferPtr& buf, size_t len);
  private:
    std::string m_temp;
    reply_t m_type = no_reply;
    size_t m_mutSize = 0;
    size_t m_bufSize = 0;
    size_t m_recvSize = 0;
    std::vector<std::string> m_recvList;
    RedisBlock(const RedisBlock&) = delete;
    RedisBlock& operator = (const RedisBlock&) = delete;
    RedisBlock& operator = (RedisBlock&&) = delete;
  };

  class RedisBlock2 : public NetBlockBase {
  public:
    RedisBlock2(SessionId s, size_t size) :NetBlockBase(s, size) {

    }
    virtual ~RedisBlock2() { }

    virtual bool push(const char* data, size_t len) override {
      return m_data->tailData(data, len);
    }
    virtual BlockPtr clone(SessionId s) override {
      auto* p = new RedisBlock2(s, 0);
      p->m_data = std::move(BufferPtr(new NetBuffer(m_data->maxLength())));
      p->m_data->tailData(m_data->data(), m_data->length());
      p->m_state = m_state;
      p->m_parse = m_parse;
      return std::move(BlockPtr(p));
    }
    virtual bool recv(BufferPtr& buf) override {
      size_t readPos = 0;
      bool r = m_parse.parse(buf->readData(), buf->needReadLength(), readPos);
      buf->readData(readPos);
      buf->clearReadBuffer();
      if(m_parse.isDone()) {
        m_state = _STATE_DONE_;
      } else {
        m_state = _STATE_BODY_;
      }
      return r;
    }
    virtual void debugStr() const override {
      m_parse.debug();
    }
  protected:
  private:
    RedisParseSpace::RedisParse m_parse;
    RedisBlock2(const RedisBlock2&) = delete;
    RedisBlock2& operator = (const RedisBlock2&) = delete;
    RedisBlock2& operator = (RedisBlock2&&) = delete;
  };
}
#endif