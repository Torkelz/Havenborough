#include "SimpleText.h"

SimpleText::SimpleText(void)
{
	m_WriteFactory = nullptr;
	m_TextFormat = nullptr;

	m_Text = nullptr;

	m_D2DFactory = nullptr;
	m_RenderTarget = nullptr;
	m_BlackBrush = nullptr;
}


SimpleText::~SimpleText(void)
{
	SAFE_RELEASE(m_WriteFactory);
	SAFE_RELEASE(m_TextFormat);
	SAFE_RELEASE(m_D2DFactory);
	SAFE_RELEASE(m_RenderTarget);
	SAFE_RELEASE(m_BlackBrush);
}

void SimpleText::initialize(HWND *p_Hwnd, IDXGISurface *p_RenderTarget)
{
	HRESULT hr;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_D2DFactory);

	if(SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_WriteFactory));
	}

	m_Text = L"Havenborough Let's play";
	m_TextLength = (UINT32)wcslen(m_Text);

	if(SUCCEEDED(hr))
	{
		hr = m_WriteFactory->CreateTextFormat(L"Gabriola", NULL, DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
			72.f, L"en-us", &m_TextFormat);
	}

	if(SUCCEEDED(hr))
	{
		hr = m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}
	if(SUCCEEDED(hr))
	{
		hr = m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	GetClientRect(*p_Hwnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	FLOAT dpiX;
	FLOAT dpiY;
	m_D2DFactory->GetDesktopDpi(&dpiX, &dpiY);

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX,
		dpiY
		);

	if(!m_RenderTarget)
	{
		hr = m_D2DFactory->CreateDxgiSurfaceRenderTarget(p_RenderTarget, &props,
			&m_RenderTarget);
		//hr = m_D2DFactory->CreateHwndRenderTarget( D2D1::RenderTargetProperties(),
		//	D2D1::HwndRenderTargetProperties(*p_Hwnd, size), &m_RenderTarget);

		if(SUCCEEDED(hr))
		{
			hr = m_RenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_BlackBrush);
		}
	}

}

void SimpleText::draw()
{
	float dpiScaleX = 96.f;
	float dpiScaleY = 96.f;
	D2D1_RECT_F layoutRect = D2D1::RectF(
		0,//static_cast<FLOAT>(rc.left) / dpiScaleX,
		0,//static_cast<FLOAT>(rc.top) / dpiScaleY,
		static_cast<FLOAT>(rc.right - rc.left),// / dpiScaleX,
		static_cast<FLOAT>(rc.bottom - rc.top)// / dpiScaleY
		);
	m_RenderTarget->BeginDraw();
	m_RenderTarget->SetTransform(D2D1::IdentityMatrix());
	m_RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	//m_RenderTarget->DrawText(m_Text, m_TextLength, m_TextFormat, layoutRect, m_BlackBrush);

	m_RenderTarget->EndDraw();
}


