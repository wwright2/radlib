# radlib 

rapid application development library for unix multi-process applications. It uses SYS V IPC facilities and FIFOs to provide an RTOS-like, event-driven, distributed framework. Processes may be run as daemons or have a controlling terminal.  

- Additional Project Details  
- Languages:English  
- Intended Audience: Developers 
- User Interface: Command-line, Console/Terminal, Non-interactive (Daemon)  
- Programming Language: C  
- BSD user license  2005-07-14  

### Library
- IPC message passing through shared memeory
- message buffering system
- timers
- queue
- stack
- state machine
- 


ARCHIVE CONTENTS
------- --------

File/Directory          Remark
--------------          ------------------------------------------------------

configure                   build configuration script to be executed before
                            building radlib
config-radlib-arm-linux     example cross compilation script for radlib
                            (see the CROSS COMPILING section below for
                            more details)
COPYING                     The FreeBSD-style License file
h                           Header files for radlib
src                         Source directory for the radlib library
msgRouter                   Source directory for the radlib message routing 
                            daemon radmrouted
database                    Database server-specific source files
test/database               Database API verification test application
test/multicast              radUDPsocket multicast verification test application
test/sha                    radsha SHA-1 and SHA-256 example test application
debug                       Simple radlib debug utility with makefile - see the
                            section marked "Debugging" below for details
README                      This file
ChangeLog                   Release notes
radlib-api-reference.html   radlib HTML API Reference Manual
template-old                template/example process which uses some of the
                            more common radlib capabilities. It demonstrates the
                            older *destination-based* (QUEUE_GROUP) interprocess
                            messaging paradigm which is still supported in 
                            radlib. It includes a build environment. It is 
                            functional when built and demonstrates how to 
                            construct and build a radlib application.
template                    template/example processes which use some of the
                            more common radlib capabilities. It demonstrates the
                            new (preferred) *message ID-based* interprocess 
                            messaging paradigm which requires use of the radlib
                            message routing daemon radmrouted. It includes
                            a build environment. It is functional when built
                            and demonstrates how to construct and build
                            a multi-process radlib application. See the README
                            file in the 'template' directory for more details.
template/databaseExample.c  example program to demo the radlib database API


BUILDING
--------

1) Extract to the location <radlib_path> of your choosing.

2) Change directory to the radlib root source directory.

    cd <radlib_path>

3) Run the configure script to create the build files for your platform.
   Note: Do NOT enable more than one type of database support! Try 
   "./configure --help" to see user configurable options.
   
    [No database support (default)]
    ./configure

    ---OR---
    
    [With MySQL database support]
    ./configure --enable-mysql
    
    ---OR---
    
    [With PostgreSQL database support]
    ./configure --enable-pgresql

4) Build the library.
    
    make

5) Become root.

    su
    <password>

6) Install the library and headers.

    make install

Note: The following build targets are available:
    make                - default target builds all source
    make clean          - deletes library and object files
    make install        - makes <default> then installs the library and
                          headers to "/usr/local/lib" and
                          "/usr/local/include"

7) Add the radlib directory to your shared library cache.

    As root, edit /etc/ld.so.conf
    Add "/usr/local/lib" to the file, save and exit.
    Run ldconfig: ldconfig


CROSS COMPILING
---------------

See the script "config-radlib-arm-linux for a "./configure" alternative.
This script can be edited to suit your target and toolchain environment.

The general build sequence is:

#./config-radlib-arm-linux
#make
[become root]
#make install

The lib and headers are installed in the toolchain root, defined in the 
config script.



DEBUGGING
---------

Included in the distribution is a simple debug utility you can build and use 
to display buffer and semaphore information about a running radlib system. It 
is called "raddebug" and it is found in the "debug" directory of the distro.

It is built and installed to $prefix/bin/raddebug when you build and install
radlib as described above.

To run it:

#raddebug [radlib_system_ID] <working_directory>

radlib_system_ID        the radlib system you want to inspect in the range 1-255; 
                        it is the value passed to "radSystemInit" for the 
                        running system you are interested in
working_directory       (optional) working directory given to the radmrouted
                        daemon when it was started; this enables dumping of the
                        radmrouted routing statistics

Note: If you specify a non-existent [radlib_system_ID], it will create one and 
      report all buffers free, then destroy the radlib system and exit.
      This is probably not what you want although it is not a problem per se,
      just not very informative concerning the radlib system you were trying to
      inspect.



EXAMPLE/TEMPLATE
----------------

There are two example radlib applications included in the distribution. Either 
may be used as a template for your radlib application development.

template      - contains the newer, message router-based, multiprocess example

template-old  - contains the older, source/destination messaging single process 
                example

In either case see the README file in the respective directory for build and use
details.



GENERAL DATABASE API EXAMPLES
-----------------------------

Regardless of your database choice, radlib abstracts the specifics so you can 
write portable code which will run on any radlib-supported database engine.

The best example of the API usage is the database test application used to 
regression test the radlib database interface. Not well commented but the 
printfs in the code give you an idea what is happening. It is found in the 
distro at radlib-x.y.z/test/database/dbtest.c. You can also build and run 
this application to verify your database setup.

The other resource is the example file: 
radlib-x.y.z/template/databaseExample.c



MYSQL DATABASE SETUP (optional)
-------------------------------
This assumes the following (replace with your own preferences):
- the MySQL server is running
- mysql user name: testdbuser
- mysql user password: testdbpw
- mysql database: dbTestDB

