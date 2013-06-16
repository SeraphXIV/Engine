// Include du header associe
#include "Text.h"
#include "SPH_Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CText::CText()
{
	// Mise a zero des variables et pointeurs
	m_Font = 0;
	m_FontShader = 0;

	m_sentence1 = 0;
	m_sentence2 = 0;
}
// On ne s'en servira pas
CText::CText(const CText& osef){}
CText::~CText(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CText::Init(ID3D10Device* device, HWND hwnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix)
{
	bool bResult;

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CText::Init()");
	#endif

	// Sauve la resolution et la matrice de vue basique
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_baseViewMatrix = baseViewMatrix;

	// Cree l'objet de Font
	m_Font = new CFont;
	if(!m_Font){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::Init() : Failed to create font.");
		#endif
		return false;}

	// Init l'objet de Font
	bResult = m_Font->Init(device, "../Engine/Font_1.txt", L"../Engine/Font_1.dds");
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFont::Init() : Failed to init font.");
		#endif
		return false;}

	// Cree l'objet Font Shader
	m_FontShader = new CFontShader;
	if(!m_FontShader){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::Init() : Failed to create font shader.");
		#endif
		return false;}

	// Initialize the font shader object.
	bResult = m_FontShader->Init(device, hwnd);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::Init() : Failed to init font shader.");
		#endif
		return false;}

	// Init la premiere phrase
	bResult = InitSentence(&m_sentence1, 16, device);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::Init() : Failed to init first sentence.");
		#endif
		return false;}

	// Now update the sentence vertex buffer with the new string information.
	bResult = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::Init() : Failed to update first sentence.");
		#endif
		return false;}

	// Init la deuxieme phrase
	bResult = InitSentence(&m_sentence2, 16, device);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::Init() : Failed to init second sentence.");
		#endif
		return false;}

	// Now update the sentence vertex buffer with the new string information.
	bResult = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::Init() : Failed to update second sentence.");
		#endif
		return false;}

	return true;
}

bool CText::InitSentence(SentenceType** sentence, int maxLength, ID3D10Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Cree une structure de phrase
	*sentence = new SentenceType;
	if(!*sentence){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::InitSentence() : Failed to create sentence structure.");
		#endif
		return false;}

	// Init le buffer a null
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Met en place la taille de la phrase
	(*sentence)->maxLength = maxLength;
	// Met en place le nombre de vertices dans le vertex array
	(*sentence)->vertexCount = 6 * maxLength;
	// Idem pour l'index buffer
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Cree le vertex array
	vertices = new VertexType[(*sentence)->vertexCount];
	if(!vertices){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::InitSentence() : Failed to create vertex array.");
		#endif
		return false;}

	// Cree l'index array
	indices = new unsigned long[(*sentence)->indexCount];
	if(!indices){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::InitSentence() : Failed to create index array.");
		#endif
		return false;}

	// Init le vertex array a zero
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));
	// Init l'index array
	for(i=0; i<(*sentence)->indexCount; i++){indices[i] = i;}

	// Met en place la description du vertex buffer sur dynamique
	vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	// Envoi le pointeur des donnees a la subressource
	vertexData.pSysMem = vertices;
	// Cree le vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::InitSentence() : Failed to create vertex buffer.");
		#endif
		return false;}

	// Met en place la description de l'index buffer sur static
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Envoi le pointeur des donnees a la subressource
	indexData.pSysMem = indices;
	// Cree l'index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::InitSentence() : Failed to create index buffer.");
		#endif
		return false;}

	// Release e qui n'est plus necessaire
	delete [] vertices;
	vertices = 0;
	delete [] indices;
	indices = 0;

	return true;
}

bool CText::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	void* verticesPtr;
	HRESULT result;

	// Sauve la couleur de la phrase
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// Nombre de lettres dans la phrase
	numLetters = (int)strlen(text);

	// Check pour un possible overflow
	if(numLetters > sentence->maxLength){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::UpdateSentence() : Overflow.");
		#endif
		return false;}

	// Cree le vertex array
	vertices = new VertexType[sentence->vertexCount];
	if(!vertices){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::UpdateSentence() : Failed to create vertex array.");
		#endif
		return false;}

	// Init le vertex array a zero
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calcul les positions X et Y de l'ecran 2D pour commencer le dessin
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Utilise la classe Font pour construire le vertex array
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Init le pointeur vers le vertex buffer a zero
	verticesPtr = 0;

	// Verouille le vertex buffer
	result = sentence->vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&verticesPtr);
	if(FAILED(result)){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CText::UpdateSentence() : Failed to lock vertex buffer.");
		#endif
		return false;}

	// Copie le vertex array dans le vertex buffer
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Deverouille le vertex buffer
	sentence->vertexBuffer->Unmap();

	// Release ce dont on a plus besoin
	delete [] vertices;
	vertices = 0;

	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CText::Shutdown()
{
	// Appel les differentes fonctions de release
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);

	if(m_FontShader){
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;}

	if(m_Font){
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;}

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CText::Shutdown()");
	#endif
}

void CText::ReleaseSentence(SentenceType** sentence)
{
	// Release proprement la phrase
	if(*sentence){
		if((*sentence)->vertexBuffer){
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;}

		if((*sentence)->indexBuffer){
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;}

		delete *sentence;
		*sentence = 0;}
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Affichage																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CText::Render(ID3D10Device* device, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	RenderSentence(device, m_sentence1, worldMatrix, orthoMatrix);
	RenderSentence(device, m_sentence2, worldMatrix, orthoMatrix);
}

void CText::RenderSentence(ID3D10Device* device, SentenceType* sentence, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;

	// Vertex buffer stride et offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Active le vertex buffer dans l'input assembler pour qu'il puisse etre affiche
	device->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);
	// Idem avec l'index buffer
	device->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Type de primitive
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Couleur de la phrase
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);
	// Affiche via le shader special police
	m_FontShader->Render(device, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);

	return;
}
