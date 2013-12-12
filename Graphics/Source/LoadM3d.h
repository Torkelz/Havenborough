//#ifndef LOADM3D_H
//#define LOADM3D_H
//
////#include "MeshGeometry.h"
////#include "LightHelper.h"
////#include "SkinnedData.h"
////#include "Vertex.h"
//#include <vector>
//#include <DirectXMath.h>
//#include <map>
//#include <fstream>
//
//#include "Vertex.h"
//#include "ModelDefinition.h"
//#include "Joint.h"
//
//struct Material
//{
//	std::string m_MaterialID;
//	std::string m_DiffuseMap;
//	std::string m_NormalMap;
//	std::string m_SpecularMap;
//};
//
//struct M3dMaterial
//{
//	Material Mat;
//	bool AlphaClip;
//	std::string EffectTypeName;
//	std::wstring DiffuseMapName;
//	std::wstring NormalMapName;
//};
//
//class M3DLoader
//{
//public:
//	bool LoadM3d(const std::string& filename, 
//		std::vector<Vertex>& vertices,
//		std::vector<unsigned short>& indices,
//		std::vector<Model>& subsets,
//		std::vector<M3dMaterial>& mats);
//	bool LoadM3d(const std::string& filename, 
//		std::vector<AnimatedVertex>& vertices,
//		std::vector<unsigned short>& indices,
//		std::vector<Model>& subsets,
//		std::vector<M3dMaterial>& mats,
//		Joint& skinInfo);
//
//private:
//	void ReadMaterials(std::ifstream& fin, unsigned int numMaterials, std::vector<M3dMaterial>& mats);
//	void ReadSubsetTable(std::ifstream& fin, unsigned int numSubsets, std::vector<Model>& subsets);
//	void ReadVertices(std::ifstream& fin, unsigned int numVertices, std::vector<Vertex>& vertices);
//	void ReadSkinnedVertices(std::ifstream& fin, unsigned int numVertices, std::vector<AnimatedVertex>& vertices);
//	void ReadTriangles(std::ifstream& fin, unsigned int numTriangles, std::vector<unsigned short>& indices);
//	void ReadBoneOffsets(std::ifstream& fin, unsigned int numBones, std::vector<DirectX::XMFLOAT4X4>& boneOffsets);
//	void ReadBoneHierarchy(std::ifstream& fin, unsigned int numBones, std::vector<int>& boneIndexToParentIndex);
//	void ReadAnimationClips(std::ifstream& fin, unsigned int numBones, unsigned int numAnimationClips, std::map<std::string, KeyFrame>& animations);
//	void ReadBoneKeyframes(std::ifstream& fin, unsigned int numBones, Joint& boneAnimation);
//};
//
//#endif // LOADM3D_H