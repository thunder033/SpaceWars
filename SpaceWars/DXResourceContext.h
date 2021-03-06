#pragma once


#include <d3d11.h>
#include <string>
#include "DirectXHelper.h"
#include "SimpleShader.h"
#include <CommonStates.h>
#include <map>
#include <SpriteFont.h>
#include <SpriteBatch.h>

using namespace DirectX;

// We can include the correct library files here
// instead of in Visual Studio settings if we want
#pragma comment(lib, "d3d11.lib")

struct DeviceSupport
{
	//MSAA properties
	UINT sampleSize = 1;
	UINT qualityFlags = 0;

	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D_FEATURE_LEVEL dxFeatureLevel = D3D_FEATURE_LEVEL_11_0;
};

struct WindowDemensions
{
	UINT width = 0;
	UINT height = 0;
};

class DXResourceContext
{
	friend class DXCore;
	friend class Game;
	friend class Renderer;
	
	// DirectX related objects and variables
	IDXGISwapChain*			mSwapChain;

	// Render Targets
	ID3D11Texture2D* mPrimaryRT; //Primay multi-sample output

	ID3D11Texture2D* mPostProcessRT; //Holds the flattend primary output
	ID3D11Texture2D* mBloomMapRT; //Holds the extract bloom map
	ID3D11Texture2D* mTemporaryRT; //Holds intermedate results

	ID3D11Texture2D* mBackBufferRT; //Reference to the swap-chain back buffer

	// Render Target Views
	ID3D11RenderTargetView* mPrimaryRTV;
	ID3D11RenderTargetView* mPostProcessRTV;
	ID3D11RenderTargetView* mBloomMapRTV;
	ID3D11RenderTargetView* mTemporaryRTV;
	ID3D11RenderTargetView* mBackBufferRTV;

	// Shader Resource Views
	ID3D11ShaderResourceView* mPostProcessSRV;
	ID3D11ShaderResourceView* mBloomMapSRV;
	ID3D11ShaderResourceView* mTemporarySRV;

	ID3D11DepthStencilView* mDepthStencilView;

	// Check what multi-sampling support the device has
	// This currently only checks the largest sample size supported
	// There is more params like format and quality flag
	HRESULT checkMultiSampleSupport(UINT *sampleSize);
public:
	ID3D11Device*			mDevice;
	ID3D11DeviceContext*	mContext;

	DeviceSupport support;
	WindowDemensions dimensions;

	DXResourceContext();
	~DXResourceContext();

	// Creates the device handle and rendering context
	HRESULT createDevice();
	//Checks the capabilities of the graphics device
	HRESULT checkDeviceSupport();
	//Create the swap chain
	HRESULT createSwapChain(HWND hWnd);
	//Create render targtes for various render passes
	HRESULT createRenderTargets();
	//Create the primary depth buffer
	HRESULT createDepthBuffer();

	//Set the dimensions of the viewport
	void setViewport(const UINT &width, const UINT &height);

	//Release resources that need to be re-created on re-size
	void releaseSizeDependentResources();

	void release();
};

