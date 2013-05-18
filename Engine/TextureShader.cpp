// Include du header associe
#include "TextureShader.h"
#include "SPH_Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CTextureShader::CTextureShader()
{
	// Mise a zero des variables et pointeurs
	m_effect = 0;
	m_technique = 0;
	m_layout = 0;

	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;

	m_texturePtr = 0;
}
// On ne s'en servira pas
CTextureShader::CTextureShader(const CTextureShader& other){}
CTextureShader::~CTextureShader(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CTextureShader::Init(ID3D10Device* device, HWND hwnd)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CTextureShader::Init()"); 
	#endif

	bool bResult; // bool de test

	// Initialise le shader
	bResult = InitShader(device, hwnd, L"../Engine/Shader_SimpleTex.fx");
	// Test de succes
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CTextureShader::Init() : Failed to init Shader");
		#endif
		return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CTextureShader::Shutdown()
{
	// Shutdown the shader effect.
	ShutdownShader();

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CTextureShader::Shutdown()"); 
	#endif

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Affichage																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CTextureShader::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView* texture)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, texture);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CTextureShader::InitShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CTextureShader::InitShader()"); 
	#endif
	HRESULT result; // bool de test special Windows
	ID3D10Blob* errorMessage; // BLOB LOLILOOOL BLOB
	D3D10_INPUT_ELEMENT_DESC polygonLayout[2];  // format de transfert entre le programme et le shader
	unsigned int numElements; // Nombre d'element dans le layout
	D3D10_PASS_DESC passDesc; // Pass du shader


	// Mise a zero du message d'erreur
	errorMessage = 0;

	// Compile et charge le shader
	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
					    device, NULL, NULL, &m_effect, &errorMessage, NULL);
	if(FAILED(result)){
		// Si la compilation a echouee, elle devrait renvoyee un message d'erreur
		if(errorMessage){
			OutputShaderErrorMessage(errorMessage, hwnd, filename);}
		// Si la compilation a echouee au point de meme pas renvoyer d'erreur
		else{
			#ifdef _SPHDEBUG_H 
				SPHDebug::Msg("\t /!\\ CTextureShader::InitShader() : Shader is missing.");
			#endif
		}
		return false;
	}

	// Acces a la technique du shader
	m_technique = m_effect->GetTechniqueByName("TextureTechnique");
	// Test de succes (membre utilise, lui seul peut detecter la validite de la technique)
	if(!m_technique->IsValid()){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CTextureShader::InitShader() : Technique invalid.");
		#endif
		return false;}

	// Mise en place du format des donness a envoyer au shader
	// Ce format doit correspondre a la structure VertexType dans la classe CModel et dans le shader
	polygonLayout[0].SemanticName = "POSITION"; // Nom donne a cette donnee dans le shader
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // Format a 3 float, X Y Z
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD"; // Nom donne a cette donnee dans le shader
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT; // Format a 2 float, U V
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT; // Aligne automatiquement les donnees, peut etre fait manuellement
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Calcul le nombre d'element a envoyer. Taille totale / Taille d'un element
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Description de la premiere pass de la technique du shader
	m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Cree la passerelle formatee entre le programme et le shader
	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_layout);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CTextureShader::InitShader() : Unable to create Input Layout.");
		#endif
		return false;}

	// Enregistre 3 pointeurs vers les matrices declarees dans le shader
	m_worldMatrixPtr = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
	m_viewMatrixPtr = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
	m_projectionMatrixPtr = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();
	// Enregistre le pointeur vers la ressource texture declaree dans le shader
	m_texturePtr = m_effect->GetVariableByName("shaderTexture")->AsShaderResource();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CTextureShader::ShutdownShader()
{
	// Remet a zero les pointeurs
	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;
	m_texturePtr = 0;

	// Release du pointeur de la passerelle formatee
	if(m_layout){
		m_layout->Release();
		m_layout = 0;}

	// Release du pointeur de la technique
	m_technique = 0;

	// Release du pointeur vers le shader
	if(m_effect){
		m_effect->Release();
		m_effect = 0;}

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CTextureShader::ShutdownShader()"); 
	#endif

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Debug																		//
//////////////////////////////////////////////////////////////////////////////////////////
void CTextureShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("Error compiling shader.  Check shader-error.txt for message."); 
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Parametres																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CTextureShader::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D10ShaderResourceView* texture)
{
	// Envoi les matrices au shader
	m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);
	m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);
	m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);
	// Bind la texture
	m_texturePtr->SetResource(texture);
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Affichage																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CTextureShader::RenderShader(ID3D10Device* device, int indexCount)
{
	D3D10_TECHNIQUE_DESC techniqueDesc;
	unsigned int i;
	
	// Choisi le format
	device->IASetInputLayout(m_layout);

	// Description de la structure de la technique, utilisee pour afficher
	m_technique->GetDesc(&techniqueDesc);

	// Circule a travers chaque passe (une seule pour le moment) et dessine les donnees envoyees
	for(i=0; i<techniqueDesc.Passes; ++i)
	{
		// Utilisation de cette pass
		m_technique->GetPassByIndex(i)->Apply(0);
		// Dessin indexe
		device->DrawIndexed(indexCount, 0, 0);
	}

	return;
}