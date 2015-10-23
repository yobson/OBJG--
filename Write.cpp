////////////////////////////////////////////////
//                                            //
//                 Write.cpp                  //
//                                            //
//        Copyright Sean O'Connor 2015        //
//           sean@windowsgames.co.uk          //
//           www.windowsgames.co.uk           //
//                                            //
////////////////////////////////////////////////


// includes
#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <iostream>
#include <fstream>

#include "Write.h"

using namespace std;

//////////////////////////////////////////
//        Constructor/destructor        //
//////////////////////////////////////////

Write::Write()
{
	m_szOutput = NULL;
	m_iLengthOutput = 0;
}


Write::~Write()
{
	if (m_szOutput != NULL)
	{
		free(m_szOutput);
	}
}


//////////////////////////
//        Output        //
//////////////////////////

void Write::Output(Node* pRoot, char* szHomeDirectory, char* szFile)
{
	ofstream 		hFile;
	//DWORD	dwWrite;
	char	szFilename[FILENAME_MAX+1];


	////////////////////////////////
	//        write output        //
	////////////////////////////////
	
	// create output
	m_szOutput = NULL;
	printf("\nDONE 1 %s \n", pRoot);
	this->recurseNode(pRoot, -1);
	printf("\nDONE 2\n");
	// write it to a file
	strcpy(szFilename, szHomeDirectory);
	strcat(szFilename, "Code\\");
	strcat(szFilename, szFile);
	strcat(szFilename, this->getFileExtension());
	printf("\nDONE SETUP FILE\n");
	//hFile = CreateFile(szFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
	remove(szFilename);
	printf("Deleting: %s", szFilename);
	hFile.open (szFilename, ios::trunc);
	//printf("\nOpened File\n");
	hFile << m_szOutput;
	hFile.close();
	
	
	
	
}


void Write::recurseNodesChildren(Node* pCurrent, int iNumberTabs)
{
	int		iNumberChildren;
	int		i;

	iNumberChildren = pCurrent->getNumberChildren();

	iNumberTabs ++;

	for (i=0; i<iNumberChildren; i++)
	{
		this->recurseNode(pCurrent->getChild(i), iNumberTabs);
	}

	iNumberTabs --;
}


void Write::appendToOutput(const char* Buffer)
{
	
	char* szBuffer = (char*)malloc(sizeof(Buffer));
	strcpy(szBuffer, Buffer);
	
	int		iLength;

	iLength = strlen(szBuffer);

	m_szOutput = (char*)realloc(m_szOutput, m_iLengthOutput + iLength + 1);

	memcpy(m_szOutput + m_iLengthOutput, szBuffer, iLength);

	m_iLengthOutput += iLength;

	m_szOutput[m_iLengthOutput] = 0;
}


void Write::appendToOutputWithTabs(const char* Buffer, int iNumberTabs)
{
	char* szBuffer = (char*)malloc(sizeof(Buffer));
	strcpy(szBuffer, Buffer);
	
	int		i;
	int		iLength;

	// check!
	if (iNumberTabs < 0)
	{
		iNumberTabs = 0;
	}

	// extra length
	iLength = strlen(szBuffer);

	m_szOutput = (char*)realloc(m_szOutput, m_iLengthOutput + iNumberTabs + iLength + 1);

	// tabs
	for (i=0; i<iNumberTabs; i++)
	{
		m_szOutput[m_iLengthOutput + i] = '\t';
	}


	// buffer
	memcpy(m_szOutput + m_iLengthOutput + iNumberTabs, szBuffer, iLength);

	m_iLengthOutput += iLength + iNumberTabs;

	m_szOutput[m_iLengthOutput] = 0;
}




