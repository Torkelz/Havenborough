#include <boost/test/unit_test.hpp>
#include "../../../BinaryConverter/Source/ModelLoader.h"
#include "../../../BinaryConverter/Source/ModelConverter.h"

BOOST_AUTO_TEST_SUITE(TestModelTXLoader)

class testLoader : public ModelLoader
{
public:
	bool testLoadModel(std::string p_FilePath)
	{
		return loadFile(p_FilePath, "..\\Source\\Loader");
	}

	void testHeader(std::istream& p_Input)
	{
		readHeader(p_Input);
	}
	void testMaterial(std::istream& p_Input)
	{
		readMaterials(p_Input);
	}
	void testVertex(std::istream& p_Input)
	{
		readVertex(p_Input); 
	}
	void testNormal(std::istream& p_Input)
	{
		readNormals(p_Input);
	}
	void testUV(std::istream& p_Input)
	{
		readUV(p_Input);
	}
	void testTangent(std::istream& p_Input)
	{
		readTangents(p_Input);
	}
	void testFaces(std::istream& p_Input)
	{
		readFaces(p_Input);
	}
	void testWeights(std::istream& p_Input)
	{
		readWeights(p_Input);
	}
	void testHierarchy(std::istream& p_Input)
	{
		readHierarchy(p_Input);
	}
	void testJointOffset(std::istream& p_Input)
	{
		readJointOffset(p_Input);
	}
	void testAnimation(std::istream& p_Input)
	{
		readAnimation(p_Input);
	}
	void testStart(std::istream& p_Input)
	{
		startReading(p_Input);
	}
};

BOOST_AUTO_TEST_CASE(TestLoadModel)
{
	testLoader loader;
	bool result;
	result = loader.testLoadModel("");
	BOOST_CHECK_EQUAL(result, false);
	result = loader.testLoadModel("..\\Source\\Loader\\testModelLoader.tx");
	BOOST_CHECK_EQUAL(result, true);
}

BOOST_AUTO_TEST_CASE(TestHeaderAndMaterialFile)
{
	std::string tempString = "#Tansparent 1 \n#Collidable 0 \n#Materials 1 \n#MESH ABC \n#Vertices 8 \n#Triangles 12\n";
	std::istringstream tempStream(tempString); 
	testLoader loader;
	
	loader.testHeader(tempStream);
	
	BOOST_CHECK_EQUAL(loader.getMeshName(), "ABC");
	
	tempString = "Material: ABC1 \nDiffuseMap: COLOR1.jpg \nNormalMap: NRM.jpg \nSpecularMap: SPEC.jpg\n";
	std::istringstream tempStream2(tempString);

	loader.testMaterial(tempStream2);

	BOOST_CHECK_EQUAL(loader.getMaterial().at(0).m_MaterialID , "ABC1");
	BOOST_CHECK_EQUAL(loader.getMaterial().at(0).m_DiffuseMap , "COLOR1.jpg");

	loader.clear();
}

