#include <boost/test/unit_test.hpp>
#include "../../Graphics/Source/ModelFactory.h"

BOOST_AUTO_TEST_SUITE(TestModelFactory)

using namespace DirectX;

class BufferStub : public Buffer
{
public:
	Type type;
	Usage usage;
	UINT32 numOfElements;
	UINT32 sizeOfElement;
	BufferStub(){}
	~BufferStub()
	{

	}

	void initialize(ID3D11Device *p_Device, ID3D11DeviceContext *p_DeviceContext, Description &p_Description)
	{
		type = p_Description.type;
		usage = p_Description.usage;
		numOfElements = p_Description.numOfElements;
		sizeOfElement = p_Description.sizeOfElement;
	}
};

class ModelFactoryStub : public ModelFactory
{
private:
	static ModelFactoryStub *mInstance;
public:
	static ModelFactoryStub *getInstance()
	{
		if(!mInstance)
			mInstance = new ModelFactoryStub();
		
		return mInstance;
	}

	ModelDefinition createModel(const char *filename, bool isAnimated)
	{
		ModelDefinition model;
		BufferStub::Description bufferDescription;
		
		if(!isAnimated)
		{
			vector<StaticVertex> vertexData;
			StaticVertex vertex;
			vertex.m_Binormal = XMFLOAT3(1.0f, 1.0f, 1.0f);
			vertex.m_Normal = XMFLOAT3(1.0f, 1.0f, 1.0f);
			vertex.m_Position = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex.m_Tangent = XMFLOAT3(1.0f, 1.0f, 1.0f);
			vertex.m_UV = XMFLOAT2(1.0f, 1.0f);
			vertexData.push_back(vertex);
			bufferDescription = createBufferDescription(vertexData, Buffer::Usage::USAGE_IMMUTABLE); //Change to default when needed to change data.
		}
		else
		{
			vector<AnimatedVertex> vertexData;
			AnimatedVertex vertex;
			vertex.m_Binormal = XMFLOAT3(1.0f, 1.0f, 1.0f);
			vertex.m_BoneId = XMINT4(1, 1, 1, 1);
			vertex.m_Normal = XMFLOAT3(1.0f, 1.0f, 1.0f);
			vertex.m_Position = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex.m_Tangent = XMFLOAT3(1.0f, 1.0f, 1.0f);
			vertex.m_UV = XMFLOAT2(1.0f, 1.0f);
			vertex.m_Weights = XMFLOAT3(1.0f, 1.0f, 1.0f);
			vertexData.push_back(vertex);
			bufferDescription = createBufferDescription(vertexData, Buffer::Usage::USAGE_IMMUTABLE); //Change to default when needed to change data.
		}
		std::unique_ptr<BufferStub> vertexBuffer(new BufferStub());
		vertexBuffer->initialize(nullptr, nullptr, bufferDescription);
		model.vertexBuffer.swap(std::unique_ptr<Buffer>(vertexBuffer.release()));

		return model;
	}

	Buffer::Description createBufferDescription(vector<StaticVertex> vertexData, Buffer::Usage usage)
	{
		Buffer::Description bufferDescription;
		bufferDescription.initData = nullptr;
		bufferDescription.numOfElements = vertexData.size();
		bufferDescription.sizeOfElement = sizeof(StaticVertex);
		bufferDescription.type = Buffer::Type::VERTEX_BUFFER;
		bufferDescription.usage = usage;

		return bufferDescription;
	}

	Buffer::Description createBufferDescription(vector<AnimatedVertex> vertexData, Buffer::Usage usage)
	{
		Buffer::Description bufferDescription;
		bufferDescription.initData = nullptr;
		bufferDescription.numOfElements = vertexData.size();
		bufferDescription.sizeOfElement = sizeof(AnimatedVertex);
		bufferDescription.type = Buffer::Type::VERTEX_BUFFER;
		bufferDescription.usage = usage;

		return bufferDescription;
	}

	~ModelFactoryStub(){}

private:
	ModelFactoryStub(){}
};

ModelFactoryStub *ModelFactoryStub::mInstance = nullptr;

//BOOST_AUTO_TEST_CASE(TestCreateStaticModel)
//{
//	ModelFactoryStub *factory = ModelFactoryStub::getInstance();
//
//	ModelDefinition *model = &factory->createModel("n/a", false);
//	
//	BOOST_CHECK(model != nullptr);
//
//	//SAFE_DELETE(model);
//	SAFE_DELETE(factory);
//}
//
//BOOST_AUTO_TEST_CASE(TestCreateAnimatedModel)
//{
//	ModelFactoryStub *factory = ModelFactoryStub::getInstance();
//
//	ModelDefinition *model = &factory->createModel("n/a", true);
//
//	BOOST_CHECK(model != nullptr);
//
//	//SAFE_DELETE(model);
//	SAFE_DELETE(factory);
//}

BOOST_AUTO_TEST_SUITE_END()