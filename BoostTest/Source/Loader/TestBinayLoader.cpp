#include <boost/test/unit_test.hpp>
#include "../../../Graphics/Source/ModelBinaryLoader.h"
BOOST_AUTO_TEST_SUITE(TestBinaryModelLoader)

class testBinaryLoader : public ModelBinaryLoader
{
public:
	void testByteToInt(std::istream* p_Input, int& temp)
	{
		byteToInt(p_Input, temp);
	}

	void testByteToString(std::istream* p_Input, std::string& temp)
	{
		byteToString(p_Input, temp);
	}

	ModelBinaryLoader::Header testReadHeader(std::istream* p_Input)
	{
		return readHeader(p_Input);
	}

	std::vector<Material> testReadMaterial(int p_NumberOfMaterial, std::istream* p_Input)
	{
		return readMaterial(p_NumberOfMaterial, p_Input);
	}

	std::vector<MaterialBuffer> testReadMaterialBuffer(int p_NumberOfMaterialBuffers, std::istream* p_Input)
	{
		return readMaterialBuffer(p_NumberOfMaterialBuffers, p_Input);
	}

	std::vector<StaticVertex> testReadVertex(int p_NumberOfVertex, std::istream* p_Input)
	{
		return readVertexBuffer(p_NumberOfVertex, p_Input);
	}

	std::vector<AnimatedVertex> testReadVertexAnimation(int p_NumberOfVertex, std::istream* p_Input)
	{
		return readVertexBufferAnimation(p_NumberOfVertex, p_Input);
	}

	//std::vector<Joint> testReadJoint(int p_NumberOfJoint, int p_NumberOfFrames, std::istream* p_Input)
	//{
	//	return readJointList(p_NumberOfJoint, p_NumberOfFrames, p_Input);
	//}
};

BOOST_AUTO_TEST_CASE(TestByteToInt)
{
	struct byteInt
	{
		union
		{
			int i;
			char c[sizeof(int)];
		};
	};
	byteInt tempInt;
	tempInt.i = 15;
	std::istringstream tempString(std::string(tempInt.c, tempInt.c + sizeof(int)));
	testBinaryLoader loader;
	int result = 0;
	loader.testByteToInt(&tempString, result);

	BOOST_CHECK_EQUAL(result, tempInt.i);
}

BOOST_AUTO_TEST_CASE(TestByteToString)
{
	char tempS[] = "\x11\0\0\0polySurfaceShape1";
	std::istringstream tempString(std::string(tempS, tempS + sizeof(tempS)));
	testBinaryLoader loader;
	std::string result = "";
	loader.testByteToString(&tempString, result);

	BOOST_CHECK_EQUAL(result, "polySurfaceShape1");
}

BOOST_AUTO_TEST_CASE(TestReadHeader)
{
	ModelBinaryLoader::Header tempHeader;
	char binHeader[] =
		"\x11\0\0\0polySurfaceShape1"
		"\x05\0\0\0"
		"\x02\0\0\0"
		"\x01\0\0\0"
		"\x01\0\0\0"
		"\x03\0\0\0";
	std::istringstream tempString(std::string(binHeader, binHeader + sizeof(binHeader)));
	testBinaryLoader loader;
	tempHeader = loader.testReadHeader(&tempString);

	BOOST_CHECK_EQUAL(tempHeader.m_modelName, "polySurfaceShape1");
	BOOST_CHECK_EQUAL(tempHeader.m_numMaterial, 5);
	BOOST_CHECK_EQUAL(tempHeader.m_numVertex, 2);
	BOOST_CHECK_EQUAL(tempHeader.m_numMaterialBuffer, 1);
	BOOST_CHECK_EQUAL(tempHeader.m_numJoints, 1);
//	BOOST_CHECK_EQUAL(tempHeader.m_numFrames, 3);
}

BOOST_AUTO_TEST_CASE(TestReadMaterial)
{
	char binMaterial[] =
		"\b\0\0\0MATERIAL"
		"\x4\0\0\0NONE"
		"\x4\0\0\0NONE"
		"\x4\0\0\0TEST";
	std::istringstream tempString(std::string(binMaterial, binMaterial + sizeof(binMaterial)));
	testBinaryLoader loader;
	std::vector<Material> tempMaterial;

	tempMaterial = loader.testReadMaterial(1,&tempString);

	BOOST_CHECK_EQUAL(tempMaterial.at(0).m_MaterialID, "MATERIAL");
	BOOST_CHECK_EQUAL(tempMaterial.at(0).m_DiffuseMap, "NONE");
	BOOST_CHECK_EQUAL(tempMaterial.at(0).m_NormalMap, "NONE");
	BOOST_CHECK_EQUAL(tempMaterial.at(0).m_SpecularMap, "TEST");
}

