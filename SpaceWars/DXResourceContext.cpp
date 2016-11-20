#include "DXResourceContext.h"


DXResourceContext::DXResourceContext()
{
	mDevice = 0;
	mContext = 0;
	mSwapChain = 0;

	mPrimaryRT = 0;
	mPostProcessRT = 0;
	mBackBufferRT = 0;

	mPrimaryRTV = 0;
	mPostProcessRTV = 0;
	mBackBufferRTV = 0;

	mDepthStencilView = 0;
}


DXResourceContext::~DXResourceContext()
{
	release();
}

HRESULT DXResourceContext::createDevice()
{
	// This will hold options for DirectX initialization
	unsigned int deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// If we're in debug mode in visual studio, we also
	// want to make a "Debug DirectX Device" to see some
	// errors and warnings in Visual Studio's output window
	// when things go wrong!
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(
		0,							// Video adapter (physical GPU) to use, or null for default
		D3D_DRIVER_TYPE_HARDWARE,	// We want to use the hardware (GPU)
		0,							// Used when doing software rendering
		deviceFlags,				// Any special options
		0,							// Optional array of possible verisons we want as fallbacks
		0,							// The number of fallbacks in the above param
		D3D11_SDK_VERSION,			// Current version of the SDK
		&mDevice,
		&support.dxFeatureLevel,
		&mContext);

	return hr;
}

HRESULT DXResourceContext::checkMultiSampleSupport(UINT *sampleSize)
{
	const UINT DXGI_FORMAT_MAX = 116;
	const UINT MAX_SAMPLES_CHECK = 128;

	//https://msdn.microsoft.com/en-us/library/windows/desktop/dn458384.aspx
	for (UINT i = 1; i < DXGI_FORMAT_MAX; i++) {
		//MS says this is supposed to be safe_cast, but couldn't find how to make it work
		DXGI_FORMAT inFormat = static_cast<DXGI_FORMAT>(i);
		UINT formatSupport = 0;
		HRESULT hr = mDevice->CheckFormatSupport(inFormat, &formatSupport);

		if ((formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE) &&
			(formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET))
		{
			//NOT IMPLEMENTED YET
		}
	}

	for (UINT sampleCount = 1; sampleCount <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; sampleCount++)
	{
		UINT numQualityFlags = 0;
		HRESULT hr = mDevice->CheckMultisampleQualityLevels(support.format, sampleCount, &numQualityFlags);

		if (SUCCEEDED(hr) && numQualityFlags > 0)
		{
			(*sampleSize) = sampleCount;
			//mQualityFlags = numQualityFlags;
		}
	}

	return S_OK;
}

HRESULT DXResourceContext::checkDeviceSupport()
{
	//Check what sample size the device supports, and store it
	return checkMultiSampleSupport(&support.sampleSize);
}

HRESULT DXResourceContext::createSwapChain(HWND hWnd)
{

	// Create a description of how our swap
	// chain should work
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc.Width = dimensions.width;
	swapDesc.BufferDesc.Height = dimensions.height;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = support.format;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.Flags = 0;
	swapDesc.OutputWindow = hWnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = true;

	//Get the DXGI Device from the D3D device
	IDXGIAdapter* dxgiDevice = 0;
	DX::ThrowIfFailed(mDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));

	//Get the representation of the graphics adapter
	IDXGIAdapter* dxgiAdapter = 0;
	DX::ThrowIfFailed(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter)));

	//Retrieve the factory (that created the device) from the adapter
	IDXGIFactory* dxgiFactory = 0;
	DX::ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory)));

	//Create the Swap Chain
	DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(mDevice, &swapDesc, &mSwapChain));

	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	return S_OK;
}

void DXResourceContext::setViewport(const UINT &width, const UINT &height)
{
	dimensions.width = width;
	dimensions.height = height;

	// set up a viewport so we render into
	// to correct portion of the window
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)dimensions.width;
	viewport.Height = (float)dimensions.height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mContext->RSSetViewports(1, &viewport);

	// Resize the underlying swap chain buffers
	mSwapChain->ResizeBuffers(
		1,
		dimensions.width,
		dimensions.height,
		support.format,
		0);

}

