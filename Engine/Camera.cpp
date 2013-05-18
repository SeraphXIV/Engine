// Include du header associe
#include "Camera.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CCamera::CCamera()
{
	// Mise a zero des variables
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}
// On ne s'en servira pas
CCamera::CCamera(const CCamera& other){}
CCamera::~CCamera(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Mise en place																//
//////////////////////////////////////////////////////////////////////////////////////////
void CCamera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}
void CCamera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Accesseurs																	//
//////////////////////////////////////////////////////////////////////////////////////////
D3DXVECTOR3 CCamera::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}
D3DXVECTOR3 CCamera::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}
void CCamera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Mise a jour																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CCamera::Update()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	// Defini l'axe qui symbolise la verticalite, Y
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Position de la cam
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Endroit ou la cam regarde
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Yaw (Y axis), pitch (X axis), et roll (Z axis) en radians
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	// Cree la matrice de rotation pour les yaw/pitch/roll
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transferme les vecteurs lookAt et Up via la matrice de rotation pour que l'angle de vue soit correcte
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	// Relativise la direction de vue par rapport a la position de la cam
	lookAt = position + lookAt;

	// Cree la matrice a partir des trois vecteurs
	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

	return;
}