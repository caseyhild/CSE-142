#include <cstdlib>
#include "pin_tags.h"

int main(int argc, char *argv[])
{

	int * t = new int[1024*1024];
	int * s = new int[1024*1024];

	START_TRACE();
	DUMP_START( "s", &s[0], &s[1024*1024], true);
	DUMP_START( "t", &t[0], &t[1024*1024], true);
	DUMP_START( "both", &s[0], &t[1024*1024], true);


	for (int i = 0; i < 1024*1024;i++) {
		t[i] = 0;
	}

	for (int i = 0; i < 1024*1024;i++) {
		s[i] = t[i];
	}

	DUMP_STOP("t");
	DUMP_STOP("s");
	DUMP_STOP("both");

	return 0;
}
