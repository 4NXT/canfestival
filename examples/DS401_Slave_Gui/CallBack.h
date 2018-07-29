#ifndef _CANFESTIVAL_EXAMPLES_DS401_SLAVE_GUI_CALLBACK_H
#define _CANFESTIVAL_EXAMPLES_DS401_SLAVE_GUI_CALLBACK_H

extern "C"
{
#include "ObjDict.h"
}

void Call_heartbeatError (CO_Data * d, UNS8);

UNS8 Call_canSend (Message *);

void Call_initialisation (CO_Data * d);
void Call_preOperational (CO_Data * d);
void Call_operational (CO_Data * d);
void Call_stopped (CO_Data * d);

void Call_post_sync (CO_Data * d);
void Call_post_TPDO (CO_Data * d);
void Call_storeODSubIndex (CO_Data * d, UNS16 wIndex, UNS8 bSubindex);

#endif // _CANFESTIVAL_EXAMPLES_DS401_SLAVE_GUI_CALLBACK_H
