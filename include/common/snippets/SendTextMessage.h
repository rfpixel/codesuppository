#ifndef SEND_TEXT_MESSAGE_H

#define SEND_TEXT_MESSAGE_H

#include "NxSimpleTypes.h"

enum TextMessageChannel
{
  TMC_CONSOLE = 0,
  TMC_ERROR
};

class SendTextMessage
{
public:
	virtual bool         sendTextMessage(NxU32 /**priority**/,const char * /* fmt */,...) { return false; };

};

#define SEND_TEXT_MESSAGE if ( gSendTextMessage ) gSendTextMessage->sendTextMessage

extern SendTextMessage *gSendTextMessage;

#endif
