#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

// Direct3D includes
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include <map>

class GraphicsManager
{

	struct strCmp 
	{
		bool operator()( const char* s1, const char* s2 ) const 
		{
			return strcmp( s1, s2 ) < 0;
		}
	};


	public:	
		//Variables
		///////////////////////////////////////////////////////////////////////////////////	
		std::map<char *, ID3DXMesh *, strCmp> Meshes;				// meshs	
		IDirect3DDevice9 * pd3dDevice;							//3d device		
	

		//Functions
		///////////////////////////////////////////////////////////////////////////////////
		GraphicsManager		(void); 
		~GraphicsManager	();

		void insertMesh		(char * filename);		

		//accessor methods		
		ID3DXMesh *				getMesh			(char * filename);		
	
		//release resources
		void ReleaseResource ();

};


#endif 
