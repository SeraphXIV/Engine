#ifndef _SPHDEBUG_H 
#define _SPHDEBUG_H 

#include <string>
#include <iostream>
#include <fstream>

namespace SPHDebug
{
	void Open(); // Ouverture du flux
	void Close(); // Fermeture du flux
	void Msg(std::string); // Ecriture dans le flux
	void Msg(char); // Ecriture dans le flux
	void Msg(char*); // Ecriture dans le flux
	void Msg(int); // Ecriture dans le flux
	void Msg(float); // Ecriture dans le flux
	static std::ofstream Output; // Fichier de debug
}

#endif