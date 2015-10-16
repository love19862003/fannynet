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
#include <vector>
#include "redisblock.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace {
 
  const std::string  REDIS_LBR("\r\n");
  const std::string  REDIS_STATUS_REPLY_OK("OK");
  const std::string  REDIS_PREFIX_STATUS_REPLY_ERROR("-ERR ");
  const std::string  REDIS_PREFIX_STATUS_REPLY_ERR_C("-");
  const std::string  REDIS_PREFIX_STATUS_REPLY_VALUE("+");
  const std::string  REDIS_PREFIX_SINGLE_BULK_REPLY("$");
  const std::string  REDIS_PREFIX_MULTI_BULK_REPLY("*");
  const std::string  REDIS_PREFIX_INT_REPLY(":");
  const std::string  REDIS_PREFIX_LBR("\n");
  const std::string  REDIS_WHITESPACE(" \f\n\r\t\v");
  const std::string  REDIS_MISSING("**nonexistent-key**");
}
namespace FannyNet {
  inline reply_t getReplyType(const std::string& str) {
    if(0 == str.find_first_of(REDIS_PREFIX_STATUS_REPLY_ERR_C)) {
      return  error_reply;
    }
    if(0 == str.find_first_of(REDIS_PREFIX_STATUS_REPLY_VALUE)) {
      return  status_code_reply;
    }
    if(0 == str.find_first_of(REDIS_PREFIX_SINGLE_BULK_REPLY)) {
      return  bulk_reply;
    }
    if(0 == str.find_first_of(REDIS_PREFIX_INT_REPLY)) {
      return  int_reply;
    }
    if(0 == str.find_first_of(REDIS_PREFIX_MULTI_BULK_REPLY)) {
      return  multi_bulk_reply;
    }
    return no_reply;
  }

  inline std::string & rtrim(std::string & str, const std::string & ws = REDIS_WHITESPACE) {
    std::string::size_type pos = str.find_last_not_of(ws);
    str.erase(pos + 1);
    return str;
  }

  inline size_t split(const std::string & str, const std::string&delim, std::vector<std::string> & elems) {
    boost::split(elems, str, boost::is_any_of(delim), boost::token_compress_on);
    for (auto& s : elems){ s = rtrim(s, REDIS_WHITESPACE);}
    return elems.size();
  }
  inline void split_lines(const std::string & str, std::vector<std::string> & elems) {
    split(str, REDIS_PREFIX_LBR , elems);
    for(std::vector<std::string>::iterator it = elems.begin(); it != elems.end(); ++it)
      rtrim(*it);
  }

  RedisBlock::RedisBlock(SessionId s, size_t size) : NetBlockBase(s, size) {

  }
  RedisBlock::~RedisBlock() {

  }
  void RedisBlock::check_reply_t(reply_t reply_type) const {

    if(m_type != reply_type || !bodyDone())
      throw std::runtime_error("invalid reply type");
  }

  const std::string & RedisBlock::get_status_code_reply() const {
    check_reply_t(status_code_reply);
    return m_recvList[0];
  }

  const std::string & RedisBlock::get_error_reply() const {
    check_reply_t(error_reply);
    return m_recvList[0];
  }

  int RedisBlock::get_int_reply() const {
    check_reply_t(int_reply);
    return boost::lexical_cast<int>(m_recvList[0]);
  }

  const std::string & RedisBlock::get_bulk_reply() const {
    check_reply_t(bulk_reply);
    return m_recvList[0];
  }

  const std::vector<std::string> & RedisBlock::get_multi_bulk_reply() const {
    check_reply_t(multi_bulk_reply);
    return m_recvList;
  }
  const std::vector<std::string> & RedisBlock::debug() const {
    return m_recvList;
  }

  void RedisBlock::makeRedisBuffer() {
    if(0 == m_recvSize) { m_recvSize = 1; }
    m_data.reset(new NetBuffer(m_recvSize));
    m_data->tailPod<reply_t>(m_type);
    switch(m_type) {
    case FannyNet::no_reply:
      break;
    case FannyNet::status_code_reply:{
      auto& str = get_status_code_reply();
      m_data->tailData(str.c_str(), str.length()); 
    }
      break;
    case FannyNet::error_reply:  {
      auto& str = get_error_reply();
      m_data->tailData(str.c_str(), str.length());
    }
      break;
    case FannyNet::int_reply:  {
      int v = get_int_reply();
      m_data->tailPod<int>(v);
    }
      break;
    case FannyNet::bulk_reply: {
      auto& str = get_bulk_reply();
      m_data->tailData(str.c_str(), str.length());
    }
      break;
    case FannyNet::multi_bulk_reply:  {
      m_data->tailPod<int>(static_cast<int>(m_recvList.size()));
      for(auto& v : m_recvList) {
        m_data->tailPod<int>(static_cast<int>(v.length()));
        m_data->tailData(v.data(), v.length());
      }
    }
      break;
    default:
      break;
    }
  }
  std::string RedisCommand::makeCommand(const std::string& cmd, const std::vector<std::string>& args) {
    std::string cmdd = cmd;
    boost::algorithm::to_upper(cmdd);
    std::ostringstream oss;
    oss << REDIS_PREFIX_MULTI_BULK_REPLY << args.size() + 1 << REDIS_LBR;
    oss << REDIS_PREFIX_SINGLE_BULK_REPLY << cmdd.size() << REDIS_LBR;
    oss << cmdd << REDIS_LBR;
    for(auto& param : args) {
      oss << REDIS_PREFIX_SINGLE_BULK_REPLY << param.size() << REDIS_LBR;
      oss << param << REDIS_LBR;
    }
    return oss.str();
  }

