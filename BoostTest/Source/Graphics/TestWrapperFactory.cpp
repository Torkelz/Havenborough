#include <boost/test/unit_test.hpp>
#include "../../Graphics/Source/WrapperFactory.h"
#include "../../Graphics/Source/GraphicsExceptions.h"
#include "Utilities/MemoryUtil.h"

using std::string;
using std::vector;

BOOST_AUTO_TEST_SUITE(TestWrapperFactory)
	
	class DummyShader : public Shader
	{
	public:
		HRESULT createShader(ID3DBlob *p_ShaderData) override
		{
			if(p_ShaderData == nullptr)
			{
				throw ShaderException("Creation of shader failed, ID3DBlob was nullptr.",__LINE__,__FILE__);
			}
			else
			{
				return S_OK;
			}
		}
		void createInputLayoutFromShaderSignature(ID3DBlob *p_ShaderData) override
		{
			if(p_ShaderData == nullptr)
			{
				throw ShaderException("Creation of input layour failed, ID3DBlob was nullptr.",__LINE__,__FILE__);
			}
		}

		class FakeBlob : public ID3DBlob
		{
		public:
			virtual ULONG STDMETHODCALLTYPE Release() { return 0; };
			virtual SIZE_T STDMETHODCALLTYPE GetBufferSize() { return 0; };
			virtual LPVOID STDMETHODCALLTYPE GetBufferPointer() { return nullptr; };
			virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID&, void**) { return S_OK; };
			virtual ULONG STDMETHODCALLTYPE AddRef() { return 0; };
		};

		HRESULT compileShader(LPCWSTR p_Filename, D3D_SHADER_MACRO *p_Defines, const char *p_EntryPoint,
			const char *p_ShaderModel, DWORD p_ShaderFlags,
			ID3DBlob *&p_ShaderData, ID3DBlob *&p_ErrorMessage) override
		{
			if(wcscmp(p_Filename,L"") == 0)
			{
				throw ShaderException("Filename missing",__LINE__,__FILE__);
			}
			if(strcmp(p_EntryPoint, "") == 0)
			{
				throw ShaderException("Entrypoint is missing",__LINE__,__FILE__);
			}
			if(strcmp(p_ShaderModel,"") == 0)
			{
				throw ShaderException("Shadermodel is missing",__LINE__,__FILE__);
			}
			if(p_ShaderFlags == 0)
			{
				throw ShaderException("Shaderflags is missing",__LINE__,__FILE__);
			}
			if(wcscmp(p_Filename, L"dummy.hlsl") == 0)
			{
				return S_FALSE;
			}
			static FakeBlob fakeBlob;
			p_ShaderData = &fakeBlob;
			p_ErrorMessage = &fakeBlob;
			return S_OK;
		}
	};

	class DummyBuffer : public Buffer
	{
	public:
		HRESULT createBuffer(D3D11_BUFFER_DESC *p_BufferDescription, D3D11_SUBRESOURCE_DATA *p_Data, ID3D11Buffer **p_Buffer) override
		{
			if(p_BufferDescription->BindFlags < 7 && p_BufferDescription->Usage ==  4)
			{
				throw BufferException("Buffer creation failed",__LINE__,__FILE__);
			}
			else
			{
				return S_OK;
			}
		}
	};

	class DummyWrapper : public WrapperFactory
	{
	private:
		static DummyWrapper *instance;
		
		~DummyWrapper()
		{
			 
		}

	public:

		static DummyWrapper *getDummyInstance()
		{
			if(!instance)
				throw WrapperFactoryException("Wrapper factory has not initialized before use", __LINE__, __FILE__);

			return instance;
		}

		static void initializeDummy()
		{
			if (!instance)
			{
				instance = new DummyWrapper();
			}
		}

		Shader *createShader(LPCWSTR p_Filename, const char *p_EntryPoint, const char *p_ShaderModel,
			ShaderType p_Type) override
		{
			DummyShader *shader = new DummyShader();


			vector<string> entryPointList = createEntryPointList(p_EntryPoint);
			string entryPoint;

			try
			{
				for(int i = 0; i < 5; i++)
				{
					if(p_Type & ShaderType((int)std::pow(2,i)))
					{
						entryPoint = entryPointList.back();
						entryPointList.pop_back();
						addShaderStep(shader, p_Filename, nullptr, entryPoint.c_str(), p_ShaderModel,
							Shader::Type(i));
						if(entryPointList.size() == 0)
							break;
					}
				}
				return shader;
			}
			catch(...)
			{
				SAFE_DELETE(shader);
				throw;
			}
		}

		void addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, D3D_SHADER_MACRO *p_Defines, const char *p_EntryPoint, const char *p_ShaderModel,
			Shader::Type p_ShaderType) override
		{
			std::string temp = getShaderModel(p_ShaderModel, p_ShaderType);
			p_Shader->compileAndCreateShader(p_Filename, p_Defines, p_EntryPoint, temp.c_str(), p_ShaderType, nullptr);
		}

		void addShaderStep(Shader *p_Shader, LPCWSTR p_Filename, D3D_SHADER_MACRO *p_Defines, const char *p_EntryPoint, const char *p_ShaderModel,
			Shader::Type p_ShaderType, const D3D11_INPUT_ELEMENT_DESC *p_VertexLayout) override
		{
			std::string temp = getShaderModel(p_ShaderModel, p_ShaderType);
			p_Shader->compileAndCreateShader(p_Filename, p_Defines, p_EntryPoint, temp.c_str(), p_ShaderType, p_VertexLayout);
		}

		Buffer *createBuffer(Buffer::Description &p_Description)
		{
			DummyBuffer *buffer = new DummyBuffer();
			HRESULT result;
			try
			{
				result = buffer->initialize(nullptr, nullptr, p_Description);
				if(result == S_OK)
				{
					return buffer;
				}
				else
				{
					SAFE_DELETE(buffer);
					return nullptr;
				}

			}
			catch(...)
			{
				SAFE_DELETE(buffer);
				throw;
			}
		}

		void shutdown(void) override
		{
			SAFE_DELETE(instance);
		}
	};

	DummyWrapper *DummyWrapper::instance = nullptr;

	BOOST_AUTO_TEST_CASE(TestInitShader)
	{
		DummyShader shady;
		shady.initialize(nullptr,nullptr,1);

		D3D11_INPUT_ELEMENT_DESC desc[] = 
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		BOOST_CHECK_THROW(shady.compileAndCreateShader(L"dummy.hlsl", 0, "vs_main", "vs_5_0", Shader::Type::VERTEX_SHADER, desc),
			ShaderException);
	}

	BOOST_AUTO_TEST_CASE(TestInitBuffer)
	{
		DummyBuffer buff; 

		float ff[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}; 

		Buffer::Description desc;
		desc.initData =  &ff;
		for(int i = 0; i < 5;i++)
		{
			for(int j = 0; j < 7;j++)
			{
				desc.numOfElements = 4;
				desc.sizeOfElement = 12;
				desc.usage = (Buffer::Usage)i;
				desc.type = (Buffer::Type)j;

				std::string msg = "BufferDesc combination of (" + std::to_string(i) + "," + std::to_string(j) + ") failed.";
				if(i == 4 && j < 7)
				{
					BOOST_CHECK_THROW(buff.initialize(nullptr, nullptr, desc), BufferException);
				}
				else
				{
					BOOST_CHECK_MESSAGE(buff.initialize(nullptr, nullptr, desc) == S_OK, msg);					
				}				
			}
		}
	}

	BOOST_AUTO_TEST_CASE(TestShaderFactory)
	{
		DummyWrapper *wraps = nullptr;
		DummyWrapper::initializeDummy();
		wraps = DummyWrapper::getDummyInstance();
		
		DummyShader *shady = nullptr;

		D3D11_INPUT_ELEMENT_DESC desc[] = 
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		int size = sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC);

		shady = new DummyShader();
		shady->initialize(nullptr, nullptr, size);

		BOOST_CHECK_NO_THROW(wraps->addShaderStep(shady, L"Source/dummy.hlsl", 0, "main", "5_0",
			Shader::Type::VERTEX_SHADER, desc));

		BOOST_CHECK(shady != nullptr);
		
		SAFE_DELETE(shady);

		wraps->shutdown();
	}

	BOOST_AUTO_TEST_CASE(TestShaderFactoryWODesc)
	{
		DummyWrapper *wraps = nullptr;
		DummyWrapper::initializeDummy();
		wraps = DummyWrapper::getDummyInstance();

		DummyShader *shady = nullptr;
		shady = new DummyShader();
		shady->initialize(nullptr, nullptr, 0);

		BOOST_CHECK_NO_THROW(wraps->addShaderStep(shady, L"Source/dummy.hlsl", 0, "main", "5_0",
			Shader::Type::VERTEX_SHADER));

		BOOST_CHECK(shady != nullptr);

		SAFE_DELETE(shady);

		wraps->shutdown();
	}

	BOOST_AUTO_TEST_CASE(TestShaderFactoryWODescMultipleShaders)
	{
		DummyWrapper *wraps = nullptr;
		DummyWrapper::initializeDummy();
		wraps = DummyWrapper::getDummyInstance();

		DummyShader *shady = nullptr;
		
		BOOST_CHECK_NO_THROW(shady = dynamic_cast<DummyShader*>(wraps->createShader(L"Source/dummy.hlsl", "main,PS",
			"5_0", ShaderType::VERTEX_SHADER | ShaderType::PIXEL_SHADER )));

		BOOST_CHECK(shady != nullptr);

		SAFE_DELETE(shady);

		wraps->shutdown();
	}

BOOST_AUTO_TEST_SUITE_END()