/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;
float4 pixelColor;


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

/////////////////////
// BLENDING STATES //
// Permet d'effacer le rectangle noir derriere chaque caractere //
/////////////////////
BlendState AlphaBlendingState
{
    BlendEnable[0] = TRUE;
    DestBlend = INV_SRC_ALPHA;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FontVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FontPixelShader(PixelInputType input) : SV_Target
{
    float4 color;
	
	// Sample le pixel texture a ce point
    color = shaderTexture.Sample(SampleType, input.tex);
	
	// Si le pixel est noir, c'est le fond, on lui met l'alpha a 0 pour le faire disparaitre
	// Pour le coup on test que le rouge, plus rapide
    if(color.r == 0.0f){color.a = 0.0f;}
	
	// Sinon, c'est un pixel du caractere
    else{
        color.a = 1.0f;
        color = color * pixelColor;
    }
    return color;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
// BlendState par defaut, il sera actif jusqu'a ce qu'un autre shader le desactive. Le switch d'etat coute en ressource
// Absolument regrouper les dessins qui en ont besoin ou pas pour eviter les switch successifs a chaque frame
technique10 FontTechnique
{
    pass pass0
    {
        SetBlendState(AlphaBlendingState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, FontVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, FontPixelShader()));
        SetGeometryShader(NULL);
    }
}