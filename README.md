## Project Discontinued as of Feb. 3, 2025. 
Snapster replaces Snapture.

# Snapture
Screen capturing is a .NET Framework CLI Library that uses GDI+\DirectX. 

# Known Issues 
DirectX Desktop Duplication eats up memory

# Usage

``` C#
//-- global declarations
Snapture snapture;
```

``` C#
public void MainWindow()
{
     snapture = new Snapture();
     snapture.onFrameCaptured += Snapture_onFrameCaptured;
     snapture.SetBitmapResolution((int)snapture.MonitorInfo.Monitors[0].Dpi.X); //-- Uses DPI from Monitor
     if(ScreenShot) {
       snapture.Start(FrameCapturingMethod.GDI);
     }
}
private void Snapture_onFrameCaptured(object sender, FrameCapturedEventArgs e)
{
   if(e.ScreenCapturedBitmap != null)
   {
      //-- Do whatever you want with the captured bitmap
   }
}
```

``` C#
private void BeginDesktopCapture() {
    snapture.CaptureDesktop();
}
```

``` C#
private void CaptureRegion(int x, int y, int width, int height) {
   snapture.CaptureRegion(x,y,width,height); 
}
```

``` C#
private void MainWindow_Closing()
{
   if(snapture != null)
   {
     snapture.Release();
   }
}
```
