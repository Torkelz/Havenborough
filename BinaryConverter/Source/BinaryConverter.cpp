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
	bool result;
	if(argc == 2)
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
			std::vector<char> outputBuffer(strlen(argv[1])+2);
			strcpy(outputBuffer.data(), argv[1]);
			int length = outputBuffer.size();
			strcpy(outputBuffer.data()+length-5, ".btx");
			result = loader.loadFile(argv[1]);
			if(!result){std::cout<<"Error loading file";return EXIT_FAILURE;}
			setFileInfo(&loader, &converter);
			std::cout << outputBuffer.data();
			result = converter.writeFile(outputBuffer.data());
			if(!result){std::cout<<"Error writing file";return EXIT_FAILURE;}
			loader.clear();
			converter.clear();
			return EXIT_SUCCESS;
		}
		if(strcmp(type, "txl") == 0)
		{
			std::vector<char> outputBuffer(strlen(argv[1])+2);
			strcpy(outputBuffer.data(), argv[1]);
			int length = outputBuffer.size();
			strcpy(outputBuffer.data()+length-6, ".btxl");
			result = levelLoader.loadLevel(argv[1]);
			if(!result){std::cout<<"Error loading file";return EXIT_FAILURE;}
			setLevelInfo(&levelLoader, &levelConverter);
			result = levelConverter.writeFile(outputBuffer.data());
			if(!result){std::cout<<"Error writing file";return EXIT_FAILURE;}
			std::cout << outputBuffer.data();
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
		std::cout << "Usage: " << argv[0] << " _in_file_ " << std::endl;
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
	p_Converter->setLevelDirectionalLightList(&p_Loader->getLevelDirectionalLightList());
	p_Converter->setLevelPointLightList(&p_Loader->getLevelPointLightList());
	p_Converter->setLevelSpotLightList(&p_Loader->getLevelSpotLightList());
	p_Converter->setLevelCheckPointList(&p_Loader->getLevelCheckPointList());
	p_Converter->setLevelCheckPointStart(p_Loader->getLevelCheckPointStart());
	p_Converter->setLevelCheckPointEnd(p_Loader->getLevelCheckPointEnd());
}