/*---------------------------------------------------------------------------
 
  FILENAME:
        template2.c
 
  PURPOSE:
        Provide the 2nd radlib template process entry point.
 
  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        01/01/2004      Your Name       0               Original
 
  NOTES:
        All references to "template" should be replaced by a meaningful
        process name, including the file names and data structures.
 
----------------------------------------------------------------------------*/

// System include files
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// radlib include files

// Local include files
#include "template.h"


// global memory declarations

// global memory referenced

// static (local) memory declarations

// declare the process work area
static TEMPLATE_WORK    templateWork;


// methods

static void msgHandler
(
    char        *srcQueueName,
    UINT        msgType,
    void        *msg,
    UINT        length,
    void        *userData
)
{
    STIM        stim;

    stim.type           = STIM_QMSG;
    stim.srcQueueName   = srcQueueName;
    stim.msgType        = msgType;
    stim.msg            = msg;
    stim.length         = length;

    radStatesProcess (templateWork.stateMachine, &stim);

    return;
}

static void evtHandler
(
    UINT        eventsRx,
    UINT        rxData,
    void        *userData
)
{
    STIM        stim;

    stim.type           = STIM_EVENT;
    stim.eventsRx       = eventsRx;
    stim.eventData      = rxData;

    radStatesProcess (templateWork.stateMachine, &stim);

    return;
}

// process initialization
static int templateSysInit (void)
{
    char            devPath[256], temp[32];
    char            *installPath;
    struct stat     fileData;
    FILE            *pidfile;

    // get the install path
    installPath = getenv ("APPLICATION_RUN_DIRECTORY");
    if (installPath == NULL)
    {
        installPath = ".";
    }
    chdir (installPath);


    // check for our pid file, don't run if it is there
    sprintf (devPath, "%s/%s", installPath, TEMPLATE2_LOCK_FILENAME);
    if (stat (devPath, &fileData) == 0)
    {
        printf ("lock file %s exists, older copy may be running - aborting!\n",
                devPath);
        return -1;
    }


    // create our device directory if it is not there
    sprintf (devPath, "%s/dev", installPath);
    if (stat (devPath, &fileData) != 0)
    {
        if (mkdir (devPath, 0755) != 0)
        {
            printf ("Cannot create device dir: %s - aborting!\n",
                    devPath);
            return -1;
        }
    }

    return 0;
}

// system exit
static int templateSysExit (void)
{
    char            devPath[256];
    char            *installPath;
    struct stat     fileData;

    // get the install path
    installPath = getenv ("APPLICATION_RUN_DIRECTORY");
    if (installPath == NULL)
    {
        installPath = ".";
    }

    // delete our pid file
    sprintf (devPath, "%s/%s", installPath, TEMPLATE2_LOCK_FILENAME);
    if (stat (devPath, &fileData) == 0)
    {
        printf ("\nlock file %s exists, deleting it...\n",
                devPath);
        if (unlink (devPath) == -1)
        {
            printf ("lock file %s delete failed!\n",
                    devPath);
        }
    }

    return 0;
}

static void defaultSigHandler (int signum)
{
    switch (signum)
    {
        case SIGPIPE:
            // if you are using sockets or pipes, you will need to catch this
            // we have a far end socket disconnection, we'll handle it in the
            // "read/write" code
            signal (signum, defaultSigHandler);
            break;

        case SIGILL:
        case SIGBUS:
        case SIGFPE:
        case SIGSEGV:
        case SIGXFSZ:
        case SIGSYS:
            // unrecoverable signal - we must exit right now!
            radMsgLog(PRI_CATASTROPHIC, "template2d: recv sig %d: bailing out!", signum);
            abort ();
        
        case SIGCHLD:
            wait (NULL);
            signal (signum, defaultSigHandler);
            break;

        default:
            // can we allow the process to exit normally?
            if (radProcessGetExitFlag())
            {
                // NO! - we gotta bail here!
                radMsgLog(PRI_HIGH, "template2d: recv sig %d: exiting now!", signum);
                exit (0);                
            }
            
            // we can allow the process to exit normally...
            radMsgLog(PRI_HIGH, "template2d: recv sig %d: exiting!", signum);
        
            radProcessSetExitFlag ();
        
            signal (signum, defaultSigHandler);
            break;
    }

    return;
}


