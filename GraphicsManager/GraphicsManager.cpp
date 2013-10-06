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
	std::map<char *, ID3DXMesh *, strCmp>::iterator mIter			
		= Meshes.begin();				

	//delete meshes
	for (; mIter != Meshes.end(); mIter++)
	{				
		if(mIter->second)
			mIter->second->Release();
	}	
}

//load mesh and insert to map
void GraphicsManager::insertMesh (char * filename)
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
	
	//load mesh
	if(D3DXLoadMeshFromX( temp,
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
	if( pMesh )
		pMesh->Release();

    pMesh = tempMesh;

	pMesh->GenerateAdjacency(1e-6f, (DWORD*)bufAdjacency->GetBufferPointer());

	//optimize the mesh for performance
	pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE,
                    (DWORD*)bufAdjacency->GetBufferPointer(), NULL, NULL, NULL );


    // Done with the material buffer
    pD3DXMtrlBuffer->Release();

	//insert to map
	Meshes[filename] = pMesh; 
}


//Looks up filename in map, if it exists return address, else returns null.
ID3DXMesh * GraphicsManager::getMesh(char * filename)
{

	if(Meshes.find(filename) != Meshes.end())
		return Meshes[filename];

	insertMesh(filename);

	return getMesh(filename);
}

void GraphicsManager::ReleaseResource()
{
	//Effects.get
}	
