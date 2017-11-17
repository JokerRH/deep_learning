#include <stdlib.h>
#include "Compat.h"

#ifdef __MSVC_VER
int wmain( int argc, filechar_t **argv )
#else
int main( int argc, filechar_t **argv )
#endif
{
	return EXIT_SUCCESS;
}