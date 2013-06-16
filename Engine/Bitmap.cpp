// Include du header associe
#include "Bitmap.h"
#include "SPH_Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CBitmap::CBitmap()
{
	// Mise a zero des variables et pointeurs
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}
// On ne s'en servira pas
CBitmap::CBitmap(const CBitmap& osef){}
CBitmap::~CBitmap(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CBitmap::Init(ID3D10Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CBitmap::Init()");
	#endif

	bool bResult;

	// Enregistre la taille de l'ecran
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Enregistre la taille de l'image
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// Init la position precedente a -1, pour signifier que c'est la première fois.
	// La position de l'image a la frame precendente servira pour rafraichir, ou pas, l'image dans le shader
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Ensuite on cree le buffer vers le shader et on charge la texture

	// Init les buffers
	bResult = InitBuffers(device);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::Init() : Failed to create buffers.");
		#endif
		return false;}

	// Load the texture for this model.
	bResult = LoadTexture(device, textureFilename);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::Init() : Failed to load texture.");
		#endif
		return false;}

	return true;
}

bool CBitmap::InitBuffers(ID3D10Device* device)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CBitmap::Initbuffers()");
	#endif

	VertexType* vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Carre de 2 triangles -> 6 vertex
	m_vertexCount = 6;
	m_indexCount = m_vertexCount;

	// Cree le vertex array
	vertices = new VertexType[m_vertexCount];
	if(!vertices){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::InitBuffers() : Failed to create vertex array.");
		#endif
		return false;}

	// Cree l'index array
	indices = new unsigned long[m_indexCount];
	if(!indices){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::InitBuffers() : Failed to create index array.");
		#endif
		return false;}

	// Init le vertex array a 0
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Charge l'index array
	for(i=0; i<m_indexCount; i++){
		indices[i] = i;}

	// Gros changement par rapport aux models 3D, ce vertex buffer est dynamique pour qu'on puisse le modifier a chaque frame sans le detruire et le re-creer de 0.
	
	// Description du vertex buffer dynamique
	vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	// Donne un pointeur vers les donnees comme ressource
	vertexData.pSysMem = vertices;

	// Cree le vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::InitBuffers() : Failed to create vertex buffer.");
		#endif
		return false;}

	// L'index buffer reste le meme qu'en 3D, statique, les indices ne changent pas

	// Description de l'index buffer
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Donne un pointeur vers les donnees comme ressource
	indexData.pSysMem = indices;

	// Cree l'index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::InitBuffers() : Failed to create index buffer.");
		#endif
		return false;}

	// Release ce dont on a plus besoin
	delete [] vertices;
	vertices = 0;
	delete [] indices;
	indices = 0;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CBitmap::Shutdown()
{
	// Appel la fonction pour release la texture
	ReleaseTexture();
	// Appel la fonction pour release les buffers
	ShutdownBuffers();

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CBitmap::Shutdown()");
	#endif
}

void CBitmap::ShutdownBuffers()
{
	// Release les buffers
	if(m_indexBuffer){
		m_indexBuffer->Release();
		m_indexBuffer = 0;}
	if(m_vertexBuffer){
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;}

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CBitmap::ShutdownBuffers()");
	#endif

	return;
}

void CBitmap::ReleaseTexture()
{
	// Release l'objet de texture
	if(m_Texture){
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;}

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CBitmap::ReleaseTexture()");
	#endif

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Affichage																	//
//////////////////////////////////////////////////////////////////////////////////////////
bool CBitmap::Render(ID3D10Device* device, int positionX, int positionY)
{
	bool bResult;

	// Si la position de l'image 2D est differente on re-construit le vertex buffer
	bResult = UpdateBuffers(positionX, positionY);
	if(!bResult){return false;}
	// Met les buffers (vertex et index) dans la pipeline graphique pour les preparer a l'affichage
	RenderBuffers(device);

	return true;
}

// fonction appelee a chaque frame pour mettre a jour le dynamic vertex buffer si besoin est
bool CBitmap::UpdateBuffers(int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	void* verticesPtr;
	HRESULT result;

	// Check si la position de l'image a change, si oui on MaJ le buffer, si non on arrete la
	if((positionX == m_previousPosX) && (positionY == m_previousPosY)){return true;}

	// Puisque la position a change, on sauve les nouvelles coordonnees
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// Calcul les quatres cotes de l'image
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
	right = left + (float)m_bitmapWidth;
	top = (float)(m_screenHeight / 2) - (float)positionY;
	bottom = top - (float)m_bitmapHeight;

	// Cree un vertex array temporaire avec les nouvelles donnees
	vertices = new VertexType[m_vertexCount];
	if(!vertices){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::UpdateBuffers() : Failed to create vertex array.");
		#endif
		return false;}

	// Charge les donnees
	// Premier triangle
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	// Copie le contenu du nouveau vertex array dans le vertex buffer en utilisant les fonctions Map et memcpy

	// Init un pointeur vers le vertex buffer a null
	verticesPtr = 0;

	// Lock le vertex buffer
	result = m_vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&verticesPtr);
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::UpdateBuffers() : Failed to lock vertex buffer.");
		#endif
		return false;}

	// Copie les donnees dans le vertex buffer
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock le vertex buffer
	m_vertexBuffer->Unmap();

	// Release
	delete [] vertices;
	vertices = 0;

	return true;
}

// Mets en place les buffers sur le gpu pour pouvoir etre affiches
void CBitmap::RenderBuffers(ID3D10Device* device)
{
	unsigned int stride;
	unsigned int offset;

	// Vertex buffer stride et offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Met le vertex buffer a l'etat "actif" dans l'input assembler pour qu'il puisse etre dessine
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	// Idem avec l'index buffer
	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Type de primitive qu'on veux en rendu, des triangles
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool CBitmap::LoadTexture(ID3D10Device* device, WCHAR* filename)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CBitmap::LoadTexture()");
	#endif

	bool bResult;

	// Create the texture object.
	m_Texture = new CTexture;
	if(!m_Texture){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::LoadTexture() : Failed to create texture.");
		#endif
		return false;}

	// Initialize the texture object.
	bResult = m_Texture->Init(device, filename);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CBitmap::LoadTexture() : Failed to init texture.");
		#endif
		return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Accesseur																	//
//////////////////////////////////////////////////////////////////////////////////////////
int CBitmap::GetIndexCount(){return m_indexCount;}
ID3D10ShaderResourceView* CBitmap::GetTexture(){return m_Texture->GetTexture();}