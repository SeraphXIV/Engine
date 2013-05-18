// Definition pour le pre-processeur, pour n'inclure qu'une seule fois cet header
#ifndef _INPUT_H // Si on a jamais appele cet header
#define _INPUT_H // On signifie qu'on l'appel

// Classe de gestion des inputs
class CInput
{
	public:
		CInput(); // Constructeur
		CInput(const CInput&); // Constructeur de copie
		~CInput(); // Destructeur

		void Init(); // Initialisation

		void KeyDown(unsigned int); // Si une touche a ete enfoncee
		void KeyUp(unsigned int); // Si une touche a ete relachee

		bool IsKeyDown(unsigned int); // Savoir l'etat d'une touche

	private:
		bool bKeys[256]; // Stock les etats des touches
};

#endif // Fin du test du pre-proc