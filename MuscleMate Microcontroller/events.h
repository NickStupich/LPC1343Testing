/* Active events.*/
extern unsigned int eventToProcess;

/* Processes events in the queue.  Processes up to 1 of each event */
void ProcessEvents(void);

/* Adds an event to the queue.  If the event is already raised, it waits for it to complete */
void AddEvent(unsigned int eventNum, unsigned int data);
