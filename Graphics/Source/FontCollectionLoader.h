#pragma once
#include <memory>
#include <vector>
#include <dwrite.h>

// Releases a COM object and nullifies pointer.
template <typename InterfaceType>
inline void SafeRelease(InterfaceType** currentObject)
{
    if (*currentObject != NULL)
    {
        (*currentObject)->Release();
        *currentObject = NULL;
    }
}

// Acquires an additional reference, if non-null.
template <typename InterfaceType>
inline InterfaceType* SafeAcquire(InterfaceType* newObject)
{
    if (newObject != NULL)
        newObject->AddRef();

    return newObject;
}

// Sets a new COM object, releasing the old one.
template <typename InterfaceType>
inline void SafeSet(InterfaceType** currentObject, InterfaceType* newObject)
{
    SafeAcquire(newObject);
    SafeRelease(&currentObject);
    currentObject = newObject;
}


// Maps exceptions to equivalent HRESULTs,
inline HRESULT ExceptionToHResult() throw()
{
    try
    {
        throw;  // Rethrow previous exception.
    }
    catch(std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_FAIL;
    }
}

class FontCollectionLoader : public IDWriteFontCollectionLoader
{
private:
	ULONG refCount_;

	static std::unique_ptr<IDWriteFontCollectionLoader> instance_;

public:
	FontCollectionLoader(void) : refCount_(0)
	{
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject) override;
	virtual ULONG STDMETHODCALLTYPE AddRef() override;
    virtual ULONG STDMETHODCALLTYPE Release() override;

	virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(
        IDWriteFactory* factory,
        void const* collectionKey,
        UINT32 collectionKeySize,
        OUT IDWriteFontFileEnumerator** fontFileEnumerator
        ) override;

	static IDWriteFontCollectionLoader* GetLoader()
	{
		return instance_.get();
	}

	static bool IsLoaderInitialized()
	{
		return instance_ != NULL;
	}
};

class FontFileEnumerator : public IDWriteFontFileEnumerator
{
private:
	ULONG refCount_;

	IDWriteFactory* factory_;
	IDWriteFontFile* currentFile_;
	std::vector<std::string> m_Filenames;
	size_t nextIndex_;

public: 
	FontFileEnumerator(IDWriteFactory* factor);
	
	HRESULT Initialize(const std::vector<std::string>& p_Filenames);

	~FontFileEnumerator()
	{
		SafeRelease(&currentFile_);
		SafeRelease(&factory_);
	}

	// IUnknown methods
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject) override;
	virtual ULONG STDMETHODCALLTYPE AddRef() override;
	virtual ULONG STDMETHODCALLTYPE Release() override;

	// IDWriteFontFileEnumerator methods
	virtual HRESULT STDMETHODCALLTYPE MoveNext(OUT BOOL* hasCurrentFile) override;
	virtual HRESULT STDMETHODCALLTYPE GetCurrentFontFile(OUT IDWriteFontFile** fontFile) override;
};

class FontFileLoader : public IDWriteFontFileLoader
{
private:
	ULONG refCount_;

	static std::unique_ptr<IDWriteFontFileLoader> instance_;

public:
	FontFileLoader() : refCount_(0)
	{
	}

	// IUnknown methods
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject) override;
	virtual ULONG STDMETHODCALLTYPE AddRef() override;
	virtual ULONG STDMETHODCALLTYPE Release() override;

	// IDWriteFontFileLoader method
	virtual HRESULT STDMETHODCALLTYPE CreateStreamFromKey(
		void const* fontFileReferenceKey,
		UINT32 fontFileReferenceKeySize,
		OUT IDWriteFontFileStream** fontFileStream
		) override;

	// Gets singleton loader instance.
	static IDWriteFontFileLoader* GetLoader()
	{
		return instance_.get();
	}

	static bool IsLoaderInitialized()
	{
		return instance_ != NULL;
	}
};

class FontFileStream : public IDWriteFontFileStream
{
private:
	ULONG refCount_;
	std::vector<char> m_FileContent;

public:
	explicit FontFileStream(const std::string& p_FilePath);

	// IUnknown methods
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject) override;
	virtual ULONG STDMETHODCALLTYPE AddRef() override;
	virtual ULONG STDMETHODCALLTYPE Release() override;

	// IDWriteFontFileStream methods
	virtual HRESULT STDMETHODCALLTYPE ReadFileFragment(
		void const** fragmentStart,
		UINT64 fileOffset,
		UINT64 fragmentSize,
		OUT void** fragmentContext
		) override;

	virtual void STDMETHODCALLTYPE ReleaseFileFragment(
		void* fragmentContext
		) override;

	virtual HRESULT STDMETHODCALLTYPE GetFileSize(
		OUT UINT64* fileSize
		) override;

	virtual HRESULT STDMETHODCALLTYPE GetLastWriteTime(
		OUT UINT64* lastWriteTime
		) override;

	bool IsInitialized()
	{
		return !m_FileContent.empty();
	}
};

class FontContext
{
private:
	IDWriteFactory* m_WriteFactory;

	// Not copyable or assignable
	FontContext(FontContext const&);
	void operator=(FontContext const&);

	HRESULT InitializeInternal(IDWriteFactory* p_WriteFactory);

	// Error code from Initialize().
	HRESULT hr_;

public:
	FontContext();
	~FontContext();

	HRESULT Initialize(IDWriteFactory* p_WriteFactory);
	void shutdown();

	HRESULT CreateFontCollection(
		IDWriteFactory* p_WriteFactory,
		const std::string& p_FontFolderPath,
		OUT IDWriteFontCollection** result
		);
};
