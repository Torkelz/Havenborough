#include "BaseGameApp.h"

#include "Input\InputTranslator.h"

const std::string BaseGameApp::m_GameTitle = "The Apprentice of Havenborough";

void BaseGameApp::init()
{
	m_SceneManager.init();
	m_Window.init(getGameTitle(), getWindowSize());

	m_Graphics = IGraphics::createGraphics();
	//TODO: Need some input setting variable to handle fullscreen.
	bool fullscreen = false;
	m_Graphics->initialize(m_Window.getHandle(), m_Window.getSize().x, m_Window.getSize().y, fullscreen);

	using namespace std::placeholders;
	m_Window.registerCallback(WM_CLOSE, std::bind(&BaseGameApp::handleWindowClose, this, _1, _2, _3));

	InputTranslator::ptr translator(new InputTranslator);
	translator->init(&m_Window);
	translator->addKeyboardMapping(VK_ESCAPE, "exit");
	translator->addKeyboardMapping('W', "moveForward");
	translator->addKeyboardMapping('S', "moveBackward");
	translator->addKeyboardMapping('A', "moveLeft");
	translator->addKeyboardMapping('D', "moveRight");
	m_InputQueue.init(std::move(translator));

	//physics = IPhysics::createPhysics();


	//TEMPORARY -------------------------------------------------------
	BufferDescription bdesc;
	bdesc.initData = createBOX(10,0.0f,0.0f,0.0f);
	bdesc.numOfElements = 36;
	bdesc.sizeOfElement = sizeof(BaseGameApp::vertex);
	bdesc.type = VERTEX_BUFFER;
	bdesc.usage = BUFFER_DEFAULT;

	m_Buffer = m_Graphics->createBuffer(bdesc);

	cBuffer cb;
	DirectX::XMFLOAT4 eye,lookat,up;
	eye = DirectX::XMFLOAT4(0,0,-20,0);
	lookat = DirectX::XMFLOAT4(0,0,0,0);
	up = DirectX::XMFLOAT4(0,1,0,0);
	DirectX::XMStoreFloat4x4(&cb.view,
							DirectX::XMMatrixLookAtRH(
								DirectX::XMLoadFloat4(&eye),
								DirectX::XMLoadFloat4(&lookat),
								DirectX::XMLoadFloat4(&up)));
	DirectX::XMStoreFloat4x4(&cb.proj,
							DirectX::XMMatrixPerspectiveFovRH(
								0.4f*PI,
								(float)m_Window.getSize().x / (float)m_Window.getSize().y,
								1.0f,
								1000.0f));
	BufferDescription cbdesc;
	cbdesc.initData = &cb;
	cbdesc.numOfElements = 1;
	cbdesc.sizeOfElement = sizeof(BaseGameApp::cBuffer);
	cbdesc.type = CONSTANT_BUFFER_ALL;
	cbdesc.usage = BUFFER_DEFAULT;
	m_CBuffer = m_Graphics->createBuffer(cbdesc);

	m_Shader = m_Graphics->createShader(L"../../Graphics/Source/DummyVertexShader.hlsl","VSmain","vs_5_0",VERTEX_SHADER);
	m_Graphics->addShaderStep(m_Shader,L"../../Graphics/Source/DummyVertexShader.hlsl","PSmain","ps_5_0",PIXEL_SHADER);
	//TEMPORARY --------------------------------------------------------
}

void BaseGameApp::run()
{
	m_ShouldQuit = false;

	while (!m_ShouldQuit)
	{
		m_InputQueue.onFrame();
		m_Window.pollMessages();
		//Temp ------------------------------------------------
		DirectX::XMFLOAT4X4 tempMatrix;
		DirectX::XMMATRIX tempMatrix2;
		tempMatrix2 = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4(&tempMatrix, tempMatrix2);

		m_Graphics->renderModel(m_Buffer, m_CBuffer, m_Shader, &tempMatrix, false);
		//Temp -------------------------------------------------
		m_Graphics->drawFrame();

		for (auto& in : m_InputQueue.getFrameInputs())
		{
			if (in.m_Action == "exit")
			{
				m_ShouldQuit = true;
			}
			else
			{
				printf("Received input action: %s (%.1f)\n", in.m_Action.c_str(), in.m_Value);
			}
		}
	}
}

void BaseGameApp::shutdown()
{
	m_InputQueue.destroy();
	
	IGraphics::deleteGraphics(m_Graphics);
	m_Graphics = nullptr;

	m_Window.destroy();
}

std::string BaseGameApp::getGameTitle() const
{
	return m_GameTitle;
}

UVec2 BaseGameApp::getWindowSize() const
{
	// TODO: Read from user option
	
	const static UVec2 size = {800, 480};
	return size;
}

