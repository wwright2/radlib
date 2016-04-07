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

/*  ... System include files
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

/*  ... radlib include files
*/
#include <radsysdefs.h>
#include <radsemaphores.h>
#include <radbuffers.h>
#include <radqueue.h>
#include <radtimers.h>
#include <radevents.h>
#include <radtimeUtils.h>
#include <radprocess.h>
#include <radstates.h>

/*  ... Local include files
*/


/*  ... process definitions
*/
//  ... this is the unique ID for the "template" radlib system
#define TEMPLATE_SYSTEM_ID          0x01

//  ... common macros for radlib process creation and use
#define PROC_NAME_TEMPLATE          "templated"
#define TEMPLATE_LOCK_FILENAME      "template.pid"
#define TEMPLATE_CONFIG_FILENAME    "template.conf"
#define PROC_NUM_TIMERS_TEMPLATE    4



/*  !!!!!!!!!!!!!!!!!!!!!!!  HIDDEN, NOT FOR API USE  !!!!!!!!!!!!!!!!!!!!!!!
*/

enum ProcessTimers
{
    TEMPLATE_TIMER_NUM1         = 1,
    TEMPLATE_TIMER_NUM2         = 2
};


//  ... the "template" process work area
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


//  ... define the states for the state machine
typedef enum
{
    TEMPLATE_STATE_IDLE         = 0,
    TEMPLATE_STATE_RUN,
    TEMPLATE_STATE_ERROR
} TEMPLATE_STATES;


//  ... prototype the state handlers
extern int templateIdleState (int state, void *stimulus, void *data);
extern int templateRunState (int state, void *stimulus, void *data);
extern int templateErrorState (int state, void *stimulus, void *data);

/*  !!!!!!!!!!!!!!!!!!!!!!!!!  END HIDDEN SECTION  !!!!!!!!!!!!!!!!!!!!!!!!!!
*/

// ... function prototypes



#endif

