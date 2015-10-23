////////////////////////////////////////////////
//                                            //
//             WriteCPlusPlus.cpp             //
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

#include "WriteCPlusPlus.h"


/////////////////////////////////
//        Alloc/Dealloc        //
/////////////////////////////////

WriteCPlusPlus::WriteCPlusPlus()
{
}


WriteCPlusPlus::~WriteCPlusPlus()
{
}


char* WriteCPlusPlus::getFileExtension()
{
	return (char*)".cpp";
}


/////////////////////////////////
//        Output as C++        //
/////////////////////////////////

void WriteCPlusPlus::recurseNode(Node* pCurrent, int iNumberTabs)
{
	NodeFunctionHeader*				pNodeFunctionHeader;
	NodeClassName*					pNodeClassName;
	NodeFunctionHeaderParameter*	pNodeFunctionHeaderParameter;
	int		i;
	int	bProtectiveBracketsNeeded;


	////////////////////////////////////
	//        output this node        //
	////////////////////////////////////

	switch (pCurrent->getType())
	{
		// no action needed
		case TYPE_ROOT:
			this->recurseNodesChildren(pCurrent, iNumberTabs);

			return;

		case TYPE_CLASS_IMPLEMENTATION:
			// recurse through children
			this->recurseNodesChildren(pCurrent, iNumberTabs);

			return;

		// comment
		case TYPE_COMMENT:
			this->appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput(pCurrent->getToken());

			// comment never has children so just return
			return;

		// #
		case TYPE_HASH_IMPORT:
		case TYPE_HASH_INCLUDE:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#include");									// change "#import" to "#include"
			appendToOutput(pCurrent->getToken());
			return;

		case TYPE_HASH_DEFINE:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#define");
			appendToOutput(pCurrent->getToken());
			return;

		case TYPE_HASH_PRAGMA:
			/*								// just remove PRAGMAs
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#pragma");
			appendToOutput(pCurrent->getToken());
			*/
			return;

		case TYPE_HASH_IF:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#if");
			appendToOutput(pCurrent->getToken());
			return;

		case TYPE_HASH_ELSE:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#define");
			appendToOutput(pCurrent->getToken());
			return;

		case TYPE_HASH_ENDIF:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#endif");
			appendToOutput(pCurrent->getToken());
			return;

		case TYPE_HASH_IFDEF:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#ifdef");
			appendToOutput(pCurrent->getToken());
			return;

		case TYPE_HASH_IFNDEF:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#ifndef");
			appendToOutput(pCurrent->getToken());
			return;


		/////////////////////////////////////
		//        functions headers        //
		/////////////////////////////////////

		case TYPE_FUNCTION:
			appendToOutput(pCurrent->getPrecedingWhitespace());

			this->recurseNodesChildren(pCurrent, iNumberTabs);

			return;

		case TYPE_FUNCTION_HEADER:
			appendToOutput(pCurrent->getPrecedingWhitespace());

			pNodeFunctionHeader = (NodeFunctionHeader*)pCurrent;
		
			if (strcmp(pNodeFunctionHeader->getFunctionHeaderName(), "init") == 0)
			{
				////////////////////////
				//        init        //
				////////////////////////

				// class name
				pNodeClassName = (NodeClassName*)pNodeFunctionHeader->getParent()->getParent();
				appendToOutput(pNodeClassName->getToken());
				appendToOutput("::");
				appendToOutput(pNodeClassName->getToken());

				// function name
				appendToOutput("(");
			}
			else if (strcmp(pNodeFunctionHeader->getFunctionHeaderName(), "dealloc") == 0)
			{
				///////////////////////////
				//        dealloc        //
				///////////////////////////

				// class name
				pNodeClassName = (NodeClassName*)pNodeFunctionHeader->getParent()->getParent();
				appendToOutput(pNodeClassName->getToken());
				appendToOutput("::~");
				appendToOutput(pNodeClassName->getToken());

				// function name
				appendToOutput("(");
			}
			else
			{
				// return type
				appendToOutput(pNodeFunctionHeader->getFunctionHeaderReturnType());
				appendToOutput(" ");

				// class name
				pNodeClassName = (NodeClassName*)pNodeFunctionHeader->getParent()->getParent();
				appendToOutput(pNodeClassName->getToken());
				appendToOutput("::");

				// function name
				appendToOutput(pNodeFunctionHeader->getFunctionHeaderName());
				appendToOutput("(");
			}

			// if there are no function header parameters then close up the bracket now, otherwise the last parameter will do this
			if (pNodeFunctionHeader->getNumberOfFunctionHeaderParameters() == 0)
			{
				appendToOutput(")");
			}

			this->recurseNodesChildren(pCurrent, iNumberTabs);
			
			return;

		case TYPE_FUNCTION_HEADER_PARAMETER:
			appendToOutput(pCurrent->getPrecedingWhitespace());

			pNodeFunctionHeaderParameter = (NodeFunctionHeaderParameter*)pCurrent;
			
			appendToOutput(pNodeFunctionHeaderParameter->getReturnType());
			appendToOutput(" ");
			appendToOutput(pNodeFunctionHeaderParameter->getParameterName());

			pNodeFunctionHeader = (NodeFunctionHeader*)pCurrent->getParent();

			if (pNodeFunctionHeaderParameter->getParameterNumber() != pNodeFunctionHeader->getNumberOfFunctionHeaderParameters() - 1)
			{
				appendToOutput(",");
			}
			else
			{
				appendToOutput(")");
			}

			return;


		////////////////////////
		//        Code        //
		////////////////////////

		// { }
		case TYPE_FUNCTION_CODE_BLOCK:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			
			appendToOutput("{");
			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutput("}");

			return;

		// []
		case TYPE_ARRAY:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			
			appendToOutput("[");
			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutput("]");

			return;

		// ( )
		case TYPE_FUNCTION_CODE_BRACKET:					
			appendToOutputWithTabs("<BRACKET>\n", iNumberTabs);
			this->recurseNodesChildren(pCurrent, iNumberTabs);
			appendToOutputWithTabs("</BRACKET>\n", iNumberTabs);

			return;

		// method call [ xxx]
		case TYPE_METHOD_CALL:
			appendToOutput(pCurrent->getPrecedingWhitespace());

			if (strcmp(pCurrent->getToken(), "init") == 0
			 && pCurrent->getNumberChildren() > 0
			 && pCurrent->getChild(0)->getNumberChildren() > 0
			 && pCurrent->getChild(0)->getChild(0)->getNumberChildren() > 0
			 && strcmp(pCurrent->getChild(0)->getChild(0)->getToken(), "alloc") == 0)
			{
				//////////////////////////////
				//        alloc init        //
				//////////////////////////////

				appendToOutput("new ");

				appendToOutput(pCurrent->getChild(0)->getChild(0)->getChild(0)->getChild(0)->getToken());
			}
			else if (strcmp(pCurrent->getToken(), "release") == 0)
			{
				///////////////////////////
				//        release        //
				///////////////////////////

				appendToOutput("free ");

				recurseNode(pCurrent->getChild(0), iNumberTabs);
			}
			else
			{
				///////////////////////////////
				//        method call        //
				///////////////////////////////

				appendToOutput(pCurrent->getChild(0)->getPrecedingWhitespace());

				// if the object is being casted then C++ needs a pair of brackets around the whole cast and object
				bProtectiveBracketsNeeded = 0;

				if (pCurrent->getChild(0)->getNumberChildren() > 0
				 && pCurrent->getChild(0)->getChild(0)->getType() == TYPE_FUNCTION_CODE_TOKEN
				 && strcmp(pCurrent->getChild(0)->getChild(0)->getToken(), "(") == 0)
				{
					bProtectiveBracketsNeeded = 1;
				}

				if (bProtectiveBracketsNeeded == 1)
				{
					appendToOutput("(");
					recurseNode(pCurrent->getChild(0), iNumberTabs);
					appendToOutput(")");
				}
				else
				{
					recurseNode(pCurrent->getChild(0), iNumberTabs);
				}

				if (((NodeMethodCall*)pCurrent)->getIsStatic() == 1)
				{
					appendToOutput("::");
				}
				else
				{
					appendToOutput("->");
				}

				// method
				appendToOutput(pCurrent->getToken());

				appendToOutput("(");

				if (pCurrent->getNumberChildren() > 1)
				{
					//////////////////////////////////////////
					//        method call paramaters        //
					//////////////////////////////////////////

					for (i=1; i<pCurrent->getNumberChildren(); i++)
					{
						recurseNode(pCurrent->getChild(i), iNumberTabs);
					}
				}

				appendToOutput(")");
			}

			return;

		// method call object [xxx ]
		case TYPE_METHOD_OBJECT:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			this->recurseNodesChildren(pCurrent, iNumberTabs);

			return;

		case TYPE_METHOD_PARAMETER:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			this->recurseNodesChildren(pCurrent, iNumberTabs);

			NodeMethodCall* pNodeMethodCall;
			NodeMethodParameter* pNodeMethodParameter;

			pNodeMethodCall = (NodeMethodCall*)pCurrent->getParent();
			pNodeMethodParameter = (NodeMethodParameter*)pCurrent;

			if (pNodeMethodParameter->getMethodCallParameterNumber() != pNodeMethodCall->getNumberOfMethodCallParameters() - 1)
			{
				appendToOutput(",");
			}

			return;

		// just code
		case TYPE_FUNCTION_CODE_TOKEN:
			appendToOutput(pCurrent->getPrecedingWhitespace());

			// change 'self' to 'this'
			if (strcmp(pCurrent->getToken(), "self") == 0)
			{
				appendToOutput("this");
			}
			else
			{
				appendToOutput(pCurrent->getToken());
			}

			return;
	}


	///////////////////////////////////////////////
	//        output this node's children        //
	///////////////////////////////////////////////

	this->recurseNodesChildren(pCurrent, iNumberTabs);
}

