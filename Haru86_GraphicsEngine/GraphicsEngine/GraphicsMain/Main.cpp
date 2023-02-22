#include <Windows.h>
#include "GraphicsEngine/GraphicsMain/GraphicsMain.h"

int WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	GraphicsMain::Create();
	
	if (!GraphicsMain::GetInstance()->CreateApp())return 0;
	if (!GraphicsMain::GetInstance()->Initialize())return 0;
	if (!GraphicsMain::GetInstance()->RunLoop())return 0;

	GraphicsMain::Destroy();

	return 0;
}