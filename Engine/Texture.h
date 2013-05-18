// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _TEXTURE_H // Si on a jamais appele cet header
#define _TEXTURE_H // On signifie qu'on l'appel

// Include
#include <d3d10.h>
#include <d3dx10.h>

// Classe de gestion d'UNE texture, plusieurs objets necessaire pour plusieurs textures
class CTexture
{
	public:
		CTexture(); // Constructeur
		CTexture(const CTexture&); // Constructeur de copie
		~CTexture(); // Destructeur

		bool Init(ID3D10Device*, WCHAR*); // Initialisation
		void Shutdown(); // Destruction propre

		ID3D10ShaderResourceView* GetTexture(); // Renvoi un pointeur vers la ressource texture a envoyer au shader

	private:

		ID3D10ShaderResourceView* m_texture; // La ressource texture

};
#endif // Fin du test du pre-proc