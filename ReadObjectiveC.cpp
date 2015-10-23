////////////////////////////////////////////////
//                                            //
//            ReadObjectiveC.cpp              //
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

#include <iostream>
#include <fstream>
#include <string.h>

#include "ReadObjectiveC.h"


// defines
#define TOKEN_TYPE_SINGLE_LINE_COMMENT			1
#define TOKEN_TYPE_CODE							2

using namespace std;

/////////////////////////////////
//        Alloc/Dealloc        //
/////////////////////////////////

ReadObjectiveC::ReadObjectiveC(char* szHomeDirectory) : Read(szHomeDirectory)
{
}


ReadObjectiveC::ReadObjectiveC()
{
}


ReadObjectiveC::~ReadObjectiveC()
{
}


////////////////////////////////////
//        Read Objective C        //
////////////////////////////////////

Node* ReadObjectiveC::Input(char* szHomeDirectory, char* szFile, int* pbSuccess)
{
	// load data
	//DWORD		dwRead;
	char		szFilename[FILENAME_MAX+1];
	int			iFileSize;
	char*		szData;
	


	////////////////////////////////
	//        read in file        //
	////////////////////////////////

	// create filename
	strcpy(szFilename, szHomeDirectory);
	strcat(szFilename, "Code/");
	strcat(szFilename, szFile);
	strcat(szFilename, ".m");

	//hFile = CreateFile(szFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ifstream	hFile (szFilename);
	printf ("Reading From:%s\n", szFilename);

	

	// allocate memory
	//iFileSize = GetFileSize(hFile, NULL);
	
	ifstream in(szFilename);
	string contents((std::istreambuf_iterator<char>(in)), 
		istreambuf_iterator<char>());

	const char* Data = contents.c_str();
	int size = contents.length();
	
	szData = (char*)malloc(size+1);
	strncpy (szData, Data, size);
	
	
	
	hFile.close();

	// read data
	//ReadFile(hFile, szData, sizeof(char) * iFileSize, &dwRead, NULL); 

	// null terminate it
	//strcat(szData, -1);
	
	printf("%s\n %d", Data, size);

	// close file
	//CloseHandle(hFile);


	//////////////////////////////
	//        parse data        //
	//////////////////////////////

	Node*	pRoot;
	Node*	pCurrent;
	Node*	pChild;

	pRoot = new Node;
	pRoot->setType(TYPE_ROOT);


	char	szToken[10024];
	char	szLeadingWhitespace[10024];
	int		iTokenType;
						
	NodeMethodCall*					pMethodCall;
	NodeMethodObject*				pMethodObject;
	NodeMethodParameter*			pMethodParameter;

	NodeFunctionCodeToken*			pFunctionCodeToken;
	NodeFunctionCodeBlock*			pFunctionCodeBlock;

	NodeArray*						pArray;	
	
	// parse 
	pCurrent = pRoot;

	m_szCurrent = szData;
	
	iFileSize = size;

	while (m_szCurrent < szData + iFileSize)
	{
		iTokenType = getNextObjectiveCToken(&m_szCurrent, szToken, szLeadingWhitespace, 1);

		printf("%s\n" ,szToken);

		switch (iTokenType)
		{
			case TOKEN_TYPE_SINGLE_LINE_COMMENT:
				/////////////////////////////
				//        <COMMENT>        //
				/////////////////////////////

				// create new child
				pChild = new Node;

				pChild->setType(TYPE_COMMENT);
				pChild->setPrecedingWhitespace(szLeadingWhitespace);
				pChild->setToken(szToken);

				// add it 
				if (pCurrent->getType() == TYPE_METHOD_PARAMETER)
				{
					pCurrent->getParent()->addChild(pChild);
				}
				else
				{
					pCurrent->addChild(pChild);
				}

				break;

			case TOKEN_TYPE_CODE:
				/////////////////////
				//        #        //
				/////////////////////

				if (strlen(szToken) > 0
				 && szToken[0] == '#')
				{
					
					this->addHashCommand(&pCurrent, szLeadingWhitespace, szToken);
					break;
				}
	
				//////////////////////////
				//        <ROOT>        //
				//////////////////////////

				if (pCurrent->getType() == TYPE_ROOT)
				{
					if (this->processRoot(&pCurrent, szToken, szLeadingWhitespace) == 1)
					{
						// made use of this token so break
						break;
					}
				}
				
				if (pCurrent->getParent() != NULL
				 && pCurrent->getParent()->getType() == TYPE_ROOT)
				{
					// found "@end"?
					if (strcmp(szToken, "@end") == 0)
					{
						// @end so move up a level
						pCurrent = pCurrent->getParent();

						// made use of this token so break
						break;
					}
				}


				////////////////////////////////////
				//        <IMPLEMENTATION>        //
				////////////////////////////////////

				if (pCurrent->getType() == TYPE_CLASS_IMPLEMENTATION)
				{
					if (this->processImplementation(&pCurrent, szToken, szLeadingWhitespace) == 1)
					{
						// made use of this token so break
						break;
					}
				}

				
				/////////////////////////////////////
				//        <FUNCTION_HEADER>        //
				/////////////////////////////////////

				if (pCurrent->getType() == TYPE_FUNCTION_HEADER)
				{
					if (this->processFunctionHeader(&pCurrent, szToken, szLeadingWhitespace) == 1)
					{
						// made use of this token so break
						break;
					}
				}


				////////////////////////////////////////////////////
				//        <TYPE_FUNCTION_HEADER_PARAMETER>        //
				////////////////////////////////////////////////////

				if (pCurrent->getType() == TYPE_FUNCTION_HEADER_PARAMETER)
				{
					if (this->processFunctionHeaderParameter(&pCurrent, szToken, szLeadingWhitespace) == 1)
					{
						// made use of this token so break
						break;
					}
				}

				/////////////////////////////////////
				//        <TYPE_CODE_BLOCK>        //
				/////////////////////////////////////

				if (pCurrent->getType() == TYPE_FUNCTION_CODE_BLOCK)
				{
					if (strcmp(szToken, "{") == 0)
					{
						pFunctionCodeBlock = new NodeFunctionCodeBlock;
						pFunctionCodeBlock->setPrecedingWhitespace(szLeadingWhitespace);
						pCurrent->addChild(pFunctionCodeBlock);
						pCurrent = pFunctionCodeBlock;

						break;
					}
					else if (strcmp(szToken, "}") == 0)
					{
						// finished the code block
						if (strlen(szLeadingWhitespace) > 0)
						{
							pFunctionCodeToken = new NodeFunctionCodeToken;
							pFunctionCodeToken->setPrecedingWhitespace(szLeadingWhitespace);
							pFunctionCodeToken->setToken("");
						}

						pCurrent->addChild(pFunctionCodeToken);

						pCurrent = pCurrent->getParent();

						// finished the <FUNCTION>?
						if (pCurrent->getType() == TYPE_FUNCTION)
						{
							pCurrent = pCurrent->getParent();
						}

						break;
					}
					else if (strcmp(szToken, "[") == 0)
					{
						/////////////////////
						//        [        //
						/////////////////////

						if (this->previousChildIsAnArrayName(pCurrent) == 1)
						{
							// array
							pArray = new NodeArray;
							pArray->setPrecedingWhitespace(szLeadingWhitespace);

							pCurrent->addChild(pArray);

							pCurrent = pArray;
						}
						else
						{
							// method call
							pMethodCall = new NodeMethodCall;			
							pMethodCall->setPrecedingWhitespace(szLeadingWhitespace);
							pCurrent->addChild(pMethodCall);
							pCurrent = pMethodCall;

							// create call object
							pMethodObject = new NodeMethodObject;
							pCurrent->addChild(pMethodObject);
							pCurrent = pMethodObject;
						}

						break;
					}
					else
					{
						// just some code
						pFunctionCodeToken = new NodeFunctionCodeToken;
						pFunctionCodeToken->setPrecedingWhitespace(szLeadingWhitespace);
						pFunctionCodeToken->setToken(szToken);

						pCurrent->addChild(pFunctionCodeToken);
					}

					break;
				}

				//////////////////////////////////////
				//        <TYPE_METHOD_CALL>        //
				//////////////////////////////////////

				if (pCurrent->getType() == TYPE_METHOD_CALL)
				{
					if (strcmp(szToken, "]") == 0)
					{
						// finished the method call
						pCurrent = pCurrent->getParent();
					}

					break;
				}


				////////////////////////////////////////
				//        <TYPE_METHOD_OBJECT>        //
				////////////////////////////////////////

				if (pCurrent->getType() == TYPE_METHOD_OBJECT)
				{
					if (strlen(szLeadingWhitespace) > 0)
					{
						// found method name so move back up into the <CALL>
						pCurrent = pCurrent->getParent();

						pMethodCall = (NodeMethodCall*)pCurrent;

						// is this a static method/super call?
						if (this->isStaticMethodCall(pMethodCall) == 1)
						{
							////////////////////////////////
							//        static class        //
							////////////////////////////////

							pMethodCall->setIsStatic(1);
						}
						else if (strcmp(pMethodCall->getChild(0)->getChild(0)->getToken(), "super") == 0)
						{
							/////////////////////////
							//        super        //
							/////////////////////////

							pMethodCall->getChild(0)->getChild(0)->setToken(m_pClassList->getSuperClassOfClass(pMethodCall->getParent()->getParent()->getParent()->getToken()));	
						
							pMethodCall->setIsStatic(1);		// fudge just show C++ output uses :: instead of ->
						}

						// is there a ':'?
						int		bParameterToFollow;

						bParameterToFollow = 0;

						if (strlen(szToken) > 0
						 && szToken[strlen(szToken) - 1] == ':')
						{
							// yes, so parameter will follow
							bParameterToFollow = 1;

							// remove ':'
							szToken[strlen(szToken) - 1] = 0;
						}

						// set/append to the function call name
						if (strlen(pMethodCall->getToken()) > 0)
						{
							pMethodCall->appendToken("_");
						}

						pMethodCall->appendToken(szToken);

						if (bParameterToFollow == 1)
						{
							pMethodCall->incrementNumberOfMethodCallParameters();

							pMethodParameter = new NodeMethodParameter;			
							pMethodParameter->setPrecedingWhitespace("");
							pMethodParameter->setMethodCallParameterNumber(pMethodCall->getNumberOfMethodCallParameters() - 1);

							pCurrent->addChild(pMethodParameter);
							pCurrent = pMethodParameter;
						}

						break;
					}
					else
					{
						// found a '[' within a method object?
						if (strcmp(szToken, "[") == 0)
						{
							// yes, so is it an array or start of a new method call?
							if (this->previousChildIsAnArrayName(pCurrent) == 0)
							{
								// method call
								pMethodCall = new NodeMethodCall;			
								pMethodCall->setPrecedingWhitespace(szLeadingWhitespace);
								pCurrent->addChild(pMethodCall);
								pCurrent = pMethodCall;

								// create call object
								pMethodObject = new NodeMethodObject;
								pCurrent->addChild(pMethodObject);
								pCurrent = pMethodObject;
							}
							else
							{
								// array
								pArray = new NodeArray;
								pArray->setPrecedingWhitespace(szLeadingWhitespace);

								pCurrent->addChild(pArray);

								pCurrent = pArray;
							}
						}
						else
						{
							pFunctionCodeToken = new NodeFunctionCodeToken;
							pFunctionCodeToken->setPrecedingWhitespace(szLeadingWhitespace);
							pFunctionCodeToken->setToken(szToken);

							pCurrent->addChild(pFunctionCodeToken);
						}

						break;
					}
				}


				///////////////////////////////////////////
				//        <TYPE_METHOD_PARAMETER>        //
				///////////////////////////////////////////

				if (pCurrent->getType() == TYPE_METHOD_PARAMETER)
				{
					if (strcmp(szToken, "]") == 0)
					{
						// finished the code block
						pCurrent = pCurrent->getParent();
						pCurrent = pCurrent->getParent();

						break;
					}
					else if (strlen(szToken) > 0
					 && szToken[strlen(szToken) - 1] == ':')
					{
						// started next parameter
						pCurrent = pCurrent->getParent();

						szToken[strlen(szToken) - 1] = 0;

						pMethodCall = (NodeMethodCall*)pCurrent;

						if (strlen(pMethodCall->getToken()) > 0)
						{
							pMethodCall->appendToken("_");
						}

						pMethodCall->appendToken(szToken);
						pMethodCall->incrementNumberOfMethodCallParameters();

						pMethodParameter = new NodeMethodParameter;			
						pMethodParameter->setPrecedingWhitespace(szLeadingWhitespace);
						pMethodParameter->setMethodCallParameterNumber(pMethodCall->getNumberOfMethodCallParameters() - 1);

						pCurrent->addChild(pMethodParameter);
						pCurrent = pMethodParameter;

						break;
					}
					else
					{
						// found a '[' within a method parameter
						if (strcmp(szToken, "[") == 0)
						{
							if (this->previousChildIsAnArrayName(pCurrent) == 0)
							{
								// method call
								pMethodCall = new NodeMethodCall;			
								pMethodCall->setPrecedingWhitespace(szLeadingWhitespace);
								pCurrent->addChild(pMethodCall);
								pCurrent = pMethodCall;

								// create call object
								pMethodObject = new NodeMethodObject;
								pCurrent->addChild(pMethodObject);
								pCurrent = pMethodObject;
							}
							else
							{
								// array
								pArray = new NodeArray;
								pArray->setPrecedingWhitespace(szLeadingWhitespace);

								pCurrent->addChild(pArray);

								pCurrent = pArray;
							}

							break;
						}
						else
						{
							pFunctionCodeToken = new NodeFunctionCodeToken;
							pFunctionCodeToken->setPrecedingWhitespace(szLeadingWhitespace);
							pFunctionCodeToken->setToken(szToken);

							pCurrent->addChild(pFunctionCodeToken);

							break;
						}
					}
				}


				////////////////////////////////
				//        <TYPE_ARRAY>        //
				////////////////////////////////

				if (pCurrent->getType() == TYPE_ARRAY)
				{
					if (strcmp(szToken, "]") == 0)
					{
						// finished the array
						pCurrent = pCurrent->getParent();
					}
					else
					{
						pFunctionCodeToken = new NodeFunctionCodeToken;
						pFunctionCodeToken->setPrecedingWhitespace(szLeadingWhitespace);
						pFunctionCodeToken->setToken(szToken);

						pCurrent->addChild(pFunctionCodeToken);
					}

					break;
				}

				printf("!!! LEFT OVER:\n");
				//printf(szToken);

				break;
		}
	}


	///////////////////////////////
	//        free memory        //
	///////////////////////////////

	free(szData);


	////////////////////////
	//        done        //
	////////////////////////

	// check
	if (pCurrent == pRoot)
	{
		printf("*****************************");
		printf("* SUCCESS pCurrent == pRoot *");
		printf("*****************************");

		*pbSuccess = 1;
	}
	else
	{
		printf("**************************");
		printf("* FAIL pCurrent != pRoot *");
		printf("**************************");

		*pbSuccess = 0;
	}

	// return root of tree
	return pRoot;
}


