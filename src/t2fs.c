#include <string.h>
#include "logging.h"
#include "t2fs.h"

int identify2 (char *name, int size) {
    char *id = "Lucas Leal (206438)\nLuis Mollmann (206440)";

    strncpy(name, id, size);

    if (strncmp(name, id, size) != 0) {
		logwarning("identify2: couldn't copy correctly");
		return -1;
	}

    return 0;
}
