#ifndef TESTSTRUCT.H
#define TESTSTRUCT.H
static int nextStructNum = 0;

struct teststruct {
	int structnum;
	char *name;
};

typedef struct teststruct tstruct;

tstruct * alloc_tstruct(char *aname);
void dealloc_tstruct(tstruct* atstruct);
#endif