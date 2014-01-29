#include <boost/test/unit_test.hpp>
#include "IGraphics.h"
#include "../../Graphics/Source/WrapperFactory.h"
#include "../../Graphics/Source/ModelFactory.h"
#include "../../Graphics/Source/Buffer.h"
#include "../../Graphics/Source/Shader.h"
#include "../../Graphics/Source/SkyDome.h"
#include "../../Graphics/Source/VRAMInfo.h"
#include "../../Client/Source/Window.h"
#include "../../Common/Source/ResourceManager.h"

#include <string>

#if _DEBUG
#include <vld.h> 
#endif

BOOST_AUTO_TEST_SUITE(GraphicsEngine)
	static std::string testId = "1> ";

	BOOST_AUTO_TEST_CASE(TestGraphics)
{
	BOOST_MESSAGE(testId + "Testing Graphics...");
	
	//Step 1
	Window window;
	Vector2 winSize = Vector2(1280, 720);
	BOOST_MESSAGE(testId + "Creating window");
	BOOST_CHECK_NO_THROW(window.init("Test Graphics", Vector2ToXMFLOAT2(&winSize)));

	IGraphics *graphics = IGraphics::createGraphics();
	BOOST_MESSAGE(testId + "Checking init graphics with window");
	BOOST_CHECK(graphics->initialize(window.getHandle(), (int)winSize.x, (int)winSize.y, false));
	

	//Step 2
	WrapperFactory *wrapperFactory = nullptr;
	wrapperFactory = WrapperFactory::getInstance();
	BOOST_MESSAGE(testId + "Checking the pointer to WrapperFactory is getting hold of.");
	BOOST_CHECK(wrapperFactory != nullptr);

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
	
	
	//Step 3
	Shader *testShader = nullptr;
	BOOST_MESSAGE(testId + "Testing WrapperFactory...");
	BOOST_MESSAGE(testId + "Creating vertex and pixel shader with WrapperFactory using hlsl shader layout");
	BOOST_CHECK_NO_THROW(testShader = wrapperFactory->createShader(L"../Source/testShader_VS_PS.hlsl", "mainVS,mainPS", "5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER));
	BOOST_CHECK(testShader != nullptr);

	BOOST_MESSAGE(testId + "Trying to add shader step which does not exist, expecting ShaderException");
	BOOST_CHECK_THROW(wrapperFactory->addShaderStep(testShader, L"../Source/testShader_VS_PS.hlsl", "mainGS", "5_0",
		ShaderType::GEOMETRY_SHADER), ShaderException);
	SAFE_DELETE(testShader);

	BOOST_MESSAGE(testId + "Creating vertex shader with WrapperFactory using hlsl shader layout");
	BOOST_CHECK_NO_THROW(testShader = wrapperFactory->createShader(L"../Source/testShader_VS.hlsl", "mainVS", "5_0",
		ShaderType::VERTEX_SHADER));

	BOOST_MESSAGE(testId + "Adding pixel shader with WrapperFactory");
	BOOST_CHECK_NO_THROW(wrapperFactory->addShaderStep(testShader, L"../Source/testShader_PS.hlsl", "mainPS", "5_0",
		ShaderType::PIXEL_SHADER));
	SAFE_DELETE(testShader);

	BOOST_MESSAGE(testId + "Trying to create vertex shader with wrong filename, expecting ShaderException");
	BOOST_CHECK_THROW(testShader = wrapperFactory->createShader(L"../Source/IDoNotExist.hlsl", "mainVS", "5_0",
		ShaderType::VERTEX_SHADER), ShaderException);

	BOOST_MESSAGE(testId + "Trying to create vertex shader with wrong entry point, expecting ShaderException");
	BOOST_CHECK_THROW(testShader = wrapperFactory->createShader(L"../Source/testShader_VS.hlsl", "entry", "5_0",
		ShaderType::VERTEX_SHADER), ShaderException);

	ShaderInputElementDescription desc[] = 
	{
		{"POSITION", 0, Format::R32G32B32_FLOAT, 0, 0, 0, 0},
		{"NORMAL", 0, Format::R32G32B32_FLOAT, 1, 12, 0, 0},
		{"TEXCOORD", 0, Format::R32G32B32_FLOAT, 2, 24, 0, 0}
	};
	int size = sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	BOOST_MESSAGE(testId + "Creating vertex shader with user defined input description");
	BOOST_CHECK_NO_THROW(testShader = wrapperFactory->createShader(L"../Source/testShader_VS.hlsl", "mainVS", "5_0",
		ShaderType::VERTEX_SHADER, desc, size));

	BOOST_MESSAGE(testId + "Trying to add vertex shader to already existing vertex shader, expecting WrapperFactoryException");
	BOOST_CHECK_THROW(wrapperFactory->addShaderStep(testShader, L"../Source/testShader_VS.hlsl", "mainVS", "5_0",
		ShaderType::VERTEX_SHADER), WrapperFactoryException);
	SAFE_DELETE(testShader);

	BOOST_MESSAGE(testId + "Trying to create vertex shader with user defined input description, wrong shader model, expecting ShaderException");
	BOOST_CHECK_THROW(testShader = wrapperFactory->createShader(L"../Source/testShader_VS.hlsl", "mainVS", "5_5",
		ShaderType::VERTEX_SHADER, desc, size), ShaderException);
	SAFE_DELETE(testShader);
	BOOST_MESSAGE(testId + "...finished WrapperFactory");

	//Step 4
	BOOST_MESSAGE(testId + "Trying to create textures that does not exist, expecting exception");
	BOOST_CHECK_THROW(graphics->createTexture("MyTexture", "IDoNotExist.png"), TextureLoaderException);
	BOOST_CHECK_THROW(graphics->createTexture("MyTexture", "IDoNotExistEither.dds"), TextureLoaderException);

	BOOST_MESSAGE(testId + "Trying to create texture with wrong extension");
	BOOST_CHECK(!graphics->createTexture("MyTexture", "../../Client/Bin/assets/textures/Default_COLOR.dss"));

	BOOST_MESSAGE(testId + "Creating texture from DDS");
	BOOST_CHECK(graphics->createTexture("MyTexture", "../../Client/Bin/assets/textures/Default_COLOR.dds"));
	
	BOOST_MESSAGE(testId + "Trying to release a texture that has not been loaded");
	BOOST_CHECK(!graphics->releaseTexture("ATextureThatDoesNotExist"));

	BOOST_MESSAGE(testId + "Releasing a loaded texture");
	BOOST_CHECK(graphics->releaseTexture("MyTexture"));


	//Step 5
	int memUsage = graphics->getVRAMUsage();
	BOOST_MESSAGE(testId + "Testing the VRAM usage update logic");
	BOOST_CHECK_NO_THROW(VRAMInfo::getInstance()->updateUsage(VRAMInfo::getInstance()->calculateFormatUsage(
		DXGI_FORMAT::DXGI_FORMAT_BC3_UNORM, 16 , 16)));
	BOOST_CHECK(graphics->getVRAMUsage() - memUsage == 256);
	

	//Step 6
	ResourceManager *resourceManager = new ResourceManager();
	BOOST_MESSAGE(testId + "Setting callback functions for load/release textures");
	BOOST_CHECK_NO_THROW(graphics->setLoadModelTextureCallBack(&ResourceManager::loadModelTexture, resourceManager));
	BOOST_CHECK_NO_THROW(graphics->setReleaseModelTextureCallBack(&ResourceManager::releaseModelTexture, resourceManager));

	
	//Step 7
	ModelFactory *modelFactory = nullptr;
	modelFactory = ModelFactory::getInstance();
	BOOST_MESSAGE(testId + "Checking the pointer to ModelFactory is getting hold of");
	BOOST_CHECK(wrapperFactory != nullptr);

	BOOST_MESSAGE(testId + "Creating model object of Barrel1.btx");
	BOOST_REQUIRE(graphics->createTexture("barrelColor", "../../Client/Bin/assets/textures/Barrel_COLOR.dds"));
	BOOST_REQUIRE(graphics->createTexture("barrelNormal", "../../Client/Bin/assets/textures/Barrel_NRM.dds"));
	BOOST_REQUIRE(graphics->createTexture("barrelSpecular", "../../Client/Bin/assets/textures/Barrel_SPEC.dds"));
	BOOST_CHECK(graphics->createModel("barrel", "../Source/Barrel1.btx"));

	BOOST_MESSAGE(testId + "Creating model object of Witch_Running_5.btx");
	BOOST_REQUIRE(graphics->createTexture("bodyColor", "../../Client/Bin/assets/textures/body_COLOR.dds"));
	BOOST_REQUIRE(graphics->createTexture("bodyNormal", "../../Client/Bin/assets/textures/body_NRM.dds"));
	BOOST_REQUIRE(graphics->createTexture("defaultSpecular", "../../Client/Bin/assets/textures/Default_SPEC.dds"));
	BOOST_REQUIRE(graphics->createTexture("accessoriesColor", "../../Client/Bin/assets/textures/Accessories_COLOR.dds"));
	BOOST_REQUIRE(graphics->createTexture("accessoriesNormal", "../../Client/Bin/assets/textures/Accessories_NRM.dds"));
	BOOST_CHECK(graphics->createModel("witch", "../Source/Witch_5_Running.btx"));

	//Step 8
	BOOST_MESSAGE(testId + "Testing Graphics.createShader...");
	BOOST_MESSAGE(testId + "Creating vertex and index shaders using hlsl shader layout");
	BOOST_CHECK_NO_THROW(graphics->createShader("myID", L"../Source/testShader_VS_PS.hlsl", "mainVS,mainPS", "5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER));

	BOOST_MESSAGE(testId + "Trying to create vertex and index shaders using hlsl shader layout and same ID, expecting WrapperFactoryException");
	BOOST_CHECK_THROW(graphics->createShader("myID", L"../Source/testShader_VS_PS.hlsl", "mainVS,mainPS", "5_0",
		ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER), WrapperFactoryException);

	BOOST_MESSAGE(testId + "Trying to add shader step which does not exist, expecting ShaderExpection");
	BOOST_CHECK_THROW(graphics->createShader("myID", L"../Source/testShader_VS_PS.hlsl", "mainGS", "5_0",
		ShaderType::GEOMETRY_SHADER), ShaderException);

	BOOST_MESSAGE(testId + "Trying to create vertex shader with wrong filename, expecting ShaderException");
	BOOST_CHECK_THROW(graphics->createShader("myOtherID", L"../Source/IDoNotExist.hlsl", "main", "5_0",
		ShaderType::VERTEX_SHADER), ShaderException);

	BOOST_MESSAGE(testId + "Trying to create vertex shader with wrong entry point, expecting ShaderException");
	BOOST_CHECK_THROW(graphics->createShader("myAnotherID", L"../Source/testShader_VS.hlsl", "entry", "5_0",
		ShaderType::VERTEX_SHADER), ShaderException);

	BOOST_MESSAGE(testId + "Creating vertex shader with user defined input description");
	BOOST_CHECK_NO_THROW(graphics->createShader("userDefinedDesc", L"../Source/testShader_VS.hlsl", "mainVS", "5_0",
		ShaderType::VERTEX_SHADER, desc, size));

	BOOST_MESSAGE(testId + "Trying to create vertex shader with user defined input description, wrong shader model, expecting shader exception");
	BOOST_CHECK_THROW(graphics->createShader("anotherUserDefinedDesc", L"../Source/testShader_VS.hlsl", "main", "5_5",
		ShaderType::VERTEX_SHADER, desc, size), ShaderException);

	BOOST_MESSAGE(testId + "Creating ForwardShader.hlsl");
	BOOST_CHECK_NO_THROW(graphics->createShader("forwardShader", L"../../Graphics/Source/DeferredShaders/ForwardShader.hlsl",
		"VS,PS", "5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER));

	BOOST_MESSAGE(testId + "Creating AnimatedGeometryPass.hlsl");
	BOOST_CHECK_NO_THROW(graphics->createShader("animatedShader", L"../../Graphics/Source/DeferredShaders/AnimatedGeometryPass.hlsl",
		"VS,PS", "5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER));
	BOOST_MESSAGE(testId + "...finished Graphics.createShader");


	//Step 9 TODO: Might need to verify
	BOOST_MESSAGE(testId + "Linking barrel with forwardShader");
	graphics->linkShaderToModel("forwardShader", "barrel");
	BOOST_MESSAGE(testId + "Linking witch with animatedShader");
	graphics->linkShaderToModel("animatedShader", "witch");


	//Step 10
	BOOST_MESSAGE(testId + "Creating model instances of existing objects");
	BOOST_CHECK_EQUAL(graphics->createModelInstance("barrel"), 1);
	BOOST_CHECK_EQUAL(graphics->createModelInstance("barrel"), 2);
	BOOST_CHECK_EQUAL(graphics->createModelInstance("barrel"), 3);
	BOOST_CHECK_EQUAL(graphics->createModelInstance("barrel"), 4);
	BOOST_CHECK_EQUAL(graphics->createModelInstance("witch"), 5);
	BOOST_CHECK_EQUAL(graphics->createModelInstance("witch"), 6);

	BOOST_MESSAGE(testId + "Trying to create model instances of non existing objects, expecting -1");
	BOOST_CHECK_EQUAL(graphics->createModelInstance("box"), -1);
	BOOST_CHECK_EQUAL(graphics->createModelInstance("house"), -1);

	BOOST_MESSAGE(testId + "Creating model instance of existing objects to verify ID's continue correctly");
	BOOST_CHECK_EQUAL(graphics->createModelInstance("barrel"), 7);


	//Step 11
	BOOST_MESSAGE(testId + "Setting position for barrels");
	BOOST_CHECK_NO_THROW(graphics->setModelPosition(2, Vector3(1.0f, 1.0f, 1.0f)));
	BOOST_CHECK_NO_THROW(graphics->setModelPosition(7, Vector3(5.0f, 1.0f, 1.0f)));
	BOOST_MESSAGE(testId + "Setting position for witch");
	BOOST_CHECK_NO_THROW(graphics->setModelPosition(6, Vector3(2.0f, 1.0f, 3.0f)));

	BOOST_MESSAGE(testId + "Setting position for non existant instance, expecting GraphicsException");
	BOOST_CHECK_THROW(graphics->setModelPosition(8, Vector3(2.0f, 1.0f, 3.0f)), GraphicsException);

	BOOST_MESSAGE(testId + "Setting rotation for barrels");
	BOOST_CHECK_NO_THROW(graphics->setModelRotation(4, Vector3(5.0f, 2.0f, 5.0f)));
	BOOST_CHECK_NO_THROW(graphics->setModelRotation(7, Vector3(4.0f, 3.0f, 1.0f)));
	BOOST_MESSAGE(testId + "Setting rotation for witch");
	BOOST_CHECK_NO_THROW(graphics->setModelRotation(5, Vector3(2.0f, 1.0f, 3.0f)));

	BOOST_MESSAGE(testId + "Setting rotation for non existant instance, expecting GraphicsException");
	BOOST_CHECK_THROW(graphics->setModelRotation(8, Vector3(2.0f, 1.0f, 3.0f)), GraphicsException);

	BOOST_MESSAGE(testId + "Setting scale for barrels");
	BOOST_CHECK_NO_THROW(graphics->setModelScale(3, Vector3(0.5f, 0.5f, 0.5f)));
	BOOST_CHECK_NO_THROW(graphics->setModelScale(7, Vector3(5.0f, 5.0f, 5.0f)));
	BOOST_MESSAGE(testId + "Setting scale for witch");
	BOOST_CHECK_NO_THROW(graphics->setModelScale(5, Vector3(2.0f, 2.0f, 2.0f)));

	BOOST_MESSAGE(testId + "Setting scale for non existant instance, expecting GraphicsException");
	BOOST_CHECK_THROW(graphics->setModelScale(8, Vector3(2.0f, 1.0f, 3.0f)), GraphicsException);

	BOOST_MESSAGE(testId + "Setting color tone for barrels");
	BOOST_CHECK_NO_THROW(graphics->setModelColorTone(1, Vector3(0.5f, 0.5f, 1.0f)));
	BOOST_CHECK_NO_THROW(graphics->setModelColorTone(4, Vector3(0.0f, 1.0f, 0.3f)));
	BOOST_MESSAGE(testId + "Setting color tone for witch");
	BOOST_CHECK_NO_THROW(graphics->setModelColorTone(6, Vector3(0.5f, 0.0f, 0.7f)));

	BOOST_MESSAGE(testId + "Setting color tone for non existant instance, expecting GraphicsException");
	BOOST_CHECK_THROW(graphics->setModelColorTone(8, Vector3(1.0f, 1.0f, 1.0f)), GraphicsException);


	//Step 12
	SkyDome *skydome = new SkyDome();
	BOOST_MESSAGE(testId + "Initializing standalone skydome");
	BOOST_CHECK(skydome->init(1000.0f));
	BOOST_MESSAGE(testId + "Checking size of vertices vector to be expected size 240");
	BOOST_CHECK(skydome->getVertices().size() == 240);
	BOOST_MESSAGE(testId + "Deleting standalone skydome");
	SAFE_DELETE(skydome);

	BOOST_MESSAGE(testId + "Creating skydome using Graphics");
	BOOST_REQUIRE(graphics->createTexture("skydomeTexture", "../../Client/Bin/assets/textures/Skybox0.dds"));
	BOOST_CHECK_NO_THROW(graphics->createSkydome("skydomeTexture", 1000.0f));

	
	//Step 13
	BOOST_MESSAGE(testId + "Creating model object of Checkpoint.btx");
	BOOST_CHECK(graphics->createModel("checkpoint", "../Source/Checkpoint.btx"));

	BOOST_MESSAGE(testId + "Setting model object checkpoint to transparent");
	BOOST_CHECK_NO_THROW(graphics->setModelDefinitionTransparency("checkpoint", true));
	BOOST_MESSAGE(testId + "Trying to set non-existing model object to transparent, expecting GraphicsException");
	BOOST_CHECK_THROW(graphics->setModelDefinitionTransparency("doNotExist", true), GraphicsException);	
	
	BOOST_MESSAGE(testId + "Linking checkpoint with forwardShader");
	graphics->linkShaderToModel("forwardShader", "checkpoint");

	BOOST_MESSAGE(testId + "Creating model instances of checkpoint");
	BOOST_CHECK_EQUAL(graphics->createModelInstance("checkpoint"), 8);
	BOOST_CHECK_EQUAL(graphics->createModelInstance("checkpoint"), 9);


	//Step 14
	BOOST_MESSAGE(testId + "Setting clear color and adding frame lights");
	graphics->setClearColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	graphics->useFrameDirectionalLight(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, -1.0f, 1.0f));
	graphics->useFramePointLight(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 1000.0f);
	graphics->useFrameSpotLight(Vector3(1.0f, 5.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3(1.0f, -1.0f, 0.0f),
		Vector2(1.7f, 0.5f), 10000.0f);
	for(int i = 1; i <= 9; i++)
	{
		BOOST_MESSAGE(testId + "Rendering model instance: " + std::to_string(i));
		BOOST_CHECK_NO_THROW(graphics->renderModel(i));
	}
	
	BOOST_MESSAGE(testId + "Drawing frame using the prepared lights and model instances");
	BOOST_CHECK_NO_THROW(graphics->drawFrame());


	//Step 15
	BOOST_MESSAGE(testId + "Erasing model instance 6");
	BOOST_CHECK_NO_THROW(graphics->eraseModelInstance(6));
	BOOST_MESSAGE(testId + "Erasing non-existant model instance, expecting GraphicsException");
	BOOST_CHECK_THROW(graphics->eraseModelInstance(10), GraphicsException);
	BOOST_MESSAGE(testId + "Releasing model object of Barrel1.btx");
	BOOST_CHECK(graphics->releaseModel("barrel"));
	BOOST_MESSAGE(testId + "Releasing model object of Witch_Running_5.btx");
	BOOST_CHECK(graphics->releaseModel("witch"));
	BOOST_MESSAGE(testId + "Releasing model object of Checkpoint.btx");
	BOOST_CHECK(graphics->releaseModel("checkpoint"));
	BOOST_CHECK(graphics->releaseTexture("barrelColor"));
	BOOST_CHECK(graphics->releaseTexture("barrelNormal"));
	BOOST_CHECK(graphics->releaseTexture("barrelSpecular"));
	BOOST_CHECK(graphics->releaseTexture("bodyColor"));
	BOOST_CHECK(graphics->releaseTexture("bodyNormal"));
	BOOST_CHECK(graphics->releaseTexture("defaultSpecular"));
	BOOST_CHECK(graphics->releaseTexture("accessoriesColor"));
	BOOST_CHECK(graphics->releaseTexture("accessoriesNormal"));
	

	//Step 16
	BOOST_MESSAGE(testId + "Deleting shader forwardShader");
	BOOST_CHECK_NO_THROW(graphics->deleteShader("forwardShader"));
	BOOST_MESSAGE(testId + "Trying to delete non-existing shader, expecting GraphicsException");
	BOOST_CHECK_THROW(graphics->deleteShader("nonExistingShader"), GraphicsException);


	//Step 17
	BOOST_MESSAGE(testId + "Shutting down and deleting Graphics");
	SAFE_DELETE(resourceManager);
	IGraphics::deleteGraphics(graphics);
	wrapperFactory = nullptr;
	modelFactory = nullptr;

	BOOST_MESSAGE(testId + "Testing Graphics completed\n");
}

BOOST_AUTO_TEST_SUITE_END()