// the main entry point for the template process
int main (int argc, char *argv[])
{
    void            (*alarmHandler)(int);
    STIM            stim;
    int             i;
    char            qname[256], cfgname[256], instance[32], value[32];
    char            pidName[256];
    char            *installPath;
    CF_ID           configFileId;
    struct stat     fileStatus;
    FILE            *pidfile;


    // initialize some system stuff first
    if (templateSysInit () == -1)
    {
        radMsgLogInit (PROC_NAME_TEMPLATE2, TRUE, TRUE);
        radMsgLog(PRI_CATASTROPHIC, "system init failed!\n");
        radMsgLogExit ();
        exit (1);
    }

    // create some file paths for later use
    installPath = getenv ("APPLICATION_RUN_DIRECTORY");
    if (installPath == NULL)
    {
        installPath = ".";
    }
    sprintf (qname, "%s/dev/%s", installPath, PROC_NAME_TEMPLATE2);
    sprintf (cfgname, "%s/%s", installPath, TEMPLATE_CONFIG_FILENAME);
    sprintf (pidName, "%s/%s", installPath, TEMPLATE2_LOCK_FILENAME);

    memset (&templateWork, 0, sizeof (templateWork));



    // call the global radlib system init function
    if (radSystemInit (TEMPLATE_SYSTEM_ID) == ERROR)
    {
        radMsgLogInit (PROC_NAME_TEMPLATE2, TRUE, TRUE);
        radMsgLog(PRI_CATASTROPHIC, "%s: radSystemInit failed!");
        radMsgLogExit ();
        exit (1);
    }


    // call the radlib process init function
    if (radProcessInit (PROC_NAME_TEMPLATE2,
                        qname,
                        0,                          // no timers
                        TRUE,                       // TRUE => daemon
                        msgHandler,
                        evtHandler,
                        NULL)
            == ERROR)
    {
        radMsgLogInit (PROC_NAME_TEMPLATE2, TRUE, TRUE);
        radMsgLog(PRI_CATASTROPHIC, "radProcessInit failed: %s",
                   PROC_NAME_TEMPLATE2);
        radMsgLogExit ();

        radSystemExit (TEMPLATE_SYSTEM_ID);

        exit (1);
    }

    // save our process pid and create the lock file 
    templateWork.myPid = getpid ();
    pidfile = fopen (pidName, "w");
    if (pidfile == NULL)
    {
        radMsgLog(PRI_CATASTROPHIC, "lock file create failed!\n");

        radProcessExit ();
        radSystemExit (TEMPLATE_SYSTEM_ID);

        exit (1);
    }
    fprintf (pidfile, "%d", templateWork.myPid);
    fclose (pidfile);


    // save the current alarm signal handler, set all signal handlers
    // to the default handler, then set the alarm handler back to original
    alarmHandler = radProcessSignalGetHandler (SIGALRM);
    radProcessSignalCatchAll (defaultSigHandler);
    radProcessSignalCatch (SIGALRM, alarmHandler);


    // create our state machine - where all run-time work is done
    templateWork.stateMachine = radStatesInit (&templateWork);
    if (templateWork.stateMachine == NULL)
    {
        radMsgLog(PRI_HIGH, "radStatesInit failed");

        radProcessExit ();
        radSystemExit (TEMPLATE_SYSTEM_ID);

        exit (1);
    }

    if (radStatesAddHandler (templateWork.stateMachine, TEMPLATE_STATE_IDLE,
                             template2IdleState)
            == ERROR)
    {
        radMsgLog(PRI_HIGH, "radStatesAddHandler failed");
        radStatesExit (templateWork.stateMachine);

        radProcessExit ();
        radSystemExit (TEMPLATE_SYSTEM_ID);

        exit (1);
    }
    if (radStatesAddHandler (templateWork.stateMachine, TEMPLATE_STATE_RUN,
                             template2RunState)
            == ERROR)
    {
        radMsgLog(PRI_HIGH, "radStatesAddHandler failed");
        radStatesExit (templateWork.stateMachine);

        radProcessExit ();
        radSystemExit (TEMPLATE_SYSTEM_ID);

        exit (1);
    }
    if (radStatesAddHandler (templateWork.stateMachine, TEMPLATE_STATE_ERROR,
                             template2ErrorState)
            == ERROR)
    {
        radMsgLog(PRI_HIGH, "radStatesAddHandler failed");
        radStatesExit (templateWork.stateMachine);

        radProcessExit ();
        radSystemExit (TEMPLATE_SYSTEM_ID);

        exit (1);
    }

    radStatesSetState (templateWork.stateMachine, TEMPLATE_STATE_IDLE);


    // initialize the radlib message router interface
    if (radMsgRouterInit (".") == ERROR)
    {
        radMsgLog(PRI_HIGH, "radMsgRouterInit failed");
        radStatesExit (templateWork.stateMachine);

        radProcessExit ();
        radSystemExit (TEMPLATE_SYSTEM_ID);

        exit (1);
    }

    radMsgRouterMessageRegister (TEMPLATE_MSGID_USER_REQUEST);


    radMsgLog(PRI_STATUS, "running ...");


    // dummy up a stimulus to get the state machine running
    stim.type = STIM_DUMMY;
    radStatesProcess (templateWork.stateMachine, &stim);


    while (!templateWork.exiting)
    {
        // wait on timers, events, file descriptors, msgs, everything!
        if (radProcessWait (0) == ERROR)
        {
            templateWork.exiting = TRUE;
        }
    }


    radMsgLog(PRI_STATUS, "exiting normally...");

    radStatesExit (templateWork.stateMachine);
    templateSysExit ();

    radProcessExit ();
    radSystemExit (TEMPLATE_SYSTEM_ID);

    exit (0);
}

