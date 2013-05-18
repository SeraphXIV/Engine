// Include du header associe
#include "SPH_Debug.h"

namespace SPHDebug
{
	void Open()
	{
		if(!Output || !Output.is_open()){
		Output.open("SPHDebug.txt", std::ios::out); // Creation du fichier de debug
		Output<<"=== DEBUG STARTED ==="<<std::endl<<std::endl;}
	}
	void Close()
	{
		if(Output && Output.is_open()){
			Output<<std::endl<<"=== DEBUG ENDED ==="<<std::endl;
			Output.close();}
	}
	void Msg(std::string sMsg)
	{
		Output<<sMsg<<std::endl;
	}
	void Msg(char cMsg)
	{
		Output<<cMsg<<std::endl;
	}
	void Msg(char* pcMsg)
	{
		Output<<pcMsg<<std::endl;
	}
	void Msg(int iMsg)
	{
		Output<<iMsg<<std::endl;
	}
	void Msg(float fMsg)
	{
		Output<<fMsg<<std::endl;
	}
}