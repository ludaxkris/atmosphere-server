#include "GraphicsManager.h"
#include <fstream>


D3DVERTEXELEMENT9 elements[] =
	{
		{ 0, sizeof( float ) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, sizeof( float ) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, sizeof( float ) * 6, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, sizeof( float ) * 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, sizeof( float ) * 15, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};

//constructor
GraphicsManager::GraphicsManager()
{	
}

GraphicsManager::~GraphicsManager()
{
	std::map<char *, MyEffect *, strCmp>::iterator eIter		
		= Effects.begin();
	std::map<char *, ID3DXMesh *, strCmp>::iterator mIter			
		= Meshes.begin();				
	std::map<char *, IDirect3DTexture9 *, strCmp>::iterator	t2Iter	
		= Textures.begin();
	std::map<char *, IDirect3DCubeTexture9 *, strCmp>::iterator t3Iter
		= cubeTextures.begin();

	//delete My Effects
	for (; eIter != Effects.end(); eIter++)
	{		
		SAFE_RELEASE(eIter->second->effect);
		delete eIter->second;
	}	

	//delete meshes
	for (; mIter != Meshes.end(); mIter++)
	{				
		SAFE_RELEASE(mIter->second);
	}	

	//delete meshes
	for (; t2Iter != Textures.end(); t2Iter++)
	{				
		 SAFE_RELEASE(t2Iter->second);
	}	

	//delete meshes
	for (; t3Iter != cubeTextures.end(); t3Iter++)
	{				
		 SAFE_RELEASE(t3Iter->second);
	}	

}

void GraphicsManager::initialize(LPDIRECT3DDEVICE9 pd3dDevice)
{

	//hold on to DX device
	this->pd3dDevice = pd3dDevice;
	
	//hold on to world shader
	//hold on to ship shader
	//hold on to skybox shader
	//hold on to particle shaders, if Yuriy wants it

	//initalize lights
	initLight();
}

//initialize lights
void GraphicsManager::initLight(void)
{
	lights[0].SetDiffuse (D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f));
	lights[0].SetSpecular(D3DXCOLOR (0.0f, 0.0f, 0.5f, 1.0f));
	lights[0].SetPosition(D3DXVECTOR3 (0.0f, 1.0f, 0.0f));  
	lights[0].Update();


	lights[1].SetDiffuse (D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f));
	lights[1].SetSpecular(D3DXCOLOR (0.0f, 0.0f, 0.5f, 1.0f));
	lights[1].SetPosition(D3DXVECTOR3 (-20.0f, 0.0f, 0.0f));  
	lights[1].Update();

	lights[2].SetDiffuse (D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f));
	lights[2].SetSpecular(D3DXCOLOR (0.0f, 0.0f, 0.5f, 1.0f));
	lights[2].SetPosition(D3DXVECTOR3 (0.0f, 20.0f, 0.0f));  
	lights[2].Update();

	
	lights[3].SetDiffuse (D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f));
	lights[3].SetSpecular(D3DXCOLOR (0.0f, 0.0f, 0.5f, 1.0f));
	lights[3].SetPosition(D3DXVECTOR3 (0.0f, 20.0f, 20.0f));  
	lights[3].Update();
}

//recomile the saders
void GraphicsManager::compileShaders(LPDIRECT3DDEVICE9 pd3dDevice)
{
	std::map<char *, MyEffect *, strCmp>::iterator iter; 
	
	for (iter = Effects.begin(); iter != Effects.end(); iter++)
	{
		printf("Recompiling Effecs %s\n", iter->first);
		iter->second->CompileEffect(pd3dDevice, iter->first);
	}	
}


//Check if effect is loaded if so, just recompile, load effect and insert to map
void GraphicsManager::insertEffect(char * filename, LPDIRECT3DDEVICE9 g_pd3dDevice)
{		
	MyEffect * effect;
	
	//check if its inserted if so just recompile
	if(Effects.find(filename) != Effects.end())
	{	
		effect = Effects[filename];
		effect->CompileEffect(g_pd3dDevice, filename);
		
		return;
	}	
	
	effect = new MyEffect();
	effect->CompileEffect(g_pd3dDevice, filename);
	
	//insert to map
	Effects[filename] = effect; 
}

//load texture and insert to map
void GraphicsManager::insertCubeTexture(char * filename, LPDIRECT3DDEVICE9 pd3dDevice)
{
	
	IDirect3DCubeTexture9 * texture;
	WCHAR temp[MAX_PATH];
	WCHAR str[MAX_PATH];

	// convert char to wide char set
	MultiByteToWideChar(CP_ACP, 0, filename, (int)strlen (filename),  temp, MAX_PATH);

	//null string
	temp[strlen (filename)] = '\0';

	//search for the media file
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, temp);

	//load texture
	if(D3DXCreateCubeTextureFromFileEx( pd3dDevice, str, 512, 1, 0, D3DFMT_X8R8G8B8, 
                                               D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, 
											   D3DX_FILTER_LINEAR, 0, NULL, NULL, 
											   &texture ) < 0)
	//if(D3DXCreateCubeTextureFromFile( pd3dDevice, str, &texture ) < 0)
	{
		printf("Could not load cube texture: %ws\n", str);
	}
	
	//insert to map
	cubeTextures[filename] = texture; 
}



