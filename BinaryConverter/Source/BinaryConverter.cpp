#pragma warning(disable : 4996)

#include "ModelConverter.h"
#include "ModelLoader.h"
#include "LevelLoader.h"
#include "LevelConverter.h"
#include <iostream>

void setFileInfo(ModelLoader* p_Loader, ModelConverter* p_Converter);
void setLevelInfo(LevelLoader* p_Loader, LevelConverter* p_Converter);

int main(int argc, char* argv[])
{
	ModelLoader loader;
	ModelConverter converter;
	LevelLoader levelLoader;
	LevelConverter levelConverter;
	if(argc == 3)
	{
		std::vector<char> buffer(strlen(argv[1])+1);
		strcpy(buffer.data(), argv[1]);
		char *tmp, *type = nullptr;
		tmp = strtok(buffer.data(), ".");
		while(tmp != nullptr)
		{
			type = tmp;
			tmp = strtok(NULL,".");
		}
		if(strcmp(type, "tx") == 0)
		{
			loader.loadFile(argv[1]);
			setFileInfo(&loader, &converter);
			converter.writeFile(argv[2]);
			loader.clear();
			converter.clear();
			return EXIT_SUCCESS;
		}
		if(strcmp(type, "txl") == 0)
		{
			levelLoader.loadLevel(argv[1]);
			setLevelInfo(&levelLoader, &levelConverter);
			levelConverter.writeFile(argv[2]);
			levelLoader.clear();
			levelConverter.clear();
			return EXIT_SUCCESS;
		}

		std::cout << argv[0] << " does not support files of type: " << type << std::endl
			<< "Supported types are: " << std::endl << "      .tx" << std::endl << "      .txl";


		return EXIT_FAILURE;
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

void setLevelInfo(LevelLoader* p_Loader, LevelConverter* p_Converter)
{
	p_Converter->setLevelHead(p_Loader->getLevelHeader());
	p_Converter->setLevelModelList(&p_Loader->getLevelModelList());
}