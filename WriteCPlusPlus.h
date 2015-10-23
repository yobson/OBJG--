#include "Write.h"

#include "Node.h"

class WriteCPlusPlus : public Write
{
	public:
		// constructor/destructor
		WriteCPlusPlus();
		~WriteCPlusPlus();

	protected:
		// functions
		void recurseNode(Node* pCurrent, int iNumberTabs);
		char* getFileExtension();

		void recurseNode2(Node* pCurrent, int iNumberTabs);

		char* GetClassName(Node* pThisNode);
};
