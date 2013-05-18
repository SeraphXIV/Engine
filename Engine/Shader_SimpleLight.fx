// Shader simple + texture

// Global
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;
float4 ambientColor;
float4 diffuseColor;
float3 lightDirection;
float3 cameraPosition;
float4 specularColor;
float specularPower;

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
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

// Vertex Shader
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;
    
	// 4eme parametre du vecteur de position, necessaire
    input.position.w = 1.0f;

	// Calcul la position du vertex d'apres les matrices du monde, de la cam et de la projection
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Sauvegarde l'input de texture pour le pixel shader
    output.tex = input.tex;

	// Calcul le vecteur normal d'apres la matrice du monde uniquement
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
	// Normalise le vecteur normal
    output.normal = normalize(output.normal);

	// Calcul la position du vertex
    worldPosition = mul(input.position, worldMatrix);

	// Determine la direction de la vue selon la position de la camera et celle du vertex
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
	// Normalise le tout
    output.viewDirection = normalize(output.viewDirection);
    
    return output;
}

// Pixel Shader
float4 LightPixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;
	float3 lightDir;
    float lightIntensity;
    float4 color;
	float3 reflection;
    float4 specular;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Met la valeur par defaut de chaque pixel a la lumiere ambiante
    color = ambientColor;

	// Initialise la couleur speculaire
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Inverse la direction de la lumiere pour les calculs
    lightDir = -lightDirection;

	// Calcul la quantite de lumiere pour ce pixel et la sature
    lightIntensity = saturate(dot(input.normal, lightDir));

	// Verifie si la lumiere diffuse (Normal dot Light) est superieur a zero, si c'est le cas on l'ajoute au pixel.
	// Si ce n'est pas le cas, il ne faut pas ajouter la lumiere diffuse, car elle peux etre negative et pourrir la lumiere ambiante
	if(lightIntensity > 0.0f)
    {
		// Determine la lumiere diffuse finale
        color += (diffuseColor * lightIntensity);

		// Sature la couleur ambiante et diffuse
        color = saturate(color);

		// Calcul le vecteur de reflection selon l'intensite de la lumiere, la normale et la direction de la lumiere
        reflection = normalize(2 * lightIntensity * input.normal - lightDir); 

		// Determine la quantite de lumiere speculaire selon le vecteur de reflection, la direction de la vue et la puissance speculaire
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }

	// Multiplie la texture par la lumiere
    color = color * textureColor;

	// Ajoute la lumiere speculaire et sature la couleur au cas ou elle depasse 1
    color = saturate(color + specular);

    return color;
}

// Technique
technique10 LightTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, LightVertexShader()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, LightPixelShader()));
    }
}