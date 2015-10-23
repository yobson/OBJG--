#ifndef NODE_DEFINE
#define NODE_DEFINE


#define TYPE_ROOT						0

#define TYPE_COMMENT					10

#define TYPE_HASH_IMPORT				20
#define TYPE_HASH_INCLUDE				21
#define TYPE_HASH_PRAGMA				22
#define TYPE_HASH_DEFINE				23
#define TYPE_HASH_IF					24
#define TYPE_HASH_ELSE					25
#define TYPE_HASH_IFDEF					26
#define TYPE_HASH_IFNDEF				27
#define TYPE_HASH_ENDIF					28

#define TYPE_CLASS_IMPLEMENTATION		30

#define TYPE_FUNCTION					40

#define TYPE_FUNCTION_HEADER			50
#define TYPE_FUNCTION_HEADER_PARAMETER	51

#define TYPE_FUNCTION_CODE_BLOCK		60
#define TYPE_FUNCTION_CODE_BRACKET		61
#define TYPE_FUNCTION_CODE_TOKEN		62

#define TYPE_METHOD_CALL				70
#define TYPE_METHOD_OBJECT				71
#define TYPE_METHOD_FUNCTION			72
#define TYPE_METHOD_PARAMETER			73

#define TYPE_ARRAY						80

#include <iostream>

using namespace std;



class Node
{
	public:
		// constructor/destructor
		Node();
		~Node();

		void	setType(int iType);
		int		getType();

		void	setPrecedingWhitespace(const char* PrecedingWhitespace);
		char*	getPrecedingWhitespace();

		void	setToken(const char* Token);
		void	appendToken(const char* Token);
		char*	getToken();

		// children
		void	addChild(Node* pChild);
		int		getNumberChildren();
		Node*	getChild(int i);

		// parent
		Node*	getParent();

	protected:
		// parent
		Node*			m_pParent;

		// type
		unsigned int	m_iType;

		// preceeding whitespace
		char*			m_szPreceedingWhitespace;

		// token
		char*			m_szToken;

		// children
		Node**			m_listOfChildren;
		unsigned int	m_iNumberOfChildren;
};


class NodeClassName : public Node				// <IMPLEMENTATION>
{
	public:
		// constructor/destructor
		NodeClassName();
		~NodeClassName();

		//void	setClassName(char* szClassName);
		//char*	getClassName();

	protected:
		//char*	m_szClassName;
};


///////////////////////////////////
//        Function header        //
///////////////////////////////////

class NodeFunctionHeader : public Node						// <FUNCTION_HEADER>
{	
	public:
		// constructor/destructor
		NodeFunctionHeader();
		~NodeFunctionHeader();

		void	setFunctionHeaderName(char* szFunctionName);
		void	appendFunctionHeaderName(const char* Token);
		char*	getFunctionHeaderName();

		int	getFunctionHeaderReturnTypeCompleted();
		void	setFunctionHeaderReturnTypeCompleted(int bSet);
		void	appendFunctionHeaderReturnType(char* szToken);
		char*	getFunctionHeaderReturnType();

		void	incrementNumberOfFunctionHeaderParameters();
		int		getNumberOfFunctionHeaderParameters();

	protected:
		char*	m_szFunctionHeaderName;

		char*	m_szFunctionHeaderReturnType;
		int	m_bFunctionHeaderReturnTypeCompleted;

		int		m_iNumberOfFunctionHeaderParameters;
};


class NodeFunctionHeaderParameter : public Node				// <PARAMETER>
{
	public:
		// constructor/destructor
		
		NodeFunctionHeaderParameter();
		~NodeFunctionHeaderParameter();

		void	setParameterName(char* szParameterName);
		char*	getParameterName();

		int	getReturnTypeCompleted();
		void	setReturnTypeCompleted(int bSet);
		void	appendReturnType(char* szToken);
		char*	getReturnType();

		void	setParameterNumber(int i);
		int		getParameterNumber();

	protected:
		char*	m_szParameterName;

		char*	m_szReturnType;
		int	m_bReturnTypeCompleted;

		int		m_iParameterNumber;
};


////////////////////////
//        Code        //
////////////////////////

class NodeFunctionCodeBlock : public Node					// <CODE_BLOCK>
{
	public:
		// constructor/destructor
		NodeFunctionCodeBlock();
		~NodeFunctionCodeBlock();
};


class NodeFunctionCodeBracket : public Node					// (
{
	public:
		// constructor/destructor
		NodeFunctionCodeBracket();
		~NodeFunctionCodeBracket();
};


class NodeFunctionCodeToken : public Node					// <CODE>
{
	public:
		// constructor/destructor
		NodeFunctionCodeToken();
		~NodeFunctionCodeToken();
};


class NodeArray : public Node								// <ARRAY>
{
	public:
		// constructor/destructor
		NodeArray();
		~NodeArray();
};


///////////////////////////////
//        Method Call        //
///////////////////////////////

class NodeMethodCall : public Node							// <CALL>
{
	public:
		// constructor/destructor
		NodeMethodCall();					// 1st child is the object, following children are the parameters
		~NodeMethodCall();

		void	incrementNumberOfMethodCallParameters();
		int		getNumberOfMethodCallParameters();

		void	setIsStatic(int bIsStatic);
		int	getIsStatic();
	
	protected:
		int		m_iNumberOfMethodCallParameters;
		int	m_bIsStatic;
};


class NodeMethodObject : public Node						// <OBJECT>
{
	public:
		// constructor/destructor
		NodeMethodObject();
		~NodeMethodObject();
};


class NodeMethodParameter : public Node						// <PARAMETER>
{
	public:
		// constructor/destructor
		NodeMethodParameter();
		~NodeMethodParameter();

		void	setMethodCallParameterNumber(int i);
		int		getMethodCallParameterNumber();

	protected:
		int		m_iMethodCallParameterNumber;
};


#endif