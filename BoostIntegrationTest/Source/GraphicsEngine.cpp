#include <boost/test/unit_test.hpp>
#include "IGraphics.h"
#include "../../Client/Source/Window.h"

BOOST_AUTO_TEST_SUITE(GraphicsEngine)

BOOST_AUTO_TEST_CASE(TestGraphics)
{
	Window win;
	UVec2 winSize = {1280, 720};
	win.init("Test Graphics", winSize);

	IGraphics *gr = IGraphics::createGraphics();

	BOOST_MESSAGE("Checking init graphics with window");
	BOOST_CHECK(gr->initialize(win.getHandle(), winSize.x, winSize.y, false));
	BOOST_MESSAGE("Init graphics with window success");
	
	float buffData[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; 

	Buffer *buff;
	BufferDescription bd;
	bd.initData = &buffData;
	bd.numOfElements = 12;
	bd.sizeOfElement = sizeof(float);
	bd.type = VERTEX_BUFFER;
	bd.usage = BUFFER_CPU_READ;

	BOOST_MESSAGE("Trying to create vertex buffer with CPU read, expecting exception");
	BOOST_CHECK_THROW(buff = gr->createBuffer(bd), MyGraphicsExceptions);
	BOOST_MESSAGE("Exception catched, no buffer created");

	bd.usage = BUFFER_DEFAULT;
	BOOST_MESSAGE("Trying to create normal vertex buffer");
	BOOST_CHECK(buff = gr->createBuffer(bd));
	SAFE_DELETE(buff);
	BOOST_CHECK(buff == nullptr);
	BOOST_MESSAGE("Vertex buffer created and deleted successfully");
	
	DWORD indexData[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	bd.initData = &indexData;
	bd.numOfElements = 16;
	bd.sizeOfElement = sizeof(DWORD);
	bd.type = INDEX_BUFFER;
	bd.usage = BUFFER_CPU_WRITE;
	
	BOOST_MESSAGE("Trying to create normal index buffer with dynamic usage");
	BOOST_CHECK(buff = gr->createBuffer(bd));
	SAFE_DELETE(buff);
	BOOST_CHECK(buff == nullptr);
	BOOST_MESSAGE("Index buffer created and deleted successfully");

	bd.type = CONSTANT_BUFFER_ALL;
	
	
	IGraphics::deleteGraphics(gr);
}


BOOST_AUTO_TEST_SUITE_END()