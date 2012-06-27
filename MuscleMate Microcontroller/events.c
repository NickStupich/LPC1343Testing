#include "events.h"
#include "settings.h"
#include "coreFunctions.h"

unsigned int eventToProcess = 0;
unsigned int eventDatas[EVENT_PROCESS_NUM_CMDS];

void ProcessEvents(void)
{
	if(eventToProcess & EVENT_PROCESS_UART_MASK)
	{
		//process the 'event'
		ProcessUartCommand(eventDatas[EVENT_PROCESS_UART_CMD]);
		
		//clear the 'event'
		eventToProcess &= ~EVENT_PROCESS_UART_MASK;
	}
}


void AddEvent(unsigned int eventNum, unsigned int data)
{
	//make sure there isn't another command waiting to be processed, or wait for it
	while(eventToProcess & EVENT_CMD_TO_MASK(eventNum));
	
	//store data about the event
	eventDatas[eventNum] = data;
	
	//set the event for the ProcessEvent func
	eventToProcess |= EVENT_CMD_TO_MASK(eventNum);
}
