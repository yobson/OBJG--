// includes
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <iostream>

using namespace std;

#include "Node.h"


//////////////////////////////////////////
//        Constructor/destructor        //
//////////////////////////////////////////

Node::Node()
{
	// parent
	m_pParent = NULL;

	// type
	m_iType = 0;

	// preceeding whitespace
	m_szPreceedingWhitespace = (char*)malloc(sizeof(char));
	m_szPreceedingWhitespace[0] = 0;

	// token
	m_szToken = (char*)malloc(sizeof(char));
	m_szToken[0] = 0;

	// children
	m_listOfChildren = NULL;
	m_iNumberOfChildren = 0;
}


Node::~Node()
{
	int		i;

	free(m_szPreceedingWhitespace);
	free(m_szToken);

	for (i=0; i<m_iNumberOfChildren; i++)
	{
		delete m_listOfChildren[i];
	}

	free(m_listOfChildren);
}


////////////////////////
//        Type        //
////////////////////////

void Node::setType(int iType)
{
	m_iType = iType;
}


int	Node::getType()
{
	return m_iType;
}


////////////////////////////
//        Children        //
////////////////////////////

void Node::addChild(Node* pChild)
{
	// add child
	m_listOfChildren = (Node**)realloc(m_listOfChildren, sizeof(Node*) * (m_iNumberOfChildren + 1));

	m_listOfChildren[m_iNumberOfChildren] = pChild;

	m_iNumberOfChildren ++;

	// set child's parent to 'this'
	pChild->m_pParent = this;
}


int Node::getNumberChildren()
{
	return m_iNumberOfChildren;
}


Node* Node::getChild(int i)
{
	return m_listOfChildren[i];
}


Node* Node::getParent()
{
	return m_pParent;
}


//////////////////////////////
//        Whitespace        //
//////////////////////////////

void Node::setPrecedingWhitespace(const char* PrecedingWhitespace)
{
	char* szPrecedingWhitespace = (char*)malloc(sizeof(PrecedingWhitespace));
	strcpy (szPrecedingWhitespace, PrecedingWhitespace);
	if (m_szPreceedingWhitespace != NULL)
	{
		free(m_szPreceedingWhitespace);
	}

	m_szPreceedingWhitespace = (char*)malloc(sizeof(char) * (strlen(szPrecedingWhitespace) + 1));

	strcpy(m_szPreceedingWhitespace, szPrecedingWhitespace);
}


char* Node::getPrecedingWhitespace()
{
	return m_szPreceedingWhitespace;
}


void Node::setToken(const char* Token)
{
	char* szToken = (char*)malloc(sizeof(Token));
	strcpy (szToken, Token);
	if (m_szToken != NULL)
	{
		free(m_szToken);
	}

	if (szToken == NULL)
	{
		m_szToken = (char*)malloc(sizeof(char) * (1));

		m_szToken[0] = 0;

		printf("ERROR: setToken(): szToken was NULL");
	}
	else
	{
		m_szToken = (char*)malloc(sizeof(char) * (strlen(szToken) + 1));

		strcpy(m_szToken, szToken);
	}
}


void Node::appendToken(const char* Token)
{
	char* szToken = (char*)malloc(sizeof(Token));
	strcpy (szToken, Token);
	if (m_szToken == NULL)
	{
		m_szToken = (char*)malloc(sizeof(char) * (strlen(szToken) + 1));
	}
	else
	{
		m_szToken = (char*)realloc(m_szToken, sizeof(char) * (strlen(m_szToken) + strlen(szToken) + 1));

		strcat(m_szToken, szToken);
	}
}


char* Node::getToken()
{
	return m_szToken;
}


/////////////////////////////
//        ClassName        //
/////////////////////////////

NodeClassName::NodeClassName()
{
	/*
	// class name
	m_szClassName = (char*)malloc(sizeof(char));
	m_szClassName[0] = 0;
	*/

	m_iType = TYPE_CLASS_IMPLEMENTATION;
}


NodeClassName::~NodeClassName()
{
	//free(m_szClassName);
}

