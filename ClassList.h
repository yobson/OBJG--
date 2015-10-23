#ifndef HEADER_ClassList
#define HEADER_ClassList


class ClassList
{
	public:
		// constructor/destructor
		ClassList();
		~ClassList();

		void addClassToList(char* szClass);
		int isOnTheClassList(char* szClass);

		void setSuperClassOfLastClassAddedTo(char* szSuperClass);
		char* getSuperClassOfClass(char* szClass);

	protected:
		int		m_iNumberClasses;

		char**	m_pszList;
		char**	m_pszSuperClassList;
};


#endif



