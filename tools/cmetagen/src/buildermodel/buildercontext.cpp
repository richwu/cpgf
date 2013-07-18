#include "buildercontext.h"
#include "builderclass.h"
#include "builderconstructor.h"
#include "buildercontainer.h"
#include "builderdestructor.h"
#include "builderenum.h"
#include "builderfield.h"
#include "builderfile.h"
#include "builderitem.h"
#include "buildermethod.h"
#include "buildernamespace.h"
#include "builderoperator.h"
#include "builderutil.h"
#include "buildersection.h"
#include "builderwriter.h"

#include "model/cppfile.h"
#include "model/cppcontext.h"
#include "project.h"
#include "util.h"

#include "cpgf/gassert.h"

// test
#include "codewriter/codewriter.h"
#include "codewriter/codeblock.h"

#include <algorithm>

using namespace std;


namespace metagen {


BuilderItem * createBuilderItem(const CppItem * cppItem)
{
	switch(cppItem->getCategory()) {
		case icFile:
			return new BuilderFile(cppItem);

		case icNamespace:
			return new BuilderNamespace(cppItem);

		case icClass:
			return new BuilderClass(cppItem);

		case icField:
			return new BuilderField(cppItem);

		case icMethod:
			return new BuilderMethod(cppItem);

		case icEnum:
			return new BuilderEnum(cppItem);

		case icOperator:
			return new BuilderOperator(cppItem);

		case icConstructor:
			return new BuilderConstructor(cppItem);

		case icDestructor:
			return new BuilderDestructor(cppItem);

		default:
			GASSERT(false);
			break;
	}
	
	return NULL;
}


BuilderContext::BuilderContext(const Project * project, const std::string & sourceFileName)
	:	project(project),
		sourceFileName(normalizePath(sourceFileName)),
		sectionList(new BuilderSectionList())
{
	this->sourceBaseFileName = this->sourceFileName.getBaseName();
}

BuilderContext::~BuilderContext()
{
	clearPointerContainer(this->itemList);
}

BuilderItem * BuilderContext::createItem(const CppItem * cppItem)
{
	BuilderItem * builderItem = createBuilderItem(cppItem);
	builderItem->setProject(this->project);
	return builderItem;
}

void BuilderContext::process(const CppContext * cppContext)
{
	this->doProcessFile(cppContext->getCppFile());
}

void BuilderContext::doProcessFile(const CppFile * cppFile)
{
	BuilderFile * file = static_cast<BuilderFile *>(this->createItem(cppFile));
	this->itemList.push_back(file);
	file->setProject(this->project);

	this->flatten(file);

	this->generateCodeSections();
	this->generateCreationFunctionSections();

	this->getSectionList()->dump();
}

void BuilderContext::generateCodeSections()
{
	BuilderWriter builderWriter(this);
	for(ItemListType::iterator it = this->getItemList()->begin(); it != this->getItemList()->end(); ++it) {
		(*it)->writeMetaData(&builderWriter);
	}
}

bool partialCreationSectionComparer(BuilderSection * a, BuilderSection * b)
{
	return a->getTotalPayload() > b->getTotalPayload();
}

void BuilderContext::generateCreationFunctionSections()
{
	TempBuilderSectionListType partialCreationSections;
	
	this->doCollectPartialCreationFunctions(&partialCreationSections);

	std::sort(partialCreationSections.begin(), partialCreationSections.end(), &partialCreationSectionComparer);

	this->doGenerateCreationFunctions(&partialCreationSections);
}

void BuilderContext::doCollectPartialCreationFunctions(TempBuilderSectionListType * partialCreationSections)
{
	for(BuilderSectionList::iterator it = this->getSectionList()->begin();
		it != this->getSectionList()->end();
		++it) {
		if((*it)->isPartialCreationFunction()) {
			partialCreationSections->push_back(*it);
		}
	}
}

void BuilderContext::doGenerateCreationFunctions(TempBuilderSectionListType * partialCreationSections)
{
	while(! partialCreationSections->empty()) {
		TempBuilderSectionListType sectionsInOneFile;
		this->doExtractPartialCreationFunctions(partialCreationSections, &sectionsInOneFile);
		printf("TTTTTTTTTTTT \n");
		for(TempBuilderSectionListType::iterator it = sectionsInOneFile.begin();
			it != sectionsInOneFile.end();
			++it) {
		CodeWriter codeWriter;
		(*it)->getCodeBlock()->write(&codeWriter);
		printf("%s\n\n", codeWriter.getText().c_str());
		}
	}
}

void BuilderContext::doExtractPartialCreationFunctions(TempBuilderSectionListType * partialCreationSections,
	TempBuilderSectionListType * outputSections)
{
	if(partialCreationSections->empty()) {
		return;
	}
	
	const size_t maxItemCountPerFile = this->getProject()->getMaxItemCountPerFile();
	size_t totalPayload = partialCreationSections->front()->getTotalPayload();

	outputSections->push_back(partialCreationSections->front());
	partialCreationSections->pop_front();

	bool found = true;
	while(found) {
		found = false;
		for(TempBuilderSectionListType::iterator it = partialCreationSections->begin();
			it != partialCreationSections->end();
			) {
			if(maxItemCountPerFile == 0
				||  totalPayload + (*it)->getTotalPayload() <= maxItemCountPerFile) {
				found = true;
				totalPayload += (*it)->getTotalPayload();
				outputSections->push_back(*it);
				it = partialCreationSections->erase(it);
			}
			else {
				++it;
			}
		}
	}
}

void BuilderContext::flatten(BuilderFile * file)
{
	this->doFlatten(file, file);
}

void BuilderContext::doFlatten(BuilderFile * file, BuilderContainer * builderContainer)
{
	for(CppContainer::ItemListType::const_iterator it = builderContainer->getCppContainer()->getItemList()->begin();
		it != builderContainer->getCppContainer()->getItemList()->end(); ++it) {
		if(! (*it)->isInMainFile()) {
			continue;
		}
		if(this->shouldSkipItem(*it)) {
			continue;
		}

		cpgf::GScopedPointer<BuilderItem> item(this->createItem(*it));
		if(! item->canBind()) {
			continue;
		}
		if(item->shouldSkipBind()) {
			continue;
		}

		BuilderItem * itemPointer = item.get();
		this->itemList.push_back(item.take());
		builderContainer->addItem(itemPointer);
		if((*it)->isContainer()) {
			this->doFlatten(file, static_cast<BuilderContainer *>(itemPointer));
		}
	}
}

bool BuilderContext::shouldSkipItem(const CppItem * cppItem)
{
	return ! isVisibilityAllowed(cppItem->getVisibility(), this->project);
}


} // namespace metagen

