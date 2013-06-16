// Include du header associe
#include "Font.h"
#include "SPH_Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CFont::CFont()
{
	// Mise a zero des variables et pointeurs
	m_Font = 0;
	m_Texture = 0;
}
// On ne s'en servira pas
CFont::CFont(const CFont& osef){}
CFont::~CFont(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CFont::Init(ID3D10Device* device, char* fontFilename, WCHAR* textureFilename)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CFont::Init()");
	#endif

	bool bResult;

	// Charge le fichier texte contenant les donnees de la police
	bResult = LoadFontData(fontFilename);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFont::Init() : Failed to load font data.");
		#endif
		return false;}

	// Charge la texture qui contient la police
	bResult = LoadTexture(device, textureFilename);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFont::Init() : Failed to load font texture.");
		#endif
		return false;}

	return true;
}

// Charge le fichier texte et traite les donnees relative a la police
bool CFont::LoadFontData(char* filename)
{
	ifstream fin; // Flux de lecture
	int i; // Variable d'incrementation
	char temp; // Caractere lu

	// Cree le tableau de stockage
	m_Font = new FontType[95];
	if(!m_Font){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFont::LoadFontData() : Failed to create font array.");
		#endif
		return false;}

	// Ouvre le fichier texte
	fin.open(filename);
	if(fin.fail()){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFont::LoadFontData() : Failed to open font text file.");
		#endif
		return false;}

	// Lecture des 95 caracteres utilises pour le texte
	for(i=0; i<95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// Fermeture du fichier texte
	fin.close();

	return true;
}

// Charge la texture de police
bool CFont::LoadTexture(ID3D10Device* device, WCHAR* filename)
{
	bool bResult;


	// Create the texture object.
	m_Texture = new CTexture;
	if(!m_Texture){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFont::LoadTexture() : Failed to create texture.");
		#endif
		return false;}

	// Initialize the texture object.
	bResult = m_Texture->Init(device, filename);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CFont::LoadTexture() : Failed to init texture.");
		#endif
		return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CFont::Shutdown()
{
	// Appel les differentes fonctions de release
	ReleaseTexture();
	ReleaseFontData();

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CFont::Shutdown()");
	#endif

	return;
}
void CFont::ReleaseFontData()
{
	// Release le tableau des caracteres
	if(m_Font){
		delete [] m_Font;
		m_Font = 0;}
	return;
}
void CFont::ReleaseTexture()
{
	// Release la texture
	if(m_Texture){
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;}
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Affichage																	//
//////////////////////////////////////////////////////////////////////////////////////////
// Pas vraiment "affichage". Construit le vertex buffer a partir d'une phrase
void CFont::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	// Contraint les donnees a adopter la structure VertexType
	vertexPtr = (VertexType*)vertices;

	// Nombre de caractere dans la phrase
	numLetters = (int)strlen(sentence);

	// Init l'index du tableau de vertex
	index = 0;

	// Boucle qui construit le vertex et index arrays. Prends chaque caractere et creer 2 triangles
	// puis map le rectangle a la lettre texturee
	for(i=0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32; // Int correspondant au Char

		// Si le caractere est un espace, on decale juste de 3px
		if(letter == 0){drawX += 3.0f;}
		else{
			// Premier triangle
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 1.0f);
			index++;

			// Deuxieme triangle
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(m_Font[letter].right, 1.0f);
			index++;

			// Met a jour la position X du prochain rectangle/caractere par rapport a la taille du dernier caractere + 1px
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Accesseur																	//
//////////////////////////////////////////////////////////////////////////////////////////
ID3D10ShaderResourceView* CFont::GetTexture()
{
	return m_Texture->GetTexture();
}