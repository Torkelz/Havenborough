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

	m_Window.registerCallback(WM_CLOSE, std::bind(&BaseGameApp::handleWindowClose, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));


	InputTranslator::ptr translator(new InputTranslator);
	translator->init(&m_Window);
	translator->addKeyboardMapping(VK_ESCAPE, "exit");
	translator->addKeyboardMapping('W', "moveForward");
	translator->addKeyboardMapping('S', "moveBackward");
	translator->addKeyboardMapping('A', "moveLeft");
	translator->addKeyboardMapping('D', "moveRight");
	translator->addKeyboardMapping('C', "connect");
	translator->addKeyboardMapping('Z', "changeViewN");
	translator->addKeyboardMapping('X', "changeViewP");

	translator->addMouseMapping(InputTranslator::Axis::HORIZONTAL, "mousePosHori", "mouseMoveHori");
	translator->addMouseMapping(InputTranslator::Axis::VERTICAL, "mousePosVert", "mouseMoveVert");

	translator->addMouseButtonMapping(InputTranslator::MouseButton::LEFT, "gogogogo");
	translator->addMouseButtonMapping(InputTranslator::MouseButton::MIDDLE, "rollMe!");

	m_InputQueue.init(std::move(translator));

	m_Physics = IPhysics::createPhysics();

	m_Body = m_Physics->createSphere(50.f, false, Vector3(0.f, 5.f, 0.f), 1.f);
	m_Object = m_Physics->createSphere(50.f, true, Vector3(0.f, 0.f, 0.f), 1.f);

	dt = (1.f/60.f);
	
	//TEMPORARY -------------------------------------------------------
	//BufferDescription bdesc;
	//bdesc.initData = createBOX(31,0.0f,0.0f,0.0f);
	//bdesc.numOfElements = 36;
	//bdesc.sizeOfElement = sizeof(BaseGameApp::vertex);
	//bdesc.type = VERTEX_BUFFER;
	//bdesc.usage = BUFFER_DEFAULT;
	//
	//m_Buffer = m_Graphics->createBuffer(bdesc);
	//
	//m_Shader = m_Graphics->createShader(L"../../Graphics/Source/DeferredShaders/GeometryPass.hlsl","VS","vs_5_0",VERTEX_SHADER);
	//m_Graphics->addShaderStep(m_Shader,L"../../Graphics/Source/DeferredShaders/GeometryPass.hlsl","PS","ps_5_0",PIXEL_SHADER);
	//
	////BufferDescription bdesc;
	//bdesc.initData = createBOX(25,0.0f,0.0f,0.0f);
	//bdesc.numOfElements = 36;
	//bdesc.sizeOfElement = sizeof(BaseGameApp::vertex);
	//bdesc.type = VERTEX_BUFFER;
	//bdesc.usage = BUFFER_DEFAULT;
	//
	//m_Buffer2 = m_Graphics->createBuffer(bdesc);
	//
	//m_Shader2 = m_Graphics->createShader(L"../../Graphics/Source/DeferredShaders/GeometryPass.hlsl","VS","vs_5_0",VERTEX_SHADER);
	//m_Graphics->addShaderStep(m_Shader2,L"../../Graphics/Source/DeferredShaders/GeometryPass.hlsl","PS","ps_5_0",PIXEL_SHADER);
	//TEMPORARY --------------------------------------------------------
}

