// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _TEXT_H // Si on a jamais appele cet header
#define _TEXT_H // On signifie qu'on l'appel

// Include
#include "Font.h"
#include "FontShader.h"

// Classe de gestion d'UNE texture, plusieurs objets necessaire pour plusieurs textures
class CText
{
	private:
		// Structure de stockage des info d'affichage pour chaque phrase
		struct SentenceType{
			ID3D10Buffer *vertexBuffer, *indexBuffer;
			int vertexCount, indexCount, maxLength;
			float red, green, blue;};
		// Doit etre la meme que dans Font.h
		struct VertexType{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;};

	public:
		CText(); // Constructeur
		CText(const CText&); // Constructeur de copie
		~CText(); // Destructeur

		bool Init(ID3D10Device*, HWND, int, int, D3DXMATRIX);
		void Shutdown();
		void Render(ID3D10Device*, D3DXMATRIX, D3DXMATRIX);

	private:
		bool InitSentence(SentenceType**, int, ID3D10Device*);
		bool UpdateSentence(SentenceType*, char*, int, int, float, float, float);
		void ReleaseSentence(SentenceType**);
		void RenderSentence(ID3D10Device*, SentenceType*, D3DXMATRIX, D3DXMATRIX);

	private:
		CFont* m_Font;
		CFontShader* m_FontShader;
		int m_screenWidth, m_screenHeight;
		D3DXMATRIX m_baseViewMatrix;

		SentenceType* m_sentence1;
		SentenceType* m_sentence2;


};
#endif // Fin du test du pre-proc