BOOST_AUTO_TEST_CASE(TestBuildingFaces)
{
	std::string tempString = "#Tansparent 1 \n#Collidable 0 \n#Materials 0 \n#MESH ABC \n#Vertices 3 \n#Triangles 1\n";
	std::istringstream tempStream(tempString); 
	testLoader loader;
	
	loader.testHeader(tempStream);

	tempStream.clear();
	tempString = "v -0.5 -0.5 0.5 \nv -0.5 -0.5 0.5 \nv -0.57 0.5 0.5\n";
	tempStream = std::istringstream(tempString); 
	
	loader.testVertex(tempStream);
	
	BOOST_CHECK_EQUAL(loader.getVertices().at(0).x, -0.5f);
	BOOST_CHECK_EQUAL(loader.getVertices().at(0).y, -0.5f);
	BOOST_CHECK_EQUAL(loader.getVertices().at(0).z, 0.5f);
	
	tempStream.clear();
	tempString = "n 0 0 1 \nn 0 0 1 \nn 0 0 1\n";
	tempStream = std::istringstream(tempString);

	loader.testNormal(tempStream);

	BOOST_CHECK_EQUAL(loader.getNormals().at(0).x , 0);
	BOOST_CHECK_EQUAL(loader.getNormals().at(0).y , 0);
	BOOST_CHECK_EQUAL(loader.getNormals().at(0).z , 1);
	
	tempStream.clear();
	tempString = "uv 0.375 0 \nuv 0.625 0 \nuv 0.375 0.25\n";
	tempStream = std::istringstream(tempString);

	loader.testUV(tempStream);

	BOOST_CHECK_EQUAL(loader.getTextureCoords().at(0).x , 0.375f);
	BOOST_CHECK_EQUAL(loader.getTextureCoords().at(0).y , 1.f);

	tempStream.clear();
	tempString = "t 1 0 0 \nt 57 0 0 \nt 1 1 0\n";
	tempStream = std::istringstream(tempString);

	loader.testTangent(tempStream);

	BOOST_CHECK_EQUAL(loader.getTangents().at(0).x , 1);
	BOOST_CHECK_EQUAL(loader.getTangents().at(0).y , 0);
	BOOST_CHECK_EQUAL(loader.getTangents().at(0).z , 0);

	tempStream.clear();
	tempString = "-blinn1 \nface: 3 \n2 / 0 / 1 / 2 | 1 / 1 / 1 / 1 | 0 / 2 / 2 / 2 | \n\n";
	tempStream = std::istringstream(tempString);

	loader.testFaces(tempStream);

	ModelLoader::IndexDesc tempIndex = loader.getIndices().at(0).at(0);
	BOOST_CHECK_EQUAL(loader.getVertices().at(tempIndex.m_Vertex).x , -0.57f);
	BOOST_CHECK_EQUAL(loader.getNormals().at(tempIndex.m_Normal).x , 0.f);
	BOOST_CHECK_EQUAL(loader.getTangents().at(tempIndex.m_Tangent).x , 1.f);
	BOOST_CHECK_EQUAL(loader.getTextureCoords().at(tempIndex.m_TextureCoord).x , 0.375f);
	loader.clear();
}

BOOST_AUTO_TEST_CASE(TestAnimation)
{
	std::string tempString = "w 0.993041 0 0 0 \nj 1 0 0 0 \nw 0.960454 0.0378095 0 0 \nj 1 2 0 0\n";
	std::istringstream tempStream(tempString); 
	testLoader loader;
	
	loader.testWeights(tempStream);

	float weightSum =
		loader.getWeightsList().at(0).first.x + 
		loader.getWeightsList().at(0).first.y;
	
	BOOST_CHECK_EQUAL(weightSum, 1.f);
	BOOST_CHECK_EQUAL(loader.getWeightsList().at(0).second.x, 1);
	BOOST_CHECK_EQUAL(loader.getWeightsList().at(1).second.y, 2);

	tempStream.clear();
	tempString = "joint1 1 | 0 \njoint2 2 | 1 \njoint3 3 | 2\n";
	tempStream = std::istringstream(tempString);

	loader.testHierarchy(tempStream);

	tempStream.clear();
	tempString = "joint1 0 1 0 0 -1 0 0 0 0 0 1 0 0 -0.492047 0 1\n";
	tempStream = std::istringstream(tempString);

	loader.testJointOffset(tempStream);

	tempStream.clear();
	tempString = "Start 0.0416667 \nEnd 1 \nNum_frames 25 \n \n1"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n"
 "Trans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1\n";
	tempStream = std::istringstream(tempString);
	DirectX::XMFLOAT3 Trans( 0.f, -0.492047f, 0.f);
	DirectX::XMFLOAT4 Rot(0.f, 0.f, 0.f, 1.f);
	DirectX::XMFLOAT3 Scale(1.f, 1.f, 1.f);
	loader.testAnimation(tempStream);

	BOOST_CHECK_EQUAL(loader.getAnimationStartValue(), 0.0416667f);
	BOOST_CHECK_EQUAL(loader.getAnimationEndValue(), 1);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_ID , 1);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointName , "joint1");
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_Parent , 0);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointOffsetMatrix._33 , 1);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointAnimation.at(0).m_Trans.x , Trans.x);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointAnimation.at(0).m_Rot.x , Rot.x);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointAnimation.at(0).m_Scale.x , Scale.x);
	BOOST_CHECK_EQUAL(loader.getNumberOfFrames(), 25);
	loader.clear();
}

