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

	void testFloatToByte(float p_Float, std::ostream* p_Output)
	{
		floatToByte(p_Float, p_Output);
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

BOOST_AUTO_TEST_CASE(TestFloatToByte)
{
	struct byteFloat
	{
		union
		{
			float f;
			char c[sizeof(int)];
		};
	};

	testConv conv;
	byteFloat test;
	test.f = 5.45f;
	std::ostringstream output;
	conv.testFloatToByte(test.f, &output);

	std::string tempString = output.str();
	byteFloat test2;
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

	std::string resHeader = output.str();
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin(), resHeader.end(), binMaterial, binMaterial + sizeof(binMaterial) - 1);
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

	std::string resHeader = output.str();
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin(), resHeader.end(), binMaterialBuffer, binMaterialBuffer + sizeof(binMaterialBuffer) - 1);
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

	std::vector<DirectX::XMFLOAT3> tempVertex;
	byteFloat tempFloat3Vec[4];tempFloat3Vec[0].f = 0.f; tempFloat3Vec[1].f = 1.f; tempFloat3Vec[2].f = 2.f;tempFloat3Vec[3].f = 1.f;
	tempVertex.push_back(DirectX::XMFLOAT3(tempFloat3Vec[0].f, tempFloat3Vec[1].f, tempFloat3Vec[2].f));

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

	conv.setIndices(&indexList);
	conv.setVertices(&tempVertex);
	conv.setNormals(&tempNormal);
	conv.setTangents(&tempTangent);
	conv.setTextureCoords(&tempUV);

	std::ostringstream output;
	conv.testCreateVertexBuffer(&output);

	std::string resHeader = output.str();
	char c[16] = {
		tempFloat3Vec[0].c[0],tempFloat3Vec[0].c[1],tempFloat3Vec[0].c[2],tempFloat3Vec[0].c[3],
		tempFloat3Vec[1].c[0],tempFloat3Vec[1].c[1],tempFloat3Vec[1].c[2],tempFloat3Vec[1].c[3],
		tempFloat3Vec[2].c[0],tempFloat3Vec[2].c[1],tempFloat3Vec[2].c[2],tempFloat3Vec[2].c[3],
		tempFloat3Vec[3].c[0],tempFloat3Vec[3].c[1],tempFloat3Vec[3].c[2],tempFloat3Vec[3].c[3]};
	BOOST_CHECK_EQUAL_COLLECTIONS(resHeader.begin(), resHeader.begin() + 16, c, c + sizeof(c));
}

BOOST_AUTO_TEST_SUITE_END()