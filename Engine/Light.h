// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _LIGHT_H // Si on a jamais appele cet header
#define _LIGHT_H // On signifie qu'on l'appel

// Include
#include <d3dx10math.h>

// Classe de gestion des models
class CLight
{
	public:
		CLight(); // Constructeur
		CLight(const CLight&); // Constructeur de copie
		~CLight(); // Destructeur

		void SetAmbientColor(float, float, float, float);
		void SetDiffuseColor(float, float, float, float);
		void SetDirection(float, float, float);
		void SetSpecularColor(float, float, float, float);
		void SetSpecularPower(float);

		// Accesseurs
		D3DXVECTOR4 GetAmbientColor();
		D3DXVECTOR4 GetDiffuseColor();
		D3DXVECTOR3 GetDirection();
		D3DXVECTOR4 GetSpecularColor();
		float GetSpecularPower();

	private:
		D3DXVECTOR4 m_ambientColor; // Lumiere ambiante
		D3DXVECTOR4 m_diffuseColor; // Lumiere diffuse
		D3DXVECTOR3 m_direction; // Direction de la lumiere diffuse
		D3DXVECTOR4 m_specularColor;
		float m_specularPower;
};

#endif // Fin du test du pre-proc