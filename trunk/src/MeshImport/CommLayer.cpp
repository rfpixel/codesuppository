#include "CommLayer.h"
#include "common/snippets/WinMsg.h"
#include "common/snippets/Telnet.h"
#include "common/snippets/UserMemAlloc.h"

#pragma warning(disable:4100 4996)

namespace MESHIMPORT
{

class MyCommLayer : public CommLayer
{
public:

  MyCommLayer(const char *address,unsigned int port)
  {
    mTelnet = TELNET::createTelnet(address,port);
    mWinMsg = 0;
    mWindowName = 0;
  }

  MyCommLayer(const char *appName,const char *destName)
  {
    mTelnet = 0;
    unsigned int slen = strlen(destName);
    mWindowName = (char *)MEMALLOC_MALLOC(slen+1);
    memcpy(mWindowName,destName,slen+1);
    mWinMsg = createWinMsg(appName);
  }

  ~MyCommLayer(void)
  {
    if ( mTelnet )
        TELNET::releaseTelnet(mTelnet);
    if ( mWindowName )
        MEMALLOC_FREE(mWindowName);
  }

  virtual bool          sendMessage(unsigned int client,const char *fmt,...) // send a message to the server, all clients (client=0) or just a specific client.
  {
	  bool ret = false;


   	  char wbuff[8192];
      wbuff[8192-1] = 0;
      _vsnprintf(wbuff,8192-1, fmt, (char *)(&fmt+1));
      if ( mTelnet )
      {
        ret = mTelnet->sendMessage(client,"%s", wbuff );
      }
      else if ( mWinMsg )
      {
        ret = mWinMsg->sendWinMsg(mWindowName,"%s",wbuff);
      }

	  return ret;
  }

  virtual const char *  receiveMessage(unsigned int &client) // receive an incoming message (client=0) means it came from the server, otherwise it designates a specific client.
  {
    const char *ret = 0;
    client = 0;

    if ( mTelnet )
    {
        ret = mTelnet->receiveMessage(client);
    }
    else if ( mWinMsg )
    {
        ret = mWinMsg->receiveWindowsMessage();
    }

    return ret;
  }

  virtual const char ** getArgs(const char *input,unsigned int &argc) // parse string into a series of arguments.
  {
    const char **ret = 0;
    argc = 0;

    if ( mTelnet )
    {
        ret = mTelnet->getArgs(input,argc);
    }
    else if ( mWinMsg )
    {
        ret = mWinMsg->getArgs(input,argc);
    }

    return ret;
  }

  virtual bool          sendBlob(unsigned int client,const char *blobType,const void *data,unsigned int dlen)
  {
    bool ret = false;

    if ( mTelnet )
    {
        ret = mTelnet->sendBlob(client,blobType,data,dlen);
    }
    else if ( mWinMsg )
    {
        ret = mWinMsg->sendWinMsgBinary(mWindowName,blobType,data,dlen);
    }

    return ret;
  }

  virtual const char *  receiveBlob(unsigned int &client,const void *&data,unsigned int &dlen)
  {
    const char *ret = 0;
    client = 0;
    data = 0;
    dlen = 0;

    if ( mTelnet )
    {
        ret = mTelnet->receiveBlob(client,data,dlen);
    }
    else if ( mWinMsg )
    {
        ret = mWinMsg->receiveWindowsMessageBlob(data,dlen);
    }

    return ret;
  }

private:
  char              *mWindowName;
  TELNET::Telnet    *mTelnet;
  WinMsg            *mWinMsg;
};

CommLayer *      CreateCommLayerTelent(const char *address,unsigned int port)
{
    MyCommLayer *mc = MEMALLOC_NEW(MyCommLayer)(address,port);
    return static_cast< CommLayer *>(mc);
}

CommLayer *      CreateCommLayerWindowsMessage(const char *appName,const char *destWindow)
{
    MyCommLayer *mc = MEMALLOC_NEW(MyCommLayer)(appName,destWindow);
    return static_cast< CommLayer *>(mc);
}

void             ReleaseCommLayer(CommLayer *t)
{
    MyCommLayer *mcl = static_cast< MyCommLayer *>(t);
    MEMALLOC_DELETE(MyCommLayer,mcl);
}


};

