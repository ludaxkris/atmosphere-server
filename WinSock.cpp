#include "stdafx.h"
#include "server.h"
#include "./GraphicsManager/Common/dxstdafx.h"


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#include <iostream>
using namespace std;

#define DEFAULT_PORT "2303"

//FOR GRAPHICS
////////////////////////////////////////////////////
#define IDI_DIRECTX_ICON		107

IDirect3DDevice9*	pd3dDevice		= NULL;
LPDIRECT3D9			g_pD3D			= NULL;		//directx version
HWND				g_hWnd			= NULL;		
//GraphicsManager *	graphicsManager	= NULL;


void ReleaseResource();
LRESULT CALLBACK WindowProc( HWND   hWnd, UINT   msg, WPARAM wParam, LPARAM lParam );
void init();




void* testing (void* test, int* dataSize)
{
	(*dataSize) = 16;
	char* clientData = (char*)malloc(16);
	clientData[0] = 'F'; 
	clientData[1] = 'R';
	clientData[2] = 'O';
	clientData[3] = 'M';
	clientData[4] = ' ';
	clientData[5] = 'T';
	clientData[6] = 'H';
	clientData[7] = 'E';
	clientData[8] = ' ';
	clientData[9] = 'S';
	clientData[10] = 'E';
	clientData[11] = 'R';
	clientData[12] = 'V';
	clientData[13] = 'E';
	clientData[14] = 'R';
	clientData[15] = 0;

	return ((void*)clientData);
}




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow) 
{

	//Nick's Graphics
	///////////////////////////////////////////////////
	if(hInstance || hPrevInstance || lpCmdLine){};
	WNDCLASSEX winClass;
	MSG        uMsg;

    memset(&uMsg,0,sizeof(uMsg));

	winClass.lpszClassName = L"MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW ;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
    winClass.hIconSm	   = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;
	
	if( !RegisterClassEx(&winClass) )
		return E_FAIL;

	g_hWnd = CreateWindowEx( NULL,L"MY_WINDOWS_CLASS", 
                             L"SERVER",
						     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
							 0, 0, 80, 60, 
							 NULL, NULL, hInstance, NULL );

	if( g_hWnd == NULL )
		return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );
    UpdateWindow( g_hWnd );

	//init directx
	init();	
   
	//graphics manager
	//graphicsManager = new GraphicsManager();

	//Arjay's Server
	///////////////////////////////////////////////////
	server Server;
	Server.openAndAcceptNewClients(DEFAULT_PORT);


	//clean up resources
	ReleaseResource();
    UnregisterClass(L"MY_WINDOWS_CLASS", winClass.hInstance );
	return (int)uMsg.wParam;


	//stop accepting new clients....
		// No longer need server socket

	/*	char* recivedData;
		// Receive until the peer shuts down the connection
		do {
			recivedData = (char*)Server.reciveDataFromClient();
			if( (int)recivedData > 0)
				printf("MSG:: %s\n",recivedData);
			delete recivedData;

		} while ( (int)recivedData > 0);

		Server.closeConnection();
	*/
}


void init( void )
{
	HRESULT hr;
    D3DDISPLAYMODE d3ddm;
	D3DCAPS9 d3dCaps;
	DWORD dwBehaviorFlags = 0;
	g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	if( g_pD3D == NULL )
	{
		fprintf(stderr, "TO DO: Respond to failure of Direct3DCreate8\n");
		return;
	}
   
    if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
	{
		// TO DO: Respond to failure of GetAdapterDisplayMode
		fprintf(stderr, " TO DO: Respond to failure of GetAdapterDisplayMode\n");
		return;
	}

	if( FAILED( hr = g_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
												d3ddm.Format, D3DUSAGE_DEPTHSTENCIL,
												D3DRTYPE_SURFACE, D3DFMT_D16 ) ) )
	{
		if( hr == D3DERR_NOTAVAILABLE )
			// POTENTIAL PROBLEM: We need at least a 16-bit z-buffer!
			return;
	}

	if( FAILED( g_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, 
		                               D3DDEVTYPE_HAL, &d3dCaps ) ) )
	{
		// TO DO: Respond to failure of GetDeviceCaps
		return;
	}	

	if( d3dCaps.VertexProcessingCaps != 0 )
		dwBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		dwBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	
	// Everything checks out - create a simple, windowed device.
	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

    d3dpp.BackBufferFormat       = d3ddm.Format;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed               = TRUE;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                                      dwBehaviorFlags, &d3dpp, &pd3dDevice ) ) )
	{
		fprintf(stderr, "TO DO: Respond to failure of CreateDevice\n");
		return;
	}			
}

LRESULT CALLBACK WindowProc( HWND   hWnd, UINT   msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
			}    
			break;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
		}
		
        case WM_DESTROY:
		{
            PostQuitMessage(0);
		}
        break;

		default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}
		


	return 0;
}

void ReleaseResource()
{
    int num = 0;
   
	//release effects 
	//if(graphicsManager != NULL )
	//	delete graphicsManager;
	
	if( (num =pd3dDevice->Release()) > 0 )  
    {
		cerr<<"DX9 Resources not released: "<<num<<"\nPress Enter"<<endl;
		getchar();
    }
	
}