HRESULT DXResourceContext::createRenderTargets()
{
	// 1. -----PRIMARY RENDER TARGET--------
	// First create the primary render target
	// All objects in the scene will first be drawn to this RT
	// This render target will configured with MSAA if it is supported

	D3D11_TEXTURE2D_DESC primaryBufferDesc = {};
	ZeroMemory(&primaryBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	primaryBufferDesc.Width = dimensions.width;
	primaryBufferDesc.Height = dimensions.height;
	primaryBufferDesc.MipLevels = 1;
	primaryBufferDesc.ArraySize = 1;
	primaryBufferDesc.Format = support.format;
	primaryBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	primaryBufferDesc.SampleDesc.Count = support.sampleSize;
	primaryBufferDesc.SampleDesc.Quality = support.qualityFlags;

	DX::ThrowIfFailed(
		mDevice->CreateTexture2D(
			&primaryBufferDesc,
			nullptr,
			&mPrimaryRT));

	//Describe a multi-sample render target view
	CD3D11_RENDER_TARGET_VIEW_DESC primaryRTVDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS);

	mDevice->CreateRenderTargetView(
		mPrimaryRT,
		&primaryRTVDesc,
		&mPrimaryRTV);

	// 2. -----POST PROCESS RENDER TARGETS--------
	// Next create a single-sample render target for post process render passes
	D3D11_TEXTURE2D_DESC textureDesc = {};
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = dimensions.width;
	textureDesc.Height = dimensions.height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	DX::ThrowIfFailed(mDevice->CreateTexture2D(&textureDesc, 0, &mPostProcessRT));
	DX::ThrowIfFailed(mDevice->CreateTexture2D(&textureDesc, 0, &mBloomMapRT));
	DX::ThrowIfFailed(mDevice->CreateTexture2D(&textureDesc, 0, &mTemporaryRT));

	//Create Post Process Render Target Views
	D3D11_RENDER_TARGET_VIEW_DESC postProcessRTVDesc = {};
	ZeroMemory(&postProcessRTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	postProcessRTVDesc.Format = textureDesc.Format;
	postProcessRTVDesc.Texture2D.MipSlice = 0;
	postProcessRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	DX::ThrowIfFailed(mDevice->CreateRenderTargetView(mPostProcessRT, &postProcessRTVDesc, &mPostProcessRTV));
	DX::ThrowIfFailed(mDevice->CreateRenderTargetView(mBloomMapRT, &postProcessRTVDesc, &mBloomMapRTV));
	DX::ThrowIfFailed(mDevice->CreateRenderTargetView(mTemporaryRT, &postProcessRTVDesc, &mTemporaryRTV));

	//Create Shader Resource Views
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = support.format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	DX::ThrowIfFailed(mDevice->CreateShaderResourceView(mPostProcessRT, &srvDesc, &mPostProcessSRV));
	DX::ThrowIfFailed(mDevice->CreateShaderResourceView(mBloomMapRT, &srvDesc, &mBloomMapSRV));
	DX::ThrowIfFailed(mDevice->CreateShaderResourceView(mTemporaryRT, &srvDesc, &mTemporarySRV));

	// 3. -----BACK BUFFER RENDER TARGET--------
	// Finally create the render target to the back buffer 
	// - post processes will render into this
	// Get a reference to the currrent back buffer render target
	mSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&mBackBufferRT);

	// Now that we have the texture, create a render target view
	// for the back buffer so we can render into it
	DX::ThrowIfFailed(mDevice->CreateRenderTargetView(mBackBufferRT, 0, &mBackBufferRTV));

	return S_OK;
}

HRESULT DXResourceContext::createDepthBuffer()
{
	// Set up the description of the texture to use for the depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = dimensions.width;
	depthStencilDesc.Height = dimensions.height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = support.sampleSize;
	depthStencilDesc.SampleDesc.Quality = support.qualityFlags;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth buffer and its view, then 
	// release our reference to the texture
	ID3D11Texture2D* depthBufferTexture;
	DX::ThrowIfFailed(mDevice->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture));
	DX::ThrowIfFailed(mDevice->CreateDepthStencilView(depthBufferTexture, 0, &mDepthStencilView));
	depthBufferTexture->Release();

	return S_OK;
}

void DXResourceContext::releaseSizeDependentResources()
{
	if (mDepthStencilView) { mDepthStencilView->Release(); }

	if (mPostProcessSRV) { mPostProcessSRV->Release(); }
	if (mBloomMapSRV) { mBloomMapSRV->Release(); }
	if (mTemporarySRV) { mTemporarySRV->Release(); }

	if (mPrimaryRTV) { mPrimaryRTV->Release(); }
	if (mTemporaryRTV) { mTemporaryRTV->Release(); }
	if (mBloomMapRTV) { mBloomMapRTV->Release(); }
	if (mPostProcessRTV) { mPostProcessRTV->Release(); }
	if (mBackBufferRTV) { mBackBufferRTV->Release(); }

	if (mPrimaryRT) { mPrimaryRT->Release(); }
	if (mTemporaryRT) { mTemporaryRT->Release(); }
	if (mBloomMapRT) { mBloomMapRT->Release(); }
	if (mPostProcessRT) { mPostProcessRT->Release(); }
	if (mBackBufferRT) { mBackBufferRT->Release(); }
}

void DXResourceContext::release()
{
	// Release all DirectX resources
	if (mDepthStencilView) { mDepthStencilView->Release(); }

	if (mPostProcessSRV) { mPostProcessSRV->Release(); }
	if (mBloomMapSRV) { mBloomMapSRV->Release(); }
	if (mTemporarySRV) { mTemporarySRV->Release(); }

	if (mPrimaryRTV) { mPrimaryRTV->Release(); }
	if (mTemporaryRTV) { mTemporaryRTV->Release(); }
	if (mBloomMapRTV) { mBloomMapRTV->Release(); }
	if (mPostProcessRTV) { mPostProcessRTV->Release(); }
	if (mBackBufferRTV) { mBackBufferRTV->Release(); }

	if (mPrimaryRT) { mPrimaryRT->Release(); }
	if (mTemporaryRT) { mTemporaryRT->Release(); }
	if (mBloomMapRT) { mBloomMapRT->Release(); }
	if (mPostProcessRT) { mPostProcessRT->Release(); }
	if (mBackBufferRT) { mBackBufferRT->Release(); }

	if (mSwapChain) { mSwapChain->Release(); }
	if (mContext) { mContext->Release(); }
	if (mDevice) { mDevice->Release(); }
}