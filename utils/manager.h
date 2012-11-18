#ifndef _MANAGER
#define _MANAGER 1


#include "singleton.h"

#include <vector>
#include <string>

using namespace std;

template <typename C,typename T>

class Manager : public Singleton<T>
{
public:
	struct Entry
	{
		int valid;
		std::string name;
		C data;
	};

	void Setup(void);
	void Clear(void)
	{
		Entries.clear();
	};
  Entry Search(string Name)
	{
	unsigned int i;
	for(i=0;i<Entries.size();i++)
	{
		Entry MyEntry=Entries.at(i);
		if (MyEntry.name==Name)
			return MyEntry;
	}
	Entry ret;
	ret.valid=0;
	return ret;
	};
  C Get(string Name)
  {
	Entry MyEntry=Search(Name);
	return MyEntry.data;
  }
  C Get(unsigned int n)
  {
	C e;
	if (n<Entries.size())
		return Entries[n].data;
	return e;
  }
  unsigned int Size()
  {	
	  return Entries.size();
  }
  void Push_Back(C c)
  {
		Entry entry;
		entry.data=c;
		Entries.push_back(entry);
  }
  C Require(string Name,string FileHandle)
  {
		Entry MyEntry=Search(Name);
		if (!MyEntry.valid)
		{		
			MyEntry.data=Load(FileHandle);
			MyEntry.name=Name;
			MyEntry.valid=1;
			Entries.push_back(MyEntry);
		}
		return MyEntry.data;
  };
  virtual C Load(std::string Filename) {C ret;return ret;};
private:
	vector<Entry> Entries;
};


#endif