int ReadObjectiveC::isStaticMethodCall(NodeMethodCall* pMethodCall)
{
	// check there is a  
	if (pMethodCall->getNumberChildren() == 0
	 || pMethodCall->getChild(0)->getNumberChildren() == 0)
	{
		return 0;
	}

	// is this method object a classname?
	if (m_pClassList->isOnTheClassList(pMethodCall->getChild(0)->getChild(0)->getToken()) == 1)
	{
		return 1;
	}

	// no
	return 0;
}


int ReadObjectiveC::processRoot(Node** ppCurrent, char* szToken, char* szLeadingWhitespace)	// return 1 if dealt with
{
	NodeFunctionCodeToken*	pFunctionCodeToken;
	NodeClassName*			pClassName;

	
	//////////////////////////
	//        <ROOT>        //
	//////////////////////////

	if (strcmp(szToken, "@implementation") == 0)
	{
		///////////////////////////////////////////
		//        found "@implementation"        //
		///////////////////////////////////////////

		// add the whitespace
		if (strlen(szLeadingWhitespace) > 0)
		{
			pFunctionCodeToken = new NodeFunctionCodeToken;
			pFunctionCodeToken->setPrecedingWhitespace(szLeadingWhitespace);
			(*ppCurrent)->addChild(pFunctionCodeToken);
		}

		// create new child
		pClassName = new NodeClassName;

		// add it 
		(*ppCurrent)->addChild(pClassName);

		// move down to it (move back up again when the '@end' is reached)
		(*ppCurrent) = pClassName;
	}
	else
	{
		// add anything else as <CODE>
		pFunctionCodeToken = new NodeFunctionCodeToken;
		pFunctionCodeToken->setPrecedingWhitespace(szLeadingWhitespace);
		pFunctionCodeToken->setToken(szToken);

		(*ppCurrent)->addChild(pFunctionCodeToken);
	}

	return 1;
}


