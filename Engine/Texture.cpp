// Include du header associe
#include "Texture.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CTexture::CTexture()
{
	// Mise a zero des variables et pointeurs
	m_texture = 0;
}
// On ne s'en servira pas
CTexture::CTexture(const CTexture& osef){}
CTexture::~CTexture(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CTexture::Init(ID3D10Device* device, WCHAR* filename)
{
	HRESULT result; // bool de test special Windows

	// Charge la texture
	result = D3DX10CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	// Test de succes
	if(FAILED(result)){return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CTexture::Shutdown()
{
	// Release le pointeur et le remet a zero
	if(m_texture){
		m_texture->Release();
		m_texture = 0;}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Accesseur																	//
//////////////////////////////////////////////////////////////////////////////////////////
ID3D10ShaderResourceView* CTexture::GetTexture()
{
	// Renvoi simplement la texture
	return m_texture;
}