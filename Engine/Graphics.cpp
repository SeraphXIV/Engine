// Include du header associe
#include "graphics.h"
#include "SPH_Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CGraphics::CGraphics()
{
	// Mise a zero des variables et pointeurs
	Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_Bitmap = 0;
	m_LightShader = 0;
	m_Light = 0;
}
// On ne s'en servira pas
CGraphics::CGraphics(const CGraphics& osef){}
CGraphics::~CGraphics(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CGraphics::Init(int screenWidth, int screenHeight, HWND hwnd)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CGraphics::Init()"); 
	#endif

	// Declaration des variables
	bool bResult; // bool de test

	// Creation de l'objet qui gere D3D
	Direct3D = new CDirect3D;
	// Test de succes
	if(!Direct3D){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to create new Direct3D"); 
		#endif
		return false;}

	// Initialisation
	bResult = Direct3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	// Test de succes
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to init Direct3D"); 
		#endif
		return false;}

	// Creation de l'objet qui gere la cam
	m_Camera = new CCamera;
	// Test de succes
	if(!m_Camera){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to create new Camera"); 
		#endif
		return false;}
	// Position initiale de la cam
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// Creation de l'objet qui gere le model 3D
	m_Model = new CModel;
	// Test de succes
	if(!m_Model){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to create new 3D Model"); 
		#endif
		return false;}

	// Creation de l'objet qui gere le model 2D
	m_Bitmap = new CBitmap;
	if(!m_Bitmap){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to create new 2D Model"); 
		#endif
		return false;}

	// Init l'objet bitmap
	bResult = m_Bitmap->Init(Direct3D->GetDevice(), screenWidth, screenHeight, L"../Engine/Texture_1.dds", 256, 256);
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to init 2D Model"); 
		#endif
		return false;}

	// Initialisation
	bResult = m_Model->Init(Direct3D->GetDevice(), "../Engine/Cube.txt", L"../Engine/Texture_1.dds");
	// Test de succes
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to init Model"); 
		#endif
		return false;}

	// Creation de l'objet qui gere le shader
	m_LightShader = new CLightShader;
	// Test de succes
	if(!m_LightShader)
	{
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to create Shader"); 
		#endif
		return false;}

	// Initialize the light shader object.
	bResult = m_LightShader->Init(Direct3D->GetDevice(), hwnd);
	// Test de succes
	if(!bResult)
	{
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to init Shader"); 
		#endif
		return false;}

	// Create the light object.
	m_Light = new CLight;
	// Test de succes
	if(!m_Light)
	{
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CGraphics::Init() : Failed to create Light"); 
		#endif
		return false;}

	// Initialisation de l'objet Light
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f); // Lumiere ambiante a 15%
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f); // Lumiere diffuse blanche
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CGraphics::Shutdown()
{
	// Destructeur perso, plus propre que celui de base

	// Release des classes et remise a zero des pointeurs
	if(m_Light){
		delete m_Light;
		m_Light = 0;}

	if(m_LightShader){
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;}

	if(m_Model){
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;}

	if(m_Bitmap){
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;}

	if(m_Camera){
		delete m_Camera;
		m_Camera = 0;}

	if(Direct3D){
		Direct3D->Shutdown();
		delete Direct3D;
		Direct3D = 0;}

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CGraphics::Shutdown()"); 
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Ce qui sera execute a chaque frame											//
//////////////////////////////////////////////////////////////////////////////////////////
bool CGraphics::Frame()
{
	static float rotation = 0.0f;
	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.005f;
	if(rotation > 360.0f){rotation -= 360.0f;}
	// Affichage de la scene
	bFrame = Render(rotation);
	// Test de succes
	if(!bFrame)	{return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Affichage																	//
//////////////////////////////////////////////////////////////////////////////////////////
bool CGraphics::Render(float rotation)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix, view2DMatrix, world2DMatrix;
	bool result;

	// Vide les buffers pour commencer
	Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Genere une matrice de vue basee sur la position actuelle de la cam
	m_Camera->Update();

	// Accede aux trois matrices
	m_Camera->GetViewMatrix(viewMatrix);
	Direct3D->GetWorldMatrix(worldMatrix);
	Direct3D->GetProjectionMatrix(projectionMatrix);
	Direct3D->GetOrthoMatrix(orthoMatrix);
	D3DXMatrixIdentity(&view2DMatrix);
	D3DXMatrixIdentity(&world2DMatrix);

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Envoi les vertex et index buffer dans la pipeline graphique pour les preparer a l'affichage
	m_Model->Render(Direct3D->GetDevice());

	// Affiche le model en utlisant le shader
	m_LightShader->Render(Direct3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	
	Direct3D->TurnZBufferOff();
	result = m_Bitmap->Render(Direct3D->GetDevice(), 100, 100);
	if(!result)
	{
		return false;
	}
	m_LightShader->Render(Direct3D->GetDevice(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_Bitmap->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	
	Direct3D->TurnZBufferOn();

	// Affichage de la scene calculee
	Direct3D->EndScene();

	return true;
}