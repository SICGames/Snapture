#pragma once
#include <Windows.h>
#include <Presentation.h>

using namespace System::Windows::Media::Imaging;
using namespace System::Runtime::CompilerServices;
using namespace System::IO;

namespace Drawing = System::Drawing;

namespace com {
	namespace HellstormGames 
	{
		namespace Imaging 
		{
			namespace Extensions 
			{
	
				[ExtensionAttribute]
				public ref class BitmapExtensions abstract sealed {
				public:
					[ExtensionAttribute]
					static Drawing::Bitmap^ ToBitmap(BitmapSource^ bitmapsource) {
						
						if (bitmapsource == nullptr)
							return nullptr;

						try {
							auto ms = gcnew MemoryStream();
							auto encoder = gcnew PngBitmapEncoder();
							encoder->Frames->Add(BitmapFrame::Create(bitmapsource));
							encoder->Save(ms);
							auto bmp = reinterpret_cast<Drawing::Bitmap^>(Drawing::Bitmap::FromStream(ms));

							encoder = nullptr;
							ms->Close();
							ms = nullptr;
							return bmp;
						}
						catch (System::Exception^ ex) 
						{
							throw gcnew System::Exception(ex->Message);
						}
						return nullptr;
					}
					[ExtensionAttribute]
					static BitmapSource^ ToBitmapSource(Drawing::Bitmap^ bitmap) 
					{
						try {
							auto ms = gcnew MemoryStream();
							bitmap->Save(ms, System::Drawing::Imaging::ImageFormat::Png);
							
							auto bitmapsource = gcnew BitmapImage();
							bitmapsource->BeginInit();
							ms->Position = 0;
							bitmapsource->StreamSource = ms;
							bitmapsource->CacheOption = BitmapCacheOption::OnLoad;
							//-- oddly enough, EndInit throwing exception.

							bitmapsource->EndInit();
							ms->Close();
							ms = nullptr;

							return bitmapsource;
						}
						catch (System::Exception^ ex) 
						{
							throw gcnew System::Exception(ex->Message);
						}

						return nullptr;
					}
				private:

				};
				
			}
		}
	}
}