void BaseGameApp::run()
{
	m_ShouldQuit = false;

	int currView = 3; // FOR DEBUGGING
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	//Temp ------------------------------------------------
		//DirectX::XMFLOAT4X4 tempMatrix;
		//DirectX::XMMATRIX tempMatrix2;
		//tempMatrix2 = DirectX::XMMatrixIdentity();
		//DirectX::XMStoreFloat4x4(&tempMatrix, tempMatrix2);
	//Temp -------------------------------------------------

	while (!m_ShouldQuit)
	{
		m_Physics->update(dt);

		for(int i = 0; i < m_Physics->getHitDataSize(); i++)
		{
			HitData hit = m_Physics->getHitDataAt(i);
			if(hit.intersect)
			{
				int i = 0;
			}
		}
		__int64 currTimeStamp = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
		float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

		m_InputQueue.onFrame();
		m_Window.pollMessages();
		//Temp ------------------------------------------------
		//m_Graphics->renderModel(m_Buffer, m_CBuffer, m_Shader, &tempMatrix, false);
		/*for (int i = 0; i < 100; i++)
		{
			m_Graphics->renderModel(m_Buffer2, m_CBuffer, m_Shader2, &tempMatrix, false);
			m_Graphics->renderModel(m_Buffer2, m_CBuffer, m_Shader2, &tempMatrix, false);
			m_Graphics->renderModel(m_Buffer2, m_CBuffer, m_Shader2, &tempMatrix, false);
			m_Graphics->renderModel(m_Buffer2, m_CBuffer, m_Shader2, &tempMatrix, false);
			m_Graphics->renderModel(m_Buffer2, m_CBuffer, m_Shader2, &tempMatrix, false);
		}*/
		//Temp -------------------------------------------------
		m_Graphics->drawFrame(currView);

		for (auto& in : m_InputQueue.getFrameInputs())
		{
			if (in.m_Action == "exit")
			{
				m_ShouldQuit = true;
			}
			else if (in.m_Action == "connect" && in.m_Value == 1.0f)
			{
				m_Network.connect("localhost");
			}
			else if(in.m_Action ==  "changeViewN" && in.m_Value == 1)
			{
				currView--;
				if(currView < 0)
					currView = 3;
				printf("Changeview--\n", in.m_Action.c_str(), in.m_Value);
			}
			else if(in.m_Action ==  "changeViewP" && in.m_Value == 1)
			{
				currView++;
				if(currView >= 4)
					currView = 0;
				printf("Changeview++\n", in.m_Action.c_str(), in.m_Value);
			}
			else
			{
				//printf("Received input action: %s (%.2f)\n", in.m_Action.c_str(), in.m_Value);
			}
		}

		//TEMP ---------------------------------------------------------------------
		std::string output = "DeltaTime: " + std::to_string(dt) + " FPS: " + std::to_string(1.0f/dt);
		SetWindowTextA(m_Window.getHandle(),output.c_str()); 
		//TEMP ---------------------------------------------------------------------
		prevTimeStamp = currTimeStamp;
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
	
	const static UVec2 size = {1280, 720};
	return size;
}

bool BaseGameApp::handleWindowClose(WPARAM p_WParam, LPARAM p_LParam, LRESULT& p_Result)
{
	m_ShouldQuit = true;
	p_Result = 0;
	return true;
}

//BaseGameApp::vertex* BaseGameApp::createBOX(unsigned int size, float x, float y, float z)
//{
//	vertex* box = new vertex[36];
//	using namespace DirectX;
//	XMFLOAT4 center = XMFLOAT4(x,y,z,1.0f);
//    XMFLOAT4 color;
//	XMFLOAT3 tangent, binormal, normal;
//
//    XMFLOAT3 vert0 = XMFLOAT3(-1.0f*size, -1.0f*size,  1.0f*size); // 0 --+ 
//    XMFLOAT3 vert1 = XMFLOAT3( 1.0f*size, -1.0f*size,  1.0f*size); // 1 +-+ 
//    XMFLOAT3 vert2 = XMFLOAT3(-1.0f*size,  1.0f*size,  1.0f*size); // 2 -++ 
//    XMFLOAT3 vert3 = XMFLOAT3( 1.0f*size,  1.0f*size,  1.0f*size); // 3 +++ 
//    XMFLOAT3 vert4 = XMFLOAT3(-1.0f*size,  1.0f*size, -1.0f*size); // 4 -+- 
//    XMFLOAT3 vert5 = XMFLOAT3( 1.0f*size,  1.0f*size, -1.0f*size); // 5 ++- 
//    XMFLOAT3 vert6 = XMFLOAT3(-1.0f*size, -1.0f*size, -1.0f*size); // 6 --- 
//    XMFLOAT3 vert7 = XMFLOAT3( 1.0f*size, -1.0f*size, -1.0f*size); // 7 +-- 
//
//	//UV Coordinates
//	XMFLOAT2 uv[] = {
//				XMFLOAT2(0.375f, 1.0f - 0.f   ),
//				XMFLOAT2(0.625f, 1.0f - 0.f   ),
//				XMFLOAT2(0.375f, 1.0f - 0.25f),
//				XMFLOAT2(0.625f, 1.0f - 0.25f),
//				XMFLOAT2(0.375f, 1.0f - 0.5f ),
//				XMFLOAT2(0.625f, 1.0f - 0.5f ),
//				XMFLOAT2(0.375f, 1.0f - 0.75f),
//				XMFLOAT2(0.625f, 1.0f - 0.75f),
//				XMFLOAT2(0.375f, 1.0f - 1.f  ),
//				XMFLOAT2(0.625f, 1.0f - 1.f   ),
//				XMFLOAT2(0.875f, 1.0f - 0.f   ),
//				XMFLOAT2(0.875f, 1.0f - 0.25f),
//				XMFLOAT2(0.125f, 1.0f - 0.f   ),
//				XMFLOAT2(0.125f, 1.0f - 0.25f)
//	};
//	XMFLOAT3 normals[] = {	XMFLOAT3(0,0,-1),
//							XMFLOAT3(0,-1,0), 
//							XMFLOAT3(0,0,1),
//							XMFLOAT3(0,1,0),
//							XMFLOAT3(-1,0,0),
//							XMFLOAT3(1,0,0)};
//
//    // Back
//    color = XMFLOAT4(0.f,1.f,0.f,1.f);
//	//normal = XMFLOAT3(0.f,0.f,1.f);
//	CalculateTangentBinormal(vert0,vert2,vert1,uv[0],uv[2],uv[1],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//	//normal.z = -normal.z;
//    box[0] = vertex(vert0, normal,uv[0], tangent, binormal);
//    box[1] = vertex(vert2, normal,uv[2], tangent, binormal);
//    box[2] = vertex(vert1, normal,uv[1], tangent, binormal);
//
//	CalculateTangentBinormal(vert1,vert2,vert3,uv[1],uv[2],uv[3],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//    box[3] = vertex(vert1, normal, uv[1], tangent, binormal);
//    box[4] = vertex(vert2, normal, uv[2], tangent, binormal);
//    box[5] = vertex(vert3, normal, uv[3], tangent, binormal);
//
//    // Front
//	//color = XMFLOAT4(0.f,0.f,1.f,1.f);
//	//normal = XMFLOAT4(0.f,0.f,1.f,0.f);
//	CalculateTangentBinormal(vert2,vert4,vert3,uv[2],uv[4],uv[3],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//    box[6] =  vertex(vert2, normal,uv[2], tangent, binormal);
//    box[7] =  vertex(vert4, normal,uv[4], tangent, binormal);
//    box[8] =  vertex(vert3, normal,uv[3], tangent, binormal);
//
//	CalculateTangentBinormal(vert3,vert4,vert5,uv[3],uv[4],uv[5],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//	box[9] =  vertex(vert3, normal, uv[3], tangent, binormal);
//    box[10] = vertex(vert4, normal, uv[4], tangent, binormal);
//    box[11] = vertex(vert5, normal, uv[5], tangent, binormal);
//
//    // Top
//	//color = XMFLOAT4(1.f,0.f,0.f,1.f);
//	//normal = XMFLOAT3(0.f,-1.f,0.f);
//	/*CalculateTangentBinormal(vert4,vert6,vert5,uv[4],uv[6],uv[5],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//
//    box[12] = vertex(vert4, normal, uv[4], tangent, binormal);
//    box[13] = vertex(vert6, normal, uv[6], tangent, binormal);
//    box[14] = vertex(vert5, normal, uv[5], tangent, binormal);
//
//	CalculateTangentBinormal(vert5,vert6,vert7,uv[5],uv[6],uv[7],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//    box[15] = vertex(vert5, normal, uv[5], tangent, binormal);
//    box[16] = vertex(vert6, normal, uv[6], tangent, binormal);
//    box[17] = vertex(vert7, normal, uv[7], tangent, binormal);*/
//
//    // Bottom
//	//color = XMFLOAT4(1.f,1.f,0.f,1.f);
//	//normal = XMFLOAT3(0.f,1.f,0.f);
//	CalculateTangentBinormal(vert6,vert0,vert7,uv[6],uv[8],uv[7],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//    box[18] = vertex(vert6, normal, uv[6], tangent, binormal);
//    box[19] = vertex(vert0, normal, uv[8], tangent, binormal);
//    box[20] = vertex(vert7, normal, uv[7], tangent, binormal);
//
//	CalculateTangentBinormal(vert7,vert0,vert1,uv[7],uv[8], uv[9],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//    box[21] = vertex(vert7, normal, uv[7], tangent, binormal);
//    box[22] = vertex(vert0, normal, uv[8], tangent, binormal);
//    box[23] = vertex(vert1, normal, uv[9], tangent, binormal);
//
//    // Right
//	//color = XMFLOAT4(0.f,1.f,1.f,1.f);
//	//normal = XMFLOAT3(-1.f,0.f,0.f);
//	CalculateTangentBinormal(vert1,vert3,vert7,uv[1],uv[3],uv[10],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//    box[24] = vertex(vert1, normal, uv[1], tangent, binormal);
//    box[25] = vertex(vert3, normal, uv[3], tangent, binormal);
//    box[26] = vertex(vert7, normal, uv[10], tangent, binormal);
//
//	CalculateTangentBinormal(vert7,vert3,vert5,uv[10],uv[3], uv[11],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//    box[27] = vertex(vert7, normal, uv[10], tangent, binormal);
//    box[28] = vertex(vert3, normal, uv[3], tangent, binormal);
//    box[29] = vertex(vert5, normal, uv[11], tangent, binormal);
//
//    // Left
//	//color = XMFLOAT4(1.f,0.f,1.f,1.f);
//	//normal = XMFLOAT3(1.f,0.f,0.f);
//	CalculateTangentBinormal(vert6,vert4,vert0,uv[12],uv[13],uv[0],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//    box[30] = vertex(vert6, normal, uv[12], tangent, binormal);
//    box[31] = vertex(vert4, normal, uv[13], tangent, binormal);
//    box[32] = vertex(vert0, normal, uv[0], tangent, binormal);
//
//	CalculateTangentBinormal(vert0,vert4,vert2,uv[0],uv[13],uv[2],tangent,binormal);
//	CalculateNormal(tangent,binormal, normal);
//    box[33] = vertex(vert0, normal, uv[0], tangent, binormal);
//    box[34] = vertex(vert4, normal, uv[13], tangent, binormal);
//    box[35] = vertex(vert2, normal, uv[2], tangent, binormal);
//
//    return box;
//}
//void BaseGameApp::CalculateTangentBinormal(DirectX::XMFLOAT3 vertex1, DirectX::XMFLOAT3 vertex2, DirectX::XMFLOAT3 vertex3,
//										   DirectX::XMFLOAT2 uv1,DirectX::XMFLOAT2 uv2, DirectX::XMFLOAT2 uv3,
//											DirectX::XMFLOAT3& tangent, DirectX::XMFLOAT3& binormal)
//{
//	float vector1[3], vector2[3];
//	float tuVector[2], tvVector[2];
//	float den;
//	float length;
//
//
//	// Calculate the two vectors for this face.
//	vector1[0] = vertex2.x - vertex1.x;
//	vector1[1] = vertex2.y - vertex1.y;
//	vector1[2] = vertex2.z - vertex1.z;
//
//	vector2[0] = vertex3.x - vertex1.x;
//	vector2[1] = vertex3.y - vertex1.y;
//	vector2[2] = vertex3.z - vertex1.z;
//
//	// Calculate the tu and tv texture space vectors.
//	tuVector[0] = uv2.x - uv1.x;
//	tvVector[0] = uv2.y - uv1.y;
//
//	tuVector[1] = uv3.x - uv1.x;
//	tvVector[1] = uv3.y - uv1.y;
//
//	// Calculate the denominator of the tangent/binormal equation.
//	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);
//
//	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
//	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
//	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
//	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;
//
//	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
//	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
//	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;
//
//	// Calculate the length of this normal.
//	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
//			
//	// Normalize the normal and then store it
//	tangent.x = tangent.x / length;
//	tangent.y = tangent.y / length;
//	tangent.z = tangent.z / length;
//
//	// Calculate the length of this normal.
//	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));
//			
//	// Normalize the normal and then store it
//	binormal.x = binormal.x / length;
//	binormal.y = binormal.y / length;
//	binormal.z = binormal.z / length;
//
//	return;
//}
//void BaseGameApp::CalculateNormal(DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT3 binormal, DirectX::XMFLOAT3& normal)
//{
//	float length;
//
//
//	// Calculate the cross product of the tangent and binormal which will give the normal vector.
//	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
//	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
//	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);
//	
//	// Calculate the length of the normal.
//	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
//
//	// Normalize the normal.
//	normal.x = normal.x / length;
//	normal.y = normal.y / length;
//	normal.z = normal.z / length;
//
//	return;
//}