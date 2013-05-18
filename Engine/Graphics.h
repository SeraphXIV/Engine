// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _GRAPHICS_H // Si on a jamais appele cet header
#define _GRAPHICS_H // On signifie qu'on l'appel

#include <windows.h>

// Include des sous-classes
#include "Direct3D.h"
#include "Camera.h"
#include "Model.h"
#include "LightShader.h"
#include "Light.h"

// Variables globales, c'est pas bien, mais bon, ca passe pour celles la
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

// Classe de gestion de l'affichage
class CGraphics
{
	public:
		CGraphics(); // Constructeur
		CGraphics(const CGraphics&); // Constructeur de copie
		~CGraphics(); // Destructeur

		bool Init(int, int, HWND); // Initialisation
		void Shutdown(); // Destruction propre
		bool Frame(); // Ce qui sera execute a chaque frame

	private:
		bool Render(float); // Affichage de la scene

	private:
		bool bFrame; // bool de test
		CDirect3D* Direct3D; // Classe qui gere D3D
		CCamera* m_Camera; // Classe qui gere la camera
		CModel* m_Model; // Classe qui gere le model
		CLightShader* m_LightShader; // Classe qui gere le shader
		CLight* m_Light; // Classe qui gere la lumiere
};

#endif // Fin du test du pre-proc
