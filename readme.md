
An on-going small game engine!

# How to run this program
## Need to set the windows kit folder
https://www.braynzarsoft.net/viewtutorial/q16390-setting-up-directx-12-for-visual-studio-2015

In the project property
First add Windows Kits->10->Include->{version}->shared and Windows Kits->10->Include->{version}->um to the Include Directories
For example:
	C:\Program Files (x86)\Windows Kits\10\Include\10.0.10240.0\shared
	C:\Program Files (x86)\Windows Kits\10\Include\10.0.10240.0\um

Second add Windows Kits->10->Lib->{version}->um->x64 to the Library Directories
For example:
	C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10240.0\um\x64

Third, in the linker->input->additional dependencies, add
	d3d12.lib
	dxgi.lib
	d3dcompiler.lib
