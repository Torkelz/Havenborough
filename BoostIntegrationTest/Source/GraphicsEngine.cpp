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
	static std::string testId = "1> ";

	BOOST_AUTO_TEST_CASE(TestGraphics)
{
	
	BOOST_MESSAGE(testId + "Running TestGraphics...");
	Window window;
	Vector2 winSize = Vector2(1280, 720);
	BOOST_MESSAGE(testId + "Creating window");
	BOOST_CHECK_NO_THROW(window.init("Test Graphics", Vector2ToXMFLOAT2(&winSize)));

	IGraphics *graphics = IGraphics::createGraphics();
	

	BOOST_MESSAGE(testId + "Checking init graphics with window");
	BOOST_CHECK(graphics->initialize(window.getHandle(), (int)winSize.x, (int)winSize.y, false));
	
	WrapperFactory *wrapperFactory = WrapperFactory::getInstance();
	ModelFactory *modelFactory = ModelFactory::getInstance();


	float bufferData[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; 

	Buffer *buffer = nullptr;
	Buffer::Description bufferDescription;
	bufferDescription.initData = &bufferData;
	bufferDescription.numOfElements = 12;
	bufferDescription.sizeOfElement = sizeof(float);
	bufferDescription.type = Buffer::Type::VERTEX_BUFFER;
	bufferDescription.usage = Buffer::Usage::CPU_READ;

	BOOST_MESSAGE(testId + "Creating vertex buffer with CPU read, expecting buffer exception");
	BOOST_CHECK_THROW(buffer = wrapperFactory->createBuffer(bufferDescription), BufferException);
	BOOST_CHECK(buffer == nullptr);

	bufferDescription.usage = Buffer::Usage::DEFAULT;
	BOOST_MESSAGE(testId + "Creating normal vertex buffer");
	BOOST_CHECK(buffer = wrapperFactory->createBuffer(bufferDescription));
	SAFE_DELETE(buffer);
	BOOST_CHECK(buffer == nullptr);

	DWORD indexData[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	bufferDescription.initData = &indexData;
	bufferDescription.numOfElements = 16;
	bufferDescription.sizeOfElement = sizeof(DWORD);
	bufferDescription.type = Buffer::Type::INDEX_BUFFER;
	bufferDescription.usage = Buffer::Usage::CPU_WRITE;

	BOOST_MESSAGE(testId + "Creating normal index buffer with dynamic usage");
	BOOST_CHECK(buffer = wrapperFactory->createBuffer(bufferDescription));
	SAFE_DELETE(buffer);
	BOOST_CHECK(buffer == nullptr);

	struct ConstantBuffer 
	{
		float dummy;
	};
	ConstantBuffer testCB;
	testCB.dummy = 0.0f;
	bufferDescription.initData = &testCB;
	bufferDescription.numOfElements = 1;
	bufferDescription.sizeOfElement = sizeof(ConstantBuffer);
	bufferDescription.type = Buffer::Type::CONSTANT_BUFFER_ALL;
	bufferDescription.usage = Buffer::Usage::USAGE_IMMUTABLE;

	BOOST_MESSAGE(testId + "Creating constant buffer with immutable usage");
	BOOST_CHECK(buffer = wrapperFactory->createBuffer(bufferDescription));
	SAFE_DELETE(buffer);
	BOOST_CHECK(buffer == nullptr);

	BOOST_MESSAGE(testId + "Creating vertex shader using hlsl shader layout");
	BOOST_CHECK_NO_THROW(graphics->createShader("myID", L"../Source/dummyShader.hlsl", "mainVS,mainPS", "5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER));

	BOOST_MESSAGE(testId + "Trying to add shader step which does not exist, expecting shader expection");
	BOOST_CHECK_THROW(graphics->createShader("myID", L"../Source/dummyShader.hlsl", "mainGS", "5_0",
		ShaderType::GEOMETRY_SHADER), ShaderException);

	BOOST_MESSAGE(testId + "Trying to create vertex shader with wrong filename, expecting shader exception");
	BOOST_CHECK_THROW(graphics->createShader("myOtherID", L"../Source/IDoNotExist.hlsl", "main", "5_0",
		ShaderType::VERTEX_SHADER), ShaderException);

	BOOST_MESSAGE(testId + "Trying to create vertex shader with wrong entry point, expecting shader exception");
	BOOST_CHECK_THROW(graphics->createShader("myAnotherID", L"../Source/dummyShader.hlsl", "entry", "5_0",
		ShaderType::VERTEX_SHADER), ShaderException);

	ShaderInputElementDescription desc[] = 
	{
		{"POSITION", 0, Format::R32G32B32_FLOAT, 0, 0, 0, 0},
		{"NORMAL", 0, Format::R32G32B32_FLOAT, 1, 12, 0, 0},
		{"TEXCOORD", 0, Format::R32G32B32_FLOAT, 2, 24, 0, 0}
	};
	int size = sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	BOOST_MESSAGE(testId + "Creating vertex shader with user defined input description");
	BOOST_CHECK_NO_THROW(graphics->createShader("userDefinedDesc", L"../Source/dummyShader.hlsl", "mainVS,mainPS", "5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER, desc, size));

	BOOST_MESSAGE(testId + "Trying to create vertex shader with user defined input description, wrong shader model, expecting shader exception");
	BOOST_CHECK_THROW(graphics->createShader("anotherUserDefinedDesc", L"../Source/dummyShader.hlsl", "main", "5_5",
		ShaderType::VERTEX_SHADER, desc, size), ShaderException);

	BOOST_MESSAGE(testId + "Trying to create textures that does not exist, expecting exception");
	BOOST_CHECK_THROW(graphics->createTexture("MyTexture", "lol.png"), TextureLoaderException);
	BOOST_CHECK_THROW(graphics->createTexture("MyTexture", "lol.dds"), TextureLoaderException);

	IGraphics::deleteGraphics(graphics);
	
	wrapperFactory = nullptr;
	modelFactory = nullptr;
}

BOOST_AUTO_TEST_SUITE_END()