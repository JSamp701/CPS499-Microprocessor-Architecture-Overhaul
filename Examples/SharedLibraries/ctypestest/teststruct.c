#include "teststruct.h"
#include <stdlib.h>

tstruct * alloc_tstruct(char *aname){
	tstruct *atstruct = ((tstruct*)(malloc(sizeof(tstruct))));
	atstruct->structnum = (nextStructNum++);
	atstruct->name = aname;
	return atstruct;
}

void dealloc_tstruct(tstruct * atstruct){
	free(atstruct->name);
	free(atstruct);
}