#ifndef _CANFESTIVAL_EXAMPLES_DS401_SLAVE_GUI_TESTSLAVEGUI_H
#define _CANFESTIVAL_EXAMPLES_DS401_SLAVE_GUI_TESTSLAVEGUI_H

extern "C"
{
#include "canfestival.h"
}

int main_can (s_BOARD SlaveBoard, char *LibraryPath);
void stop_slave ();

#endif // _CANFESTIVAL_EXAMPLES_DS401_SLAVE_GUI_TESTSLAVEGUI_H
