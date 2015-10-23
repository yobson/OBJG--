#include "Write.h"

#include "Node.h"

class WriteTree : public Write
{
	public:
		// constructor/destructor
		WriteTree();
		~WriteTree();

	protected:
		// functions
		void recurseNode(Node* pCurrent, int iNumberTabs);
		char* getFileExtension();
};
