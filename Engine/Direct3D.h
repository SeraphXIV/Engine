// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _D3D_H // Si on a jamais appele cet header
#define _D3D_H // On signifie qu'on l'appel

// Link vers les bibliotheques D3D10
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dxgi.lib")

// Include de D3D10
#include <d3d10.h>
#include <d3dx10.h>

class CDirect3D
{
	public:
		CDirect3D(); // Constructeur
		CDirect3D(const CDirect3D&); // Constructeur de copie
		~CDirect3D(); // Destructeur

		bool Init(int, int, bool, HWND, bool, float, float); // Initialisation
		void Shutdown(); // Destruction propre
	
		void BeginScene(float, float, float, float); // Debut de la creation de la scene
		void EndScene(); // Fin de la scene : affichage a l'ecran

		ID3D10Device* GetDevice(); // Accesseur vers le Device

		void GetProjectionMatrix(D3DXMATRIX&); // Accesseur vers la matrice de projection
		void GetWorldMatrix(D3DXMATRIX&); // Accesseur vers la matrice du monde
		void GetOrthoMatrix(D3DXMATRIX&); // Accesseur vers la matrice ortho

		void GetVideoCardInfo(char*, int&); // Accesseur des infos de la carte graphique

	private:

		bool m_vsync_enabled;
		int m_videoCardMemory;
		char m_videoCardDescription[128];
		IDXGISwapChain* m_swapChain;
		ID3D10Device* m_device;
		ID3D10RenderTargetView* m_renderTargetView;
		ID3D10Texture2D* m_depthStencilBuffer;
		ID3D10DepthStencilState* m_depthStencilState;
		ID3D10DepthStencilView* m_depthStencilView;
		ID3D10RasterizerState* m_rasterState;
		D3DXMATRIX m_projectionMatrix;
		D3DXMATRIX m_worldMatrix;
		D3DXMATRIX m_orthoMatrix;
};
#endif // Fin du test du pre-proc