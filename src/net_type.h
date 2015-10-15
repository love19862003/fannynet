/********************************************************************

  Filename:   net_type

  Description:net_type

  Version:  1.0
  Created:  14:10:2015   10:10
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_net_type_H__
#define __FANNY_net_type_H__
#include <memory>
#include <functional>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
namespace FannyNet {
  class Connection;
  typedef std::shared_ptr<Connection> ConnectPtr;
  typedef std::weak_ptr<Connection> ConnectWeakPtr;
  class TcpObj;
  typedef std::unique_ptr<TcpObj> NetObjectPtr;
  class IoObject;
  typedef std::unique_ptr<IoObject> IoObjectPtr;
  class IoObjectPool;
  typedef std::unique_ptr<IoObjectPool> IoObjectPoolPtr;
 
  typedef boost::asio::ip::tcp::socket NetSocket;
  typedef std::unique_ptr<NetSocket> SocketPtr;
  typedef boost::asio::io_service NetIOService;
  typedef boost::asio::io_service::work NetWorkType;
  typedef boost::system::error_code NetErrorType;
  typedef boost::asio::ip::tcp::acceptor NetAcceptor;
  typedef boost::asio::ip::tcp::endpoint EndPointType;
  typedef std::unique_ptr<NetAcceptor> AcceptorPtr;
  typedef std::function<void()> TcpEvent;  //网络事件回调

  // connection property
  struct ConnectionProperty {
    typedef std::function<void(BlockPtr)> PostMsgFun;
    typedef std::function<void(ConnectPtr)> PostStateFun;
    typedef std::function<void(ConnectPtr )> IOEventPostFun;
    typedef std::function<SocketPtr()> SocketCreateFun;
    typedef std::function<void()> PostFaildFun;

    explicit ConnectionProperty(NetIOService& io) :_ioService(io){
    
    }
    NetIOService&     _ioService;               //io

    PostMsgFun        _eventPostRecv;           //投递消息事件
    PostStateFun      _eventConnectFun;         //投递连接成功事件
    PostStateFun      _eventCloseFun;           //投递连接关闭事件
    PostFaildFun      _eventConnectFailedFun;   //投递连接失败

    FunMakeBlock      _blockMakerFun;

  };
}
#endif