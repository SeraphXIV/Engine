//////////////////////////////////////////////////////////////////////////////////////////
//			Fichier main, simple passerelle entre Windows et le moteur					//
//////////////////////////////////////////////////////////////////////////////////////////

// Include du systeme du moteur
#include "Engine.h"

// Fonction barbare, mais Windows la demande comme :
// Ca :    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
// Ou ca : int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
// hInstance -> ID de notre programme
// hPrevInstance -> Relique d'ancienne version de Windows
// Cmdline et Cmdshow -> Parametres passes au lancement du programme
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	// Pour dire a Windows que ces arguments servent a rien, sinon il lance un warning
	UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(pScmdline);
	UNREFERENCED_PARAMETER(iCmdshow);

	// Declaration des variables
	CEngine* Engine; // Le moteur, en pointeur pour permettre une meilleur gestion de l'allocation/suppression
	bool bLoop; // Bool pour la boucle infinie
	
	// Creation du moteur
	Engine = new CEngine;
	// Test de succes
	if(!Engine){return 0;}

	// Lance l'initialisation du moteur
	bLoop = Engine->Init();
	// Si l'initialisation s'est bien passee, on lance la boucle infinie
	if(bLoop){Engine->Run();}

	// La boucle est brisee, on efface tout
	Engine->Shutdown();
	delete Engine;
	Engine = 0;

	return 0;
}