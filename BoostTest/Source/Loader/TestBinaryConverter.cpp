#include <boost/test/unit_test.hpp>
#include "../../../BinaryConverter/Source/ModelConverter.h"
BOOST_AUTO_TEST_SUITE(TestModelConverter)

class testConv : public ModelConverter
{
public:
	void testIntToByte(int p_Int, std::ostream* p_Output)
	{
		intToByte(p_Int,p_Output);
	}

	void testStringToByte(std::string p_String, std::ostream* p_Output)
	{
		stringToByte(p_String, p_Output);
	}

	void testCreateHeader(std::ostream* p_Output)
	{
		createHeader(p_Output);
	}

	void testCreateMaterial(std::ostream* p_Output)
	{
		createMaterial(p_Output);
	}

	void testCreateMaterialBuffer(std::ostream* p_Output)
	{
		createMaterialBuffer(p_Output);
	}

	void testCreateVertexBuffer(std::ostream* p_Output)
	{
		createVertexBuffer(p_Output);
	}

	void testCreateVertexBufferAnimated(std::ostream* p_Output)
	{
		createVertexBufferAnimation(p_Output);
	}

	void testCreateJointBuffer(std::ostream* p_Output)
	{
		createJointBuffer(p_Output);
	}
};

BOOST_AUTO_TEST_CASE(TestIntToByte)
{
	struct byteInt
	{
		union
		{
			int i;
			char c[sizeof(int)];
		};
	};

	testConv conv;
	byteInt test;
	test.i = 0x12345678;
	std::ostringstream output;
	conv.testIntToByte(test.i, &output);

	std::string tempString = output.str();
	byteInt test2;
	std::copy(tempString.begin(), tempString.end(), test2.c);
	BOOST_CHECK_EQUAL_COLLECTIONS(test.c, test.c+sizeof(int), test2.c, test2.c+sizeof(int));
}

BOOST_AUTO_TEST_CASE(TestStringToByte)
{
	testConv conv;
	std::string test = "HELLO WORLD";
	std::ostringstream output;
	conv.testStringToByte(test, &output);

	static const char tempChara[] = {"\v\0\0\0HELLO WORLD"};
	std::string tempString = output.str();
	BOOST_CHECK_EQUAL_COLLECTIONS(tempString.begin(), tempString.end(), tempChara, tempChara + sizeof(tempChara) - 1);
}

BOOST_AUTO_TEST_CASE(TestCreateHeader)
{
	testConv conv;

	static const char binHeader[] =
		"\x11\0\0\0polySurfaceShape1"
		"\x01\0\0\0"
		"\x01\0\0\0"
		"\x01\0\0\0"
		"\x01\0\0\0"
		"\x01\0\0\0";

	std::vector<ModelLoader::Material> materials;
	materials.push_back(ModelLoader::Material());

	std::vector<std::vector<ModelLoader::IndexDesc>> indexDescs;
	indexDescs.push_back(std::vector<ModelLoader::IndexDesc>());
	indexDescs.back().push_back(ModelLoader::IndexDesc());

	std::vector<ModelLoader::Joint> joints;
	joints.push_back(ModelLoader::Joint());

	conv.setMeshName("polySurfaceShape1");
	conv.setMaterial(&materials);
	conv.setIndices(&indexDescs);
	conv.setListOfJoints(&joints);
	conv.setNumberOfFrames(1);

	std::ostringstream output;
	conv.testCreateHeader(&output);

	std::string resHeader = output.str();

	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin(), resHeader.end(), binHeader, binHeader + sizeof(binHeader) - 1);
}