int ReadObjectiveC::processImplementation(Node** ppCurrent, char* szToken, char* szLeadingWhitespace)
{
	Node*				pNode;
	NodeFunctionHeader*	pFunctionHeader;

	// just whitespace
	if (strcmp((*ppCurrent)->getToken(), "") == 0)
	{
		(*ppCurrent)->setToken(szToken);

		return 1;
	}

	// found a function header?
	if (strcmp(szToken, "-") == 0
	 || strcmp(szToken, "+") == 0)
	{
		/////////////////////
		//        -        //
		/////////////////////

		// create <FUNCTION>
		pNode = new Node;
		pNode->setType(TYPE_FUNCTION);

		pNode->setPrecedingWhitespace(szLeadingWhitespace);

		(*ppCurrent)->addChild(pNode);
		(*ppCurrent) = pNode;

		// create <FUNCTION_HEADER>
		pFunctionHeader = new NodeFunctionHeader;

		(*ppCurrent)->addChild(pFunctionHeader);
		(*ppCurrent) = pFunctionHeader;

		// made use of this token
		return 1;
	}

	return 0;
}


int ReadObjectiveC::processFunctionHeader(Node** ppCurrent, char* szToken, char* szLeadingWhitespace)	// return 1 if dealt with
{
	NodeFunctionHeader*				pFunctionHeader;
	NodeFunctionHeaderParameter*	pFunctionHeaderParameter;
	NodeFunctionCodeBlock*			pFunctionCodeBlock;
	int	bParameterToFollow;

	pFunctionHeader = (NodeFunctionHeader*)*ppCurrent;

	////////////////////////////
	//        finished        //
	////////////////////////////

	if (strcmp(szToken, "{") == 0)
	{
		// finished function header so move back up
		*ppCurrent = (*ppCurrent)->getParent();

		// and add a code block
		pFunctionCodeBlock = new NodeFunctionCodeBlock;
		pFunctionCodeBlock->setPrecedingWhitespace(szLeadingWhitespace);
		(*ppCurrent)->addChild(pFunctionCodeBlock);
		(*ppCurrent) = pFunctionCodeBlock;

		return 1;
	}
	

	////////////////////////////////////////
	//        function return type        //
	////////////////////////////////////////

	// still creating the function's return type?
	if (pFunctionHeader->getFunctionHeaderReturnTypeCompleted() == 0)
	{
		if (strcmp(szToken, "(") == 0)
		{
			// ignore
		}
		else if (strcmp(szToken, ")") == 0)
		{
			// finished creating the return type now
			pFunctionHeader->setFunctionHeaderReturnTypeCompleted(1);
		}
		else
		{
			// append to the return type
			pFunctionHeader->appendFunctionHeaderReturnType(szLeadingWhitespace);
			pFunctionHeader->appendFunctionHeaderReturnType(szToken);
		}

		return 1;
	}


	/////////////////////////////////
	//        function name        //
	/////////////////////////////////

	// is there a ':'?
	bParameterToFollow = 0;

	if (strlen(szToken) > 0
	 && szToken[strlen(szToken) - 1] == ':')
	{
		// yes, so parameter will follow
		bParameterToFollow = 1;

		// remove ':'
		szToken[strlen(szToken) - 1] = 0;
	}

	// set/append to the function name
	if (strlen(pFunctionHeader->getFunctionHeaderName()) > 0)
	{
		pFunctionHeader->appendFunctionHeaderName("_");
	}

	pFunctionHeader->appendFunctionHeaderName(szToken);

	// if there was a ':' then a parameter is to follow
	if (bParameterToFollow == 1)
	{
		// new parameter
		pFunctionHeaderParameter = new NodeFunctionHeaderParameter;

		pFunctionHeaderParameter->setParameterNumber(pFunctionHeader->getNumberOfFunctionHeaderParameters());		
		pFunctionHeaderParameter->setPrecedingWhitespace(szLeadingWhitespace);
	
		pFunctionHeader->incrementNumberOfFunctionHeaderParameters();

		(*ppCurrent)->addChild(pFunctionHeaderParameter);

		(*ppCurrent) = pFunctionHeaderParameter;
	}

	return 1;
}


