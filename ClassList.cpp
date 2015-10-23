////////////////////////////////////////////////
//                                            //
//                ClassList.cpp               //
//                                            //
//        Copyright Sean O'Connor 2015        //
//           sean@windowsgames.co.uk          //
//           www.windowsgames.co.uk           //
//                                            //
////////////////////////////////////////////////


// includes
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "ClassList.h"


//////////////////////////////////////////
//        Constructor/destructor        //
//////////////////////////////////////////

ClassList::ClassList()
{
	m_iNumberClasses = 0;

	m_pszList = NULL;
	m_pszSuperClassList = NULL;
}


ClassList::~ClassList()
{
	int		i;

	// classes
	if (m_pszList != NULL)
	{
		for (i=0; i<m_iNumberClasses; i++)
		{
			free(m_pszList[i]);
		}

		free(m_pszList);
	}

	// coresponding super classes
	if (m_pszSuperClassList != NULL)
	{
		for (i=0; i<m_iNumberClasses; i++)
		{
			free(m_pszSuperClassList[i]);
		}

		free(m_pszSuperClassList);
	}
}


void ClassList::addClassToList(char* szClass)
{
	/////////////////////////
	//        class        //
	/////////////////////////

	// add a new entry to the list
	m_pszList = (char**)(realloc(m_pszList, sizeof(char*) * (m_iNumberClasses + 1)));

	// create this string
	m_pszList[m_iNumberClasses] = (char*)malloc(sizeof(char) * (strlen(szClass) + 1));

	strcpy(m_pszList[m_iNumberClasses], szClass);


	///////////////////////////////
	//        super class        //
	///////////////////////////////

	// add a new entry to the list
	m_pszSuperClassList = (char**)(realloc(m_pszSuperClassList, sizeof(char*) * (m_iNumberClasses + 1)));

	// add a blank super class
	m_pszSuperClassList[m_iNumberClasses] = (char*)malloc(sizeof(char) * (1));

	m_pszSuperClassList[m_iNumberClasses] = 0;


	/////////////////////////////
	//        increment        //
	/////////////////////////////

	m_iNumberClasses ++;
}


int ClassList::isOnTheClassList(char* szClass)
{
	int		i;

	for (i=0; i<m_iNumberClasses; i++)
	{
		if (strcmp(szClass, m_pszList[i]) == 0)
		{
			return 1;
		}
	}

	return 0;
}


void ClassList::setSuperClassOfLastClassAddedTo(char* szSuperClass)
{
	if (m_iNumberClasses > 0)
	{
		// free existing
		if (m_pszSuperClassList[m_iNumberClasses - 1] != NULL)
		{
			free(m_pszSuperClassList[m_iNumberClasses - 1]);
		}

		// set
		m_pszSuperClassList[m_iNumberClasses-1] = (char*)malloc(sizeof(char) * (strlen(szSuperClass) + 1));

		strcpy(m_pszSuperClassList[m_iNumberClasses-1], szSuperClass);
	}
}


char* ClassList::getSuperClassOfClass(char* szClass)
{
	int		i;

	for (i=0; i<m_iNumberClasses; i++)
	{
		if (strcmp(szClass, m_pszList[i]) == 0)
		{
			return m_pszSuperClassList[i];
		}
	}

	return NULL;
}