BOOST_AUTO_TEST_CASE(TestCreateMaterial)
{
	testConv conv;

	static const char binMaterial[] =
		"\b\0\0\0MATERIAL"
		"\x4\0\0\0NONE"
		"\x4\0\0\0NONE"
		"\x4\0\0\0TEST";

	std::vector<ModelLoader::Material> materials;
	ModelLoader::Material temp;
	temp.m_MaterialID = "MATERIAL";
	temp.m_DiffuseMap = "NONE";
	temp.m_NormalMap = "NONE";
	temp.m_SpecularMap = "TEST";

	materials.push_back(temp);

	conv.setMaterial(&materials);

	std::ostringstream output;
	conv.testCreateMaterial(&output);

	std::string resMaterial = output.str();
	BOOST_CHECK_EQUAL_COLLECTIONS(resMaterial.begin(), resMaterial.end(), binMaterial, binMaterial + sizeof(binMaterial) - 1);
}

BOOST_AUTO_TEST_CASE(TestCreateMaterialBuffer)
{
	testConv conv;

	static const char binMaterialBuffer[] =
		"\b\0\0\0MATERIAL"
		"\0\0\0\0"
		"\x1\0\0\0";

	std::vector<std::vector<ModelLoader::IndexDesc>> indexList;
	std::vector<ModelLoader::IndexDesc> indexDesc;
	ModelLoader::IndexDesc temp;
	temp.m_MaterialID = "MATERIAL";

	indexDesc.push_back(temp);
	indexList.push_back(indexDesc);

	conv.setIndices(&indexList);

	std::ostringstream output;
	conv.testCreateMaterialBuffer(&output);

	std::string resMaterialBuffer = output.str();
	BOOST_CHECK_EQUAL_COLLECTIONS(resMaterialBuffer.begin(), resMaterialBuffer.end(), binMaterialBuffer, binMaterialBuffer + sizeof(binMaterialBuffer) - 1);
}