BOOST_AUTO_TEST_CASE(TestMainLoop)
{
	std::string tempString = "*Header\n #Tansparent 1 \n#Collidable 0 \n#Materials 2 \n#MESH polySurfaceShape1 \n#Vertices 3 \n#Triangles 1 \n"
	"*Materials \nMaterial: lambert1 \nDiffuseMap: NONE \nNormalMap: NONE \nSpecularMap: NONE \n \n"
	"Material: lambert1 \nDiffuseMap: NONE \nNormalMap: NONE \nSpecularMap: NONE \n \n"
	"*Vertices \nv -0.499455 -0.499619 0.499924 \nv -0.499668 0.501486 0.500748 \nv -0.496855 0.498541 -0.498709 \n\n"
	"*Normals \nn -0.00028988 -0.000753547 1 \nn -0.0020181 -0.00166746 0.999997 \nn -0.000190268 -0.000774098 1 \n\n"
	"*UV COORDS \nuv 0.375 0 \nuv 0.375 0.25 \nuv 0.375 0.5 \n\n"
	"*Tangets \nt 1 -0.000514396 0.000289492 \nt 0.999997 -0.001133 0.00201621 \nt 0.999999 -0.00112993 0.000189394 \n\n"
	"*FACES \n-lambert1 \nface: 3 \n0 / 0 / 0 / 0 | 2 / 1 / 1 / 2 | 0 / 2 / 2 / 0 | \n\n-lambert1 \nface: 3 \n0 / 0 / 0 / 0 | 2 / 1 / 1 / 2 | 0 / 2 / 2 / 0 | \n\n"
	"*WEIGHTS \nw 0.993041 0 0 0 \nj 1 0 0 0 \nw 0.2 0.2 0.2 0.2 \nj 1 1 1 1 \n\n"
	"*HIERARCHY \njoint1 1 | 0 \n\n"
	"*JOINT_OFFSET \njoint1 0 1 0 0 -1 0 0 0 0 0 1 0 0 -0.492047 0 1 \n\n"
	"*ANIMATION \nStart 0.0416667 \nEnd 1 \nNum_frames 1 \n\n\1 \nTrans: 0 -0.492047 0 Rot: 0 0 0 1 Scale: 1 1 1 \n\n";
	std::istringstream tempStream(tempString); 
	testLoader loader;
	
	loader.testStart(tempStream);

	ModelLoader::IndexDesc tempIndex = loader.getIndices().at(0).at(0);
	BOOST_CHECK_EQUAL(loader.getCollidable(), false);
	BOOST_CHECK_EQUAL(loader.getTransparent(), true);
	BOOST_CHECK_EQUAL(loader.getMaterial().at(0).m_MaterialID , "lambert1");
	BOOST_CHECK_EQUAL(loader.getMaterial().at(0).m_DiffuseMap , "NONE");
	BOOST_CHECK_EQUAL(loader.getVertices().at(tempIndex.m_Vertex).x , -0.499455f);
	BOOST_CHECK_EQUAL(loader.getNormals().at(tempIndex.m_Normal).x , -0.00028988f);
	BOOST_CHECK_EQUAL(loader.getTangents().at(tempIndex.m_Tangent).x , 1.f);
	BOOST_CHECK_EQUAL(loader.getTextureCoords().at(tempIndex.m_TextureCoord).x , 0.375f);
	BOOST_CHECK_EQUAL(loader.getWeightsList().at(0).first.x, 1.f);
	BOOST_CHECK_EQUAL(loader.getWeightsList().at(0).second.x, 1);
	BOOST_CHECK_EQUAL(loader.getAnimationStartValue(), 0.0416667f);
	BOOST_CHECK_EQUAL(loader.getAnimationEndValue(), 1);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_ID , 1);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointName , "joint1");
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_Parent , 0);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointOffsetMatrix._33 , 1);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointAnimation.at(0).m_Trans.x , 0);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointAnimation.at(0).m_Rot.x , 0);
	BOOST_CHECK_EQUAL(loader.getListOfJoints().at(0).m_JointAnimation.at(0).m_Scale.x , 1);
	BOOST_CHECK_EQUAL(loader.getNumberOfFrames(), 1);
}



BOOST_AUTO_TEST_SUITE_END()