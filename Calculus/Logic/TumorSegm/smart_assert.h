/**
Author:
Liqun Liu (lliu329@uwo.ca)                                   
Danfeng Chen (dchen285@uwo.ca)  

The code is for research use only.
Details about the code please refer to README.txt
**/

/* Smart Assertion
 *  - Show assertion under debug mode
 *  - Print assertion message udner release mode
 *
 * Created by Yuchen on Apr 30th, 2013
 * Last Modified by Yuchen on Jul 2nd, 2013
 *
 * Example:
	Code:
		int i = -1;
		smart_assert(i>=0 && i<3, "Index out of bound" );
	Output:
		Assertion failed: i>0 && i<3
		  Messages: Index out of bound
		  Location: file main.cpp, line 17
 */
#ifndef SMART_ASSERT_H
#define SMART_ASSERT_H
#include <assert.h>
#include <iostream>
using namespace std;

#define smart_assert( condition, message ) \
	if( !(condition) ) { \
		assert( condition && message ); \
	}


#define smart_exit( condition, message ) \
	if( !(condition) ) { \
		system("pause"); \
		exit(0); \
	}

#endif
