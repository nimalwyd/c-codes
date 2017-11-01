

#include"db.h"
#include "webpage.h"
#include <string>
#include <fstream>
#include<cstdio>
#include<iostream>
#include <sstream>
using namespace std;

sqlitedb dbObject1;
webpage webpageObject;



int main(int argc, char* argv[]) {
 char * dey="";
	try
	{
		webpageObject.getDataFromWebpageansStoreit();
	}
	catch(std::exception const& e)
	{
cout<<"caught exception"<<endl;
	}
	try
		{
	dey= webpageObject.searchWord("font");

		}
	catch(std::exception const& e)
		{
		cout<<"caught exception"<<endl;

		}
	try
			{
		//string dey1( dey, 5 );

	dbObject1.insertIntoDatabase(14,dey,26,"insert after search",30);
			}
	catch(std::exception const& e)
		{
		cout<<"caught exception"<<endl;
		}
	try

			{
	dbObject1.displayDatainDatabase();
			}
	catch(std::exception const& e)
		{
		cout<<"caught exception"<<endl;
		}



   return 0;
}
