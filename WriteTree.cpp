////////////////////////////////////////////////
//                                            //
//              WriteTree.cpp                 //
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

#include "WriteTree.h"


/////////////////////////////////
//        Alloc/Dealloc        //
/////////////////////////////////

WriteTree::WriteTree()
{
}


WriteTree::~WriteTree()
{
}


char* WriteTree::getFileExtension()
{
	return (char*)".txt";
}


//////////////////////////////////
//        Output as Tree        //
//////////////////////////////////

void WriteTree::recurseNode(Node* pCurrent, int iNumberTabs)
{
	NodeFunctionHeader*				pNodeFunctionHeader;
	NodeClassName*					pNodeClassName;
	NodeFunctionHeaderParameter*	pNodeFunctionHeaderParameter;


	////////////////////////////////////
	//        output this node        //
	////////////////////////////////////
	printf("here?\n");
	switch (pCurrent->getType())
	{
		// no action needed
		case TYPE_ROOT:
			this->recurseNodesChildren(pCurrent, iNumberTabs);

			return;

		case TYPE_CLASS_IMPLEMENTATION:
			// recurse through children
			appendToOutputWithTabs("<IMPLEMENTATION>name=\"", iNumberTabs);

			pNodeClassName = (NodeClassName*)pCurrent;
			appendToOutput(pNodeClassName->getToken());
			appendToOutput("\"\n");

			this->recurseNodesChildren(pCurrent, iNumberTabs);

			appendToOutputWithTabs("</IMPLEMENTATION>\n", iNumberTabs);

			return;

		// comment
		case TYPE_COMMENT:
			appendToOutputWithTabs("<COMMENT>\n", iNumberTabs);

			// comment never has children so just return
			return;

		case TYPE_HASH_IMPORT:
			appendToOutputWithTabs("<#IMPORT>", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutputWithTabs("</#IMPORT>\n", iNumberTabs);

			return;

		case TYPE_HASH_INCLUDE:
			appendToOutputWithTabs("<#INCLUDE>", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutputWithTabs("</#INCLUDE>\n", iNumberTabs);

			return;

		case TYPE_HASH_DEFINE:
			appendToOutputWithTabs("<#DEFINE>", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutputWithTabs("</#DEFINE>\n", iNumberTabs);

			return;

		case TYPE_HASH_PRAGMA:
			appendToOutputWithTabs("<#PRAGMA>", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutputWithTabs("</#PRAGMA>\n", iNumberTabs);

			return;

		case TYPE_HASH_IF:
			appendToOutputWithTabs("<#IF>", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutputWithTabs("</#IF>\n", iNumberTabs);

			return;

		case TYPE_HASH_ELSE:
			appendToOutputWithTabs("<#ELSE>", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutputWithTabs("</#ELSE>\n", iNumberTabs);

			return;

		case TYPE_HASH_ENDIF:
			appendToOutputWithTabs("<#ENDIF>", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutputWithTabs("</#ENDIF>\n", iNumberTabs);

			return;

		case TYPE_HASH_IFDEF:
			appendToOutputWithTabs("<#IFDEF>", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutputWithTabs("</#IFDEF>\n", iNumberTabs);

			return;

		case TYPE_HASH_IFNDEF:
			appendToOutputWithTabs("<#IFNDEF>", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutputWithTabs("</#IFNDEF>\n", iNumberTabs);

			return;


		/////////////////////////////////////
		//        functions headers        //
		/////////////////////////////////////

		case TYPE_FUNCTION:
			appendToOutputWithTabs("<FUNCTION>\n", iNumberTabs);
			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutputWithTabs("</FUNCTION>\n", iNumberTabs);

			return;

		case TYPE_FUNCTION_HEADER:
			pNodeFunctionHeader = (NodeFunctionHeader*)pCurrent;
		
			//LOG(pNodeFunctionHeader->getFunctionHeaderName());

			if (strcmp(pNodeFunctionHeader->getFunctionHeaderName(), "init") == 0)
			{
				////////////////////////
				//        init        //
				////////////////////////

				pNodeClassName = (NodeClassName*)pNodeFunctionHeader->getParent();

				appendToOutputWithTabs("<FUNCTION_HEADER>name=\"INIT\"", iNumberTabs);
				appendToOutput("\n");
			}
			else if (strcmp(pNodeFunctionHeader->getFunctionHeaderName(), "dealloc") == 0)
			{
				///////////////////////////
				//        dealloc        //
				///////////////////////////

				pNodeClassName = (NodeClassName*)pNodeFunctionHeader->getParent();
				
				appendToOutputWithTabs("<FUNCTION_HEADER>name=\"DEALLOC\"", iNumberTabs);
				appendToOutput("\n");
			}
			else
			{
				////////////////////////////
				//        function        //
				////////////////////////////

				pNodeClassName = (NodeClassName*)pNodeFunctionHeader->getParent();
				
				appendToOutputWithTabs("<FUNCTION_HEADER>name=\"", iNumberTabs);
				appendToOutput(pNodeFunctionHeader->getParent()->getParent()->getToken());
				appendToOutput("::");
				appendToOutput(pNodeFunctionHeader->getFunctionHeaderName());
				appendToOutput("\", returns=\"");
				appendToOutput(pNodeFunctionHeader->getFunctionHeaderReturnType());
				appendToOutput("\"\n");
			}

			this->recurseNodesChildren(pCurrent, iNumberTabs);

			appendToOutputWithTabs("</FUNCTION_HEADER>\n", iNumberTabs);

			return;

			break;

		case TYPE_FUNCTION_HEADER_PARAMETER:
			pNodeFunctionHeaderParameter = (NodeFunctionHeaderParameter*)pCurrent;

			appendToOutputWithTabs("<PARAMETER>", iNumberTabs);

			appendToOutput("variable=\"");
			appendToOutput(pNodeFunctionHeaderParameter->getParameterName());
			appendToOutput("\", type=\"");
			appendToOutput(pNodeFunctionHeaderParameter->getReturnType());

			appendToOutput("\"</PARAMETER>\n");

			break;


		////////////////////////
		//        Code        //
		////////////////////////

		// { }
		case TYPE_FUNCTION_CODE_BLOCK:
			appendToOutputWithTabs("<CODE_BOCK>\n", iNumberTabs);
			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutputWithTabs("</CODE_BOCK>\n", iNumberTabs);

			return;

		// []
		case TYPE_ARRAY:
			appendToOutputWithTabs("<ARRAY>\n", iNumberTabs);
			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutputWithTabs("</ARRAY>\n", iNumberTabs);

			return;

		// ( )
		case TYPE_FUNCTION_CODE_BRACKET:					
			appendToOutputWithTabs("<BRACKET>\n", iNumberTabs);
			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutputWithTabs("</BRACKET>\n", iNumberTabs);

			return;

		// method call [ xxx]
		case TYPE_METHOD_CALL:
			appendToOutputWithTabs("<CALL>function=\"", iNumberTabs);
			appendToOutput(pCurrent->getToken());
			appendToOutput("\"\n");

			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutputWithTabs("</CALL>\n", iNumberTabs);

			return;

		// method call object [xxx ]
		case TYPE_METHOD_OBJECT:
			appendToOutputWithTabs("<OBJECT>\n", iNumberTabs);

			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutputWithTabs("</OBJECT>\n", iNumberTabs);

			/*
			if (strcmp(pCurrent->getToken(), "self") == 0)
			{
				// change 'self' to 'this'
				appendToOutput("this->");
			}
			else if (strcmp(pCurrent->getToken(), "super") == 0)
			{
				// change 'super' to 'BaseClass::'
				//appendToOutput(GetClassName(pCurrent));		// TODO: get the base class

				appendToOutput("BaseClass");		// TODO: get the base class
				appendToOutput("::");
			}
			else
			{
				//appendToOutput(pCurrent->getToken());
				for (i=0; i<pCurrent->getNumberChildren(); i++)
				{
					recurseNode(pCurrent->getChild(i), iNumberTabs);
				}

				appendToOutput("->");
			}
			*/

			return;

		case TYPE_METHOD_PARAMETER:
			appendToOutputWithTabs("<PARAMETER>\n", iNumberTabs);
			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutputWithTabs("</PARAMETER>\n", iNumberTabs);

			return;

		// just code
		case TYPE_FUNCTION_CODE_TOKEN:
			//if (strlen(pCurrent->getToken()) > 0)
			{
				appendToOutputWithTabs("<CODE>", iNumberTabs);
				appendToOutput(pCurrent->getToken());
				appendToOutput("</CODE>\n");
			}

			return;
	}


	///////////////////////////////////////////////
	//        output this node's children        //
	///////////////////////////////////////////////

	this->recurseNodesChildren(pCurrent, iNumberTabs);
}




