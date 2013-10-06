//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

//global lights
float3 sunLightDir[4];              // Light's direction in world space
float4 sunLightDiffuse[4];          // Light's diffuse color
float4 sunLightAmbient[4];          // Light's ambient color
int numSunLights;					// Number of Sun lights


float4 planetMatAmbient;			// Material's ambient color
float4 planetMatDiffuse;			// Material's diffuse color

//textures
texture colorMap;					// Color map for the terrain
texture normalMap;					// Color map for the terrain
texture grass;					    // Color texture for mesh
texture stone;					    // Color texture for mesh
texture sand;					    // Color texture for mesh
texture snow;					    // Color texture for mesh
texture cubeTexture;				// light cube map

float4x4 g_mWorld;                  // World matrix for object
float4x4 g_mWorldViewProjection;    // World * View * Projection matrix

//other lightsources
float3	lightPositions[4];			// Position of ship
float4	lightDiffuse[4];			// Ship light color
int		numLights;					// number of ships

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler snowTexture = sampler_state
{
    Texture = <snow>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler grassTexture = sampler_state
{
    Texture = <grass>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler stoneTexture = sampler_state
{
    Texture = <stone>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler sandTexture = sampler_state
{
    Texture = <sand>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};

sampler normalMapTex = sampler_state
{
    Texture = <normalMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};

sampler colorMapTex = sampler_state
{
    Texture = <colorMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};


sampler CubeTexture = sampler_state
{ 
    Texture = (cubeTexture);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = Clamp;
    ADDRESSV = Clamp;
};


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct INPUT
{ 
    float4 position   : POSITION0;
    float3 normal	  : NORMAL;
    float2 texUV      : TEXCOORD0;
    float3 tangent    : TANGENT;
    float3 binormal	  : BINORMAL;
};

struct VS_OUTPUT
{
    float4 Position		: POSITION;   // vertex position 
    float2 TextureUV	: TEXCOORD0;  // vertex texture coords 
    float3 Normal		: TEXCOORD1;  // vertex normal
    float3 PPosition	: TEXCOORD2;  // Pixel coordinates
    float3 vLight		: TEXCOORD3;  // tangent
};

//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting
//--------------------------------------------------------------------------------------
/*VS_OUTPUT RenderSceneVS( float4 vPos : POSITION, 
                         float3 vNormal : NORMAL,
                         float2 vTexCoord0 : TEXCOORD0,                        
                         uniform bool bTexture)
{
    VS_OUTPUT Output;
   
    // Transform the position from object space to homogeneous projection space
    Output.Position		= mul(vPos , g_mWorldViewProjection);
       
    // Just copy the texture coordinate through
    if( bTexture ) 
        Output.TextureUV = vTexCoord0; 
    else
        Output.TextureUV = 0; 
    
    //Pass values to the pixel shader
    Output.Normal		= mul(vNormal , g_mWorld);
	Output.PPosition	= mul(vPos , g_mWorld).xyz;
    //Output.tangent		= mul(vNormal , g_mWorld);
    
    return Output;    
}*/

VS_OUTPUT RenderSceneVS2( in INPUT In)
{
    VS_OUTPUT Output;
   
    // Transform the position from object space to homogeneous projection space
    Output.Position		= mul(In.position , g_mWorldViewProjection);
       
    // Just copy the texture coordinate through
    Output.TextureUV = In.texUV; 
    
    float3x3 TBNMatrix = float3x3(In.tangent, In.binormal , In.normal); 
    Output.vLight = mul(TBNMatrix, sunLightDir[0]);

    
    //Pass values to the pixel shader
    Output.Normal		= mul(In.normal , g_mWorld);
	Output.PPosition	= mul(In.position , g_mWorld).xyz;
    
    
    
    return Output;    
}


//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by modulating the texture's
//       color with diffuse material color
//--------------------------------------------------------------------------------------
PS_OUTPUT RenderScenePS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
        
	float3 LDiffuse;	
	float3 normal		= normalize(In.Normal);						// normalize the interpolated normals;  
	float3 vNormalMap	= 2.0f * tex2D(normalMapTex, normalize(In.TextureUV)).rgb - 1.0f;      
    
    float3 diffuse		= float3(0,0,0);
    float3 temp			= float3(0,0,0);  	
	float3 ambient		= texCUBE( CubeTexture, normalize(In.PPosition));
  
	
	
	//sunlight
    for(int i = 0; i < 3; i++)
		temp += saturate(dot(normal,sunLightDir[i])*.4);
	
			
		
    diffuse += temp;
    diffuse = max (0, diffuse);
    
    //moveable lights
	for(int j = 0; j < 1; j++)
    {
		float3 lightDir		= lightPositions[j] - In.PPosition;	
		float len			= length(lightDir); 	
		lightDir			= lightDir/len; 			
		diffuse				+=(1-temp) * lightDiffuse[j] * (dot(normal, lightDir) * (  (5/len) + (200/(len*len*len)) ));
		
	}
    
    diffuse = min(diffuse, 1);
   
    float  height = length(In.PPosition.xyz);	
    
    //add ambient
    Output.RGBColor.rgb = ambient * .29;  
	
    //color map
	Output.RGBColor.rgb += tex1D(colorMapTex, (height-18)/3.60) * diffuse;
    
    Output.RGBColor.a	= 1;    

    return Output;
}

PS_OUTPUT RenderScenePS2( VS_OUTPUT In) 
{ 
    PS_OUTPUT Output;
        
	float3 LDiffuse;	
	float3 normal		= normalize(In.Normal);						// normalize the interpolated normals;  
	float3 vNormalMap	= 2.0f * tex2D(normalMapTex, normalize(In.TextureUV)).rgb - 1.0f;      
    
    float3 diffuse		= float3(0,0,0);
    float3 temp			= float3(0,0,0);  	
	float3 ambient		= texCUBE( CubeTexture, normalize(In.PPosition));
  
	
	
	//sunlight
    //for(int i = 0; i < 3; i++)		
		//temp += saturate(dot(normal,sunLightDir[0])*.8);
		temp =  saturate(dot(vNormalMap, sunLightDir[0]))* .1;
			
		
    diffuse += temp;
    diffuse = max (0, diffuse);   
    
    diffuse = min(diffuse, 1);
   
    float  height = length(In.PPosition.xyz);	
    
    //add ambient
    //Output.RGBColor.rgb = ambient * .29;  
	
    //color map
	Output.RGBColor.rgb = tex1D(colorMapTex, (height-18)/3.60) * diffuse;
	//Output.RGBColor.rgb = tex1D(normalMapTex, In.TextureUV) * diffuse;
    
    Output.RGBColor.a	= 1;    

    return Output;
}

//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by modulating the texture's
//       color with diffuse material color
//--------------------------------------------------------------------------------------
PS_OUTPUT PSTexture( VS_OUTPUT In,
                         uniform bool bTexture ) 
{ 
    PS_OUTPUT Output;
        
	float3 LDiffuse;
	float3 normal		= normalize(In.Normal);						// normalize the interpolated normals;    
    float3 diffuse		= float3(0,0,0);
    float3 temp			= float3(0,0,0);  	
	float3 ambient		= texCUBE( CubeTexture, normalize(In.PPosition));
  
	//sunlight
    for(int i = 0; i < 4; i++)
			temp += max(0, dot(normal,sunLightDir[i])*.2);
			
		
    diffuse += temp;
    diffuse = max (0, diffuse);
  
    Output.RGBColor.rgb = float3(0,0,0);  
    
    //texture
    float  height = length(In.PPosition.xyz);
    
	if(height < 17)
		Output.RGBColor.rgb += (tex2D(sandTexture, In.TextureUV) * diffuse);
	else if(height < 19) // stone and stand		
		Output.RGBColor.rgb += (lerp(tex2D(sandTexture, In.TextureUV),tex2D(stoneTexture, In.TextureUV),.4) * diffuse);
		
    else if(height < 21) // grass
		Output.RGBColor.rgb += (tex2D(grassTexture, In.TextureUV) * diffuse);
    else if(height < 30) //snow
		Output.RGBColor.rgb += (tex2D(snowTexture, In.TextureUV) * diffuse);
    
    Output.RGBColor.a	= 1;
    

    return Output;
}
 

//--------------------------------------------------------------------------------------
// Renders scene to render target
//--------------------------------------------------------------------------------------
technique RenderSceneWithTexture1Light
{
    pass P0
    {          
        VertexShader = compile vs_2_a RenderSceneVS2();
        PixelShader  = compile ps_2_a RenderScenePS(); // trivial pixel shader (could use FF instead if desired)
    }
}