// Include du header associe
#include "Engine.h"
#include "SPH_Debug.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
CEngine::CEngine()
{
	// Mise a zero des variables et pointeurs
	Input = 0;
	Graphics = 0;

	// Ouvre le flux de debug si le header est la
	#ifdef _SPHDEBUG_H 
		SPHDebug::Open(); 
	#endif
}

// Constructeur de copie, certains compilateurs le reclament, mais on ne s'en servira pas
CEngine::CEngine(const CEngine& osef){}
// Destructeur, on ne s'en servira pas non plus, on a notre propre destructeur
// Le destructeur n'est pas appele sous certaines conditions, causant des fuites memoires, on va eviter ca
CEngine::~CEngine(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
bool CEngine::Init()
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CEngine::Init()");
	#endif

	// Declaration des variables
	int screenWidth, screenHeight; // Taille de l'ecran du client
	bool bResult; // bool de test


	// Init a 0 avant d'en faire quoi que ce soit
	screenWidth = 0;
	screenHeight = 0;

	// Initialise l'API Windows
	InitWindows(screenWidth, screenHeight);

	// Creation de l'objet qui gere les inputs
	Input = new CInput;
	// Test de succes
	if(!Input){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CEngine::Init() : Failed to create new Input"); 
		#endif
		return false;}
	// Initialisation
	Input->Init();

	// Creation de l'objet qui gere l'affichage
	Graphics = new CGraphics;
	// Test de succes
	if(!Graphics){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CEngine::Init() : Failed to create new Graphics"); 
		#endif
		return false;}
	// Initialisation
	bResult = Graphics->Init(screenWidth, screenHeight, m_hwnd);
	// Test de succes
	if(!bResult){
		#ifdef _SPHDEBUG_H 
			SPHDebug::Msg("\t /!\\ CEngine::Init() : Failed to init Graphics"); 
		#endif
		return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Destruction																	//
//////////////////////////////////////////////////////////////////////////////////////////
void CEngine::Shutdown()
{
	// Destructeur perso, plus propre que celui de base

	// Release des classes et remise a zero des pointeurs
	if(Graphics){
		Graphics->Shutdown();
		delete Graphics;
		Graphics = 0;
	}

	if(Input){
		delete Input;
		Input = 0;
	}

	ShutdownWindows();

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CEngine::Shutdown()");
		SPHDebug::Close(); 
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Boucle																		//
//////////////////////////////////////////////////////////////////////////////////////////
void CEngine::Run()
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CEngine::Run()"); 
	#endif

	// Declaration des variables
	MSG msg; // Variable des messages envoyes a notre programme par Windows
	bool bLoop, bResult; // bool de tests

	// Mise a zero du MSG
	ZeroMemory(&msg, sizeof(MSG));
	
	// Boucle infinie jusqu'a ce que le programme recoive un message de fin
	bLoop = false;
	while(!bLoop)
	{
		// Gestion des messages envoyes par Windows
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Si Windows signale que le programme doit prendre fin
		if(msg.message == WM_QUIT){
			bLoop = true;
		}else{
			// Sinon on execute le programme
			bResult = Frame();
			if(!bResult){bLoop = true;}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Frame																		//
//////////////////////////////////////////////////////////////////////////////////////////
bool CEngine::Frame()
{
	// Ce qui sera execute a chaque frame

	// Verifie si le client a appuye sur echap, si oui, on ferme tout
	if(Input->IsKeyDown(VK_ESCAPE)){return false;}

	// Affichage
	bGraphics = Graphics->Frame();
	// Si y'a eu un probleme pendant l'affichage, on ferme tout
	if(!bGraphics){return false;}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Gestionnaire des messages													//
//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CEngine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		// Verifie si une touche a ete enfoncee
		case WM_KEYDOWN:
		{
			// Si oui, on l'envoi a l'objet Input pour qu'il puisse la traiter
			Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Verifie si une touche a ete relachee
		case WM_KEYUP:
		{
			// Si oui, on l'envoi a l'objet Input pour qu'il puisse la traiter
			Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Autre messages, on les envoie au gestionnaire par defaut puisqu'on ne va pas s'en servir
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation de la fenetre												//
//////////////////////////////////////////////////////////////////////////////////////////
void CEngine::InitWindows(int& screenWidth, int& screenHeight)
{
	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CEngine::InitWindows()"); 
	#endif

	// Declaration des variables
	WNDCLASSEX wc; // Options de la fenetre
	DEVMODE dmScreenSettings; // Options d'affichage
	int iPosX, iPosY; // Position de la fenetre

	// Pointeur externe vers cet objet, trick de redirection pour les callback Windows
	AppHandle = this;

	// Get l'instance de ce programme
	m_hinstance = GetModuleHandle(NULL);

	// Donne un nom
	m_appName = L"Engine";

	// Options par defaut de la classe de la fenetre
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_appName;
	wc.cbSize = sizeof(WNDCLASSEX);
	
	// Register
	RegisterClassEx(&wc);

	// Determine la resolution de l'ecran du client
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Gere les options d'affichage selon si on est en fullscreen ou windowed
	if(FULL_SCREEN)
	{
		// Si fullscreen, on met la taille a celle de l'ecran du client, et en 32bit
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change l'affichage en plein ecran selon les parametres d'au dessus
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Position de la fenetre en haut a gauche
		iPosX = 0;
		iPosY = 0;
	}else{
		// Si windowed, on met en 800*600
		screenWidth  = 800;
		screenHeight = 600;

		// Place la fenetre au milieu de l'ecran
		iPosX = screenWidth / 2;
		iPosY = screenHeight / 2;
	}

	// Creer enfin la fenetre
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_appName, m_appName, 
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
				iPosX, iPosY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Affiche la fenetre et la met en focus
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Cache le curseur de la souris
	ShowCursor(false);
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Fermeture de la fenetre														//
//////////////////////////////////////////////////////////////////////////////////////////
void CEngine::ShutdownWindows()
{
	// Affiche le curseur de la souris
	ShowCursor(true);

	// Change l'affichage si on quitte le fullscreen
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Detruit la fenetre
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Detruit l'instance
	UnregisterClass(m_appName, m_hinstance);
	m_hinstance = NULL;

	// Release le pointeur
	AppHandle = NULL;

	#ifdef _SPHDEBUG_H 
		SPHDebug::Msg("CEngine::ShutdownWindows()"); 
	#endif
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Callback d'ecoute des messages												//
//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Verifie si la fenetre est en train de se faire detruire
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Verifie si la fenetre est en train de se faire fermer
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// Tout les autres messages seront traites par le moteur, via le trick de redirection
		default:
		{
			return AppHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}