BOOST_AUTO_TEST_CASE(TestReadMaterialBuffer)
{
	char binMaterialBuffer[] =
		"\b\0\0\0MATERIAL"
		"\0\0\0\0"
		"\x1\0\0\0";
	std::istringstream tempString(std::string(binMaterialBuffer, binMaterialBuffer + sizeof(binMaterialBuffer)));
	testBinaryLoader loader;
	std::vector<MaterialBuffer> tempMaterialBuffer;

	tempMaterialBuffer = loader.testReadMaterialBuffer(1,&tempString);

	BOOST_CHECK_EQUAL(tempMaterialBuffer.at(0).material, "MATERIAL");
	BOOST_CHECK_EQUAL(tempMaterialBuffer.at(0).start, 0);
	BOOST_CHECK_EQUAL(tempMaterialBuffer.at(0).length, 1);
}

BOOST_AUTO_TEST_CASE(TestReadVertexBuffer)
{
	char binVertex[] =
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0pA"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?";

	std::istringstream tempString(std::string(binVertex, binVertex + sizeof(binVertex)));
	testBinaryLoader loader;
	std::vector<StaticVertex> tempVertex;

	tempVertex = loader.testReadVertex(1,&tempString);

	BOOST_CHECK_EQUAL(tempVertex.at(0).m_Position.x, 0.5f);
	BOOST_CHECK_EQUAL(tempVertex.at(0).m_Normal.x, 15.0f);
}

BOOST_AUTO_TEST_CASE(TestReadVertexBufferAnimation)
{
	char binVertexAnimation[] =
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0pA"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0p?"
		"\0\0p?"
		"\0\0\0?"
		"\0\0\0?"
		"\0\0p?"
		"\0\0p?"
		"\0\0\0?";

	std::istringstream tempString(std::string(binVertexAnimation, binVertexAnimation + sizeof(binVertexAnimation)));
	testBinaryLoader loader;
	std::vector<AnimatedVertex> tempVertex;

	tempVertex = loader.testReadVertexAnimation(1,&tempString);

	BOOST_CHECK_EQUAL(tempVertex.at(0).m_Position.x, 0.5f);
	BOOST_CHECK_EQUAL(tempVertex.at(0).m_Normal.x, 15.0f);
	BOOST_CHECK_EQUAL(tempVertex.at(0).m_Weights.x, 0.9375f);
}

//BOOST_AUTO_TEST_CASE(TestReadJoint)
//{
//	char tempS[] = 
//		"\x11\0\0\0polySurfaceShape1"
//		"\x01\0\0\0"
//		"\x00\0\0\0"
//		"\0\0\0?\0\0\0?\0\0\0?\0\0\0?"
//		"\0\0\0?\0\0\0?\0\0\0?\0\0\0?"
//		"\0\0\0?\0\0\0?\0\0\0?\0\0\0?"
//		"\0\0\0?\0\0\0?\0\0\0?\0\0\0?"
//		"\0\0pA\0\0pA\0\0pA"
//		"\0\0pA\0\0pA\0\0pA\0\0pA"
//		"\0\0pA\0\0pA\0\0pA";
//
//	std::istringstream tempString(std::string(tempS, tempS + sizeof(tempS)));
//	testBinaryLoader loader;
//	std::vector<Joint> tempJoint;
//
//	tempJoint= loader.testReadJoint(1,1 ,&tempString);
//
//	BOOST_CHECK_EQUAL(tempJoint.at(0).m_JointName, "polySurfaceShape1");
//	BOOST_CHECK_EQUAL(tempJoint.at(0).m_ID, 1);
//	BOOST_CHECK_EQUAL(tempJoint.at(0).m_Parent, 0);
//	BOOST_CHECK_EQUAL(tempJoint.at(0).m_JointOffsetMatrix._11, 0.5f);
//	BOOST_CHECK_EQUAL(tempJoint.at(0).m_JointOffsetMatrix._44, 0.5f);
//	BOOST_CHECK_EQUAL(tempJoint.at(0).m_JointAnimation.at(0).m_Trans.x, 15.0f);
//	BOOST_CHECK_EQUAL(tempJoint.at(0).m_JointAnimation.at(0).m_Scale.x, 15.0f);
//}

BOOST_AUTO_TEST_SUITE_END()