// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _COLORSHADER_H // Si on a jamais appele cet header
#define _COLORSHADER_H // On signifie qu'on l'appel

// Include
#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

// Classe de relai entre le programme et le shader, (principalement : demande au shader de dessiner ce qu'on y a stocké)
class CColorShader
{
	public:
		CColorShader(); // Constructeur
		CColorShader(const CColorShader&); // Constructeur de copie
		~CColorShader(); // Destructeur

		bool Init(ID3D10Device*, HWND); // Initialisation
		void Shutdown(); // Destruction propre
		void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX); // Affiche à l'ecran les donnees passees au shader

	private:
		bool InitShader(ID3D10Device*, HWND, WCHAR*);
		void ShutdownShader();
		void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

		void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
		void RenderShader(ID3D10Device*, int);

	private:
		ID3D10Effect* m_effect; // Shader
		ID3D10EffectTechnique* m_technique; // Technique du Shader
		ID3D10InputLayout* m_layout; // 

		ID3D10EffectMatrixVariable* m_worldMatrixPtr; // Matrice du monde a envoyer au shader
		ID3D10EffectMatrixVariable* m_viewMatrixPtr; // Matrice de camera a envoyer au shader
		ID3D10EffectMatrixVariable* m_projectionMatrixPtr; // Matrice de projection a envoyer au shader
};

#endif // Fin du test du pre-proc