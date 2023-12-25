#pragma once
#include "DX.h"

namespace com {
	namespace HellstormGames {
		namespace Imaging 
		{
			namespace DirectX
			{
				public ref class DXCapture 
				{
				public:
					DXCapture()
					{

					}
					bool Initialize() {
						dxCapturerUnamanged = new DXCapturerUnmanaged();
						return dxCapturerUnamanged->Initialize();
					}

					bool CaptureDesktop() 
					{
						return dxCapturerUnamanged->CaptureDesktop();
					}

					System::Drawing::Bitmap^ GetCapturedBitmap() 
					{
						HGDIOBJ gdiObj = dxCapturerUnamanged->GetCapturedBitmap();
						System::Drawing::Bitmap ^ bitmap = System::Drawing::Image::FromHbitmap((System::IntPtr)gdiObj);
						DeleteObject(gdiObj);
						return bitmap;
					}
					bool CaptureRegion(int x, int y, int width, int height) 
					{
						return dxCapturerUnamanged->CaptureRegion(x, y, width, height);
					}
					void Release() {
						dxCapturerUnamanged->Release();
						delete dxCapturerUnamanged;
					}
				private:
					DXCapturerUnmanaged* dxCapturerUnamanged;
				};
			}
		}
	}
}
