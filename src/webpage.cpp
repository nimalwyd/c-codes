/*
 * webpage.cpp
 *
 *  Created on: Oct 30, 2017
 *      Author: nimal
 */
#include <iostream>
#include"webpage.h"
#include<string>

using namespace std;

void  webpage::getDataFromWebpageansStoreit()
{
	 curl = curl_easy_init();
		char outfilename[FILENAME_MAX] = "/home/nimal/Desktop/sandbox/c++/sqlite/test.txt";
		if(curl) {
		     fp = fopen(outfilename,"w");

		   // curl_easy_setopt(curl, CURLOPT_URL, "http://www.example.com");
		     curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com");
		    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "user=123&pass=123");
		    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		  //  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		    res = curl_easy_perform(curl);
		    //Sleep(1000);
		    curl_easy_cleanup(curl);
		    fclose(fp);
		}

		return;

}

char * webpage::searchWord(string token) {
    string line;
    ifstream FileSearch;
    bool a = false;
    int k =token.size();
    char * findword;
    FileSearch.open("test.txt");
    while (getline(FileSearch, line)) {
        if (line.find(token) != string::npos) {
        	for(int j=0;j<k;j++)
        	{
        		findword[j]=line[line.find(token)+j] ;

        	}

            a=  true;


        }
    }
   // insertIntoDatabase(8, findword,26,"address",30);
    if(a)cout<<findword<<endl;
  if(!a) cout << token << " not found" << endl;




  //can also insert data into database as below
  //sqlitedb dbObject;
  //dbObject.insertIntoDatabase(13,findword,26,"insert from class",300000);

  return findword ;
}