////Load D3D10.DLL
//m_hD3D10 = LoadLibrary(L"d3d10_1.dll");
//
////Get adapter of the current D3D11 device. Our D3D10 will run on the same adapter.
//IDXGIDevice* pDXGIDevice;
//IDXGIAdapter* pAdapter;
//pDevice11->QueryInterface<IDXGIDevice>(&pDXGIDevice);
//pDXGIDevice->GetAdapter(&pAdapter);
//SAFE_RELEASE( pDXGIDevice );
//
////Get address of the function D3D10CreateDevice1 dynamically.
//typedef HRESULT (WINAPI* FN_D3D10CreateDevice1)(
//	IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software,
//	UINT Flags, D3D10_FEATURE_LEVEL1 HardwareLevel, UINT SDKVersion, ID3D10Device1 **ppDevice );
//FN_D3D10CreateDevice1 fnCreate = (FN_D3D10CreateDevice1)GetProcAddress(m_hD3D10, "D3D10CreateDevice1");
//
////Call D3D10CreateDevice1 dynamically.
//fnCreate(pAdapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT
//		 | D3D10_CREATE_DEVICE_DEBUG, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &m_pInterD3D10);
//
//Now we have a D3D10 device: m_pInterD3D10 (which means the Intermediate Direct3D10 device).
//	Now let’s share it with D3D11. Do this on your Swapchain Resizing event. (Because size of the Render Target changes):
//
//	//Create a D3D10.1 render target texture and share it with our D3D11.
//	D3D10_TEXTURE2D_DESC tDesc;
//tDesc.Width = ScreenW;
//tDesc.Height = ScreenH;
//tDesc.MipLevels = 1;
//tDesc.ArraySize = 1;
//tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//tDesc.SampleDesc.Count = 1;
//tDesc.SampleDesc.Quality = 0;
//tDesc.Usage = D3D10_USAGE_DEFAULT;
////EVEN IF YOU WON'T USE AS SHADER RESOURCE, SET THIS BIND FLAGS:
//tDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
//tDesc.CPUAccessFlags = 0;
//tDesc.MiscFlags = D3D10_RESOURCE_MISC_SHARED;//
//
////Create the RT texture on D3D10
//m_pInterD3D10->CreateTexture2D(&tDesc,NULL,&m_pInterRTD3D10);
//
////Get DXGI Resource and retrieve the sharing handle.
//m_pInterRTD3D10->QueryInterface<IDXGISurface>(&pDXGISurf);
//pDXGISurf->QueryInterface<IDXGIResource>(&pDXGIRes);
//pDXGIRes->GetSharedHandle( &ShareHandle );
//SAFE_RELEASE( pDXGIRes );
//SAFE_RELEASE( pDXGISurf );
//
////Call D3D 11 to open shared resource (m_pDevice is the D3D11 device).
//m_pDevice->OpenSharedResource(ShareHandle,
//							  __uuidof(ID3D11Resource), (void**)&pD3D11Res);
//pD3D11Res->QueryInterface<ID3D11Texture2D>(&m_pInterRTD3D11);
//SAFE_RELEASE( pD3D11Res );
//
//Now we should make a shader resource view for D3D11 device to be able to render the shared resource. I don’t write the code because it is obvious (you’ll need ID3D11Device::CreateShaderResourceView).
//
//	Now it is time to create the D2D render target. Use this code: [Assuming m_pD2DFactory was created using D2D1CreateFactory]
//
////Get DXGI Surface from the created render target.
//IDXGISurface1* pRT10;
//m_pInterRTD3D10->QueryInterface<IDXGISurface1>(&pRT10);
//
//FLOAT dpiX;
//FLOAT dpiY;
//m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);
//
//// Create a DC render target.
//D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
//	D2D1_RENDER_TARGET_TYPE_DEFAULT,
//	D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_IGNORE),
//	static_cast<float>(dpiX),
//	static_cast<float>(dpiY)
//	);
//
//// Create a Direct2D render target.
//// Assuming m_pD2DFactory was previously created with:
////D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), NULL,
////   (void**)(&m_pD2DFactory));
//m_pD2DFactory->CreateDxgiSurfaceRenderTarget( pRT10, (const D2D1_RENDER_TARGET_PROPERTIES *)&props, &m_pRT );
//
//
//Here, we have successfully connected D3D11 to D2D1. We can now draw. [NOTE: For details on Resolving GPU Synchronization problems see below .]
//We still have a problem!!:
//We render using D2D to m_pRT. D2D uses D3D10.1 to render. Note that the render target created by D3D10 is shared among 3 users: D3D11, D3D10 and D2D. As soon as renderings to the shared RT are done using D3D10 and D2D, you are ready to render the RT onto your D3D11’s backbuffer. But this will overwrite previous data on the backbuffer – meaning that your D2D renderings will cover your D3D11 renderings, like the 3D objects you’ve rendered. The reason is that D2D’s support of straight alpha blending is limited which I think is a weakness point. That is D2D will not use alpha channel of the render target. D2D uses alpha blending to increase quality but the result alpha value is not written to the Rendertarget. (For more information, see the documentations on D2D’s render targets). So the result will be a non-transparent image with all of D2D’s renderings (including the DirectWrite texts). So what should we do? There are three ways to resolve this and I want to go into details of the third one.
//
//	-          Before the D2D renderings, copy your D3D11 renderings to the shared render target so that D2D can access them. Then do your D2D renderings. The drawn 2D shapes and text will be over your previous D3D11 drawing.
//	[I also tested this way, and it is possible.].
//	-          Better way: Do all of your D3D11 rendering on the shared render target. Then use D2D to do rest of the renderings on the same render target. This is much similar to the first method.
//	-          My usage was only to render DWrite texts. So I used another method:
//I rendered the direct write text with white color on black background (clear the D2D rendertarget using: ID2D1RenderTarget::Clear). There will be a black rendertarget with white text on. Then I used a pixel shader on D3D11 that converted the brightness (whiteness) of the Render Target to alpha value and I used the shader to color the text: [NOTE: This uses Effects11 interface to be compiled (Search for Effects11 in MSDN). You can also use D3D11’s basic shader compiler.]
//
//float4 Render2DTextPS( VS_OUTPUT_2D Input ) : SV_Target
//{
//	float4 Texture = g_TextureDiffuse.Sample( Sampler_Render2D, Input.vTexCoord );   //Read the shared render target.
//	float Intensity = (Texture.r * 0.30f + Texture.g * 0.59f + Texture.b * 0.11f);   //Calculate brightness.
//	float4 Diffuse = g_ObjectColor;      //ObjectColor was set in our CPU code.
//	Diffuse.a *= Intensity;              //Apply intensity which is the brightness read from shared render target.
//	return Diffuse;
//}
//
//Problem of this method is obvious: Most of D2D’s effects like the beautiful colorings cannot be used. Anything will be grayscale before that we color them in our shaders. But it also has some advantages! For example you could apply any effect you want on D3D11. You could also read another texture and make your text textured. (Which is also available in D2D).
//	Here is a code for rendering DirectWrite text (The text format class, m_pTextFormat, was created before.):
//
//	…
//	m_pRT->SetTransform(D2D1::IdentityMatrix());
////Setup the draw region.
//D2D1_RECT_F layoutRect = ...;
//
////Use the DrawText method of the D2D render target interface to draw
//m_pBrush->SetColor( D2D1::ColorF(0,0,0,1) );
//m_pBrush->SetColor( D2D1::ColorF(Color.r,Color.g,Color.b,Color.a) );
//m_pRT->DrawText(
//	Text, TextSize == -1 ? lstrlen(Text) : TextSize,
//	m_pTextFormat, layoutRect, m_pBrush,
//	D2D1_DRAW_TEXT_OPTIONS_NO_SNAP
//	);
//…
//
//	Now there will be two questions:
//<1> What should we do if we want text with different colors and effects? The answer is that for each text, you should use D3D11 to render it with your arbitrary color and effects. This a pseudo-code for this:
//Function: Draw Text using Direct Write
//		  Clear the Shard RT using D2D with block color [We could also draw a black box on the region of rendering]
//Draw the text using D2D on the wanted region (rectangle).
//	Call D3D11 to render the shared render target.
//	You can also specify a region to render text to and then render the same region on the backbuffer.
//
//	<2> Question 2: In D2Ds examples we haven’t seen this difficulty for having D3D and D2D renderings at once. Why here is it that messy? The answer is simple: Normally, the render targets that D3D and D2D render to are the same. But here, we are drawing with D2D on a render target (I mean the shared render target we made with D3D10), and then moving it to another render target (I mean D3D11’s backbuffer). Remember, I said that you can also use the shared RT to render both D3D11 and D2D drawings. But I haven’t tested that before. It may not work.
//
//	<3>  What about surface synchronization? How does D3D11 know if D3D10 and D2D renderings are done?
//Answer: This is an advanced topic about surface synchronization. Users can see this:  http://code.msdn.microsoft.com/D3D9ExDXGISharedSurf which gives a good way to synchronize between different D3D APIs.
//The main way suggested in this documentation is to create a STAGING resource for the renderer device (D3D10) and copy a small portion of the shared resource to it and then lock/unlock the STAGING resource. The locking will be delayed until the STAGING surface is done being copied to. At this time, you can make sure that the shared resource has done rendering. (The STAGING resource is made in small size - for example, 16x16). Remember: The main render target's usage mode is DEFAULT not STAGING and DEFAULT usage resources don't allow CPU access.
//
//	This is a simpler code I wrote for this:
//First make a staging resource:
//
////Create the synchronizer for D3D10 which is a STAGING resource which we can lock (map).
//D3D10_TEXTURE2D_DESC tSyncDesc;
//tSyncDesc.Width = 16;
//tSyncDesc.Height = 16;
//tSyncDesc.MipLevels = 1;
//tSyncDesc.ArraySize = 1;
//tSyncDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//tSyncDesc.SampleDesc.Count = 1;
//tSyncDesc.SampleDesc.Quality = 0;
//tSyncDesc.Usage = D3D10_USAGE_STAGING;
//tSyncDesc.BindFlags = 0;
//tSyncDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ; //We need to be able to lock it.
//tSyncDesc.MiscFlags = 0;
//m_pInterD3D10->CreateTexture2D(&tSyncDesc,NULL,&m_pInterD3D10_GPUSync);
//
//Then define a function like this:
//void RenderTargetGPUSync()
//{
//	if (!m_pInterD3D10_GPUSync || !m_pInterRTD3D10) return;
//	D3D10_BOX CopyBox = {0, 0, 0, 16, 16, 1};
//
//	pDeviceD3D10->CopySubresourceRegion(m_pInterD3D10_GPUSync, 0, 0, 0, 0,
//		m_pInterRTD3D10, 0, &CopyBox );
//	D3D10_MAPPED_TEXTURE2D mapRegion;
//
//	//This will wait until all GPU operations to m_pInterD3D10_GPUSync are done.
//	m_pInterD3D10_GPUSync->Map( 0, D3D10_MAP_READ, 0, &mapRegion);
//	m_pInterD3D10_GPUSync->Unmap(0);
//}