BOOST_AUTO_TEST_CASE(TestCreateVertexBuffer)
{
	testConv conv;
	struct byteFloat
	{
		union
		{
			float f;
			char c[sizeof(float)];
		};
	};
	
	std::vector<std::vector<ModelLoader::IndexDesc>> indexList;
	std::vector<ModelLoader::IndexDesc> indexDesc;
	ModelLoader::IndexDesc temp;
	temp.m_MaterialID = "MATERIAL";
	temp.m_Vertex = 0;
	temp.m_Normal = 0;
	temp.m_Tangent = 0;
	temp.m_TextureCoord = 0;

	std::vector<ModelLoader::Material> tempMaterial;
	tempMaterial.resize(1);

	std::vector<DirectX::XMFLOAT3> tempVertex;
	byteFloat tempFloat4Vec[4];tempFloat4Vec[0].f = 0.f; tempFloat4Vec[1].f = 1.f; tempFloat4Vec[2].f = 2.f;tempFloat4Vec[3].f = 1.f;
	tempVertex.push_back(DirectX::XMFLOAT3(tempFloat4Vec[0].f, tempFloat4Vec[1].f, tempFloat4Vec[2].f));

	std::vector<DirectX::XMFLOAT3> tempNormal;
	byteFloat tempFloat3Norm[3];tempFloat3Norm[0].f = 2.f; tempFloat3Norm[1].f = 1.f; tempFloat3Norm[2].f = 2.f;
	tempNormal.push_back(DirectX::XMFLOAT3(tempFloat3Norm[0].f, tempFloat3Norm[1].f, tempFloat3Norm[2].f));

	std::vector<DirectX::XMFLOAT3> tempTangent;
	byteFloat tempFloat3Tan[3];tempFloat3Tan[0].f = 8.f; tempFloat3Tan[1].f = 1.f; tempFloat3Tan[2].f = 2.f;
	tempTangent.push_back(DirectX::XMFLOAT3(tempFloat3Tan[0].f, tempFloat3Tan[1].f, tempFloat3Tan[2].f));

	std::vector<DirectX::XMFLOAT2> tempUV;
	byteFloat tempFloat2UV[2];tempFloat2UV[0].f = 0.5f; tempFloat2UV[1].f = 1.f;
	tempUV.push_back(DirectX::XMFLOAT2(tempFloat2UV[0].f, tempFloat2UV[1].f));

	
	indexDesc.push_back(temp);
	indexList.push_back(indexDesc);
	
	conv.setMaterial(&tempMaterial);
	conv.setIndices(&indexList);
	conv.setVertices(&tempVertex);
	conv.setNormals(&tempNormal);
	conv.setTangents(&tempTangent);
	conv.setTextureCoords(&tempUV);
	
	std::ostringstream output;
	conv.testCreateVertexBuffer(&output);
	
 	std::string resVertexBuffer = output.str();
	tempFloat4Vec[0].f *= -1;
	char c[16] = {
		tempFloat4Vec[0].c[0],tempFloat4Vec[0].c[1],tempFloat4Vec[0].c[2],tempFloat4Vec[0].c[3],
		tempFloat4Vec[1].c[0],tempFloat4Vec[1].c[1],tempFloat4Vec[1].c[2],tempFloat4Vec[1].c[3],
		tempFloat4Vec[2].c[0],tempFloat4Vec[2].c[1],tempFloat4Vec[2].c[2],tempFloat4Vec[2].c[3],
		tempFloat4Vec[3].c[0],tempFloat4Vec[3].c[1],tempFloat4Vec[3].c[2],tempFloat4Vec[3].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBuffer.begin(), resVertexBuffer.begin() + 16, c, c + sizeof(c));
	tempFloat3Norm[0].f *=-1;
	char d[12] = {
		tempFloat3Norm[0].c[0],tempFloat3Norm[0].c[1],tempFloat3Norm[0].c[2],tempFloat3Norm[0].c[3],
		tempFloat3Norm[1].c[0],tempFloat3Norm[1].c[1],tempFloat3Norm[1].c[2],tempFloat3Norm[1].c[3],
		tempFloat3Norm[2].c[0],tempFloat3Norm[2].c[1],tempFloat3Norm[2].c[2],tempFloat3Norm[2].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBuffer.begin() + 16, resVertexBuffer.begin() + 28, d, d + sizeof(d));
	char h[8] = {
		tempFloat2UV[0].c[0],tempFloat2UV[0].c[1],tempFloat2UV[0].c[2],tempFloat2UV[0].c[3],
		tempFloat2UV[1].c[0],tempFloat2UV[1].c[1],tempFloat2UV[1].c[2],tempFloat2UV[1].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBuffer.begin() + 28, resVertexBuffer.begin() + 36, h, h + sizeof(h));
	tempFloat3Tan[0].f *=-1;
	char l[12] = {
		tempFloat3Tan[0].c[0],tempFloat3Tan[0].c[1],tempFloat3Tan[0].c[2],tempFloat3Tan[0].c[3],
		tempFloat3Tan[1].c[0],tempFloat3Tan[1].c[1],tempFloat3Tan[1].c[2],tempFloat3Tan[1].c[3],
		tempFloat3Tan[2].c[0],tempFloat3Tan[2].c[1],tempFloat3Tan[2].c[2],tempFloat3Tan[2].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBuffer.begin() + 36, resVertexBuffer.begin() + 48, l, l + sizeof(l));
}

