////////////////////////////////////////////////
//                                            //
//                  Read.cpp                  //
//                                            //
//        Copyright Sean O'Connor 2015        //
//           sean@windowsgames.co.uk          //
//           www.windowsgames.co.uk           //
//                                            //
////////////////////////////////////////////////


// includes
#include <stdio.h>
#include <string.h>

#include "Read.h"

#include "ClassList.h"


//////////////////////////////////////////
//        Constructor/destructor        //
//////////////////////////////////////////

Read::Read(char* szHomeDirectory)
{
	m_pClassList = new ClassList;

	m_szCurrent = NULL;

	strcpy(m_szHomeDirectory, szHomeDirectory);
}


Read::Read()
{
	m_pClassList = new ClassList;

	m_szCurrent = NULL;
}


Read::~Read()
{
	delete m_pClassList;
}






