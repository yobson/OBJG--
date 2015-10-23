
#include "ClassList.h"
#include <stdio.h>


class Read
{
	public:
		// constructor/destructor
		Read(char* szHomeDirectory);
		Read();
		~Read();

	protected:
		char		m_szHomeDirectory[FILENAME_MAX];

		// list of classes (and their super classes) found so far
		ClassList*	m_pClassList;	
		
		// pointer to current position in data
		char*		m_szCurrent;
};




