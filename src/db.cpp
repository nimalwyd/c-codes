/*
 * db.cpp
 *
 *  Created on: Oct 30, 2017
 *      Author: nimal
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdlib.h>
#include <curl.h>
#include<unistd.h>
#include <fstream>
#include<string.h>
#include <sstream>
#include"db.h"
using namespace std;


int sqlitedb::callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);

   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }

   printf("\n");
   return 0;
}


// the function is same for select,update,delete operation
void  sqlitedb::displayDatainDatabase()
{

	 /* Open database */
	   rc = sqlite3_open("test.db", &db);

	   if( rc ) {
	      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	      return;
	   } else {
	      fprintf(stderr, "Opened database successfully\n");
	   }

	   /* Create SQL statement */
	   sql = "SELECT * from COMPANY";

	   /* Execute SQL statement */
	   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

	   if( rc != SQLITE_OK ) {
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Operation done successfully\n");
	   }
	   sqlite3_close(db);


}


void sqlitedb::insertIntoDatabase(int id, string namepassed, int age,string  address,int salary)
{

	/* Open database */
	   rc1 = sqlite3_open("test.db", &db);

	   if( rc1 ) {
	      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	      return;
	   } else {
	      fprintf(stderr, "Opened database successfully\n");
	   }

	   	ostringstream sql1;
	   	sql1<< "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
		        << "VALUES("<<id<<","<<"\""<<namepassed<<"\""<<","<< age<<","<<"\""<<address<<"\""<<","<<salary<<");" ;

	   	string query(sql1.str());
	   	const char * queryc = query.c_str();
	  // cout<<queryc<<endl;
	   /* Execute SQL statement */
	 rc1 = sqlite3_exec(db,queryc, callback, 0, &zErrMsg);

	   if( rc1 != SQLITE_OK ){
	      fprintf(stderr, "SQL error: %s\n", zErrMsg);
	      sqlite3_free(zErrMsg);
	   } else {
	      fprintf(stdout, "Records created successfully\n");
	   }
	   sqlite3_close(db);


}

