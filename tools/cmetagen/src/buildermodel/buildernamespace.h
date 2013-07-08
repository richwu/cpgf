#ifndef __BUILDERNAMESPACE_H
#define __BUILDERNAMESPACE_H

#include "buildercontainer.h"


class CppNamespace;

class BuilderNamespace : public BuilderContainer
{
private:
	typedef BuilderContainer super;
	
public:
	explicit BuilderNamespace(const CppItem * cppItem);
	virtual ~BuilderNamespace();

	const CppNamespace * getCppNamespace() const;
		
protected:
	virtual void doWriteMetaData(BuilderFileWriter * writer);
};

#endif