BOOST_AUTO_TEST_CASE(TestCreateVertexBufferAnimation)
{
	testConv conv;
	struct byteFloat
	{
		union
		{
			float f;
			char c[sizeof(float)];
		};
	};

	struct byteInt
	{
		union
		{
			int i;
			char c[sizeof(int)];
		};
	};
	
	std::vector<std::vector<ModelLoader::IndexDesc>> indexList;
	std::vector<ModelLoader::IndexDesc> indexDesc;
	ModelLoader::IndexDesc temp;
	temp.m_MaterialID = "MATERIAL";
	temp.m_Vertex = 0;
	temp.m_Normal = 0;
	temp.m_Tangent = 0;
	temp.m_TextureCoord = 0;

	std::vector<DirectX::XMFLOAT3> tempVertex;
	byteFloat tempFloat4Vec[4];tempFloat4Vec[0].f = 0.f; tempFloat4Vec[1].f = 1.f; tempFloat4Vec[2].f = 2.f;tempFloat4Vec[3].f = 1.f;
	tempVertex.push_back(DirectX::XMFLOAT3(tempFloat4Vec[0].f, tempFloat4Vec[1].f, tempFloat4Vec[2].f));

	std::vector<DirectX::XMFLOAT3> tempNormal;
	byteFloat tempFloat3Norm[3];tempFloat3Norm[0].f = 2.f; tempFloat3Norm[1].f = 1.f; tempFloat3Norm[2].f = 2.f;
	tempNormal.push_back(DirectX::XMFLOAT3(tempFloat3Norm[0].f, tempFloat3Norm[1].f, tempFloat3Norm[2].f));

	std::vector<DirectX::XMFLOAT3> tempTangent;
	byteFloat tempFloat3Tan[3];tempFloat3Tan[0].f = 8.f; tempFloat3Tan[1].f = 1.f; tempFloat3Tan[2].f = 2.f;
	tempTangent.push_back(DirectX::XMFLOAT3(tempFloat3Tan[0].f, tempFloat3Tan[1].f, tempFloat3Tan[2].f));

	std::vector<DirectX::XMFLOAT2> tempUV;
	byteFloat tempFloat2UV[2];tempFloat2UV[0].f = 0.5f; tempFloat2UV[1].f = 1.f;
	tempUV.push_back(DirectX::XMFLOAT2(tempFloat2UV[0].f, tempFloat2UV[1].f));

	std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMINT4>> tempWeight;
	byteFloat tempFloat4Weight[3];tempFloat4Weight[0].f = 0.f; tempFloat4Weight[1].f = 1.f; tempFloat4Weight[2].f = 2.f;
	byteInt tempFloat4Joint[4];tempFloat4Joint[0].i = 2; tempFloat4Joint[1].i = 1; tempFloat4Joint[2].i = 2; tempFloat4Joint[3].i = 0;
	tempWeight.push_back(std::make_pair(
		DirectX::XMFLOAT3(tempFloat4Weight[0].f, tempFloat4Weight[1].f, tempFloat4Weight[2].f), 
		DirectX::XMINT4(tempFloat4Joint[0].i, tempFloat4Joint[1].i, tempFloat4Joint[2].i, tempFloat4Joint[3].i)));

	indexDesc.push_back(temp);
	indexList.push_back(indexDesc);

	conv.setIndices(&indexList);
	conv.setVertices(&tempVertex);
	conv.setNormals(&tempNormal);
	conv.setTangents(&tempTangent);
	conv.setTextureCoords(&tempUV);
	conv.setWeightsList(&tempWeight);

	std::ostringstream output;
	conv.testCreateVertexBufferAnimated(&output);

 	std::string resVertexBufferAnimation = output.str();
	char c[16] = {
		tempFloat4Vec[0].c[0],tempFloat4Vec[0].c[1],tempFloat4Vec[0].c[2],tempFloat4Vec[0].c[3],
		tempFloat4Vec[1].c[0],tempFloat4Vec[1].c[1],tempFloat4Vec[1].c[2],tempFloat4Vec[1].c[3],
		tempFloat4Vec[2].c[0],tempFloat4Vec[2].c[1],tempFloat4Vec[2].c[2],tempFloat4Vec[2].c[3],
		tempFloat4Vec[3].c[0],tempFloat4Vec[3].c[1],tempFloat4Vec[3].c[2],tempFloat4Vec[3].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBufferAnimation.begin(), resVertexBufferAnimation.begin() + 16, c, c + sizeof(c));
	char d[12] = {
		tempFloat3Norm[0].c[0],tempFloat3Norm[0].c[1],tempFloat3Norm[0].c[2],tempFloat3Norm[0].c[3],
		tempFloat3Norm[1].c[0],tempFloat3Norm[1].c[1],tempFloat3Norm[1].c[2],tempFloat3Norm[1].c[3],
		tempFloat3Norm[2].c[0],tempFloat3Norm[2].c[1],tempFloat3Norm[2].c[2],tempFloat3Norm[2].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBufferAnimation.begin() + 16, resVertexBufferAnimation.begin() + 28, d, d + sizeof(d));
	char h[8] = {
		tempFloat2UV[0].c[0],tempFloat2UV[0].c[1],tempFloat2UV[0].c[2],tempFloat2UV[0].c[3],
		tempFloat2UV[1].c[0],tempFloat2UV[1].c[1],tempFloat2UV[1].c[2],tempFloat2UV[1].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBufferAnimation.begin() + 28, resVertexBufferAnimation.begin() + 36, h, h + sizeof(h));
	char l[12] = {
		tempFloat3Tan[0].c[0],tempFloat3Tan[0].c[1],tempFloat3Tan[0].c[2],tempFloat3Tan[0].c[3],
		tempFloat3Tan[1].c[0],tempFloat3Tan[1].c[1],tempFloat3Tan[1].c[2],tempFloat3Tan[1].c[3],
		tempFloat3Tan[2].c[0],tempFloat3Tan[2].c[1],tempFloat3Tan[2].c[2],tempFloat3Tan[2].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBufferAnimation.begin() + 36, resVertexBufferAnimation.begin() + 48, l, l + sizeof(l));
	char o[12] = {
		tempFloat4Weight[0].c[0],tempFloat4Weight[0].c[1],tempFloat4Weight[0].c[2],tempFloat4Weight[0].c[3],
		tempFloat4Weight[1].c[0],tempFloat4Weight[1].c[1],tempFloat4Weight[1].c[2],tempFloat4Weight[1].c[3],
		tempFloat4Weight[2].c[0],tempFloat4Weight[2].c[1],tempFloat4Weight[2].c[2],tempFloat4Weight[2].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBufferAnimation.begin() + 60, resVertexBufferAnimation.begin() + 72, o, o + sizeof(o));

	char w[16] = {
		tempFloat4Joint[0].c[0],tempFloat4Joint[0].c[1],tempFloat4Joint[0].c[2],tempFloat4Joint[0].c[3],
		tempFloat4Joint[1].c[0],tempFloat4Joint[1].c[1],tempFloat4Joint[1].c[2],tempFloat4Joint[1].c[3],
		tempFloat4Joint[2].c[0],tempFloat4Joint[2].c[1],tempFloat4Joint[2].c[2],tempFloat4Joint[2].c[3],
		tempFloat4Joint[3].c[0],tempFloat4Joint[3].c[1],tempFloat4Joint[3].c[2],tempFloat4Joint[3].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resVertexBufferAnimation.begin() + 72, resVertexBufferAnimation.begin() + 88, w, w + sizeof(w));
}