int ReadObjectiveC::processFunctionHeaderParameter(Node** ppCurrent, char* szToken, char* szLeadingWhitespace)	// return 1 if dealt with
{
	NodeFunctionHeaderParameter*	pFunctionHeaderParameter;

	pFunctionHeaderParameter = (NodeFunctionHeaderParameter*)*ppCurrent;

	if (pFunctionHeaderParameter->getReturnTypeCompleted() == 0)
	{
		// yes
		if (strcmp(szToken, "(") == 0)
		{
			// ignore
			return 1;
		}
		else if (strcmp(szToken, ")") == 0)
		{
			// finished creating the parameter's return type now
			pFunctionHeaderParameter->setReturnTypeCompleted(1);

			return 1;
		}
		else
		{
			// append to the return type
			pFunctionHeaderParameter->appendReturnType(szLeadingWhitespace);
			pFunctionHeaderParameter->appendReturnType(szToken);

			return 1;
		}
	}
	else
	{
		pFunctionHeaderParameter->setParameterName(szToken);

		// finished with this parameter so move back up into function header
		(*ppCurrent) = (*ppCurrent)->getParent();
	}

	return 1;
}


void ReadObjectiveC::addHashCommand(Node** ppCurrent, char* szLeadingWhitespace, char* szToken)
{
	Node*	pChild;
	char	szRestOfLine[10024];
	char	szFilename[FILENAME_MAX];

	pChild = new Node;

	pChild->setPrecedingWhitespace(szLeadingWhitespace);

	
	copyToEndOfLine(&m_szCurrent, szRestOfLine);
	
	pChild->setToken(szRestOfLine);

	////////////////////////////
	//        set type        //
	////////////////////////////

	// import/include
	if (strcmp(szToken, "#import") == 0)
	{
		pChild->setType(TYPE_HASH_IMPORT);

		if (this->extractFilenameFromRestOfLine(szRestOfLine, szFilename) == 1)
		{
			this->scanHeaderFileForClasses(szFilename);
		}
	}
	else if (strcmp(szToken, "#include") == 0)
	{
		pChild->setType(TYPE_HASH_INCLUDE);

		if (this->extractFilenameFromRestOfLine(szRestOfLine, szFilename) == 1)
		{
			this->scanHeaderFileForClasses(szFilename);
		}
	}

	// pragma
	else if (strcmp(szToken, "#pragma") == 0)
	{
		pChild->setType(TYPE_HASH_PRAGMA);
	}

	// define
	else if (strcmp(szToken, "#define") == 0)
	{
		pChild->setType(TYPE_HASH_DEFINE);
	}

	// conditional
	else if (strcmp(szToken, "#if") == 0)
	{
		pChild->setType(TYPE_HASH_IF);
	}
	else if (strcmp(szToken, "#else") == 0)
	{
		pChild->setType(TYPE_HASH_ELSE);
	}
	else if (strcmp(szToken, "#ifdef") == 0)
	{
		pChild->setType(TYPE_HASH_IFDEF);
	}
	else if (strcmp(szToken, "#ifndef") == 0)
	{
		pChild->setType(TYPE_HASH_IFNDEF);
	}
	else if (strcmp(szToken, "#endif") == 0)
	{
		pChild->setType(TYPE_HASH_ENDIF);
	}

	// add it 
	(*ppCurrent)->addChild(pChild);
}


