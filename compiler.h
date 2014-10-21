#ifndef COMPILER_H
#define COMPILER_H

typedef struct state state_t;

void print(char *s, ...);
void flush();

state_t get_state();
void set_state(state_t *state);

char get();
char wget();
char look();
char wlook();
void match(char s);

void error(char *s, ...);

#endif /* COMPILER_H */