/*
void NodeClassName::setClassName(char* szClassName)
{
	if (m_szClassName != NULL)
	{
		free(m_szClassName);
	}

	m_szClassName = (char*)malloc(sizeof(char) * (strlen(szClassName) + 1));

	strcpy(m_szClassName, szClassName);
}


char* NodeClassName::getClassName()
{
	return m_szClassName;
}
*/


///////////////////////////////////
//        Function Header        //
///////////////////////////////////

NodeFunctionHeader::NodeFunctionHeader()
{
	m_szFunctionHeaderName = (char*)malloc(sizeof(char));
	m_szFunctionHeaderName[0] = 0;

	m_szFunctionHeaderReturnType = (char*)malloc(sizeof(char));
	m_szFunctionHeaderReturnType[0] = 0;

	m_bFunctionHeaderReturnTypeCompleted = 0;

	m_iNumberOfFunctionHeaderParameters = 0;

	m_iType = TYPE_FUNCTION_HEADER;
}


NodeFunctionHeader::~NodeFunctionHeader()
{
	if (m_szFunctionHeaderName != NULL)
	{
		free(m_szFunctionHeaderName);
	}

	if (m_szFunctionHeaderReturnType != NULL)
	{
		free(m_szFunctionHeaderReturnType);
	}
}


void NodeFunctionHeader::setFunctionHeaderName(char* szFunctionHeaderName)
{
	if (m_szFunctionHeaderName != NULL)
	{
		free(m_szFunctionHeaderName);
	}

	m_szFunctionHeaderName = (char*)malloc(sizeof(char) * (strlen(szFunctionHeaderName) + 1));

	strcpy(m_szFunctionHeaderName, szFunctionHeaderName);
}


void NodeFunctionHeader::appendFunctionHeaderName(const char* Token)
{
	
	char* szToken = (char*)malloc(sizeof(Token));
	strcpy (szToken, Token);
	
	m_szFunctionHeaderName = (char*)realloc(m_szFunctionHeaderName, sizeof(char) * (strlen(m_szFunctionHeaderName) + strlen(szToken) + 1));

	strcat(m_szFunctionHeaderName, szToken); 
}


char* NodeFunctionHeader::getFunctionHeaderName()
{
	return m_szFunctionHeaderName;
}


int NodeFunctionHeader::getFunctionHeaderReturnTypeCompleted()
{
	return m_bFunctionHeaderReturnTypeCompleted;
}


void NodeFunctionHeader::setFunctionHeaderReturnTypeCompleted(int bSet)
{
	m_bFunctionHeaderReturnTypeCompleted = bSet;
}


void NodeFunctionHeader::appendFunctionHeaderReturnType(char* szToken)
{
	m_szFunctionHeaderReturnType = (char*)realloc(m_szFunctionHeaderReturnType, sizeof(char) * (strlen(m_szFunctionHeaderReturnType) + strlen(szToken) + 1));

	strcat(m_szFunctionHeaderReturnType, szToken); 
}


char* NodeFunctionHeader::getFunctionHeaderReturnType()
{
	return m_szFunctionHeaderReturnType;
}


void NodeFunctionHeader::incrementNumberOfFunctionHeaderParameters()
{
	m_iNumberOfFunctionHeaderParameters ++;
}


int NodeFunctionHeader::getNumberOfFunctionHeaderParameters()
{
	return m_iNumberOfFunctionHeaderParameters;
}


/////////////////////////////////////////////
//        Function Header Parameter        //
/////////////////////////////////////////////

NodeFunctionHeaderParameter::NodeFunctionHeaderParameter()
{
	m_szParameterName = (char*)malloc(sizeof(char));
	m_szParameterName[0] = 0;

	m_szReturnType = (char*)malloc(sizeof(char));
	m_szReturnType[0] = 0;

	m_bReturnTypeCompleted = 0;

	m_iType = TYPE_FUNCTION_HEADER_PARAMETER;
}


NodeFunctionHeaderParameter::~NodeFunctionHeaderParameter()
{
	if (m_szParameterName != NULL)
	{
		free(m_szParameterName);
	}
	
	if (m_szReturnType != NULL)
	{
		free(m_szReturnType);
	}
}


