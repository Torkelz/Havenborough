#include "ModelConverter.h"
#include "ModelLoader.h"
#include <iostream>

void setFileInfo(ModelLoader* p_Loader, ModelConverter* p_Converter);

int main(int argc, char* argv[])
{
	ModelLoader loader;
	ModelConverter converter;
	if(argc == 3)
	{
		loader.loadFile(argv[1]);
		setFileInfo(&loader, &converter);
		converter.writeFile(argv[2]);
		loader.clear();
		converter.clear();

		return EXIT_SUCCESS;
	}
	else
	{
		std::cout << "Usage: " << argv[0] << " _in_file_ _out_file_" << std::endl;
	}

	return EXIT_FAILURE;
}

void setFileInfo(ModelLoader* p_Loader, ModelConverter* p_Converter)
{
	p_Converter->setMeshName(p_Loader->getMeshName());
	p_Converter->setVertices(&p_Loader->getVertices());
	p_Converter->setNormals(&p_Loader->getNormals());
	p_Converter->setTextureCoords(&p_Loader->getTextureCoords());
	p_Converter->setTangents(&p_Loader->getTangents());
	p_Converter->setIndices(&p_Loader->getIndices());
	p_Converter->setMaterial(&p_Loader->getMaterial());
	p_Converter->setWeightsList(&p_Loader->getWeightsList());
	p_Converter->setListOfJoints(&p_Loader->getListOfJoints());
	p_Converter->setAnimationStartValue(p_Loader->getAnimationStartValue());
	p_Converter->setAnimationEndValue(p_Loader->getAnimationEndValue());
	p_Converter->setNumberOfFrames(p_Loader->getNumberOfFrames());
}