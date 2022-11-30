#include <stdio.h>
#include <stdlib.h>

int main () {
	char *str;

	// we intentionally leak memory to fault all memories available to the VM.
	for (;;) {
		str = (char *) aligned_alloc(4096, 4096);
		if (!str) continue;
		str[0] = '1';
		str[4095] = '1';
	}

	return(0);
}