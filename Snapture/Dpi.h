#ifndef __DPI__
#define __DPI__

#include <Windows.h>
using namespace System;
namespace com {
	namespace HellstormGames {
		namespace Imaging {

			public ref class Dpi 
			{
			public:
				property UINT X;
				property UINT Y;
				property float ScaleFactor;
				Dpi() {

				}
				~Dpi() {

				}
			private:
			protected:

			};
		}
	}
}

#endif