// Include du header associe
#include "Light.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CLight::CLight()
{
	// Valeurs par defaut : Lumiere blanche arrivant par Z
	m_diffuseColor = D3DXVECTOR4(1, 1, 1, 1);
	m_direction = D3DXVECTOR3(0, 0, 1);
}
// On ne s'en servira pas
CLight::CLight(const CLight& osef){}
CLight::~CLight(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Set																			//
//////////////////////////////////////////////////////////////////////////////////////////
void CLight::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
}
void CLight::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
}
void CLight::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
}
void CLight::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
}
void CLight::SetSpecularPower(float power)
{
	m_specularPower = power;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Accesseurs																	//
//////////////////////////////////////////////////////////////////////////////////////////
D3DXVECTOR4 CLight::GetAmbientColor()
{
	return m_ambientColor;
}
D3DXVECTOR4 CLight::GetDiffuseColor()
{
	return m_diffuseColor;
}
D3DXVECTOR3 CLight::GetDirection()
{
	return m_direction;
}
D3DXVECTOR4 CLight::GetSpecularColor()
{
	return m_specularColor;
}
float CLight::GetSpecularPower()
{
	return m_specularPower;
}
