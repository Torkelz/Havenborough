#include <boost/test/unit_test.hpp>
#include "../../Graphics/Source/ParticleInstance.h"
#include "../../Graphics/Source/GraphicsExceptions.h"

BOOST_AUTO_TEST_SUITE(ParticleInstanceTest)

BOOST_AUTO_TEST_CASE(initInvalid)
{
	ParticleInstance par;

	std::shared_ptr<Buffer> cBuffer (nullptr);
	std::shared_ptr<Buffer> particleBuffer (nullptr);
	std::shared_ptr<ParticleEffectDefinition> particleDef (nullptr);

	BOOST_CHECK_THROW(par.init(cBuffer, particleBuffer, particleDef), ParticleException);
}

BOOST_AUTO_TEST_CASE(initValid)
{
	ParticleInstance par;

	std::shared_ptr<Buffer> cBuffer (new Buffer());
	std::shared_ptr<Buffer> particleBuffer (new Buffer());
	std::shared_ptr<ParticleEffectDefinition> particleDef (new ParticleEffectDefinition());

	BOOST_CHECK_NO_THROW(par.init(cBuffer, particleBuffer, particleDef));
}

BOOST_AUTO_TEST_CASE(EmitNewDestroy)
{
	ParticleInstance par;

	std::shared_ptr<Buffer> cBuffer (new Buffer());
	std::shared_ptr<Buffer> particleBuffer (new Buffer());
	ParticleEffectDefinition def;
	def.particlesPerSec = 10000;
	def.maxParticles = 10;
	def.velocityDeviation = DirectX::XMFLOAT3(0,1,0);
	def.particlePositionDeviation = 0.5f;
	def.maxLifeDeviation = 1.f;
	def.maxLife = 10;
	def.particleColorDeviation = DirectX::XMFLOAT4(1,0,0,1);
	def.particleColorBase = DirectX::XMFLOAT4(1,0,0,1);
	def.size = DirectX::XMFLOAT2(5,5);
	def.velocitybase = DirectX::XMFLOAT3(1,1,0);
	def.sysMaxLife = 0.1f;


	std::shared_ptr<ParticleEffectDefinition> particleDef (new ParticleEffectDefinition(def));

	BOOST_CHECK_NO_THROW(par.init(cBuffer, particleBuffer, particleDef));
	for(int i = 0; i < 100; i++)
	{
		par.update(0.03f);
	}
	BOOST_CHECK(par.getSeppuku());
}

BOOST_AUTO_TEST_SUITE_END()