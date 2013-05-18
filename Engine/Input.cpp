// Include du header associe
#include "Input.h"

//////////////////////////////////////////////////////////////////////////////////////////
//			Constructeur & Co															//
//////////////////////////////////////////////////////////////////////////////////////////
// On ne s'en servira pas
CInput::CInput(){}
CInput::CInput(const CInput& osef){}
CInput::~CInput(){}

//////////////////////////////////////////////////////////////////////////////////////////
//			Initialisation																//
//////////////////////////////////////////////////////////////////////////////////////////
void CInput::Init()
{
	// Fait office de constructeur

	// Declaration des variables
	int i;
	
	// Met tout les touches a "relachee"
	for(i=0; i<256; i++){
		bKeys[i] = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//			Clavier																		//
//////////////////////////////////////////////////////////////////////////////////////////
void CInput::KeyDown(unsigned int input)
{
	// Petite verif, on sait jamais
	if(input >= 0 && input <= 256){
		// Si une touche a ete enfoncee, on sauvegarde son etat dans le tableau
		bKeys[input] = true;
	}
}
void CInput::KeyUp(unsigned int input)
{
	// Petite verif, on sait jamais
	if(input >= 0 && input <= 256){
		// Si une touche a ete relachee, on sauvegarde son etat dans le tableau
		bKeys[input] = false;
	}
}
bool CInput::IsKeyDown(unsigned int key)
{
	// Renvoi l'etat d'une touche, pas de verif, a nous de veiller ailleurs a ce qu'on ne puisse pas entrer une valeur etrange
	return bKeys[key];
}