//#include <boost/test/unit_test.hpp>
//#include "../Graphics/Source/WrapperFactory.h"
//#include "../Graphics/include/IGraphics.h"
//#include "../Graphics/Source/Graphics.h"
//#include "../Client/Source/Window.h"
//BOOST_AUTO_TEST_SUITE(TestShader)
//	
//	BOOST_AUTO_TEST_CASE(TestInitShader)
//	{
//	Window win;
//	IGraphics *g;
//
//	UVec2 s = {400,300};
//	win.init("TestInitShader",s);
//	g = IGraphics::createGraphics();
//	g->initialize(win.getHandle(), s.x, s.y, false);
//
//	Shader* shady = new Shader();
//	shady->initialize(dynamic_cast<Graphics*>(g)->m_Device, dynamic_cast<Graphics*>(g)->m_DeviceContext, 1);
//
//	D3D11_INPUT_ELEMENT_DESC desc[] = 
//	{
//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//	};
//
//	BOOST_CHECK_THROW(shady->compileAndCreateShader(L"dummy.hlsl", "vs_main", "vs_5_0", VERTEX_SHADER, desc),
//	ShaderException);
//
//	delete shady;
//	shady = nullptr;
//	IGraphics::deleteGraphics(g);
//	g = nullptr;
//	win.destroy();
//	}
//
//	BOOST_AUTO_TEST_CASE(TestShaderFactory)
//	{
//	Window win;
//	IGraphics *g;
//
//	UVec2 s = {400,300};
//	win.init("TestShaderFactory",s);
//	g = IGraphics::createGraphics();
//	g->initialize(win.getHandle(), s.x, s.y, false);
//
//	WrapperFactory wraps(dynamic_cast<Graphics*>(g)->m_Device, dynamic_cast<Graphics*>(g)->m_DeviceContext);
//	Shader *shady = nullptr;
//
//	D3D11_INPUT_ELEMENT_DESC desc[] = 
//	{
//	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
//	{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
//	};
//
//	int size = sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
//
//	BOOST_CHECK_NO_THROW(shady = wraps.createShader(L"Source/dummy.hlsl", "main", "vs_5_0", VERTEX_SHADER,
//	desc, size));
//
//	BOOST_CHECK(shady != nullptr);
//
//	SAFE_DELETE(shady);
//	IGraphics::deleteGraphics(g);
//	g = nullptr;
//	win.destroy();
//	}
//
//	BOOST_AUTO_TEST_CASE(TestShaderFactoryWODesc)
//	{
//	Window win;
//	IGraphics *g;
//
//	UVec2 s = {400,300};
//	win.init("tesTestShaderFactorytWODesc",s);
//	g = IGraphics::createGraphics();
//	g->initialize(win.getHandle(), s.x, s.y, false);
//
//	WrapperFactory wraps(dynamic_cast<Graphics*>(g)->m_Device, dynamic_cast<Graphics*>(g)->m_DeviceContext);
//	Shader *shady = nullptr;
//
//	BOOST_CHECK_NO_THROW(shady = wraps.createShader(L"Source/dummy.hlsl", "main", "vs_5_0", VERTEX_SHADER));
//
//	BOOST_CHECK(shady != nullptr);
//
//	SAFE_DELETE(shady);
//	IGraphics::deleteGraphics(g);
//	g = nullptr;
//	win.destroy();
//	}
//
//	BOOST_AUTO_TEST_CASE(TestBufferFactory)
//	{
//		Window win;
//		IGraphics *g;
//		//_crtBreakAlloc = 881;
//		UVec2 s = {400,300};
//		win.init("TestBufferFactory",s);
//		g = IGraphics::createGraphics();
//		g->initialize(win.getHandle(), s.x, s.y, false);
//
//		WrapperFactory wraps(dynamic_cast<Graphics*>(g)->m_Device, dynamic_cast<Graphics*>(g)->m_DeviceContext);
//		Buffer *buff = nullptr; 
//
//		float ff[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; 
//
//		BufferDescription desc;
//		desc.initData =  nullptr;
//		for(int i = 4; i < 5;i++)
//		{
//			for(int j = 0; j < 7;j++)
//			{
//				
//				desc.numOfElements = 4;
//				desc.sizeOfElement = 12;
//				desc.usage = (BufferUsage)i;
//				desc.type = (BufferType)j;
//				
//
//				std::string msg = "BufferDesc combination of (" + std::to_string(i) + "," + std::to_string(j) + ") failed misserably";
//				if(i == 4 && j < 7)
//				{
//					BOOST_CHECK_THROW(buff = wraps.createBuffer(desc), BufferException);
//				}
//				else
//				{
//					buff = wraps.createBuffer(desc);
//					BOOST_CHECK_MESSAGE(buff != nullptr, msg);
//					
//				}				
//				SAFE_DELETE(buff);
//			}
//		}
//		//SAFE_DELETE(buff);
//		IGraphics::deleteGraphics(g);
//		g = nullptr;
//		win.destroy();
//	}//*/
//
//BOOST_AUTO_TEST_SUITE_END()