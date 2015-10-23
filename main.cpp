////////////////////////////////////////////////
//                                            //
//          ObjG++ by James Hobson            //
//                 Based on:                  //
//             Code Converter by:             //
//        Copyright Sean O'Connor 2015        //
//           sean@windowsgames.co.uk          //
//           www.windowsgames.co.uk           //
//                                            //
////////////////////////////////////////////////


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "Node.h"
#include "ReadObjectiveC.h"
#include "Write.h"

#include "WriteTree.h"
#include "WriteCPlusPlus.h"


//Defines
#define MAX_LOADSTRING 100

//Global Variables
char		g_szHomeDirectory[FILENAME_MAX];

int main () {
	
	/////////////////////////////////////////////////
	//        get current working directory        //
	/////////////////////////////////////////////////
	
	getcwd(g_szHomeDirectory, FILENAME_MAX); //Sets g_szHomeDirectory
	
	if (g_szHomeDirectory[strlen(g_szHomeDirectory)-1] != '/') //checks for a '/' at the end
	{
		strcat(g_szHomeDirectory, "/"); //if there isn't a slash, it adds one
	}
	printf("%s\n", g_szHomeDirectory);
	
	Node*	pRoot;
	int  	bSuccess;
	char	szFilename[FILENAME_MAX];
	
	strcpy(szFilename, "TestClass"); //Hardcode TestClass for now...
	
	ReadObjectiveC*	pRead;
	
	pRead = new ReadObjectiveC(g_szHomeDirectory);
	pRoot = pRead->Input(g_szHomeDirectory, szFilename, &bSuccess);
	
		if (bSuccess == 0)
	{
		printf("Didn't match tags");
	}
	
	else {
		printf("\nDONE READ\n");
	}
	
	//MADE IT HERE
	
	Write* pWrite;

	pWrite = new WriteTree;
	pWrite->Output(pRoot, g_szHomeDirectory, szFilename);
	printf("\nDONE TREE\n");
	delete pWrite;

	pWrite = new WriteCPlusPlus;
	pWrite->Output(pRoot, g_szHomeDirectory, szFilename);
	delete pWrite;

	delete pRoot;
	
}