  bool RedisBlock::push(const char* data, size_t len) {
    return m_data->tailData(data, len);
  }
  NetBlockBase::BlockPtr RedisBlock::clone(SessionId s) {
    auto* p = new RedisBlock(s, 0);
    p->m_data = std::move(BufferPtr(new NetBuffer(m_data->maxLength())));
    p->m_data->tailData(m_data->data(), m_data->length());
    return std::move(BlockPtr(p));
  }
  bool RedisBlock::recv(BufferPtr& buf) {
    if(no_reply == m_type) { 
      auto line = readline(buf);
      if(!line) { return false; }
      std::string& str = line.get();
      m_type = getReplyType(str);
      switch(m_type) {
      case FannyNet::no_reply:  {
        assert(false);
        m_state = _STATE_DONE_;
        m_recvSize = sizeof(reply_t);
        makeRedisBuffer();
        return true; // error
      }
        break;
      case FannyNet::status_code_reply: {
        m_recvList.push_back(str.substr(1));
        m_state = _STATE_DONE_;
        m_recvSize = sizeof(reply_t) + m_recvList[0].size();
        makeRedisBuffer();
        return true;
      }
        break;
      case FannyNet::error_reply:{
        m_recvList.push_back(str.substr(4));
        m_recvSize = sizeof(reply_t) + m_recvList[0].size();
        m_state = _STATE_DONE_;
        makeRedisBuffer();
        return true;
      }
        break;
      case FannyNet::int_reply: {
        m_recvList.push_back(str.substr(1));
        m_recvSize = sizeof(reply_t) + sizeof(int);
        m_state = _STATE_DONE_;
        makeRedisBuffer();
        return true;
      }
        break;
      case FannyNet::bulk_reply: {
        m_bufSize = 0;
        int bufSize = boost::lexical_cast<int>(str.substr(1));
        m_recvSize = sizeof(reply_t);
        if(bufSize == -1) {
          m_recvList.push_back(REDIS_MISSING);
          m_recvSize += REDIS_MISSING.length();
          m_state = _STATE_DONE_;
          makeRedisBuffer();
          return true;
        }
        m_bufSize = bufSize + 2;
        m_state = _STATE_BODY_; 
      }
        break;
      case FannyNet::multi_bulk_reply:{
        m_state = _STATE_BODY_;
        m_mutSize = 0;
        int muSize = boost::lexical_cast<int>(str.substr(1));
        m_recvSize = sizeof(reply_t) + sizeof(int);
        if(muSize == -1) {
          m_state = _STATE_DONE_;
          makeRedisBuffer();
          return true;
        }
        m_mutSize = muSize;
      }
        break;
      }
    }

    if(bulk_reply == m_type) {
      auto b = readLen(buf, m_bufSize);
      if(!b) { return false; }
      b->erase(m_bufSize - 2);
      m_recvSize += b->length();
      m_recvList.push_back(b.get());
      m_state = _STATE_DONE_;
      makeRedisBuffer();
      return true;
    } else if(multi_bulk_reply == m_type) {
      do {
        auto lline = readline(buf);
        if(!lline) { return false; }
        m_state = _STATE_BODY_;
        const std::string& ssub = lline.get().substr(1);
        m_bufSize = 0;
        int bufSize = boost::lexical_cast<int>(ssub);
        if(bufSize == -1) {
          m_recvList.push_back(REDIS_MISSING);
          m_recvSize += REDIS_MISSING.length();
          m_recvSize += sizeof(int);
          if (m_recvList.size() == m_mutSize){
            m_state = _STATE_DONE_;
            makeRedisBuffer();
            return true;
          }
          continue;
        } else {
          m_bufSize = bufSize;
          m_bufSize += 2;//CRLF
          auto bb = readLen(buf, m_bufSize);
          if(!bb) { return false; }
          bb->erase(m_bufSize - 2);
          m_recvSize += sizeof(int);
          m_recvSize += bb->length();
          m_recvList.push_back(bb.get());
          if(m_recvList.size() == m_mutSize) {
            makeRedisBuffer();
            m_state = _STATE_DONE_;
            return true;
          }
        }
      } while(m_recvList.size() == m_mutSize);

    } else { assert(false); }
    return false;
  }
  boost::optional<std::string> RedisBlock::readLen(BufferPtr& buf, size_t len){
    size_t toRead = buf->needReadLength();
    size_t need = len - m_temp.length();
    bool ok = false;
    if(need <= toRead) { toRead = need; ok = true; }
    m_temp.append(buf->readData(toRead), toRead);
    buf->clearReadBuffer();
    if(ok) {
      boost::optional<std::string> result(m_temp);
      m_temp.clear();
      return result;
    }
    return nullptr;
  }
  boost::optional<std::string> RedisBlock::readline(BufferPtr& buf) {
    size_t toRead = buf->needReadLength();
    bool ok = false;
    char * eol = static_cast<char*>(memchr(buf->readData(), REDIS_PREFIX_LBR[0], buf->needReadLength()));
    if(eol) { 
      toRead = eol - buf->readData() + 1; 
      ok = true; 
    }
    m_temp.append(buf->readData(toRead), toRead);
    buf->clearReadBuffer();
    if(ok) {
      boost::optional<std::string> result(rtrim(m_temp, REDIS_LBR));
      m_temp.clear();
      return result;
    }
    return nullptr;
  }
}