//load texture and insert to map
void GraphicsManager::insertTexture(char * filename, LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	
	IDirect3DTexture9 * texture;
	WCHAR temp[MAX_PATH];
	WCHAR str[MAX_PATH];

	// convert char to wide char set
	MultiByteToWideChar(CP_ACP, 0, filename, (int)strlen (filename),  temp, MAX_PATH);

	//null string
	temp[strlen (filename)] = '\0';

	//search for the media file
    DXUTFindDXSDKMediaFileCch( str, MAX_PATH, temp);

	//load texture
	if(D3DXCreateTextureFromFileEx( g_pd3dDevice, str, D3DX_DEFAULT, D3DX_DEFAULT, 
                                       D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
                                       D3DX_DEFAULT, D3DX_DEFAULT, 0, 
                                       NULL, NULL, &texture ) < 0)
	{
		printf("Could not load texture: %ws\n", str);
	}
	
	//insert to map
	Textures[filename] = texture; 
}


//load mesh and insert to map
void GraphicsManager::insertMesh (char * filename, LPDIRECT3DDEVICE9 pd3dDevice)
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	WCHAR str[MAX_PATH];
	WCHAR temp[MAX_PATH];


	ID3DXMesh *				pMesh;					// Mesh 
	LPD3DXBUFFER			bufEffectInstances;
	LPD3DXBUFFER			bufAdjacency;			// buf adjacency
	DWORD					g_dwNumMaterials;		// number of Materials


	// convert char to wide char set
	MultiByteToWideChar(CP_ACP, 0, filename, (int)strlen (filename),  temp, MAX_PATH);
	temp[strlen (filename)] = '\0';
	DXUTFindDXSDKMediaFileCch( str, MAX_PATH, temp);
	
	//load mesh
	if(D3DXLoadMeshFromX( str,
                                D3DXMESH_SYSTEMMEM,
                                pd3dDevice,
                                &bufAdjacency,
                                &pD3DXMtrlBuffer,
                                &bufEffectInstances,
                                &g_dwNumMaterials,
                                &pMesh
                                ) < 0)
	{
		printf("Error loading Mesh: %ws\n", str);
		//exit(0);
	}	
	
	ID3DXMesh* tempMesh;

	//generate adjacency 
    pMesh->GenerateAdjacency(1e-6f, (DWORD*)bufAdjacency->GetBufferPointer());

	//load declarator
	//*	
	if ( FAILED( pMesh->CloneMesh( D3DXMESH_MANAGED, elements, pd3dDevice, &tempMesh ) ) )
	{
		printf("CloneMesh failed\n");
		return;
	}
    
	// compute normals
	D3DXComputeTangentFrame( tempMesh,  NULL);
	D3DXComputeNormals( tempMesh,  (DWORD*)bufAdjacency->GetBufferPointer());
	SAFE_RELEASE( pMesh );
    pMesh = tempMesh;
	
	//*/

	/*
	//make normals
	if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        ID3DXMesh* pTempMesh;

        pMesh->CloneMeshFVF( pMesh->GetOptions(), pMesh->GetFVF() | D3DFVF_NORMAL, pd3dDevice, &pTempMesh ) ;
        D3DXComputeNormals( pTempMesh, NULL );
		//D3DXComputeTangent(pTempMesh, NULL);
        SAFE_RELEASE( pMesh );
        pMesh = pTempMesh;
    }
	*/


	pMesh->GenerateAdjacency(1e-6f, (DWORD*)bufAdjacency->GetBufferPointer());

	//optimize the mesh for performance
	pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE,
                    (DWORD*)bufAdjacency->GetBufferPointer(), NULL, NULL, NULL );


    // Done with the material buffer
    pD3DXMtrlBuffer->Release();

	//insert to map
	Meshes[filename] = pMesh; 
}


//Looks up filename in map, if it exists return address, else insert and return.
MyEffect * GraphicsManager::getEffect(char * filename)
{
	if(Effects.find(filename) != Effects.end())
		return Effects[filename];

	insertEffect(filename, pd3dDevice);

	return getEffect(filename);	
}



//Looks up filename in map, if it exists return address, else insert and return.
IDirect3DTexture9 * GraphicsManager::getTexture(char * filename)
{

	if(Textures.find(filename) != Textures.end())
		return Textures[filename];

	insertTexture(filename, pd3dDevice);

	return getTexture(filename);
}


//Looks up filename in map, if it exists return address, else insert and return.
IDirect3DCubeTexture9 * GraphicsManager::getCubeTexture(char * filename)
{

	if(cubeTextures.find(filename) != cubeTextures.end())
		return cubeTextures[filename];
	
	insertCubeTexture(filename, pd3dDevice);

	return getCubeTexture(filename);

}


//Looks up filename in map, if it exists return address, else returns null.
ID3DXMesh * GraphicsManager::getMesh(char * filename)
{

	if(Meshes.find(filename) != Meshes.end())
		return Meshes[filename];

	insertMesh(filename, pd3dDevice);

	return getMesh(filename);
}

void GraphicsManager::ReleaseResource()
{
	//Effects.get
}	
