#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgiformat.h>
#pragma comment(lib, "d3d11.lib")

#define SAFE_RELEASE(x) if(x) { x->Release(); x=NULL; }

#include <Windows.h>
#include <atlbase.h>
#include <memory>
#include "NativeBitmap.h"

using namespace ATL;



// Driver types supported
D3D_DRIVER_TYPE gDriverTypes[] =
{
	D3D_DRIVER_TYPE_HARDWARE
};
UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

// Feature levels supported
D3D_FEATURE_LEVEL gFeatureLevels[] =
{
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_1
};

UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);

class DXCapturerUnmanaged {
public:
	DXCapturerUnmanaged() {

	}
	~DXCapturerUnmanaged() {

	}
	bool Initialize() 
	{
		auto result = S_OK;
		result = CoInitialize(NULL);

		auto desktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
		if (!desktop)
			return false;

		auto isDesktopAttached = SetThreadDesktop(desktop) != 0;

		CloseDesktop(desktop);


		D3D_FEATURE_LEVEL lFeatureLevel;
		HRESULT hr(E_FAIL);
		bool bInit = false;
		UINT CreationFlags = 0;
#if defined(_DEBUG) 
			CreationFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Create device
		for (UINT DriverTypeIndex = 0; DriverTypeIndex < gNumDriverTypes; ++DriverTypeIndex)
		{
			hr = D3D11CreateDevice(
				0,
				gDriverTypes[DriverTypeIndex],
				0,
				0,
				gFeatureLevels,
				gNumFeatureLevels,
				D3D11_SDK_VERSION,
				&pDevice,
				&lFeatureLevel,
				&pDeviceContext);

			if (SUCCEEDED(hr))
			{
				// Device creation success, no need to loop anymore
				bInit = true;
				break;
			}

			SAFE_RELEASE(pDevice);
			SAFE_RELEASE(pDeviceContext);
			bInit = false;
		}

		if (pDevice == NULL)
			return false;

		IDXGIDevice* pDXGIDevice = NULL;
		hr = pDevice->QueryInterface(IID_PPV_ARGS(&pDXGIDevice));
		if (FAILED(hr)) {
			return false;
		}
		IDXGIAdapter* pDXGIAdapter = NULL;
		hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDXGIAdapter));
		if (FAILED(hr)) 
		{
			SAFE_RELEASE(pDXGIDevice);
			return false;
		}
		SAFE_RELEASE(pDXGIDevice);
		UINT output = 0;

		IDXGIOutput* pDXGIOutput = NULL;
		hr = pDXGIAdapter->EnumOutputs(output, &pDXGIOutput);
		if (FAILED(hr)) 
		{
			SAFE_RELEASE(pDXGIAdapter);
			return false;
		}

		SAFE_RELEASE(pDXGIAdapter);

		IDXGIOutput1* pDXGIOutput1 = NULL;
		hr = pDXGIOutput->QueryInterface(IID_PPV_ARGS(&pDXGIOutput1));
		if (FAILED(hr)) {
			SAFE_RELEASE(pDXGIOutput);
			return false;
		}

		SAFE_RELEASE(pDXGIOutput);

		hr = pDXGIOutput1->DuplicateOutput(pDevice, &pDesktopDuplication);
		if (FAILED(hr)) {
			SAFE_RELEASE(pDXGIOutput1);
			return false;
		}

		SAFE_RELEASE(pDXGIOutput1);


		return true;
	}

	bool CaptureDesktop()
	{
		auto hr = S_OK;
		if (!pDesktopDuplication)
			return false;

		if (hasFrameLocked)
		{
			hasFrameLocked = false;
			pDesktopDuplication->ReleaseFrame();
		}

		CComPtr<IDXGIResource> pDeskRes = nullptr;
		DXGI_OUTDUPL_FRAME_INFO frameInfo;
		hr = pDesktopDuplication->AcquireNextFrame(0, &frameInfo, &pDeskRes);
		if (FAILED(hr))
		{
			if ((hr != DXGI_ERROR_ACCESS_LOST) && (hr != DXGI_ERROR_WAIT_TIMEOUT))
			{
				OutputDebugString(L"DX::CaptureDesktop -> Failed to acquire frame.\n");
				return false;
			}
			else if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
				OutputDebugString(L"DX::CaptureDesktop -> DXGI WAIT TIMEOUT.\n");
				return false;
			}
		}

		hasFrameLocked = true;

		CComPtr<ID3D11Texture2D> gpuTex = nullptr;
		hr = pDeskRes->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&gpuTex);
		
		if (FAILED(hr)) {
			// not expected
			return false;
		}

		CComPtr<ID3D11Texture2D> cpuTex = nullptr;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

		gpuTex->GetDesc(&desc);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.MiscFlags = 0; // D3D11_RESOURCE_MISC_GDI_COMPATIBLE ?
		
		hr = pDevice->CreateTexture2D(&desc, nullptr, &cpuTex);
		if (SUCCEEDED(hr)) 
		{
			pDeviceContext->CopyResource(cpuTex, gpuTex);
		}
		else {
			// not expected
			return false;
		}

		D3D11_MAPPED_SUBRESOURCE sr;
		ZeroMemory(&sr, sizeof(D3D11_MAPPED_SUBRESOURCE));

		hr = pDeviceContext->Map(cpuTex, 0, D3D11_MAP_READ, 0, &sr);
		if (SUCCEEDED(hr)) 
		{
			if (latestBitmap.width != desc.Width || latestBitmap.height != desc.Height) 
			{
				latestBitmap.width = desc.Width;
				latestBitmap.height = desc.Height;
				latestBitmap.Buffer.clear();
				latestBitmap.Buffer.resize(desc.Width * desc.Height * 4);
			}
			for (int y = 0; y < (int)desc.Height; y++)
				memcpy(latestBitmap.Buffer.data() + y * desc.Width * 4, (uint8_t*)sr.pData + sr.RowPitch * y, desc.Width * 4);

			pDeviceContext->Unmap(cpuTex, 0);
		}
		else 
		{
			return false;
		}

		CapturedBitmap = D3D11_CreateHBITMAP(cpuTex);
		
		return true;
	}

	HBITMAP D3D11_CreateHBITMAP(_In_ ID3D11Texture2D* src) {

		// Copy from CPU access texture to bitmap buffer
		
		D3D11_TEXTURE2D_DESC desc = { 0 };
		src->GetDesc(&desc);

		auto width = desc.Width;
		auto height = desc.Height;
		auto stride = width * height * 4;

		D3D11_MAPPED_SUBRESOURCE resource;
		UINT subresource = D3D11CalcSubresource(0, 0, 0);
		pDeviceContext->Map(src, subresource, D3D11_MAP_READ, 0, &resource);
		
		BYTE* pData = reinterpret_cast<BYTE*>(resource.pData);
		
		HBITMAP hBitmapTexture = CreateCompatibleBitmap(GetDC(NULL), width, height);
		SetBitmapBits(hBitmapTexture, stride, pData);

		pDeviceContext->Unmap(src, NULL);
		
		pData = NULL;
		resource = { 0 };

		return hBitmapTexture;
	}

	void Release() 
	{
		CoUninitialize();

		CapturedBitmap = NULL;

		SAFE_RELEASE(pDesktopDuplication);
		SAFE_RELEASE(pDeviceContext);
		SAFE_RELEASE(pDevice);
	}
	
	const HBITMAP GetCapturedBitmap() 
	{
		return CapturedBitmap;
	}
private:
	ID3D11Device* pDevice = 0;
	ID3D11DeviceContext* pDeviceContext = 0;
	IDXGIOutputDuplication* pDesktopDuplication = 0;
	DXGI_OUTDUPL_DESC pOutputDuplDesc;
	bool hasFrameLocked;
	NativeBitmap latestBitmap;
	HBITMAP CapturedBitmap;

protected:

};