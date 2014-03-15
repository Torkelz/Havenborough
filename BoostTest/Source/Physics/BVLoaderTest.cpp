#include <boost\test\unit_test.hpp>
#include "..\..\Physics\Source\BVLoader.h"
#include "..\..\Physics\include\BoundingVolume.h"

class DummyBoundingVolume : public BoundingVolume
{
public:
	DummyBoundingVolume() :
		BoundingVolume(nullptr)
	{
		m_Type = Type::NONE;
		m_BodyHandle = 0;
	}
	~DummyBoundingVolume(){}

	void updatePosition(DirectX::XMFLOAT4X4 const &p_Translation) override
	{
		
	}
	
	void setPosition(DirectX::XMVECTOR const &p_Position) override
	{
		DirectX::XMStoreFloat4(&m_Position, p_Position);
	}

	void scale(DirectX::XMVECTOR const &p_Scale) override
	{
		
	}

	void setRotation(DirectX::XMMATRIX const &p_Rotation) override
	{
		
	}
};

BOOST_AUTO_TEST_SUITE(BVLoaderTesting)

BOOST_AUTO_TEST_CASE(testLoad)
{
	std::string file =  "*Header\n" 
						"#Materials 0\n" 
						"#MESH CB_Test\n"
						"#Vertices 3\n"
						"#Triangles 1\n"
						"\n"
						"*Vertices\n"
						"v 1 1 1\n"
						"v 2 2 1\n"
						"v 2 1 1\n"
						"\n"
						"*FACES\n"
						"-BoundingVolume\n"
						"face: 3\n"
						"0 | 1 | 2 |";

	std::istringstream stream(file);
	
	BVLoader bv;
	BVLoader::Header header;
	bv.readHeader(&stream);
	header = bv.getLevelHeader();

	BOOST_CHECK_EQUAL(header.m_modelName, "CB_Test");
	BOOST_CHECK_EQUAL(header.m_numMaterial, 0);
	BOOST_CHECK_EQUAL(header.m_numVertex, 3);
	BOOST_CHECK_EQUAL(header.m_numFaces, 1);

	bv.readBoundingVolume(&stream);
	std::vector<BVLoader::BoundingVolume> vec = bv.getBoundingVolumes();
	BOOST_CHECK_EQUAL(vec[0].m_Postition.x, -1.f); // BVloader .x *-1.f :'(
	BOOST_CHECK_EQUAL(vec[0].m_Postition.y, 1.f);
	BOOST_CHECK_EQUAL(vec[0].m_Postition.z, 1.f);
	
	BOOST_CHECK_EQUAL(vec[1].m_Postition.x, -2.f); // BVloader .x *-1.f :'(
	BOOST_CHECK_EQUAL(vec[1].m_Postition.y, 2.f);
	BOOST_CHECK_EQUAL(vec[1].m_Postition.z, 1.f);
	
	BOOST_CHECK_EQUAL(vec[2].m_Postition.x, -2.f); // BVloader .x *-1.f :'(
	BOOST_CHECK_EQUAL(vec[2].m_Postition.y, 1.f);
	BOOST_CHECK_EQUAL(vec[2].m_Postition.z, 1.f);

	bv.clear();

	vec = bv.getBoundingVolumes();

	BOOST_CHECK_EQUAL(vec.size(), 0);

	header = bv.getLevelHeader();
	BOOST_CHECK_EQUAL(header.m_modelName, "");
	BOOST_CHECK_EQUAL(header.m_numMaterial, 0);
	BOOST_CHECK_EQUAL(header.m_numVertex, 0);
	BOOST_CHECK_EQUAL(header.m_numFaces, 0);
}

BOOST_AUTO_TEST_SUITE_END()