#ifndef _FONTSHADER_H_
#define _FONTSHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>
using namespace std;

// Classe de gestion du shader de la police
class CFontShader
{
public:
	CFontShader();
	CFontShader(const CFontShader&);
	~CFontShader();

	bool Init(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4);

private:
	bool InitShader(ID3D10Device*, HWND, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, D3DXVECTOR4);
	void RenderShader(ID3D10Device*, int);

private:
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr; // Matrice de position
	ID3D10EffectMatrixVariable* m_viewMatrixPtr; // Matrice de vue
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr; // Matrice de projection
	ID3D10EffectShaderResourceVariable* m_texturePtr; // Texture
	ID3D10EffectVectorVariable* m_pixelColorPtr; // Couleur
};

#endif