void NodeFunctionHeaderParameter::setParameterName(char* szParameterName)
{
	if (m_szParameterName != NULL)
	{
		free(m_szParameterName);
	}

	m_szParameterName = (char*)malloc(sizeof(char) * (strlen(szParameterName) + 1));

	strcpy(m_szParameterName, szParameterName);
}


char* NodeFunctionHeaderParameter::getParameterName()
{
	return m_szParameterName;
}


int NodeFunctionHeaderParameter::getReturnTypeCompleted()
{
	return m_bReturnTypeCompleted;
}


void NodeFunctionHeaderParameter::setReturnTypeCompleted(int bSet)
{
	m_bReturnTypeCompleted = bSet;
}


void NodeFunctionHeaderParameter::appendReturnType(char* szToken)
{
	m_szReturnType = (char*)realloc(m_szReturnType, sizeof(char) * (strlen(m_szReturnType) + strlen(szToken) + 1));

	strcat(m_szReturnType, szToken); 
}


char* NodeFunctionHeaderParameter::getReturnType()
{
	return m_szReturnType;
}


void NodeFunctionHeaderParameter::setParameterNumber(int i)
{
	m_iParameterNumber = i;
}


int NodeFunctionHeaderParameter::getParameterNumber()
{
	return m_iParameterNumber;
}


/////////////////////////////////////////
//        NodeFunctionCodeBlock        //
/////////////////////////////////////////

NodeFunctionCodeBlock::NodeFunctionCodeBlock()
{
	m_iType = TYPE_FUNCTION_CODE_BLOCK;
}


NodeFunctionCodeBlock::~NodeFunctionCodeBlock()
{
}


///////////////////////////////////////////
//        NodeFunctionCodeBracket        //
///////////////////////////////////////////

NodeFunctionCodeBracket::NodeFunctionCodeBracket()
{
	m_iType = TYPE_FUNCTION_CODE_BRACKET;
}


NodeFunctionCodeBracket::~NodeFunctionCodeBracket()
{
}


/////////////////////////////////////////
//        NodeFunctionCodeToken        //
/////////////////////////////////////////

NodeFunctionCodeToken::NodeFunctionCodeToken()
{
	m_iType = TYPE_FUNCTION_CODE_TOKEN;
}


NodeFunctionCodeToken::~NodeFunctionCodeToken()
{
}


//////////////////////////////////
//        NodeMethodCall        //
//////////////////////////////////

NodeMethodCall::NodeMethodCall()
{
	m_iType = TYPE_METHOD_CALL;

	m_iNumberOfMethodCallParameters = 0;

	m_bIsStatic = 0;
}


NodeMethodCall::~NodeMethodCall()
{
}


void NodeMethodCall::incrementNumberOfMethodCallParameters()
{
	m_iNumberOfMethodCallParameters ++;
}


int NodeMethodCall::getNumberOfMethodCallParameters()
{
	return m_iNumberOfMethodCallParameters;
}


void NodeMethodCall::setIsStatic(int bIsStatic)
{
	m_bIsStatic = bIsStatic;
}


int NodeMethodCall::getIsStatic()
{
	return m_bIsStatic;
}


////////////////////////////////////
//        NodeMethodObject        //
////////////////////////////////////

NodeMethodObject::NodeMethodObject()
{
	m_iType = TYPE_METHOD_OBJECT;
}


NodeMethodObject::~NodeMethodObject()
{
}


///////////////////////////////////////
//        NodeMethodParameter        //
///////////////////////////////////////

NodeMethodParameter::NodeMethodParameter()
{
	m_iType = TYPE_METHOD_PARAMETER;

	m_iMethodCallParameterNumber = 0;
}


NodeMethodParameter::~NodeMethodParameter()
{
}

	
void NodeMethodParameter::setMethodCallParameterNumber(int i)
{
	m_iMethodCallParameterNumber = i;
}


int NodeMethodParameter::getMethodCallParameterNumber()
{
	return m_iMethodCallParameterNumber;
}


/////////////////////////////
//        NodeArray        //
/////////////////////////////

NodeArray::NodeArray()
{
	m_iType = TYPE_ARRAY;
}


NodeArray::~NodeArray()
{
}


