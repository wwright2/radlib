#ifndef INC_templateh
#define INC_templateh
/*---------------------------------------------------------------------------
 
  FILENAME:
        template.h
 
  PURPOSE:
        Provide the radlib template process definitions.
 
  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        01/01/2004      Your Name       0               Original
 
  NOTES:
        All references to "template" should be replaced by a meaningful
        process name, including the file names and data structures.
 
----------------------------------------------------------------------------*/

// System include files
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

// radlib include files
#include <radsysdefs.h>
#include <radsemaphores.h>
#include <radbuffers.h>
#include <radqueue.h>
#include <radtimers.h>
#include <radevents.h>
#include <radtimeUtils.h>
#include <radprocess.h>
#include <radstates.h>
#include <radconffile.h>
#include <radmsgRouter.h>

// Local include files


// process definitions

// this is the unique ID for the "template" radlib system
#define TEMPLATE_SYSTEM_ID          11

// common macros for radlib process creation and use
#define PROC_NAME_TEMPLATE          "templated"
#define TEMPLATE_LOCK_FILENAME      "template.pid"
#define PROC_NAME_TEMPLATE2         "template2d"
#define TEMPLATE2_LOCK_FILENAME     "template2.pid"
#define TEMPLATE_CONFIG_FILENAME    "template.conf"
#define PROC_NUM_TIMERS_TEMPLATE    4

#define TEMPLATE_TIMER1_PERIOD      15000           // 15 seconds
#define TEMPLATE_TIMER2_PERIOD      33000           // 33 seconds


typedef enum 
{
    TEMPLATE_TIMER_NUM1         = 1,
    TEMPLATE_TIMER_NUM2         = 2
} ProcessTimers;


// the "template" process work area
typedef struct
{
    pid_t           myPid;
    STATES_ID       stateMachine;
    char            fileDev[256];
    TIMER_ID        timerNum1;
    TIMER_ID        timerNum2;
    int             verboseMode;
    int             exiting;
} TEMPLATE_WORK;


// define the states for the state machine
typedef enum
{
    TEMPLATE_STATE_IDLE         = 0,
    TEMPLATE_STATE_RUN,
    TEMPLATE_STATE_ERROR
} TEMPLATE_STATES;

// define the template message IDs
enum
{
    TEMPLATE_MSGID_USER_REQUEST     = 100,
    TEMPLATE_MSGID_USER_RESPONSE    = 101
};

// define the USER_REQUEST message
typedef struct
{
    int             number;
} TEMPLATE_MSG_USER_REQUEST;

// define the USER_RESPONSE message
typedef struct
{
    int             number;
    int             numberOfPrimes;
} TEMPLATE_MSG_USER_RESPONSE;


// prototype the state handlers
extern int templateIdleState (int state, void *stimulus, void *data);
extern int templateRunState (int state, void *stimulus, void *data);
extern int templateErrorState (int state, void *stimulus, void *data);

extern int template2IdleState (int state, void *stimulus, void *data);
extern int template2RunState (int state, void *stimulus, void *data);
extern int template2ErrorState (int state, void *stimulus, void *data);


// function prototypes



#endif

