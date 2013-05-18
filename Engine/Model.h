// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _MODEL_H // Si on a jamais appele cet header
#define _MODEL_H // On signifie qu'on l'appel

// Include
#include <d3d10.h>
#include <d3dx10math.h>

#include <fstream>
using namespace std;

#include "Texture.h"

// Classe de gestion des models
class CModel
{
	private:
		// Structure des vertices passes au shader, doit correspondre a la structure dans le shader
		struct VertexType
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
			D3DXVECTOR3 normal;
		};

		struct ModelType
		{
			float x, y, z;
			float tu, tv;
			float nx, ny, nz;
		};

	public:
		CModel(); // Constructeur
		CModel(const CModel&); // Constructeur de copie
		~CModel(); // Destructeur

		bool Init(ID3D10Device*, char*, WCHAR*); // Initialise les vertex et index buffers depuis un ficher externe
		void Shutdown(); // Detruit les vertex et index buffers
		void Render(ID3D10Device*); // Envoi les donnees a la CG

		int GetIndexCount(); // Accesseur du nombre d'indices
		ID3D10ShaderResourceView* GetTexture(); // Accesseur a la texture du model

	private:
		bool InitBuffers(ID3D10Device*);
		void ShutdownBuffers();
		void RenderBuffers(ID3D10Device*);

		// Charge et release la texture du model
		bool LoadTexture(ID3D10Device*, WCHAR*);
		void ReleaseTexture();

		// Charge et release le model
		bool LoadModel(char*);
		void ReleaseModel();

	private:
		ID3D10Buffer *m_vertexBuffer; // Vertex buffer
		ID3D10Buffer *m_indexBuffer; // Index buffer
		int m_vertexCount; // Taille du vertex buffer
		int m_indexCount; // Taille de l'index buffer
		unsigned int stride; // Stride du vertex buffer
		unsigned int offset; // Offset du vertex buffer
		CTexture* m_texture; // Texture du model
		ModelType* m_model; // Tableau des donnees du fichier externe du model
};

#endif // Fin du test du pre-proc