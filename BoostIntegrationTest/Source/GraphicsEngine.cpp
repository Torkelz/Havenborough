#include <boost/test/unit_test.hpp>
#include "IGraphics.h"
#include "../../Graphics/Source/WrapperFactory.h"
#include "../../Graphics/Source/ModelFactory.h"
#include "../../Graphics/Source/Buffer.h"
#include "../../Graphics/Source/Shader.h"
#include "../../Client/Source/Window.h"

#if _DEBUG
#include <vld.h> 
#endif

BOOST_AUTO_TEST_SUITE(GraphicsEngine)

	BOOST_AUTO_TEST_CASE(TestGraphics)
{
	BOOST_MESSAGE("1> Running TestGraphics...");
	Window window;
	Vector2 winSize = Vector2(1280, 720);
	BOOST_MESSAGE("1> Creating window");
	BOOST_CHECK_NO_THROW(window.init("Test Graphics", Vector2ToXMFLOAT2(&winSize)));

	IGraphics *graphics = IGraphics::createGraphics();
	

	BOOST_MESSAGE("1> Checking init graphics with window");
	BOOST_CHECK(graphics->initialize(window.getHandle(), (int)winSize.x, (int)winSize.y, false));
	
	WrapperFactory *wraperpFactory = WrapperFactory::getInstance();
	ModelFactory *modelFactory = ModelFactory::getInstance();


	//float buffData[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; 

	//Buffer *buff = nullptr;
	//Buffer::Description bd;
	//bd.initData = &buffData;
	//bd.numOfElements = 12;
	//bd.sizeOfElement = sizeof(float);
	//bd.type = Buffer::Type::VERTEX_BUFFER;
	//bd.usage = Buffer::Usage::CPU_READ;

	//BOOST_MESSAGE("Creating vertex buffer with CPU read, expecting buffer exception");
	//BOOST_CHECK_THROW(buff = factory->createBuffer(bd), BufferException);
	//BOOST_CHECK(buff == nullptr);

	//bd.usage = Buffer::Usage::DEFAULT;
	//BOOST_MESSAGE("Creating normal vertex buffer");
	//BOOST_CHECK(buff = factory->createBuffer(bd));
	//SAFE_DELETE(buff);
	//BOOST_CHECK(buff == nullptr);

	//DWORD indexData[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	//bd.initData = &indexData;
	//bd.numOfElements = 16;
	//bd.sizeOfElement = sizeof(DWORD);
	//bd.type = Buffer::Type::INDEX_BUFFER;
	//bd.usage = Buffer::Usage::CPU_WRITE;

	//BOOST_MESSAGE("Creating normal index buffer with dynamic usage");
	//BOOST_CHECK(buff = factory->createBuffer(bd));
	//SAFE_DELETE(buff);
	//BOOST_CHECK(buff == nullptr);

	//struct ConstantBuff 
	//{
	//	float dummy;
	//};
	//ConstantBuff test;
	//test.dummy = 0.0f;
	//bd.initData = &test;
	//bd.numOfElements = 1;
	//bd.sizeOfElement = sizeof(ConstantBuff);
	//bd.type = Buffer::Type::CONSTANT_BUFFER_ALL;
	//bd.usage = Buffer::Usage::USAGE_IMMUTABLE;

	//BOOST_MESSAGE("Creating constant buffer with immutable usage");
	//BOOST_CHECK(buff = factory->createBuffer(bd));
	//SAFE_DELETE(buff);
	//BOOST_CHECK(buff == nullptr);

	//BOOST_MESSAGE("Creating vertex shader using hlsl shader layout");
	//BOOST_CHECK_NO_THROW(gr->createShader("myID", L"Source/dummyVS.hlsl", "main", "5_0",
	//	IGraphics::ShaderType::VERTEX_SHADER));
	//BOOST_CHECK_NO_THROW(gr->createShader("myID", L"Source/dummyPS.hlsl", "main", "5_0",
	//	IGraphics::ShaderType::PIXEL_SHADER));

	//BOOST_CHECK_THROW(gr->createShader("myID", L"Source/dummyGS.hlsl", "main", "5_0",
	//	IGraphics::ShaderType::GEOMETRY_SHADER), ShaderException);

	//BOOST_MESSAGE("Creating vertex shader with wrong filename, expecting shader exception");
	//BOOST_CHECK_THROW(gr->createShader("myOtherID", L"Source/IRWrong.hlsl", "main", "5_0",
	//	IGraphics::ShaderType::VERTEX_SHADER), ShaderException);

	//BOOST_MESSAGE("Creating vertex shader with wrong entry point, expecting shader exception");
	//BOOST_CHECK_THROW(gr->createShader("myAnotherID", L"Source/dummyVS.hlsl", "entry", "5_0",
	//	IGraphics::ShaderType::VERTEX_SHADER), ShaderException);

	//BOOST_MESSAGE("Creating pixel shader with wrong shader model according to shader type, expecting shader exception");
	//BOOST_CHECK_THROW(gr->createShader("myOtherCoolID", L"Source/dummyPS.hlsl", "main", "5_0",
	//	IGraphics::ShaderType::VERTEX_SHADER), ShaderException);


	//IGraphics::ShaderInputElementDescription desc[] = 
	//{
	//	{"POSITION", 0, IGraphics::Format::R32G32B32_FLOAT, 0, 0, 0, 0},
	//	{"NORMAL", 0, IGraphics::Format::R32G32B32_FLOAT, 1, 12, 0, 0},
	//	{"TEXCOORD", 0, IGraphics::Format::R32G32B32_FLOAT, 2, 24, 0, 0}
	//};
	//int size = sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	//Shader *shader = nullptr;

	//BOOST_MESSAGE("Creating vertex shader with user defined input description");
	//BOOST_CHECK_NO_THROW(gr->createShader("LAWL", L"Source/dummy.hlsl", "mainVS,mainPS", "5_0",
	//	IGraphics::ShaderType::VERTEX_SHADER | IGraphics::ShaderType::PIXEL_SHADER, desc, size));
	//SAFE_DELETE(shader);

	//BOOST_MESSAGE("Creating vertex shader with user defined input description, wrong shader model, expecting shader exception");
	//BOOST_CHECK_THROW(gr->createShader("OMGLOL", L"Source/dummyVS.hlsl", "main", "5_5",
	//	IGraphics::ShaderType::VERTEX_SHADER, desc, size), ShaderException);
	//SAFE_DELETE(shader);

	//BOOST_MESSAGE("Creating Texture that does not exist, expecting exception");
	//BOOST_CHECK_THROW(gr->createTexture("MyTexture", "lol.png"), TextureLoaderException);

	//BOOST_CHECK_THROW(gr->createTexture("MyTexture", "lol.dds"), TextureLoaderException);

	IGraphics::deleteGraphics(graphics);
	
	wraperpFactory = nullptr;
	modelFactory = nullptr;
}

BOOST_AUTO_TEST_SUITE_END()