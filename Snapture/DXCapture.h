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
						return System::Drawing::Image::FromHbitmap((System::IntPtr)gdiObj);
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
