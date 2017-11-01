/*
 * db.h
 *
 *  Created on: Oct 30, 2017
 *      Author: nimal
 */
#ifndef DB_H_
#define DB_H_

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

class sqlitedb
{
private:
	   sqlite3 *db;
	   char *zErrMsg = 0;
	   int rc,rc1;
	   char *sql,*sql1;
	   const char* data = "Callback function called";

public:

	   void displayDatainDatabase();
	   void insertIntoDatabase(int id, string namepassed, int age,string  address,int salary);
	   static int callback(void *data, int argc, char **argv, char **azColName);


};



#endif /* DB_H_ */
