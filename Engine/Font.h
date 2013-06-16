// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _FONT_H // Si on a jamais appele cet header
#define _FONT_H // On signifie qu'on l'appel

// Include
#include <fstream>
#include "Texture.h"
using namespace std;

// Classe de gestion d'UNE texture, plusieurs objets necessaire pour plusieurs textures
class CFont
{

	private:

		struct FontType{float left, right; int size;}; // Structure du texte associe a la texture de police
		struct VertexType{D3DXVECTOR3 position; D3DXVECTOR2 texture;}; // Structure des vertices pour le shader

	public:
		CFont(); // Constructeur
		CFont(const CFont&); // Constructeur de copie
		~CFont(); // Destructeur

		bool Init(ID3D10Device*, char*, WCHAR*); // Initialisation
		void Shutdown(); // Destruction propre

		ID3D10ShaderResourceView* GetTexture(); // Renvoi un pointeur vers la ressource texture a envoyer au shader
		void BuildVertexArray(void*, char*, float, float); // Construit la phrase

	private:
		bool LoadFontData(char*);
		void ReleaseFontData();
		bool LoadTexture(ID3D10Device*, WCHAR*);
		void ReleaseTexture();

	private:

		CTexture* m_Texture; // La ressource texture
		FontType* m_Font;

};
#endif // Fin du test du pre-proc