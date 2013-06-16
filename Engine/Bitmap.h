// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _BITMAP_H // Si on a jamais appele cet header
#define _BITMAP_H // On signifie qu'on l'appel

#include "Texture.h"

// Classe de gestion des models
class CBitmap
{
	private:
		// Structure des vertices passes au shader, doit correspondre a la structure dans le shader, en 2D pas besoin de normal
		struct VertexType
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
		};

	public:
		CBitmap(); // Constructeur
		CBitmap(const CBitmap&); // Constructeur de copie
		~CBitmap(); // Destructeur

		bool Init(ID3D10Device*, int, int, WCHAR*, int, int); // Initialise les vertex et index buffers depuis un ficher externe
		void Shutdown(); // Detruit les vertex et index buffers
		bool Render(ID3D10Device*, int, int); // Envoi les donnees a la CG

		int GetIndexCount(); // Accesseur du nombre d'indices
		ID3D10ShaderResourceView* GetTexture(); // Accesseur a la texture du model

	private:
		bool InitBuffers(ID3D10Device*);
		void ShutdownBuffers();
		bool UpdateBuffers(int, int);
		void RenderBuffers(ID3D10Device*);

		// Charge et release la texture du model
		bool LoadTexture(ID3D10Device*, WCHAR*);
		void ReleaseTexture();

	private:
		ID3D10Buffer *m_vertexBuffer; // Vertex buffer
		ID3D10Buffer *m_indexBuffer; // Index buffer
		int m_vertexCount; // Taille du vertex buffer
		int m_indexCount; // Taille de l'index buffer
		CTexture* m_Texture; // Texture du model

		int m_screenWidth, m_screenHeight; // Taille de l'ecran
		int m_bitmapWidth, m_bitmapHeight; // Position de l'image 2D
		int m_previousPosX, m_previousPosY; // Derniere position de rendu
};

#endif // Fin du test du pre-proc