1) Connect to the MySQL server:
    mysql -u root -p
    <password>
    
2) Create the wview database: 
    CREATE DATABASE dbTestDB;

3) Create the user account and password for the new database: 
    GRANT ALL ON dbTestDB.* TO testdbuser@localhost IDENTIFIED BY "testdbpw";
    
4) Make sure mysqlclient libraries are installed:
    find /usr -name "mysql*" -print
    
    If you do not see the files libmysqlclient.a and mysql.h in the output,
    it isn't installed. Install the version matching your mysql server that
    is installed.
    
5) Build radlib with database support:
    cd <radlib_path>
    make distclean                  # if it had been previously built
    ./configure --enable-mysql
    make install



PostgreSQL DATABASE SETUP (optional)
------------------------------------
This assumes the following (replace with your own preferences):
- the PostgreSQL server is running
- PostgreSQL user name: testdbuser
- PostgreSQL user password: testdbpw
- PostgreSQL database: dbTestDB

1) Create a PostgreSQL user for a unix login user (dbadmin must be a 
   valid non-root account on the wview server):

    Login to the PostgreSQL account:
    root@server:# su pgsql

    Create an administrative user based on a unix account:
    pgsql@server:# /usr/local/bin/createuser dbadmin

    Answer the questions:
    Shall the new user be allowed to create databases? (y/n) y
    Shall the new user be allowed to create more new users? (y/n) y

    PostgreSQL acknowledgement:
    CREATE USER

    Logout of the PostgreSQL account:
    pgsql@server:# logout
    root@server:#

2)  Create the test database:

    Login to the admin account:
    root@server:# su dbadmin

    Create the test database:
    dbadmin@server:# /usr/local/bin/createdb dbTestDB

    PostgreSQL acknowledgement:
    CREATE DATABASE

3) Create the test user account for the test database:

    Connect to the PostgreSQL server:
    dbadmin@server:# /usr/local/bin/psql dbTestDB

    Create the user account and password for the test database: 
    mydb=# create user testdbuser with password 'testdbpw';

    PostgreSQL acknowledgement:
    CREATE USER

    Disconnect from the server and logout of the admin account:
    mydb=# \q
    dbadmin@server:# logout
    root@server:#

4) Make sure the PostgreSQL development library is installed:
    find /usr -name "libpq.a" -print
    
    If you do not see the file libpq.a in the output, it isn't installed. 
    Install the version matching your PostgreSQL server that is installed.

5) Build radlib with PostgreSQL database support:
    cd <radlib_path>
    make distclean                  # if it had been previously built
    ./configure --enable-pgresql
    make install



BUILDING AND RUNNING THE TEMPLATE
---------------------------------

1) Change directory to the template source:
    cd <radlib_path>/template

2) Build it:
    make

3) Run it:
    ./templated



BUILDING AND RUNNING THE DATABASE EXAMPLE
-----------------------------------------

1) Change directory to the template source:
    cd <radlib_path>/template

2) Build it:
    make databaseExample

3) Run it:
    ./databaseExample

Note: It will fail if you do not have the database, table and permissions 
      set properly as described above.



LINKSYS NSLU2 DEVELOPMENT
-------------------------

The Linksys NSLU2 is a network storage link which uses USB external hard drives 
and an ethernet interface to provide Network Attached Storage (NAS) to the 
masses. This is a perfect embedded platform to use for radlib development. It 
is small, inexpensive (~$70) and uses little power. In addition, there exists a 
mature linux hacking community which has developed an entire open source 
distribution including a simple package management system.

1) Install Openslug

    Download the openslug 2.7-beta binary: 
        http://www.openslug.org/

    Flash the NSLU2 with the openslug binary: 
        http://www.nslu2-linux.org/wiki/OpenSlug/UsingTheBinary

    Initialize your hard disk and transfer the root filesystem:
        http://www.nslu2-linux.org/wiki/OpenSlug/InitialisingOpenSlug

2) Prepare ipkg to Use the wviewweather.com Repository

    Once you have your slug (NSLU2) booting from the USB hard drive, do the 
    following to enable the wviewweather.com ipkg repository:

    Obtain the wview ipkg config file:
        cd /etc/ipkg
        wget http://www.wviewweather.com/ipkg/wview.conf

    Configure ipkg:
        ipkg update

3) Install the Native Development Tools

    ipkg install openslug-native

4) Install the radlib Development Package

    ipkg install radlib-dev

   Alternatively, if you just need the radlib run-time libraries for an existing
   radlib application ipkg, install the "radlib" package.

You are now ready to compile and link native radlib applications on the NSLU2!

Note: There are many other packages available for the slug - execute 
      "ipkg list" to see the available packages.



DOCS AND EXAMPLES
-----------------

radlib has been successfully deployed (in earlier incarnations) in
several mission-critical applications. In addition, it is used in my
open-source weather application for the Davis Vantage Pro weather station
(see www.wview.teel.ws for details and download). The wview application
source is also an excellent example of how to deploy a radlib based
application.

If you are new to radlib, have a look at the template process as well as the
radlib API reference. Together, these two resources should answer most of
your questions. Also, radlib is a registered SourceForge project.
There is a mailing list as well as a bugtracking system hosted by SourceForge
for the radlib project.  Comments/suggestions are welcome.
