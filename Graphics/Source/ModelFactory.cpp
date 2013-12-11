#include "ModelFactory.h"

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

ModelClass *ModelFactory::createStaticModel(const char *p_Filename)
{
	ModelClass *model = new ModelClass();

	return model;
}

ModelClass *ModelFactory::createAnimatedModel(const char *p_Filename)
{
	ModelClass *model = new ModelClass();

	return model;
}

ModelFactory::ModelFactory(void)
{
}

ModelFactory::~ModelFactory(void)
{
}