int ReadObjectiveC::previousChildIsAnArrayName(Node* pThis)
{
	Node*	pPreviousChild;
	int		i;
	int		iLength;
	char*	szToken;

	// if this node has no children the previous node can't be an array!
	if (pThis->getNumberChildren() == 0)
	{
		return 0;
	}

	// get previous child
	pPreviousChild = pThis->getChild(pThis->getNumberChildren() - 1);

	// is previous child an array element?
	if (pPreviousChild->getType() == TYPE_ARRAY)
	{
		return 1;
	}

	if (pPreviousChild->getType() != TYPE_FUNCTION_CODE_TOKEN)
	{
		return 0;
	}

	// get its token string
	szToken = pPreviousChild->getToken();

	// is it an array name?
	iLength = strlen(szToken);

	if (strcmp(szToken, "]") == 0)
	{
		return 1;
	}

	for (i=0; i<iLength; i++)
	{
		if ((szToken[i] < 'a' || szToken[i] > 'z')
		 && (szToken[i] < 'A' || szToken[i] > 'Z')
		 && (szToken[i] < '0' || szToken[i] > '9')
		 && szToken[i] != '_')
		{
			// contains a non-array name character so it can't be an array name
			return 0;
		} 
	}
	
	// previous child is an array name
	return 1;
}


