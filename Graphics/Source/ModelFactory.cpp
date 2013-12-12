#include "ModelFactory.h"


#include <boost/filesystem.hpp>

ModelFactory *ModelFactory::m_Instance = nullptr;

ModelFactory *ModelFactory::getInstance(void)
{
	if(!m_Instance)
		m_Instance = new ModelFactory();

	return m_Instance;
}

void ModelFactory::initialize(vector<pair<string, ID3D11ShaderResourceView*>> *p_TextureList)
{
	if(!m_Instance)
		throw ModelFactoryException("Error when initializing ModelFactory, no instance exists", __LINE__, __FILE__);

	m_TextureList = p_TextureList;
}

void ModelFactory::shutdown(void)
{
	SAFE_DELETE(m_Instance);
}

ModelDefinition ModelFactory::createStaticModel(const char *p_Filename)
{
	ModelDefinition model;
	
	ModelBinaryLoader modelLoader;

	modelLoader.loadBinaryFile(p_Filename);

	const vector<Vertex>& temp = modelLoader.getVertexBuffer();
	Vertex tempVertex;
	const vector<Material>& tempMaterial = modelLoader.getMaterial();
	const vector<MaterialBuffer>& tempMaterialBuffer = modelLoader.getMaterialBuffer();

	///REMOVE THIS///
	Buffer::Description bufferDescription;
	bufferDescription.initData = temp.data();
	bufferDescription.numOfElements = temp.size();
	bufferDescription.sizeOfElement = sizeof(Vertex);
	bufferDescription.type = Buffer::Type::VERTEX_BUFFER;
	bufferDescription.usage = Buffer::Usage::USAGE_IMMUTABLE; // Change to default when needed to change data.
	std::unique_ptr<Buffer> vertexBuffer(WrapperFactory::getInstance()->createBuffer(bufferDescription));

	boost::filesystem::path modelPath(p_Filename);
	boost::filesystem::path parentDir(modelPath.parent_path());

	//vertexBuffer = WrapperFactory::getInstance()->createBuffer(createBufferDescription(temp));
	vector<std::pair<int,int>> tempInterval(tempMaterialBuffer.size());
	for(unsigned int i = 0; i < tempMaterialBuffer.size(); i++)
	{
		tempInterval.at(i).first = tempMaterialBuffer.at(i).start;
		tempInterval.at(i).second = tempMaterialBuffer.at(i).length;
	}
	loadTextures(p_Filename, tempMaterial.size() , tempMaterial, model);

	model.vertexBuffer.swap(vertexBuffer);
	model.drawInterval = tempInterval;
	model.numOfMaterials = tempMaterial.size();
	modelLoader.clear();

	return model;
}

ModelDefinition ModelFactory::createAnimatedModel(const char *p_Filename)
{
	ModelDefinition model;

	return model;
}

void ModelFactory::setLoadModelTextureCallBack(loadModelTextureCallBack p_LoadModelTexture, void* p_Userdata)
{
	m_LoadModelTexture = p_LoadModelTexture;
	m_LoadModelTextureUserdata = p_Userdata;
}

ModelFactory::ModelFactory(void)
{
}

ModelFactory::~ModelFactory(void)
{
}

Buffer::Description ModelFactory::createBufferDescription(vector<Vertex> p_VertexBuffer)
{
	Buffer::Description bufferDescription;
	//bufferDescription.initData = temp.data();
	//bufferDescription.numOfElements = temp.size();
	//bufferDescription.sizeOfElement = sizeof(Vertex);
	//bufferDescription.type = Buffer::Type::VERTEX_BUFFER;
	//bufferDescription.usage = Buffer::Usage::USAGE_IMMUTABLE; // Change to default when needed to change data.
	
	return bufferDescription;
}

ID3D11ShaderResourceView *ModelFactory::getTextureFromList(string p_Identifier)
{
	for(auto it = m_TextureList->begin(); it != m_TextureList->end(); ++it)
	{
		if(it->first == p_Identifier)
		{
			return it->second;
		}
	}

	return nullptr;
}

void ModelFactory::loadTextures( const char * p_Filename, unsigned int numOfMaterials,
	const vector<Material> &tempM, ModelDefinition &model)
{
	using std::pair;

	boost::filesystem::path modelPath(p_Filename);
	boost::filesystem::path parentDir(modelPath.parent_path());

	vector<pair<string, ID3D11ShaderResourceView*>> diffuse;
	vector<pair<string, ID3D11ShaderResourceView*>> normal;
	vector<pair<string, ID3D11ShaderResourceView*>> specular;

	for(unsigned int i = 0; i < numOfMaterials; i++)
	{
		const Material& mat = tempM.at(i);
		boost::filesystem::path diff = (mat.m_DiffuseMap == "NONE") ? "assets/Default/Default_COLOR.jpg" : parentDir / mat.m_DiffuseMap;
		boost::filesystem::path norm = (mat.m_NormalMap == "NONE" || mat.m_NormalMap == "Default_NRM.jpg") ? "assets/Default/Default_COLOR.jpg" : parentDir / mat.m_NormalMap;
		boost::filesystem::path spec = (mat.m_SpecularMap == "NONE" || mat.m_SpecularMap == "Default_SPEC.jpg") ? "assets/Default/Default_SPEC.jpg" : parentDir / mat.m_SpecularMap;

		m_LoadModelTexture(mat.m_DiffuseMap.c_str(), diff.string().c_str(), m_LoadModelTextureUserdata);
		m_LoadModelTexture(mat.m_NormalMap.c_str(), norm.string().c_str(), m_LoadModelTextureUserdata);
		m_LoadModelTexture(mat.m_SpecularMap.c_str(), spec.string().c_str(), m_LoadModelTextureUserdata);

		diffuse.push_back( std::make_pair( mat.m_DiffuseMap, getTextureFromList( mat.m_DiffuseMap.c_str() ) ));
		normal.push_back( std::make_pair( mat.m_NormalMap, getTextureFromList( mat.m_NormalMap.c_str() ) ));
		specular.push_back( std::make_pair( mat.m_SpecularMap, getTextureFromList( mat.m_SpecularMap.c_str() ) ));

	}

	model.diffuseTexture	= diffuse;
	model.normalTexture		= normal;
	model.specularTexture	= specular;
}