BOOST_AUTO_TEST_CASE(TestCreateJointBuffer)
{
	testConv conv;
	
	std::vector<ModelLoader::Joint> jointList;
	std::vector<ModelLoader::KeyFrame> keyList;
	ModelLoader::KeyFrame tempKey;
	tempKey.m_Trans = DirectX::XMFLOAT3(4,5,1);
	tempKey.m_Rot = DirectX::XMFLOAT4(4,4,4,4);
	tempKey.m_Scale = DirectX::XMFLOAT3(8,6,0);
	keyList.push_back(tempKey);
	ModelLoader::Joint temp;
	temp.m_JointName = "jointName";
	temp.m_ID = 1;
	temp.m_Parent = 0;
	temp.m_JointOffsetMatrix = DirectX::XMFLOAT4X4(0,0,0,1,1,1,1,0,0,1,0,1,1,0,0,0);
	temp.m_JointAnimation = keyList;
	jointList.push_back(temp);

	conv.setListOfJoints(&jointList);

	std::ostringstream output;
	conv.testCreateJointBuffer(&output);
	std::string resHeader = output.str();
	char chartemp[14] = {"\t\0\0\0jointName"};
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin(), resHeader.begin() + 13, chartemp, chartemp + sizeof(chartemp)-1);
}



BOOST_AUTO_TEST_SUITE_END()