//////////////////////////////////////////////
//        Get next Objective C token        //
//                                          //
// - includes trailing ':'                  //
//////////////////////////////////////////////

int ReadObjectiveC::getNextObjectiveCToken(char** pszFrom, char* szToken, char* szPrecedingWhitespace, int bIncludeTrailingColon)
{
	int		iLength;


	////////////////////////////////////////////////
	//        collect preceding whitespace        //
	////////////////////////////////////////////////

	while ((**pszFrom) == ' '
	    || (**pszFrom) == '\t'
		|| (**pszFrom) == '\n')
	{
		// copy this whitespace
		*szPrecedingWhitespace = **pszFrom;

		// increment
		szPrecedingWhitespace ++;
		(*pszFrom) ++;
	}

	*szPrecedingWhitespace = 0;


	//////////////////////////////////
	//        get next token        //
	//////////////////////////////////

	*szToken = 0;

	// EOF?
	if (**pszFrom == 0)
	{
		return TOKEN_TYPE_CODE;
	}

	// single line comment
	if (((*pszFrom)[0]) == '/'
	 && ((*pszFrom)[1]) == '/')
	{
		copyToEndOfLine(pszFrom, szToken);

		return TOKEN_TYPE_SINGLE_LINE_COMMENT;
	}

	// multi-line comment
	if (((*pszFrom)[0]) == '/'
	 && ((*pszFrom)[1]) == '*')
	{
		while (((*pszFrom)[0]) != '*'
			|| ((*pszFrom)[1]) != '/')
		{
			*szToken = **pszFrom;

			szToken ++;
			(*pszFrom) ++;
		}

		*szToken = **pszFrom;

		szToken ++;
		(*pszFrom) ++;

		*szToken = **pszFrom;

		szToken ++;
		(*pszFrom) ++;

		// null terminate
		*szToken = 0;

		return TOKEN_TYPE_SINGLE_LINE_COMMENT;
	}

	// strings
	if ((**pszFrom) == '\"')
	{
		*szToken = **pszFrom;

		szToken ++;
		(*pszFrom) ++;

		iLength = 0;

		while ((**pszFrom) != '\"'
			|| ((**pszFrom) == '\"' && iLength != 0 && *((*pszFrom)-1) == '\\'))
		{
			*szToken = **pszFrom;

			szToken ++;
			(*pszFrom) ++;

			iLength ++;
		}

		*szToken = **pszFrom;

		szToken ++;
		(*pszFrom) ++;

		// null terminate
		*szToken = 0;

		return TOKEN_TYPE_CODE;
	}

	// two letter tokens
	if ((((*pszFrom)[0]) == '&' && ((*pszFrom)[1]) == '&')		// &&
	 || (((*pszFrom)[0]) == '|' && ((*pszFrom)[1]) == '|')		// ||
	 || (((*pszFrom)[0]) == '|' && ((*pszFrom)[1]) == '=')		// |=
	 || (((*pszFrom)[0]) == '&' && ((*pszFrom)[1]) == '='))		// &=
	{
		*szToken = **pszFrom;

		szToken ++;
		(*pszFrom) ++;

		*szToken = **pszFrom;

		szToken ++;
		(*pszFrom) ++;

		// null terminate
		*szToken = 0;

		return TOKEN_TYPE_CODE;
	}

	// get token
	*szToken = **pszFrom;

	// read in until token ends
	if ((*szToken >= 'a' && *szToken <= 'z') 
	 || (*szToken >= 'A' && *szToken <= 'Z')
	 || (*szToken >= '0' && *szToken <= '9')
	 || *szToken == '#'
	 || *szToken == '@'
	 || *szToken == '_')
	{
		szToken ++;
		(*pszFrom) ++;

		while (((**pszFrom) >= 'a' && (**pszFrom) <= 'z') 
			|| ((**pszFrom) >= 'A' && (**pszFrom) <= 'Z')
			|| ((**pszFrom) >= '0' && (**pszFrom) <= '9')
			|| (**pszFrom) == '_'
			|| (**pszFrom) == ':')									// read in final ':' after case statements and function parameters
		{
			*szToken = **pszFrom;

			szToken ++;
			(*pszFrom) ++;

			// end on a ':'
			if (*((*pszFrom) - 1) == ':')
			{
				break;
			}
		}
	}
	else
	{
		szToken ++;
		(*pszFrom) ++;
	}

	*szToken = 0;

	return TOKEN_TYPE_CODE;
}


