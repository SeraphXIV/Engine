// Include du header associe
#include "FontShader.h"
#include "SPH_Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CFontShader::CFontShader()
{
	// Mise a zero des variables et pointeurs
	m_effect = 0;
	m_technique = 0;
	m_layout = 0;
	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;
	m_texturePtr = 0;
	m_pixelColorPtr = 0;
}
// On ne s'en servira pas
CFontShader::CFontShader(const CFontShader& other){}
CFontShader::~CFontShader(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CFontShader::Init(ID3D10Device* device, HWND hwnd)
{
	bool bResult;

	// Initialize the shader that will be used to draw the triangle.
	bResult = InitShader(device, hwnd, L"../Engine/Font_1.fx");
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFontShader::Init() : Failed to load shader.");
		#endif
		return false;}

	return true;
}

bool CFontShader::InitShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D10_PASS_DESC passDesc;

	// Met a zero le message d'erreur
	errorMessage = 0;

	// Charge le shader
	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
					    device, NULL, NULL, &m_effect, &errorMessage, NULL);
	if(FAILED(result)){
		// Si la compilation a echouee, elle devrait renvoyer un message d'erreur
		if(errorMessage){
			OutputShaderErrorMessage(errorMessage, hwnd, filename);}
		// Si la compilation a echouee au point de meme pas renvoyer d'erreur
		else{
			MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);}
		return false;
	}

	// Attrape le pointeur vers la technique a l'interieur du shader
	m_technique = m_effect->GetTechniqueByName("FontTechnique");
	if(!m_technique){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFontShader::InitShader() : Failed to get technique.");
		#endif
		return false;}

	// Met en place le format des donnees a envoyer au shader
	// Ca doit etre compatible avec la structure VertexTyoe de la classe ET du shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Calcul le nombre d'element
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Attrape la description de la premiere pass de la technique
	m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Cree l'input layout
	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, 
					   &m_layout);
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFontShader::InitShader() : Failed to create input layout.");
		#endif
		return false;}

	// Attrape les pointeurs des trois matrices du shader
	m_worldMatrixPtr = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
	m_viewMatrixPtr = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
	m_projectionMatrixPtr = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();
	// Attrape le pointeur de la ressource de texture du shader
	m_texturePtr = m_effect->GetVariableByName("shaderTexture")->AsShaderResource();
	// Attrape le pointeur de la couleur du shader
	m_pixelColorPtr = m_effect->GetVariableByName("pixelColor")->AsVector();

	return true;
}

void CFontShader::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
					  ID3D10ShaderResourceView* texture, D3DXVECTOR4 pixelColor)
{
	// Envoi les matrices, texture et couleur au shader
	m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);
	m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);
	m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);
	m_texturePtr->SetResource(texture);
	m_pixelColorPtr->SetFloatVector((float*)&pixelColor);

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CFontShader::Shutdown()
{
	// Appel la fonction de release du shader
	ShutdownShader();
	
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CFontShader::Shutdown()");
	#endif
}

void CFontShader::ShutdownShader()
{
	// Release les ponteurs et variables
	m_pixelColorPtr = 0;
	m_texturePtr = 0;
	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;

	if(m_layout){
		m_layout->Release();
		m_layout = 0;}

	m_technique = 0;

	if(m_effect){
		m_effect->Release();
		m_effect = 0;}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Affichage																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CFontShader::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
			     D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView* texture, D3DXVECTOR4 pixelColor)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);

	return;
}

void CFontShader::RenderShader(ID3D10Device* device, int indexCount)
{
	D3D10_TECHNIQUE_DESC techniqueDesc;
	unsigned int i;
	
	// Set the input layout.
	device->IASetInputLayout(m_layout);

	// Get the description structure of the technique from inside the shader so it can be used for rendering.
	m_technique->GetDesc(&techniqueDesc);

	// Go through each pass in the technique (should be just one currently) and renders the triangles.
	for(i=0; i<techniqueDesc.Passes; ++i)
	{
		m_technique->GetPassByIndex(i)->Apply(0);
		device->DrawIndexed(indexCount, 0, 0);
	}

	return;
}

void CFontShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// Pointeur vers le message d'erreur
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	// Taille du message
	bufferSize = errorMessage->GetBufferSize();

	// Ouvre un fichier pour y ecrire le message d'erreur
	fout.open("shader-error.txt");

	// Ecrit le message
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Ferme le fichier
	fout.close();

	// Release du pointeur
	errorMessage->Release();
	errorMessage = 0;

	// Pop un message
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}