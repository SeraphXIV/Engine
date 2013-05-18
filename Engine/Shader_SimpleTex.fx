// Shader simple + texture

// Global
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;

// Sample State (options des textures) 
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// Typedef
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

// Vertex Shader
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    
	// 4eme parametre du vecteur de position, necessaire
    input.position.w = 1.0f;

	// Calcul la position du vertex d'apres les matrices du monde, de la cam et de la projection
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Sauvegarde l'input de texture pour le pixel shader
    output.tex = input.tex;
    
    return output;
}

// Pixel Shader
float4 TexturePixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    return textureColor;
}

// Technique
technique10 TextureTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, TextureVertexShader()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TexturePixelShader()));
    }
}