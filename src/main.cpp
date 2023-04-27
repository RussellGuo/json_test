#include <assert.h>
#include <sqlite3.h>
#include <stdio.h>

#include <iostream>
#include <memory>
#include <regex>
#include <set>

#include "json.hpp"
#include "time_used.hxx"

int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave) {
    int rc;                  /* Function return code */
    sqlite3 *pFile;          /* Database connection opened on zFilename */
    sqlite3_backup *pBackup; /* Backup object used to copy data */
    sqlite3 *pTo;            /* Database to copy to (pFile or pInMemory) */
    sqlite3 *pFrom;          /* Database to copy from (pFile or pInMemory) */

    /* Open the database file identified by zFilename. Exit early if this fails
    ** for any reason. */
    rc = sqlite3_open(zFilename, &pFile);
    if (rc == SQLITE_OK) {
        /* If this is a 'load' operation (isSave==0), then data is copied
        ** from the database file just opened to database pInMemory.
        ** Otherwise, if this is a 'save' operation (isSave==1), then data
        ** is copied from pInMemory to pFile.  Set the variables pFrom and
        ** pTo accordingly. */
        pFrom = (isSave ? pInMemory : pFile);
        pTo = (isSave ? pFile : pInMemory);

        /* Set up the backup procedure to copy from the "main" database of
        ** connection pFile to the main database of connection pInMemory.
        ** If something goes wrong, pBackup will be set to NULL and an error
        ** code and message left in connection pTo.
        **
        ** If the backup object is successfully created, call backup_step()
        ** to copy data from pFile to pInMemory. Then call backup_finish()
        ** to release resources associated with the pBackup object.  If an
        ** error occurred, then an error code and message will be left in
        ** connection pTo. If no error occurred, then the error code belonging
        ** to pTo is set to SQLITE_OK.
        */
        pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
        if (pBackup) {
            (void)sqlite3_backup_step(pBackup, -1);
            (void)sqlite3_backup_finish(pBackup);
        }
        rc = sqlite3_errcode(pTo);
    }

    /* Close the database connection opened on database file zFilename
    ** and return the result of this function. */
    (void)sqlite3_close(pFile);
    return rc;
}

int main(int, char *[]) {
    time_used_t total("total");
    time_used_t prepair("prepair");
    sqlite3 *db;
    if (sqlite3_open_v2(":memory:", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr)) {
        printf("Could not open the.db\n");
        exit(-1);
    }

    if (sqlite3_exec(db, "create table tab(no integer, name text, age integer)", NULL, NULL, NULL)) {
        printf("Error executing sql statement\n");
    } else {
        printf("Table created\n");
    }

    {
        auto ret = sqlite3_exec(db, "PRAGMA synchronous = off", NULL, NULL, NULL);
        printf("%d\n", ret);
    }
    //
    // Prepare a statement for multiple use:
    //
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, "insert into tab values(?, ?, ?)", -1, &stmt, NULL)) {
        printf("Error executing sql statement\n");
        exit(-1);
    }

    prepair.print();
    time_used_t insert("insert");
    for (int count = 0; count < 30 * 1000 * 1000; count++) {
        if (count % 1000000 == 0) {
            printf("%d\n", count);
        }
        //
        // Bind the values for the first insert:
        //
        sqlite3_bind_int(stmt, 1, count + 1);
        sqlite3_bind_text(stmt, 2, "three", -1, NULL);
        sqlite3_bind_int(stmt, 3, 333);

        //
        // Do the first insert:
        //
        sqlite3_step(stmt);

        //
        // Reset the prepared statement to the initial state.
        // This seems to be necessary in order to
        // use the prepared statement again for another
        // insert:
        //
        sqlite3_reset(stmt);
    }

    //
    // Get rid of the memory allocated for stmt:
    //
    sqlite3_finalize(stmt);

    insert.print();
    time_used_t index("index");

    {
        auto ret = sqlite3_exec(db, "create index main_index on tab(no)", NULL, NULL, NULL);
        printf("%d\n", ret);
    }

    index.print();

    time_used_t backup("backup");
    loadOrSaveDb(db, "test.db", true);
    backup.print();
    sqlite3_close(db);
    total.print();
    return 0;
}
