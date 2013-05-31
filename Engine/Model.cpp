// Include du header associe
#include "Model.h"
#include "SPH_Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CModel::CModel()
{
	// Mise a zero des variables et pointeurs
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_texture = 0;
	m_model = 0;
}
// On ne s'en servira pas
CModel::CModel(const CModel& osef){}
CModel::~CModel(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CModel::Init(ID3D10Device* device,  char* modelFilename, WCHAR* textureFilename)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CModel::Init()");
	#endif

	bool bResult; // bool de test

	// Load in the model data.
	bResult = LoadModel(modelFilename);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::Init() : Failed to load model.");
		#endif
		return false;}

	// Initialise les vertex et index buffer qui contiennent les donnees du triangle
	bResult = InitBuffers(device);
	// Test de succes
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::Init() : Failed to init buffers.");
		#endif
		return false;}

	// Charge la texture de ce model
	bResult = LoadTexture(device, textureFilename);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::Init() : Failed to load texture.");
		#endif
		return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CModel::Shutdown()
{
	// Appel la fonction pour release la texture
	ReleaseTexture();
	// Appel la fonction pour release les buffers
	ShutdownBuffers();
	// Release les donnees du model
	ReleaseModel();

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CModel::Shutdown()");
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Affichage																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CModel::Render(ID3D10Device* device)
{
	// Appel la fonction qui envoi les donnes au shader
	RenderBuffers(device);

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Accesseur																	//
//////////////////////////////////////////////////////////////////////////////////////////
int CModel::GetIndexCount()
{
	// Renvoi la taille de l'index buffer, le shader va en avoir besoin
	return m_indexCount;
}
ID3D10ShaderResourceView* CModel::GetTexture()
{
	// Renvoi la ressource texture
	return m_texture->GetTexture();
}
//////////////////////////////////////////////////////////////////////////////////////////
//			Chargement																	//
//////////////////////////////////////////////////////////////////////////////////////////
bool CModel::InitBuffers(ID3D10Device* device)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CModel::InitBuffers()");
	#endif

	// Declaration des variables
	VertexType* vertices; // tableau de structure des vertices
	unsigned long* indices; // tableau des indices
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc; // Description, ce qui differencie un vertex d'un index buffer
	D3D10_SUBRESOURCE_DATA vertexData, indexData; // Pointeur de stockage des donnees a envoyer au shader
	HRESULT result; // Bool de test special Windows
	int i; // Variable d'incrementation

	// Creation du tableau de vertices
	vertices = new VertexType[m_vertexCount];
	// Test de succes
	if(!vertices){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::InitBuffers() : Failed to allocate vertices.");
		#endif
		return false;}

	// Creation du tableau d'indices
	indices = new unsigned long[m_indexCount];
	// Test de succes
	if(!indices){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::InitBuffers() : Failed to allocate indices.");
		#endif
		return false;}

	// Rentre les donnees dans les tableau
	for(i=0; i<m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// OLD : couleur
	/*vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);*/

	// OLD : Texture + normale
	/*vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);
	vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].texture = D3DXVECTOR2(0.5f, 0.0f);
	vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.*/

	// Creation des vertex et index buffer, de la meme facon : reglage de la description et chargement des donnees dans la subresource

	// Description du vertex buffer
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount; // Memoire a allouer : taille d'un vertex * nombre de vertex
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER; // Type de buffer, ici Vertex
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Envoi a la subresource du pointeur vers les donnees
	vertexData.pSysMem = vertices;

	// Creation du vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	// Test de succes
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::InitBuffers() : Failed to create vertex buffer.");
		#endif
		return false;}

	// Description de l'index buffer
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount; // Memoire a allouer : taille d'un index * nombre de vertex
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER; // Type de buffer, ici Index
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Envoi a la subresource du pointeur vers les donnees
	indexData.pSysMem = indices;

	// Creation de l'index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	// Test de succes
	if(FAILED(result)){
		
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::InitBuffers() : Failed to create index buffer.");
		#endif
		return false;}

	// Suppression des tableaux temporaire
	delete [] vertices;
	vertices = 0;
	delete [] indices;
	indices = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Chargement Texture															//
//////////////////////////////////////////////////////////////////////////////////////////
bool CModel::LoadTexture(ID3D10Device* device, WCHAR* filename)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CModel::LoadTexture()");
	#endif

	bool bResult; // Bool de test

	// Create the texture object.
	// Creation de l'objet qui gere la texture du model
	m_texture = new CTexture;
	// Test de succes
	if(!m_texture){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::LoadTexture() : Failed to create new Texture.");
		#endif
		return false;}

	// Initialisation
	bResult = m_texture->Init(device, filename);
	// Test de succes
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::LoadTexture() : Failed to init Texture.");
		#endif
		return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Chargement Texture															//
//////////////////////////////////////////////////////////////////////////////////////////
bool CModel::LoadModel(char* filename)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CModel::LoadModel()");
	#endif

	ifstream fin; // Flux d'entree
	char input; // Stock le caractere lu actuellement
	int i; // Variable d'incrementation

	// Ouvre le fichier externe du model
	fin.open(filename);
	
	// Si on ne peux pas l'ouvrir, on arrete
	if(fin.fail()){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::LoadModel() : Failed to open model file.");
		#endif
		return false;}

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("\t CModel : File opened.");
	#endif

	// Avance jusqu'au nombre de vertex
	fin.get(input);
	/*#ifdef _SPHDEBUG_H 
		SPHDebug::Msg(input);
	#endif*/
	while(input != ':')
	{
		fin.get(input);
		/*#ifdef _SPHDEBUG_H 
			SPHDebug::Msg(input);
		#endif*/
	}

	// Lit et enregistre le nombre de vertex
	fin >> m_vertexCount;
	/*#ifdef _SPHDEBUG_H 
		SPHDebug::Msg(m_vertexCount);
	#endif*/

	// Le nombre d'indices est le meme que le nombre de vertices
	m_indexCount = m_vertexCount;

	// Alloue le tableau du donnees du model selon le nombre de vertex
	m_model = new ModelType[m_vertexCount];
	if(!m_model){
		/*#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CModel::LoadModel() : Failed to init Model array.");
		#endif*/
		return false;}

	// Avance jusqu'aux donnees
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
		/*#ifdef _SPHDEBUG_H 
			SPHDebug::Msg(input);
		#endif*/
	}
	fin.get(input);
	/*#ifdef _SPHDEBUG_H 
		SPHDebug::Msg(input);
	#endif*/
	fin.get(input);
	/*#ifdef _SPHDEBUG_H 
		SPHDebug::Msg(input);
	#endif*/

	// Read in the vertex data.
	for(i=0; i<m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		/*#ifdef _SPHDEBUG_H 
			SPHDebug::Msg(m_model[i].x);
			SPHDebug::Msg(m_model[i].y);
			SPHDebug::Msg(m_model[i].z);
		#endif*/
		fin >> m_model[i].tu >> m_model[i].tv;
		/*#ifdef _SPHDEBUG_H 
			SPHDebug::Msg(m_model[i].tu);
			SPHDebug::Msg(m_model[i].tv);
		#endif*/
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
		/*#ifdef _SPHDEBUG_H 
			SPHDebug::Msg(m_model[i].nx);
			SPHDebug::Msg(m_model[i].ny);
			SPHDebug::Msg(m_model[i].nz);
		#endif*/
	}

	// Close the model file.
	fin.close();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CModel::ReleaseModel()
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CModel::ReleaseModel()");
	#endif

	// Release des classes et remise a zero des pointeurs
	if(m_model){
		delete [] m_model;
		m_model = 0;}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CModel::ShutdownBuffers()
{
	// Destructeur perso, plus propre que celui de base

	// Release des classes et remise a zero des pointeurs
	if(m_indexBuffer){
		m_indexBuffer->Release();
		m_indexBuffer = 0;}

	if(m_vertexBuffer){
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;}

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CModel::ShutdownBuffers()");
	#endif

	return;
}
void CModel::ReleaseTexture()
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CModel::ReleaseTexture()");
	#endif

	// Release de la texture
	if(m_texture){
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Affichage																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CModel::RenderBuffers(ID3D10Device* device)
{
	// Envoi les buffers au shader

	// Met les stride et offset du vertex buffer
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Active le vertex buffer dans l'input assembler pour qu'il puisse etre affiche
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	// Active l'index buffer dans l'input assembler pour qu'il puisse etre affiche
	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Type de forme primitive qui doit etre utilise avec les donnees des buffers (ligne, triangle, etc)
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}