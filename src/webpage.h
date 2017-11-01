/*
 * webpage.h
 *
 *  Created on: Oct 30, 2017
 *      Author: nimal
 */

#ifndef WEBPAGE_H_
#define WEBPAGE_H_
#include <iostream>
#include <curl.h>
#include<unistd.h>
#include <fstream>
#include<string.h>
#include <sstream>
#include"db.h"
 using namespace std;

class webpage{

private:
	 CURL *curl;
	 FILE *fp;
	 CURLcode res;


public:

	 void getDataFromWebpageansStoreit();
	 char * searchWord(string token) ;




 };


#endif /* WEBPAGE_H_ */
