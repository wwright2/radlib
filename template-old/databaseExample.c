/*---------------------------------------------------------------------
 
 FILE NAME:
        databaseExample.c
 
 PURPOSE:
        Provide example code to demonstrate the radlib database API.
 
 REVISION HISTORY:
        Date        Programmer  Revision    Function
        04/23/2005  M.S. Teel   0           Original
 
 ASSUMPTIONS:
 None.
 
------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <radsysdefs.h>
#include <raddatabase.h>


/*  ... global references
*/

/*  ... local memory
*/
#define SYSTEM_DB_USER          "testdbuser"
#define SYSTEM_DB_PASSWORD      "testdbpw"
#define DATABASE_NAME           "dbTestDB"
#define TABLE_NAME              "dbExample"

/*  ... methods
*/

/*  ... the entry point
*/
int main (int argc, char *argv[])
{
    DATABASE_ID     dbId;
    ROW_ID          row;
    FIELD_ID        field;
    int             i, retVal;
    char            temp[1024];

    // open the database connection 
    if ((dbId = raddatabaseOpen (NULL,
                                 SYSTEM_DB_USER,
                                 SYSTEM_DB_PASSWORD,
                                 DATABASE_NAME))
        == NULL)
    {
        printf ("databaseExample: databaseOpen failed!\n");
        exit (1);
    }

    // does our table already exist?
    if (!raddatabaseTableIfExists (dbId, TABLE_NAME))
    {
        // no, let's create it - get an empty row description to work with
        row = raddatabaseRowDescriptionCreate ();
        if (row == NULL)
        {
            printf ("databaseExample: databaseRowDescriptionCreate failed!\n");
            raddatabaseClose (dbId);
            exit (1);
        }

        // add a column to the row description
        retVal = raddatabaseRowDescriptionAddField (row, "Column1", FIELD_INT, 0);
        if (retVal == ERROR)
        {
            printf ("databaseExample: databaseRowDescriptionAddField failed!\n");
            raddatabaseRowDescriptionDelete (row);
            raddatabaseClose (dbId);
            exit (1);
        }

        // add another column to the row description
        retVal = raddatabaseRowDescriptionAddField (row, "Column2", FIELD_STRING, 128);
        if (retVal == ERROR)
        {
            printf ("databaseExample: databaseRowDescriptionAddField failed!\n");
            raddatabaseRowDescriptionDelete (row);
            raddatabaseClose (dbId);
            exit (1);
        }

        // now try to create the table with this row description
        retVal = raddatabaseTableCreate (dbId, TABLE_NAME, row);
        if (retVal == ERROR)
        {
            raddatabaseRowDescriptionDelete (row);
            raddatabaseClose (dbId);
            exit (1);
        }

        // free the row description resources, we're done with it
        raddatabaseRowDescriptionDelete (row);
    }

    // get the table structure via a row description
    row = raddatabaseTableDescriptionGet (dbId, TABLE_NAME);
    if (row == NULL)
    {
        printf ("databaseExample: databaseTableDescriptionGet failed!\n");
        raddatabaseClose (dbId);
        exit (1);
    }

    // let's just loop through here adding some records to the table
    for (i = 0; i < 100; i ++)
    {
        // obtain the field object for the first column
        field = raddatabaseFieldGet (row, "Column1");
        if (field == NULL)
        {
            printf ("databaseExample: databaseFieldGet 'Column1' failed!\n");
            raddatabaseRowDescriptionDelete (row);
            raddatabaseClose (dbId);
            exit (1);
        }
        
        // set it's value to our loop variable
        raddatabaseFieldSetIntValue (field, i);

        // obtain the filed object for the second column
        field = raddatabaseFieldGet (row, "Column2");
        if (field == NULL)
        {
            printf ("databaseExample: databaseFieldGet 'Column1' failed!\n");
            raddatabaseRowDescriptionDelete (row);
            raddatabaseClose (dbId);
            exit (1);
        }
        
        // set it's value to something mildly interesting
        sprintf (temp, "Column 2 string #%d", i);
        raddatabaseFieldSetCharValue (field, temp, strlen (temp));

        // insert the row
        if (raddatabaseTableInsertRow (dbId, TABLE_NAME, row) == ERROR)
        {
            printf ("databaseExample: raddatabaseTableInsertRow failed!\n");
            raddatabaseRowDescriptionDelete (row);
            raddatabaseClose (dbId);
            exit (1);
        }
    }

    // we are done with this exercise, clean up our resources and leave
    printf ("databaseExample: %d rows added successfully...\n", i);
    raddatabaseRowDescriptionDelete (row);
    raddatabaseClose (dbId);

    exit (0);
}

