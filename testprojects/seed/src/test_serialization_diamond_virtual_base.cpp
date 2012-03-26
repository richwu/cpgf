#include "testserializationcommon.h"
#include "cpgf/gmetadefine.h"

#include "gmetatextstreamarchive.h"

#include "cpgf/ginterface.h"
#include "cpgf/gflags.h"
#include "cpgf/gfunctionextractor.h"

#include "gmetaarchivetypemap.h"

#include <sstream>
#include <string>

#include <iostream>

using namespace std;
using namespace cpgf;


namespace {

int readCount = 0;
int writeCount = 0;

class A
{
public:
	A() : a(0) {}
	virtual ~A() {}

	int getA() const {
		++readCount;
		return a;
	}
	
	void setA(int a) {
		++writeCount;
		this->a = a;
	}

	int a;
};

class B : virtual public A
{
public:
	B() : b(0) {}

	int b;
};

class C : virtual public A
{
public:
	C() : c(0) {}

	int c;
};

class D : public B, public C
{
public:
	D() : d(0) {}

	bool operator == (const D & other) const {
		return
			this->a == other.a
			&& this->b == other.b
			&& this->c == other.c
			&& this->d == other.d
		;
	}
	
	bool operator != (const D & other) const {
		return
			this->a != other.a
			&& this->b != other.b
			&& this->c != other.c
			&& this->d != other.d
		;
	}
	
	int d;
};



template <typename Define>
void register_TestSerializeClass(Define define)
{
	GDefineMetaClass<A> classDefineA = GDefineMetaClass<A>::declare("TestSerializeClassA");
	
	classDefineA
		._property("a", &A::getA, &A::setA)
	;

	define._class(classDefineA);
	
	GDefineMetaClass<B, A> classDefineB = GDefineMetaClass<B, A>::declare("TestSerializeClassB");
	
	classDefineB
		FIELD(B, b)
	;

	define._class(classDefineB);

	GDefineMetaClass<C, A> classDefineC = GDefineMetaClass<C, A>::declare("TestSerializeClassC");
	
	classDefineC
		FIELD(C, c)
	;

	define._class(classDefineC);
	
	GDefineMetaClass<D, B, C> classDefineD = GDefineMetaClass<D, B, C>::declare("TestSerializeClassD");
	
	classDefineD
		FIELD(D, d)
	;

	define._class(classDefineD);

}

template <typename AR>
void doTestMultipleInheritance(IMetaWriter * writer, IMetaReader * reader, const AR & ar)
{
	readCount = 0;
	writeCount = 0;

	GDefineMetaNamespace define = GDefineMetaNamespace::declare("global");
	register_TestSerializeClass(define);

	GScopedInterface<IMetaModule> module(createMetaModule(define.getMetaClass()));
	GScopedInterface<IMetaService> service(createMetaService(module.get()));

	GScopedInterface<IMetaArchiveWriter> archiveWriter(createMetaArchiveWriter(GMetaArchiveConfig().getFlags(), service.get(), writer));

	GScopedInterface<IMetaClass> metaClass(service->findClassByName("TestSerializeClassD"));

	D instance;
	instance.a = 0x1a;
	instance.b = 0x2b;
	instance.c = 0x3c;
	instance.d = 0x4d;

	serializeWriteObjectValue(archiveWriter.get(), "obj", &instance, metaClass.get());

	ar.rewind();
	
	GScopedInterface<IMetaArchiveReader> archiveReader(createMetaArchiveReader(GMetaArchiveConfig().getFlags(), service.get(), reader));
	
	D readInstance;
	
	GCHECK(instance != readInstance);

	serializeReadObject(archiveReader.get(), "", &readInstance, metaClass.get());

	GEQUAL(instance, readInstance);

	GEQUAL(1, readCount);
	GEQUAL(1, writeCount);
}

GTEST(testMultipleInheritance)
{
	GDefineMetaNamespace define = GDefineMetaNamespace::declare("global");
	register_TestSerializeClass(define);

		GScopedInterface<IMetaModule> module(createMetaModule(define.getMetaClass()));
	GScopedInterface<IMetaService> service(createMetaService(module.get()));

	stringstream stream;

	GTextStreamMetaWriter<stringstream> outputStream(stream);
	GTextStreamMetaReader<stringstream> inputStream(service.get(), stream);
	
	doTestMultipleInheritance(&outputStream, &inputStream, TestArchiveStream<stringstream>(stream));
	
//	cout << stream.str().c_str() << endl;
}


} // unnamed namespace
