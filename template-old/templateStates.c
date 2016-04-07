/*---------------------------------------------------------------------------
 
  FILENAME:
        templateStates.c
 
  PURPOSE:
        Provide the template process state machine handlers.
 
  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        01/01/2004      Your Name       0               Original
 
  NOTES:
        All references to "template" should be replaced by a meaningful
        process name, including the file names and data structures.
 
----------------------------------------------------------------------------*/

/*  ... System include files
*/
#include <termios.h>

/*  ... Library include files
*/

/*  ... Local include files
*/
#include "template.h"


/*  ... global memory declarations
*/

/*  ... global memory referenced
*/

/*  ... static (local) memory declarations
*/

/* ... methods
*/

int templateIdleState (int state, void *stimulus, void *data)
{
    STIM                *stim = (STIM *)stimulus;
    TEMPLATE_WORK       *work = (TEMPLATE_WORK *)data;
    time_t              ntime;
    struct tm           *locTime;

    switch (stim->type)
    {
    case STIM_DUMMY:
        //  ... this one starts this state machine;
        //  ... do any initialization, communication, etc. here
        
        //  ... start timer 1
        radTimerStart (work->timerNum1, 5000);      // 5 seconds

        //  ... start timer 2
        radTimerStart (work->timerNum2, 12000);     // 12 seconds

        //  ... return the new state for the machine
        return TEMPLATE_STATE_RUN;

    case STIM_QMSG:
    case STIM_EVENT:
    case STIM_TIMER:
        break;
    }

    return state;
}

int templateRunState (int state, void *stimulus, void *data)
{
    STIM                    *stim = (STIM *)stimulus;
    TEMPLATE_WORK           *work = (TEMPLATE_WORK *)data;
    char                    temp[256];
    ssize_t                 retVal;

    switch (stim->type)
    {
    case STIM_QMSG:
        break;

    case STIM_TIMER:
        //  ... process a timer expiry
        if (stim->timerNumber == TEMPLATE_TIMER_NUM1)
        {
            radMsgLog(PRI_STATUS, "Timer 1 Expiry");
            radTimerStart (work->timerNum1, 5000);
            return state;
        }
        else if (stim->timerNumber == TEMPLATE_TIMER_NUM2)
        {
            radMsgLog(PRI_STATUS, "Timer 2 Expiry");
            radTimerStart (work->timerNum2, 12000);
            return state;
        }
        break;

    case STIM_IO:
        //  ... read data from stdin
        memset (temp, 0, 256);
        retVal = read (stim->iofd, temp, 256);
        if (retVal == (ssize_t)-1)
        {
            radMsgLog(PRI_HIGH, "read error on stdin!");
            return TEMPLATE_STATE_ERROR;
        }

        radMsgLog(PRI_STATUS, "Read %d bytes from stdin: %s",
                   retVal, temp);

        //  ... return the same state that got us here (RUN)
        return state;

    case STIM_EVENT:
        break;
    }

    return state;
}

int templateErrorState (int state, void *stimulus, void *data)
{
    STIM                *stim = (STIM *)stimulus;
    TEMPLATE_WORK       *work = (TEMPLATE_WORK *)data;


    switch (stim->type)
    {
    case STIM_IO:
    case STIM_QMSG:
    case STIM_EVENT:
    case STIM_TIMER:
        radMsgLog(PRI_STATUS,
                   "%s:templateErrorState: received stimulus %d",
                   PROC_NAME_TEMPLATE,
                   stim->type);
        break;
    }

    return state;
}
