#ifndef __SIMPLE_CLI_H__
#define __SIMPLE_CLI_H__

#define INBUF_SIZE		256

typedef int (*simple_func)(int index, char *string, int len, void *printer);

typedef struct simple_cmds {
	const char *string;
    int index;
	simple_func func;
} simple_cmds;

void simple_cli_loop(void const *pc);
void simple_cli_wait_seconds_and_return(int wait);
int simple_cli_list(simple_cmds *cmds);

int GetStr_wait(char *inbuf, unsigned int *words);
char GetChar_wait();
int GetInt_wait();
int FindSpace(const char *str);

#endif //__SIMPLE_CLI_H__
