#include "Read.h"

#include "Node.h"

class ReadObjectiveC : public Read
{
	public:
		// constructor/destructor
		ReadObjectiveC(char* szHomeDirectory);
		ReadObjectiveC();
		~ReadObjectiveC();

		Node* Input(char* szHomeDirectory, char* szFile, int* pbSuccess);

	protected:
		// functions
		int previousChildIsAnArrayName(Node* pCurrent);

		static int getNextObjectiveCToken(char** pszFrom, char* szToken, char* szLeadingWhitespace, int bIncludeTrailingColon);
		static void copyToEndOfLine(char** pszFrom, char* szToken);

		int processRoot(Node** ppCurrent, char* szToken, char* szLeadingWhitespace);						// return 1 if dealt with
		int processImplementation(Node** ppCurrent, char* szToken, char* szLeadingWhitespace);				// return 1 if dealt with
		int processFunctionHeader(Node** ppCurrent, char* szToken, char* szLeadingWhitespace);				// return 1 if dealt with
		int processFunctionHeaderParameter(Node** ppCurrent, char* szToken, char* szLeadingWhitespace);	// return 1 if dealt with

		void addHashCommand(Node** ppCurrent, char* szLeadingWhitespace, char* szToken);

		int isStaticMethodCall(NodeMethodCall* pMethodCall);

		int extractFilenameFromRestOfLine(char* szRestOfLine, char* szFile);
		void scanHeaderFileForClasses(char* szFile);
};




