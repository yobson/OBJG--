#ifndef HEADER_Write
#define HEADER_Write

#include "Node.h"


class Write
{
	public:
		// constructor/destructor
		Write();
		~Write();

		void Output(Node* pRoot, char* szHomeDirectory, char* szFile);

	protected:
		virtual void recurseNode(Node* pCurrent, int iNumberTabs) = 0;
		virtual char* getFileExtension() = 0;

		void recurseNodesChildren(Node* pCurrent, int iNumberTabs);
	
		void appendToOutput(const char* Buffer);
		void appendToOutputWithTabs(const char* Buffer, int iNumberOfTabs);

		char*	m_szOutput;
		int		m_iLengthOutput;
};

#endif