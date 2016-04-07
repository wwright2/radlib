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

// System include files
#include <termios.h>

// Library include files

// Local include files
#include "template.h"


// global memory declarations

// global memory referenced

// static (local) memory declarations

static void sendRequest (int number)
{
    TEMPLATE_MSG_USER_REQUEST   request;

    request.number = number;

    radMsgRouterMessageSend (TEMPLATE_MSGID_USER_REQUEST, &request, sizeof(request));

    return;
}

static void processResponse (TEMPLATE_MSG_USER_RESPONSE *response)
{
    printf ("Number of primes < %d is %d\n", 
            response->number, 
            response->numberOfPrimes);

    return;
}


// methods

int templateIdleState (int state, void *stimulus, void *data)
{
    STIM                *stim = (STIM *)stimulus;
    TEMPLATE_WORK       *work = (TEMPLATE_WORK *)data;
    time_t              ntime;
    struct tm           *locTime;

    switch (stim->type)
    {
    case STIM_DUMMY:
        // this one starts this state machine;
        // do any initialization, communication, etc. here
        
        // start timer 1
        radTimerStart (work->timerNum1, TEMPLATE_TIMER1_PERIOD);

        // start timer 2
        radTimerStart (work->timerNum2, TEMPLATE_TIMER2_PERIOD);

        // return the new state for the machine
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
    int                     value;

    switch (stim->type)
    {
    case STIM_QMSG:
        if (stim->msgType == TEMPLATE_MSGID_USER_RESPONSE)
        {
            // print out the results
            processResponse ((TEMPLATE_MSG_USER_RESPONSE *)stim->msg);
        }

        break;

    case STIM_TIMER:
        //  process a timer expiry
        if (stim->timerNumber == TEMPLATE_TIMER_NUM1)
        {
            radMsgLog(PRI_STATUS, "Timer 1 Expiry");
            radTimerStart (work->timerNum1, TEMPLATE_TIMER1_PERIOD);
            return state;
        }
        else if (stim->timerNumber == TEMPLATE_TIMER_NUM2)
        {
            radMsgLog(PRI_STATUS, "Timer 2 Expiry");
            radTimerStart (work->timerNum2, TEMPLATE_TIMER2_PERIOD);
            return state;
        }
        break;

    case STIM_IO:
        // read data from stdin
        memset (temp, 0, 256);
        retVal = read (stim->iofd, temp, 256);
        if (retVal == (ssize_t)-1)
        {
            radMsgLog(PRI_HIGH, "read error on stdin!");
            return TEMPLATE_STATE_ERROR;
        }
        else if (retVal == 0)
        {
            return state;
        }

        // send a message to the template2d process to compute the primes
        value = atoi (temp);
        if (value <= 0)
        {
            printf ("Integer must be positive!!!\n");
        }
        else
        {
            sendRequest (value);
        }

        // return the same state that got us here (RUN)
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
