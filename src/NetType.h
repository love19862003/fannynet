/********************************************************************

  Filename:   NetType

  Description:NetType

  Version:  1.0
  Created:  13:10:2015   18:03
  Revison:  none
  Compiler: gcc vc

  Author:   wufan, love19862003@163.com

  Organization:
*********************************************************************/
#ifndef __FANNY_NetType_H__
#define __FANNY_NetType_H__
#include <memory>
namespace FannyNet {
  class NetConnection;
  typedef std::shared_ptr<NetConnection> SessionPtr;
  class TcpObj;
  typedef std::unique_ptr<TcpObj> TcpObjPtr;
  class IOPool;
  typedef std::unique_ptr<IOPool> IOPoolPtr;
}
#endif