/*
void WriteCPlusPlus::recurseNode2(Node* pCurrent, int iNumberTabs)
{
	int					i;
	int					iNumberChildren;
	NodeFunctionHeader*				pNodeFunctionHeader;
	NodeClassName*					pNodeClassName;
	NodeFunctionHeaderParameter*	pNodeFunctionHeaderParameter;
	int	bProtectiveBracketsNeeded;


	////////////////////////////////////
	//        output this node        //
	////////////////////////////////////

	switch (pCurrent->getType())
	{
		// no action needed
		case TYPE_ROOT:
		case TYPE_CLASS_IMPLEMENTATION:
			// recurse through children
			for (i=0; i<pCurrent->getNumberChildren(); i++)
			{
				recurseNode(pCurrent->getChild(i), iNumberTabs);
			}

			return;

		// comment
		case TYPE_COMMENT:
			this->appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput(pCurrent->getToken());
			appendToOutput("\n");

			// comment never has children so just return
			return;


		/////////////////////////////////////
		//        functions headers        //
		/////////////////////////////////////

		case TYPE_FUNCTION_HEADER:
			pNodeFunctionHeader = (NodeFunctionHeader*)pCurrent;
		
			appendToOutput(pNodeFunctionHeader->getPrecedingWhitespace());

			//printf(pNodeFunctionHeader->getFunctionHeaderName());

			if (strcmp(pNodeFunctionHeader->getFunctionHeaderName(), "init") == 0)
			{
				////////////////////////
				//        init        //
				////////////////////////

				// class name
				pNodeClassName = (NodeClassName*)pNodeFunctionHeader->getParent();
				appendToOutput(pNodeClassName->getToken());
				appendToOutput("::");
				appendToOutput(pNodeClassName->getToken());

				// function name
				appendToOutput("(");
			}
			else if (strcmp(pNodeFunctionHeader->getFunctionHeaderName(), "dealloc") == 0)
			{
				///////////////////////////
				//        dealloc        //
				///////////////////////////

				// class name
				pNodeClassName = (NodeClassName*)pNodeFunctionHeader->getParent();
				appendToOutput(pNodeClassName->getToken());
				appendToOutput("::~");
				appendToOutput(pNodeClassName->getToken());

				// function name
				appendToOutput("(");
			}
			else
			{
				// return type
				appendToOutput(pNodeFunctionHeader->getFunctionHeaderReturnType());
				appendToOutput(" ");

				// class name
				pNodeClassName = (NodeClassName*)pNodeFunctionHeader->getParent();
				appendToOutput(pNodeClassName->getToken());
				appendToOutput("::");

				// function name
				appendToOutput(pNodeFunctionHeader->getFunctionHeaderName());
				appendToOutput("(");
			}

			break;

		case TYPE_FUNCTION_HEADER_PARAMETER:
			pNodeFunctionHeaderParameter = (NodeFunctionHeaderParameter*)pCurrent;
			
			appendToOutput(pNodeFunctionHeaderParameter->getReturnType());
			appendToOutput(" ");
			appendToOutput(pNodeFunctionHeaderParameter->getParameterName());

			pNodeFunctionHeader = (NodeFunctionHeader*)pCurrent->getParent();

			if (pNodeFunctionHeaderParameter->getParameterNumber() != pNodeFunctionHeader->getNumberOfFunctionHeaderParameters() - 1)
			{
				appendToOutput(", ");
			}

			break;


		////////////////////////
		//        Code        //
		////////////////////////

		// { }
		case TYPE_FUNCTION_CODE_BLOCK:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			
			appendToOutput("{");

			for (i=0; i<pCurrent->getNumberChildren(); i++)
			{
				recurseNode(pCurrent->getChild(i), iNumberTabs);
			}

			appendToOutput("}");

			return;

		// []
		case TYPE_ARRAY:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			
			appendToOutput("[");

			for (i=0; i<pCurrent->getNumberChildren(); i++)
			{
				recurseNode(pCurrent->getChild(i), iNumberTabs);
			}

			appendToOutput("]");

			return;

		// ( )
		case TYPE_FUNCTION_CODE_BRACKET:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			
			appendToOutput("(");

			for (i=0; i<pCurrent->getNumberChildren(); i++)
			{
				recurseNode(pCurrent->getChild(i), iNumberTabs);
			}

			appendToOutput(")");

			return;

		// method call [ xxx]
		case TYPE_METHOD_CALL:
			appendToOutput(pCurrent->getPrecedingWhitespace());

			////////////////////////////////////
			//        [[  alloc] init]        //
			////////////////////////////////////

			if (pCurrent->getNumberChildren() > 1
			 && pCurrent->getChild(0)->getNumberChildren() > 0
			 && strcmp(pCurrent->getChild(0)->getChild(1)->getToken(), "alloc") == 0
			 && strcmp(pCurrent->getChild(1)->getToken(), "init") == 0)
			{
				appendToOutput("new ");
				appendToOutput(pCurrent->getChild(0)->getChild(0)->getToken());

				return;
			}


			/////////////////////////////
			//        [ delete]        //
			/////////////////////////////

			if (strcmp(pCurrent->getChild(1)->getToken(), "release") == 0)
			{
				appendToOutput("delete ");
				recurseNode(pCurrent->getChild(0), iNumberTabs);

				return;
			}


			//////////////////////////
			//        others        //
			//////////////////////////

			// object
			recurseNode(pCurrent->getChild(0), iNumberTabs);

			// method
			recurseNode(pCurrent->getChild(1), iNumberTabs);

			// parameters
			appendToOutput("(");

			if (pCurrent->getNumberChildren() >= 3)
			{
				for (i=2; i<pCurrent->getNumberChildren(); i++)
				{
					recurseNode(pCurrent->getChild(i), iNumberTabs);

					// add a comma if it's not the last parameter
					if (i < pCurrent->getNumberChildren() - 1)
					{
						appendToOutput(", ");
					}
				}
			}

			appendToOutput(")");

			return;

		// method call object [xxx ]
		case TYPE_METHOD_OBJECT:
			appendToOutput(pCurrent->getPrecedingWhitespace());

			bProtectiveBracketsNeeded = 0;

			if (pCurrent->getNumberChildren() > 0
			 && strcmp(pCurrent->getChild(0)->getToken(), "(") == 0)
			{
				bProtectiveBracketsNeeded = 1;
			}

			if (strcmp(pCurrent->getToken(), "self") == 0)
			{
				// change 'self' to 'this'
				appendToOutput("this->");
			}
			else if (strcmp(pCurrent->getToken(), "super") == 0)
			{
				// change 'super' to 'SuperClass::'
				//appendToOutput(GetClassName(pCurrent));		// TODO: get the super class
				appendToOutput("SuperClass");					// TODO: get the super class
				appendToOutput("::");
			}
			else
			{
				// if the object is being casted then C++ needs a pair of brackets around the whole cast and object
				// output Object
				if (bProtectiveBracketsNeeded == 1)
				{
					appendToOutput("(");
				}

				for (i=0; i<pCurrent->getNumberChildren(); i++)
				{
					recurseNode(pCurrent->getChild(i), iNumberTabs);
				}

				if (bProtectiveBracketsNeeded == 1)
				{
					appendToOutput(")");
				}

				appendToOutput("->");
			}

			return;

		// just code
		case TYPE_FUNCTION_CODE_TOKEN:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput(pCurrent->getToken());

			return;


		/////////////////////
		//        #        //
		/////////////////////

		case TYPE_HASH_IMPORT:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#include");									// change "#import" to "#include"
			appendToOutput(pCurrent->getToken());
			appendToOutput("\n");
			return;

		case TYPE_HASH_DEFINE:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#define");
			appendToOutput(pCurrent->getToken());
			appendToOutput("\n");
			return;

		case TYPE_HASH_PRAGMA:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#pragma");
			appendToOutput(pCurrent->getToken());
			appendToOutput("\n");
			return;

		case TYPE_HASH_IF:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#if");
			appendToOutput(pCurrent->getToken());
			appendToOutput("\n");
			return;

		case TYPE_HASH_ELSE:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#define");
			appendToOutput(pCurrent->getToken());
			appendToOutput("\n");
			return;

		case TYPE_HASH_ENDIF:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#endif");
			appendToOutput(pCurrent->getToken());
			appendToOutput("\n");
			return;

		case TYPE_HASH_IFDEF:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#ifdef");
			appendToOutput(pCurrent->getToken());
			appendToOutput("\n");
			return;

		case TYPE_HASH_IFNDEF:
			appendToOutput(pCurrent->getPrecedingWhitespace());
			appendToOutput("#ifndef");
			appendToOutput(pCurrent->getToken());
			appendToOutput("\n");
			return;
	}


	///////////////////////////////////////////////
	//        output this node's children        //
	///////////////////////////////////////////////

	iNumberChildren = pCurrent->getNumberChildren();

	for (i=0; i<iNumberChildren; i++)
	{
		recurseNode(pCurrent->getChild(i), iNumberTabs);
	}


	/////////////////////////////
	//        finish up        //
	/////////////////////////////

	switch (pCurrent->getType())
	{
		// function headers
		case TYPE_FUNCTION_HEADER:
			appendToOutput(")");
			break;
	}
}
*/


/*
char* WriteCPlusPlus::GetClassName(Node* pThisNode)
{
	NodeClassName*	pNodeClassName;

	while (pThisNode->getType() != TYPE_CLASS_IMPLEMENTATION)
	{
		pThisNode = pThisNode->getParent();

		if (pThisNode == NULL)
		{
			return "ERROR";
		}
	}

	pNodeClassName = (NodeClassName*)pThisNode;

	return pNodeClassName->getToken();
}
*/


