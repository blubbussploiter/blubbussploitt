
#include <cstdint>

namespace RBX
{
	namespace CustomFuncs
	{
		extern int r_getgenv(std::uintptr_t rL);
		extern int r_getreg(std::uintptr_t rL);
		extern int r_getienv(std::uintptr_t rL);
		extern int r_setreadonly(std::uintptr_t rL);
		extern int r_dofile(std::uintptr_t rL);
		extern int r_readfile(std::uintptr_t rL);
		extern int r_writefile(std::uintptr_t rL);
		extern int r_loadstring(std::uintptr_t rL);
		extern int r_setrawmetatable(std::uintptr_t rL);
		extern int r_setsimulationradius(std::uintptr_t rL);
		extern int r_getscriptsource(std::uintptr_t rL);
		extern int r_getrawmetatable(std::uintptr_t rL);
		extern int r_getcallingscript(std::uintptr_t rL);
		extern int r_getcurrentidentity(std::uintptr_t rL);
		extern int r_hookmetamethod(std::uintptr_t rL);
		extern int r_fireclickdetector(std::uintptr_t rL);
		extern int r_checkcaller(std::uintptr_t rL);
		extern int r_getcaller(std::uintptr_t rL);
		extern int r_newcclosure(std::uintptr_t rL);
	}
	namespace CustomLibs
	{
		namespace Crypt
		{
			extern int r_crypt_b64encode(std::uintptr_t rL);
			extern int r_crypt_b64decode(std::uintptr_t rL);
			extern int r_crypt_hash(std::uintptr_t rL);
		}

		namespace Debug
		{
			extern int r_debug_getconstant(std::uintptr_t rL);
			extern int r_debug_getconstants(std::uintptr_t rL);
			extern int r_debug_getinfo(std::uintptr_t rL);
		}

		namespace Bit
		{
			/* divide (no remainder) */
			extern int r_bit_bdiv(std::uintptr_t rL);
			/* multiply (w/ overflow) */
			extern int r_bit_bmul(std::uintptr_t rL);
			/* add (w/ overflow) */
			extern int r_bit_badd(std::uintptr_t rL); 
			/*  sub (w/ overflow) */
			extern int r_bit_bsub(std::uintptr_t rL);
			/* a1 & a2 */
			extern int r_bit_band(std::uintptr_t rL);
			/* a1 | a2 */
			extern int r_bit_bor(std::uintptr_t rL); 
			/* a1 ^ a2 */
			extern int r_bit_bxor(std::uintptr_t rL); 
			/* a1 ~ (invert) */
			extern int r_bit_bnot(std::uintptr_t rL); 
			/* __rotr(a1, a2) */
			extern int r_bit_ror(std::uintptr_t rL);
			/* __rotl(a1, a2) */
			extern int r_bit_rol(std::uintptr_t rL); 
			/* std::hex << a1 */
			extern int r_bit_tohex(std::uintptr_t rL); 
			/* more or less for compatibility, returns as integer */
			extern int r_bit_tobit(std::uintptr_t rL);
			/* a1 << a2 */
			extern int r_bit_lshift(std::uintptr_t rL);
			/* a1 >> a2 */
			extern int r_bit_rshift(std::uintptr_t rL); 
		}

		extern void r_open_bit_lib(std::uintptr_t rL);
		extern void r_open_debug_lib(std::uintptr_t rL);
		extern void r_open_crypt_lib(std::uintptr_t rL);
	}
}