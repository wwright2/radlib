###############################################################################
#                                                                             #
#  Makefile for the radlib-gpl template process                               #
#                                                                             #
#  Name                 Date           Description                            #
#  -------------------------------------------------------------------------  #
#  Yourname             01/01/2004     Initial Creation                       #
#                                                                             #
###############################################################################

#  Define some general usage vars


#  Libraries
LIBPATH			= \
			-L/usr/local/lib

LIBS			= \
			-lc \
			-lm \
			-lrad

# Define these according to where libmysqlclient is installed
MYSQL_LIB_DIR		= /usr/lib/mysql
MYSQL_INCLUDE_DIR	= /usr/include/mysql

DB_LIBS			= \
			-lc \
			-lz \
			-lrad \
			-lmysqlclient

DB_LIBPATH 		= \
			-L$(MYSQL_LIB_DIR)

#  Declare build defines
DEFINES			= \
			-D_DEBUG

#  Any build defines listed above should also be copied here
INCLUDES		= \
			-I/usr/local/include \
			-I.

########################### T A R G E T   I N F O  ############################
EXE_IMAGE		= ./templated
DB_EXE_IMAGE		= ./databaseExample

TEMPLATE_OBJS		= \
			./template.o \
			./templateStates.o

DB_OBJS		= \
			./databaseExample.o

#########################  E X P O R T E D   V A R S  #########################

#  Include the rules file for all of the common stuff
include ./rules.mk

################################  R U L E S  ##################################

$(EXE_IMAGE):   $(TEMPLATE_OBJS)
	@echo "Linking $@..."
	@$(LD) $(LD_OPTS) $(LIBPATH) -o $@ \
	$(TEMPLATE_OBJS) \
	$(LIBS)

$(DB_EXE_IMAGE): $(DB_OBJS) 
	@echo "Linking $@..."
	@$(LD) $(LD_OPTS) $(DB_LIBPATH) -o $@ \
	$(DB_OBJS) \
	$(DB_LIBS)


all: clean $(EXE_IMAGE) $(DB_EXE_IMAGE)


#  Cleanup rules...
clean:
	rm -rf \
	$(EXE_IMAGE) \
	$(DB_EXE_IMAGE) \
	$(TEMPLATE_OBJS) \
	$(DB_OBJS)

./databaseExample.o: ./databaseExample.c
	@echo "Building   $@"
	$(CC) $(CC_OPTS) $(SYS_DEFINES) $(INCLUDES) -I$(MYSQL_INCLUDE_DIR) -c $< -o $@
