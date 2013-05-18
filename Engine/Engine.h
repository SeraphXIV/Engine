// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _ENGINE_H // Si on a jamais appele cet header
#define _ENGINE_H // On signifie qu'on l'appel

#define WIN32_LEAN_AND_MEAN // Accelere la generation du programme en allegeant WIN32 de quelques trucs dont on se sert jamais

// Bien sur, include des fonctions Windows
#include <windows.h>
// Et de nos classes
#include "Input.h"
#include "Graphics.h"

// Classe de base du moteur, prend en charge sa construction, initialisation, destruction propre
// Plus de details dans le .cpp
class CEngine
{
	public:
		CEngine(); // Constructeur
		CEngine(const CEngine&); // Constructeur de copie
		~CEngine(); // Destructeur

		bool Init(); // Initialisation
		void Shutdown(); // Destruction propre
		void Run(); // Boucle infinie

		LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM); // Gestionnaire des messages envoyes par Windows

	private:
		bool Frame(); // Execute a chaque frame
		void InitWindows(int&, int&); // Initialisation de la fenetre
		void ShutdownWindows(); // Fermeture de la fenetre

	private:
		LPCWSTR m_appName; // Nom du programme
		HINSTANCE m_hinstance; // ID Windows du programme
		HWND m_hwnd; // Classe Windows de la fenetre du programme

		CInput* Input; // Pointeur vers la classe de gestion des inputs
		CGraphics* Graphics; // Pointeur vers la classe de gestion de l'affichage
		bool bGraphics; // bool de test pour savoir si tout s'est bien passe a l'affichage
};

// Fonction callback d'ecoute par notre instance des inputs
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// Trick de redirection utilise plus tard, les callback Windows n'aiment pas les objets, avec ca ca passe
static CEngine* AppHandle = 0;

#endif // Fin du test du pre-proc