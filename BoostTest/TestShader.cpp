//#include <boost/test/unit_test.hpp>
//#include "../Graphics/Source/WrapperFactory.h"
//#include "../Graphics/include/IGraphics.h"
//#include "../Graphics/Source/Graphics.h"
//#include "../Client/Source/Window.h"
//BOOST_AUTO_TEST_SUITE(TestShader)
//	//BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(TestShader, 1)
//	
//	BOOST_AUTO_TEST_CASE(TestInitShader)
//	{
//		Window win;
//		IGraphics *g;
//
//		UVec2 s = {400,300};
//		win.init("test",s);
//		g = IGraphics::createGraphics();
//		g->initialize(win.getHandle(), s.x, s.y, false);
//
//		Shader shady;
//		shady.initialize(dynamic_cast<Graphics*>(g)->m_Device, dynamic_cast<Graphics*>(g)->m_DeviceContext, 1);
//
//		D3D11_INPUT_ELEMENT_DESC desc[] = 
//		{
//			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//		};
//
//		BOOST_CHECK_THROW(shady.compileAndCreateShader(L"dummy.hlsl", "vs_main", "vs_5_0", VERTEX_SHADER, desc), ShaderException);
//		
//		IGraphics::deleteGraphics(g);
//		g = nullptr;
//		win.destroy();
//	}
//
//	BOOST_AUTO_TEST_CASE(TestShaderFactory)
//	{
//		Window win;
//		IGraphics *g;
//		
//		UVec2 s = {400,300};
//		win.init("test",s);
//		g = IGraphics::createGraphics();
//		g->initialize(win.getHandle(), s.x, s.y, false);
//		
//		WrapperFactory wraps(dynamic_cast<Graphics*>(g)->m_Device, dynamic_cast<Graphics*>(g)->m_DeviceContext);
//		Shader *shady;
//		
//		D3D11_INPUT_ELEMENT_DESC desc[] = 
//		{
//			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
//		};
//
//		shady = wraps.createShader(L"dummy.hlsl", "vs_main", "vs_5_0", VERTEX_SHADER, desc);
//		
//		IGraphics::deleteGraphics(g);
//		g = nullptr;
//		win.destroy();
//	}
//BOOST_AUTO_TEST_SUITE_END()