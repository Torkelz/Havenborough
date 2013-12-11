#pragma once
#include "Util.h"
#include "WrapperFactory.h"
#include "ModelClass.h"

class ModelFactory
{
private:
	static ModelFactory *m_Instance;

public:
	static ModelFactory *getInstance(void);

	virtual void shutdown(void);

	ModelClass *createStaticModel(const char *p_Filename);

	ModelClass *createAnimatedModel(const char *p_Filename);

private:
	ModelFactory(void);
	virtual ~ModelFactory(void);
};

