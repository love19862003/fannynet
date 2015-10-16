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
    const std::vector<std::string> & debug() const;
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
}
#endif