void ReadObjectiveC::copyToEndOfLine(char** pszFrom, char* szBuffer)
{
	int		i;

	i = 0;

	// do until reach the end of line or end of file
	while ((**pszFrom) != 0
	    && (**pszFrom) != '\n')
	{
		// copy
		szBuffer[i] = **pszFrom;

		// increment
		(*pszFrom) ++;

		i++;
	}

	// null terminate
	szBuffer[i] = 0;
}


/////////////////////////////////////////////////////
//        Scan header files for class names        //
/////////////////////////////////////////////////////

int ReadObjectiveC::extractFilenameFromRestOfLine(char* szRestOfLine, char* szFile)
{
	char*	szFindStart;
	char*	szFindEnd;

	szFindStart = strstr(szRestOfLine, "\"");

	if (szFindStart != NULL)
	{
		szFindStart ++;

		szFindEnd = strstr(szFindStart, "\"");

		strncpy(szFile, szFindStart, szFindEnd - szFindStart);
		szFile[szFindEnd - szFindStart] = 0;

		return 1;
	}

	// failed - this should never happen
	strcpy(szFile, "");

	return 0;
}


void ReadObjectiveC::scanHeaderFileForClasses(char* szFile)
{
	/*/ load data
	HANDLE		hFile;
	DWORD		dwRead;
	char		szFilename[FILENAME_MAX+1];
	char		szHeaderFilename[FILENAME_MAX+1];
	DWORD		iFileSize;
	char*		szData;
	
	*/
	
	char		szFilename[FILENAME_MAX+1];
	char		szHeaderFilename[FILENAME_MAX+1];
	int			iFileSize;
	char*		szData;
	
	

	////////////////////////////////
	//        read in file        //
	////////////////////////////////

	// create filename
	strcpy(szFilename, m_szHomeDirectory);
	strcat(szFilename, "Code/");
	strcat(szFilename, szFile);
	
	ifstream	hFile (szFilename);

	//hFile = CreateFile(szFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 


	// allocate memory
	//iFileSize = GetFileSize(hFile, NULL);

	szData = (char*)malloc(sizeof(char) * (iFileSize+1));
	
	ifstream in(szFilename);
	string contents((std::istreambuf_iterator<char>(in)), 
		istreambuf_iterator<char>());

	const char* Data = contents.c_str();
	
	szData = (char*)malloc(sizeof(Data+1));
	strcpy (szData, Data);
	
	hFile.close();

	// null terminate it
	szData[iFileSize] = 0;


	// parse 
	char*	szLook;
	int		iTokenType;
	char	szToken[10024];
	char	szLeadingWhitespace[10024];
	int		iStatus;

#define STATUS_SEARCH				0
#define STATUS_FOUND_INTERFACE		1
#define STATUS_FOUND_CLASS			2
#define STATUS_FOUND_COLON			3
#define STATUS_FOUND_HEADER_FILE	4

	szLook = szData;

	iStatus = STATUS_SEARCH;

	while (szLook < szData + iFileSize)
	{
		iTokenType = getNextObjectiveCToken(&szLook, szToken, szLeadingWhitespace, 0);

		switch (iTokenType)
		{
			case TOKEN_TYPE_SINGLE_LINE_COMMENT:
				break;

			case TOKEN_TYPE_CODE:
				//printf(szToken);

				switch (iStatus)
				{
					case STATUS_SEARCH:
						// found a '@'?
						if (szToken[0] == '@')
						{
							// yes
							iStatus = STATUS_FOUND_INTERFACE;
						}
						else if (strcmp(szToken, "#include") == 0
							  || strcmp(szToken, "#import") == 0)
						{
							iStatus = STATUS_FOUND_HEADER_FILE;
						}

						break;

					case STATUS_FOUND_INTERFACE:
						// found class so add it to the list
						m_pClassList->addClassToList(szToken);

						iStatus = STATUS_FOUND_CLASS;

						break;		

					case STATUS_FOUND_CLASS:
						if (szToken[0] == ':')
						{
							iStatus = STATUS_FOUND_COLON;
						}
						else
						{
							iStatus = STATUS_SEARCH;
						}
						break;

					case STATUS_FOUND_COLON:
						m_pClassList->setSuperClassOfLastClassAddedTo(szToken);

						iStatus = STATUS_SEARCH;
						break;

					case STATUS_FOUND_HEADER_FILE:
						this->extractFilenameFromRestOfLine(szToken, szHeaderFilename);

						this->scanHeaderFileForClasses(szHeaderFilename);

						iStatus = STATUS_SEARCH;
						break;
				}

				break;
		}
	}

	free(szData);
}








