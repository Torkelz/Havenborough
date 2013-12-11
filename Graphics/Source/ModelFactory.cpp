#include "ModelFactory.h"
#include "ModelLoader.h"

#include <vector>
#include <boost/filesystem.hpp>

using std::vector;

ModelFactory *ModelFactory::m_Instance = nullptr;

ModelFactory *ModelFactory::getInstance(void)
{
	if(!m_Instance)
		m_Instance = new ModelFactory();

	return m_Instance;
}

void ModelFactory::shutdown(void)
{
	SAFE_DELETE(m_Instance);
}

ModelDefinition ModelFactory::createStaticModel(const char *p_Filename)
{
	ModelDefinition model;

	//ModelLoader modelLoader;

	//modelLoader.loadFile(p_Filename);

	//vector<std::vector<ModelLoader::IndexDesc>>	tempF = modelLoader.getIndices();
	//vector<DirectX::XMFLOAT3> tempN	= modelLoader.getNormals();
	//vector<DirectX::XMFLOAT3> tempT	= modelLoader.getTangents();
	//vector<DirectX::XMFLOAT2> tempUV = modelLoader.getTextureCoords();
	//vector<DirectX::XMFLOAT3> tempVert = modelLoader.getVertices();
	//vector<ModelLoader::Material> tempM	= modelLoader.getMaterial();

	//vector<Vertex> temp;
	//vector<vector<int>> tempI;

	//vector<int> I;
	//int indexCounter = 0;
	//for(unsigned int i = 0; i < tempF.size(); i++)
	//{
	//	const vector<ModelLoader::IndexDesc>& indexDescList = tempF.at(i);

	//	I.reserve(indexDescList.size());

	//	for(unsigned int j = 0; j < indexDescList.size();j++)
	//	{
	//		const ModelLoader::IndexDesc& indexDesc = indexDescList.at(j);

	//		temp.push_back(Vertex(tempVert.at(indexDesc.m_Vertex),
	//			tempN.at(indexDesc.m_Normal),
	//			tempUV.at(indexDesc.m_TextureCoord),
	//			tempT.at(indexDesc.m_Tangent)));

	//		temp.back().position.x *= -1.f;
	//		temp.back().normal.x *= -1.f;
	//		temp.back().tangent.x *= -1.f;
	//		temp.back().binormal.x *= -1.f;

	//		I.push_back(indexCounter);
	//		indexCounter++;
	//	}

	//	tempI.push_back(I);
	//	I.clear();
	//}

	//// Create Vertex buffer.
	//Buffer::Description bufferDescription;
	//bufferDescription.initData = temp.data();
	//bufferDescription.numOfElements = temp.size();
	//bufferDescription.sizeOfElement = sizeof(ModelLoader::Vertex);
	//bufferDescription.type = Buffer::Type::VERTEX_BUFFER;
	//bufferDescription.usage = Buffer::Usage::USAGE_IMMUTABLE; // Change to default when needed to change data.
	//std::unique_ptr<Buffer> vertexBuffer(WrapperFactory::getInstance()->createBuffer(bufferDescription));
	//temp.clear();

	//// Create Index buffer.
	//unsigned int nrIndexBuffers = tempI.size();
	//std::vector<std::unique_ptr<Buffer>> indices;
	//bufferDescription.type = Buffer::Type::INDEX_BUFFER;
	////bufferDescription.usage = Buffer::Usage::USAGE_IMMUTABLE;// Change to default when needed to change data.
	//bufferDescription.sizeOfElement = sizeof(int);


	//for(unsigned int i = 0; i < nrIndexBuffers; i++)
	//{
	//	bufferDescription.initData = tempI.at(i).data();
	//	bufferDescription.numOfElements = tempI.at(i).size();

	//	indices.push_back(std::unique_ptr<Buffer>(WrapperFactory::getInstance()->createBuffer(bufferDescription)));
	//}
	//tempI.clear();
	//I.clear();

	//boost::filesystem::path modelPath(p_Filename);
	//boost::filesystem::path parentDir(modelPath.parent_path());

	//// Load textures.
	//std::vector<ID3D11ShaderResourceView*> diffuse;
	//std::vector<ID3D11ShaderResourceView*> normal;
	//std::vector<ID3D11ShaderResourceView*> specular;
	//for(unsigned int i = 0; i < nrIndexBuffers; i++)
	//{
	//	const ModelLoader::Material& mat = tempM.at(i);
	//	boost::filesystem::path diff = (mat.m_DiffuseMap == "NONE") ? "assets/Default/Default_COLOR.jpg" : parentDir / mat.m_DiffuseMap;
	//	boost::filesystem::path norm = (mat.m_NormalMap == "NONE" || mat.m_NormalMap == "Default_NRM.jpg") ? "assets/Default/Default_COLOR.jpg" : parentDir / mat.m_NormalMap;
	//	boost::filesystem::path spec = (mat.m_SpecularMap == "NONE" || mat.m_SpecularMap == "Default_SPEC.jpg") ? "assets/Default/Default_SPEC.jpg" : parentDir / mat.m_SpecularMap;


	//	m_LoadModelTexture(mat.m_DiffuseMap.c_str(), diff.string().c_str(), m_LoadModelTextureUserdata);
	//	m_LoadModelTexture(mat.m_NormalMap.c_str(), norm.string().c_str(), m_LoadModelTextureUserdata);
	//	m_LoadModelTexture(mat.m_SpecularMap.c_str(), spec.string().c_str(), m_LoadModelTextureUserdata);

	//	diffuse.push_back(getTextureFromList( mat.m_DiffuseMap.c_str() ));
	//	normal.push_back(getTextureFromList( mat.m_NormalMap.c_str() ));
	//	specular.push_back(getTextureFromList( mat.m_SpecularMap.c_str() ));
	//}

	//model.vertexBuffer.swap(vertexBuffer);
	//model.indexBuffers.swap(indices);
	//model.diffuseTexture	= diffuse;
	//model.normalTexture		= normal;
	//model.specularTexture	= specular;
	//model.numOfMaterials	= nrIndexBuffers;
	//modelLoader.clear();

	return model;
}

ModelDefinition ModelFactory::createAnimatedModel(const char *p_Filename)
{
	ModelDefinition model;

	return model;
}

ModelFactory::ModelFactory(void)
{
}

ModelFactory::~ModelFactory(void)
{
}
