/*---------------------------------------------------------------------------
 
  FILENAME:
        templateStates2.c
 
  PURPOSE:
        Provide the 2nd template process state machine handlers.
 
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

static int IsPrime (int value)
{
    int         i;

    if (value <= 3)
    {
        return TRUE;
    }

    // this isn't the best way to do this, but it works
    for (i = 2; i < value/2; i ++)
    {
        if ((value % i) == 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}

static void processRequest (TEMPLATE_MSG_USER_REQUEST *request)
{
    TEMPLATE_MSG_USER_RESPONSE      response;
    int                             i, numPrimes = 0;

    for (i = 1; i < request->number; i ++)
    {
        if (IsPrime(i))
        {
            numPrimes ++;
        }
    }

    // send the response
    response.number = request->number;
    response.numberOfPrimes = numPrimes;
    radMsgRouterMessageSend (TEMPLATE_MSGID_USER_RESPONSE, &response, sizeof(response));

    return;
}
    

// methods

int template2IdleState (int state, void *stimulus, void *data)
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
        

        // return the new state for the machine
        return TEMPLATE_STATE_RUN;

    case STIM_QMSG:
    case STIM_EVENT:
    case STIM_TIMER:
        break;
    }

    return state;
}

int template2RunState (int state, void *stimulus, void *data)
{
    STIM                    *stim = (STIM *)stimulus;
    TEMPLATE_WORK           *work = (TEMPLATE_WORK *)data;
    char                    temp[256];
    ssize_t                 retVal;

    switch (stim->type)
    {
    case STIM_QMSG:
        if (stim->msgType == TEMPLATE_MSGID_USER_REQUEST)
        {
            // process the msg, compute the primes, send the response
            processRequest ((TEMPLATE_MSG_USER_REQUEST *)stim->msg);
        }

        break;

    case STIM_TIMER:
        break;

    case STIM_IO:
        break;

    case STIM_EVENT:
        break;
    }

    return state;
}

int template2ErrorState (int state, void *stimulus, void *data)
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
