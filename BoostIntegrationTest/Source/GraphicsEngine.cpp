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
	
	float buffData[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; 

	Buffer *buff = nullptr;
	BufferDescription bd;
	bd.initData = &buffData;
	bd.numOfElements = 12;
	bd.sizeOfElement = sizeof(float);
	bd.type = VERTEX_BUFFER;
	bd.usage = BUFFER_CPU_READ;

	BOOST_MESSAGE("Creating vertex buffer with CPU read, expecting buffer exception");
	BOOST_CHECK_THROW(buff = gr->createBuffer(bd), BufferException);
	BOOST_CHECK(buff == nullptr);

	bd.usage = BUFFER_DEFAULT;
	BOOST_MESSAGE("Creating normal vertex buffer");
	BOOST_CHECK(buff = gr->createBuffer(bd));
	SAFE_DELETE(buff);
	BOOST_CHECK(buff == nullptr);
	
	DWORD indexData[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	bd.initData = &indexData;
	bd.numOfElements = 16;
	bd.sizeOfElement = sizeof(DWORD);
	bd.type = INDEX_BUFFER;
	bd.usage = BUFFER_CPU_WRITE;
	
	BOOST_MESSAGE("Creating normal index buffer with dynamic usage");
	BOOST_CHECK(buff = gr->createBuffer(bd));
	SAFE_DELETE(buff);
	BOOST_CHECK(buff == nullptr);

	struct ConstantBuff 
	{
		float dummy;
	};
	ConstantBuff test;
	test.dummy = 0.0f;
	bd.initData = &test;
	bd.numOfElements = 1;
	bd.sizeOfElement = sizeof(ConstantBuff);
	bd.type = CONSTANT_BUFFER_ALL;
	bd.usage = BUFFER_USAGE_IMMUTABLE;

	BOOST_MESSAGE("Creating constant buffer with immutable usage");
	BOOST_CHECK(buff = gr->createBuffer(bd));
	SAFE_DELETE(buff);
	BOOST_CHECK(buff == nullptr);
	
	Shader *shader = nullptr;

	BOOST_MESSAGE("Creating vertex shader using hlsl shader layout");
	BOOST_CHECK(shader = gr->createShader(L"Source/dummyVS.hlsl", "main", "vs_5_0", VERTEX_SHADER));
	
	BOOST_MESSAGE("Added pixel shader using hlsl shader layout");
	BOOST_CHECK_NO_THROW(gr->addShaderStep(shader, L"Source/dummyPS.hlsl", "main", "ps_5_0", PIXEL_SHADER));
	SAFE_DELETE(shader);


	BOOST_MESSAGE("Creating vertex shader with wrong filename, expecting shader exception");
	BOOST_CHECK_THROW(shader = gr->createShader(L"Source/IRWrong.hlsl", "main", "vs_5_0", VERTEX_SHADER),
		ShaderException);

	BOOST_MESSAGE("Creating vertex shader with wrong entry point, expecting shader exception");
	BOOST_CHECK_THROW(shader = gr->createShader(L"Source/dummyVS.hlsl", "entry", "vs_5_0", VERTEX_SHADER),
		ShaderException);

	BOOST_MESSAGE("Creating pixel shader with wrong shader model according to shader type, expecting shader exception");
	BOOST_CHECK_THROW(shader = gr->createShader(L"Source/dummyPS.hlsl", "main", "vs_5_0", PIXEL_SHADER),
		ShaderException);


	D3D11_INPUT_ELEMENT_DESC desc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	int size = sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	BOOST_MESSAGE("Creating vertex shader with user defined input description");
	BOOST_CHECK_NO_THROW(shader = gr->createShader(L"Source/dummyVS.hlsl", "main", "vs_5_0", VERTEX_SHADER,
		desc, size));

	BOOST_MESSAGE("Creating vertex shader with user defined input description, wrong shader model, expecting shader exception");
	BOOST_CHECK_THROW(shader = gr->createShader(L"Source/dummyVS.hlsl", "main", "ps_5_0", VERTEX_SHADER,
		desc, size), ShaderException);

	SAFE_DELETE(shader);
	IGraphics::deleteGraphics(gr);
}


BOOST_AUTO_TEST_SUITE_END()