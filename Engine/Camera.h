// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _CAMERA_H // Si on a jamais appele cet header
#define _CAMERA_H // On signifie qu'on l'appel

// Include
#include <d3dx10math.h>


// Classe de gestion de la matrice de vue : la camera
class CCamera
{
	public:
		CCamera(); // Constructeur
		CCamera(const CCamera&); // Constructeur de copie
		~CCamera(); // Destructeur

		void SetPosition(float, float, float); // Defini la position de la cam
		void SetRotation(float, float, float); // Defini l'angle de la vam

		// Accesseurs
		D3DXVECTOR3 GetPosition();
		D3DXVECTOR3 GetRotation();
		void GetViewMatrix(D3DXMATRIX&);

		void Update(); // Met a jour la matrice selon la nouvelle position de la cam

	private:
		float m_positionX, m_positionY, m_positionZ; // Variables de position
		float m_rotationX, m_rotationY, m_rotationZ; // Variables de rotation
		D3DXMATRIX m_viewMatrix; // Matrice de vue, stock la position/rotation de la cam dans un format utilisable par le shader
};

#endif