bool BaseGameApp::handleWindowClose(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result)
{
	m_ShouldQuit = true;
	p_Result = 0;
	return true;
}

BaseGameApp::vertex* BaseGameApp::createBOX(unsigned int size, float x, float y, float z)
{
	vertex* box = new vertex[36];
	using namespace DirectX;
	XMFLOAT4 center = XMFLOAT4(x,y,z,1.0f);
    XMFLOAT4 color, normal;

    XMFLOAT4 vert0 = XMFLOAT4(-1.0f*size, -1.0f*size, -1.0f*size, 1.0f); // 0 --- LowerLeftFront
    XMFLOAT4 vert1 = XMFLOAT4( 1.0f*size, -1.0f*size, -1.0f*size, 1.0f); // 1 +-- LowerRightFront
    XMFLOAT4 vert2 = XMFLOAT4(-1.0f*size,  1.0f*size, -1.0f*size, 1.0f); // 2 -+- UpperLeftFront
    XMFLOAT4 vert3 = XMFLOAT4( 1.0f*size,  1.0f*size, -1.0f*size, 1.0f); // 3 ++- UpperRightFront
    XMFLOAT4 vert4 = XMFLOAT4(-1.0f*size, -1.0f*size,  1.0f*size, 1.0f); // 4 --+ LowerLeftBack
    XMFLOAT4 vert5 = XMFLOAT4( 1.0f*size, -1.0f*size,  1.0f*size, 1.0f); // 5 +-+ LowerRightBack
    XMFLOAT4 vert6 = XMFLOAT4(-1.0f*size,  1.0f*size,  1.0f*size, 1.0f); // 6 -++ UpperLeftBack
    XMFLOAT4 vert7 = XMFLOAT4( 1.0f*size,  1.0f*size,  1.0f*size, 1.0f); // 7 +++ UpperRightBack
                                                                                                 
    // Back
    color = XMFLOAT4(0.f,1.f,0.f,1.f);
	normal = XMFLOAT4(0.f,0.f,-1.f,0.f);
    box[0] = vertex(vert4, normal, color);
    box[1] = vertex(vert6, normal, color);
    box[2] = vertex(vert5, normal, color);
    box[3] = vertex(vert6, normal, color);
    box[4] = vertex(vert7, normal, color);
    box[5] = vertex(vert5, normal, color);

    // Front
	color = XMFLOAT4(0.f,0.f,1.f,1.f);
	normal = XMFLOAT4(0.f,0.f,-1.f,0.f);
    box[6] =  vertex(vert1, normal, color);
    box[7] =  vertex(vert3, normal, color);
    box[8] =  vertex(vert0, normal, color);
    box[9] =  vertex(vert3, normal, color);
    box[10] = vertex(vert2, normal, color);
    box[11] = vertex(vert0, normal, color);

    // Top
	color = XMFLOAT4(1.f,0.f,0.f,1.f);
	normal = XMFLOAT4(0.f,-1.f,0.f,0.f);
    box[12] = vertex(vert3, normal, color);
    box[13] = vertex(vert7, normal, color);
    box[14] = vertex(vert2, normal, color);
    box[15] = vertex(vert7, normal, color);
    box[16] = vertex(vert6, normal, color);
    box[17] = vertex(vert2, normal, color);

    // Bottom
	color = XMFLOAT4(1.f,1.f,0.f,1.f);
	normal = XMFLOAT4(0.f,-1.f,0.f,0.f);
    box[18] = vertex(vert0, normal, color);
    box[19] = vertex(vert4, normal, color);
    box[20] = vertex(vert1, normal, color);
    box[21] = vertex(vert4, normal, color);
    box[22] = vertex(vert5, normal, color);
    box[23] = vertex(vert1, normal, color);

    // Right
	color = XMFLOAT4(0.f,1.f,1.f,1.f);
	normal = XMFLOAT4(-1.f,0.f,0.f,0.f);
    box[24] = vertex(vert5, normal, color);
    box[25] = vertex(vert7, normal, color);
    box[26] = vertex(vert1, normal, color);
    box[27] = vertex(vert7, normal, color);
    box[28] = vertex(vert3, normal, color);
    box[29] = vertex(vert1, normal, color);

    // Left
	color = XMFLOAT4(1.f,0.f,1.f,1.f);
	normal = XMFLOAT4(-1.f,0.f,0.f,0.f);
    box[30] = vertex(vert0, normal, color);
    box[31] = vertex(vert2, normal, color);
    box[32] = vertex(vert4, normal, color);
    box[33] = vertex(vert2, normal, color);
    box[34] = vertex(vert6, normal, color);
    box[35] = vertex(vert4, normal, color);


    return box;
}