
#include "addresses.h"

namespace RBX
{
	namespace GetObjects
	{

		/* plz note these arent updated */

		static unsigned int getClassNameAddress = shift(0x56D010); 
		static unsigned int getRemoteCachedSource = shift(0x7602C0);
		static unsigned int getRemoteCachedState = shift(0x681160);
		static unsigned int restartScriptA = shift(0x64A8D0);

		typedef std::string& (__thiscall* getClassName)(std::uintptr_t* instance);
		typedef char* (__thiscall* getCachedRemoteSource)(std::uintptr_t* instance);
		typedef int (__thiscall* getCachedRemoteState)(std::uintptr_t* instance);
		typedef void(__thiscall* restartScript)(std::uintptr_t* instance);

		static restartScript rbx_restartScript = (restartScript)restartScriptA;
		static getClassName rbx_getClassName = (getClassName)getClassNameAddress;
		static getCachedRemoteSource rbx_getCachedRemoteSource = (getCachedRemoteSource)getRemoteCachedSource;
		static getCachedRemoteState rbx_getCachedRemoteState = (getCachedRemoteState)getRemoteCachedState;

		extern void loadScripts(std::uintptr_t rL, std::shared_ptr<std::uintptr_t> i);
	}
}