#include "FontCollectionLoader.h"

#include <cstdint>
#include <fstream>
#include <sstream>

#include <boost/filesystem.hpp>

/////////////////////////////FONT COLLECTION LOADER/////////////////////////////
IDWriteFontCollectionLoader* FontCollectionLoader::instance_(
	new FontCollectionLoader()
	);

HRESULT STDMETHODCALLTYPE FontCollectionLoader::QueryInterface(REFIID iid, void** ppvObject)
{
	if(iid == IID_IUnknown || iid == __uuidof(IDWriteFontCollectionLoader))
	{
		*ppvObject = this;
		AddRef();
		return S_OK;
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}

ULONG STDMETHODCALLTYPE FontCollectionLoader::AddRef()
{
	return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE FontCollectionLoader::Release()
{
	ULONG newCount = InterlockedDecrement(&refCount_);
	if(newCount == 0)
		delete this;

	return newCount;
}

HRESULT STDMETHODCALLTYPE FontCollectionLoader::CreateEnumeratorFromKey(
	IDWriteFactory* factory,
	void const* collectionKey,
	UINT32 collectionKeySize,
	OUT IDWriteFontFileEnumerator** fontFileEnumeration
	)
{
	*fontFileEnumeration = NULL;

	HRESULT hr = S_OK;

	FontFileEnumerator* enumerator = new FontFileEnumerator(factory);
	if(enumerator == NULL)
		return E_OUTOFMEMORY;

	std::vector<std::string> filenames;

	size_t keyPos = 0;
	while (keyPos < collectionKeySize)
	{
		if (keyPos + sizeof(uint32_t) > collectionKeySize)
		{
			return E_INVALIDARG;
		}

		uint32_t filenameLength = *reinterpret_cast<uint32_t*>((char*)collectionKey + keyPos);
		keyPos += sizeof(uint32_t);

		if (keyPos + filenameLength > collectionKeySize)
		{
			return E_INVALIDARG;
		}

		filenames.emplace_back((char*)collectionKey + keyPos, filenameLength);
		keyPos += filenameLength;
	}

	hr = enumerator->Initialize(filenames);

	if( FAILED(hr))
	{
		delete enumerator;
		return hr;
	}

	*fontFileEnumeration = SafeAcquire(enumerator);

	return hr;
}
/////////////////////////////FONT COLLECTION LOADER/////////////////////////////

/////////////////////////////FONT FILE ENMUMERATOR/////////////////////////////
FontFileEnumerator::FontFileEnumerator(
	IDWriteFactory* factory
	) :
	refCount_(0),
	factory_(SafeAcquire(factory)),
	currentFile_(),
	nextIndex_(0)
{
}

HRESULT FontFileEnumerator::Initialize(const std::vector<std::string>& p_Filenames)
{
	try
	{
		m_Filenames = p_Filenames;
	}
	catch (...)
	{
		return ExceptionToHResult();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE FontFileEnumerator::QueryInterface(REFIID iid, OUT void** ppvObject)
{
	if(iid == IID_IUnknown || iid == __uuidof(IDWriteFontFileEnumerator))
	{
		*ppvObject = this;
		AddRef();
		return S_OK;
	}
	else
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}

ULONG STDMETHODCALLTYPE FontFileEnumerator::AddRef()
{
	return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE FontFileEnumerator::Release()
{
	ULONG newCount = InterlockedDecrement(&refCount_);
	if(newCount == 0)
		delete this;

	return newCount;
}

HRESULT STDMETHODCALLTYPE FontFileEnumerator::MoveNext(OUT BOOL* hasCurrentFile)
{
	HRESULT hr = S_OK;

	*hasCurrentFile = FALSE;

	SafeRelease(&currentFile_);

	if(nextIndex_ < m_Filenames.size())
	{
		hr = factory_->CreateCustomFontFileReference(
			m_Filenames[nextIndex_].c_str(),
			m_Filenames[nextIndex_].length(),
			FontFileLoader::GetLoader(),
			&currentFile_
			);
		if(SUCCEEDED(hr))
		{
			*hasCurrentFile = TRUE;

			++nextIndex_;
		}
	}
	return hr;
}

HRESULT STDMETHODCALLTYPE FontFileEnumerator::GetCurrentFontFile(OUT IDWriteFontFile** fontFile)
{
	*fontFile = SafeAcquire(currentFile_);

	return (currentFile_ != NULL) ? S_OK : E_FAIL;
}
/////////////////////////////FONT FILE ENMUMERATOR/////////////////////////////

/////////////////////////////FONT FILE LOADER/////////////////////////////
IDWriteFontFileLoader* FontFileLoader::instance_(
    new(std::nothrow) FontFileLoader()
    );

// QueryInterface
HRESULT STDMETHODCALLTYPE FontFileLoader::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontFileLoader))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

// AddRef
ULONG STDMETHODCALLTYPE FontFileLoader::AddRef()
{
    return InterlockedIncrement(&refCount_);
}

// Release
ULONG STDMETHODCALLTYPE FontFileLoader::Release()
{
    ULONG newCount = InterlockedDecrement(&refCount_);
    if (newCount == 0)
        delete this;

    return newCount;
}

// CreateStreamFromKey
//
//      Creates an IDWriteFontFileStream from a key that identifies the file. The
//      format and meaning of the key is entirely up to the loader implementation.
//      The only requirements imposed by DWrite are that a key must remain valid
//      for as long as the loader is registered. The same key must also uniquely
//      identify the same file, so for example you must not recycle keys so that
//      a key might represent different files at different times.
//
//      In this case the key is a UINT which identifies a font resources.
//
HRESULT STDMETHODCALLTYPE FontFileLoader::CreateStreamFromKey(
    void const* fontFileReferenceKey,       // [fontFileReferenceKeySize] in bytes
    UINT32 fontFileReferenceKeySize,
    OUT IDWriteFontFileStream** fontFileStream
    )
{
    *fontFileStream = NULL;

	std::string filename((const char*)fontFileReferenceKey, fontFileReferenceKeySize);

    // Create the stream object.
	FontFileStream* stream = new FontFileStream(filename);
    if (stream == NULL)
        return E_OUTOFMEMORY;

    if (!stream->IsInitialized())
    {
        delete stream;
        return E_FAIL;
    }

    *fontFileStream = SafeAcquire(stream);

    return S_OK;
}
/////////////////////////////FONT FILE LOADER/////////////////////////////

/////////////////////////////FONT FILE STREAM/////////////////////////////

FontFileStream::FontFileStream(const std::string& p_FilePath) :
	refCount_(0)
{
	std::ifstream stream(p_FilePath, std::ios::binary);
	if (stream.bad())
		return;

	stream.seekg(0, std::ios::end);
	std::streampos offset = stream.tellg();
	stream.seekg(0, std::ios::beg);

	if ((int)offset <= -1)
		return;

	m_FileContent.resize((size_t)offset);
	stream.read(m_FileContent.data(), offset);
}

// IUnknown methods
HRESULT STDMETHODCALLTYPE FontFileStream::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontFileStream))
    {
        *ppvObject = this;
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE FontFileStream::AddRef()
{
    return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE FontFileStream::Release()
{
    ULONG newCount = InterlockedDecrement(&refCount_);
    if (newCount == 0)
        delete this;

    return newCount;
}

// IDWriteFontFileStream methods
HRESULT STDMETHODCALLTYPE FontFileStream::ReadFileFragment(
    void const** fragmentStart, 
    UINT64 fileOffset,
    UINT64 fragmentSize,
    OUT void** fragmentContext
    )
{
	if (fileOffset <= m_FileContent.size() && 
        fragmentSize <= m_FileContent.size() - fileOffset)
    {
		*fragmentStart = m_FileContent.data() + fileOffset;
        *fragmentContext = NULL;
        return S_OK;
    }
    else
    {
        *fragmentStart = NULL;
        *fragmentContext = NULL;
        return E_FAIL;
    }
}

void STDMETHODCALLTYPE FontFileStream::ReleaseFileFragment(
    void* fragmentContext
    )
{
}

HRESULT STDMETHODCALLTYPE FontFileStream::GetFileSize(
    OUT UINT64* fileSize
    )
{
	*fileSize = m_FileContent.size();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE FontFileStream::GetLastWriteTime(
    OUT UINT64* lastWriteTime
    )
{
    *lastWriteTime = 0;
    return E_NOTIMPL;
}
/////////////////////////////FONT FILE STREAM/////////////////////////////

/////////////////////////////FONT Context/////////////////////////////
FontContext::FontContext()
	: hr_(S_FALSE),
	m_WriteFactory(nullptr)
{
}

FontContext::~FontContext()
{
}

HRESULT FontContext::Initialize(IDWriteFactory* p_WriteFactory)
{
    if (hr_ == S_FALSE)
    {
        hr_ = InitializeInternal(p_WriteFactory);
    }
    return hr_;
}

void FontContext::shutdown()
{
    m_WriteFactory->UnregisterFontCollectionLoader(FontCollectionLoader::GetLoader());
    m_WriteFactory->UnregisterFontFileLoader(FontFileLoader::GetLoader());
}

HRESULT FontContext::InitializeInternal(IDWriteFactory* p_WriteFactory)
{
    HRESULT hr = S_OK;

    if (!FontFileLoader::IsLoaderInitialized()
    ||  !FontCollectionLoader::IsLoaderInitialized())
    {
        return E_FAIL;
    }

    // Register our custom loaders with the factory object.
    //
    // Note: For this application we just use the shared DWrite factory object which is accessed via 
    //       a global variable. If we were using fonts embedded in *documents* then it might make sense 
    //       to create an isolated factory for each document. When unloading the document, one would
    //       also release the isolated factory, thus ensuring that all cached font data specific to
    //       that document would be promptly disposed of.
    //
    if (FAILED(hr = p_WriteFactory->RegisterFontFileLoader(FontFileLoader::GetLoader())))
        return hr;

    if (FAILED(hr = p_WriteFactory->RegisterFontCollectionLoader(FontCollectionLoader::GetLoader())))
		return hr;

	m_WriteFactory = p_WriteFactory;

    return hr;
}

HRESULT FontContext::CreateFontCollection(
	IDWriteFactory* p_WriteFactory,
	const std::string& p_FontFolderPath,
    OUT IDWriteFontCollection** result
    )
{
    *result = NULL;

    HRESULT hr = S_OK;

    hr = Initialize(p_WriteFactory);
    if (FAILED(hr))
        return hr;

	std::vector<std::string> files;

	boost::filesystem::directory_iterator currFile(p_FontFolderPath);
	for (; currFile != boost::filesystem::directory_iterator(); ++currFile)
	{
		auto filename = currFile->path();
		files.push_back(filename.string());
	}

	std::ostringstream stream;
	for (const auto& filename : files)
	{
		const uint32_t length = filename.length();
		stream.write((char*)(&length), 4);
		stream.write(filename.data(), length);
	}
	const std::string key = stream.str();

    hr = m_WriteFactory->CreateCustomFontCollection(
            FontCollectionLoader::GetLoader(),
            key.data(),
            key.length(),
            result
            );

    return hr;
}	  
/////////////////////////////FONT Context/////////////////////////////