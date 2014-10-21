/*
 * compiler.c: a dumb and simple compiler
 *
 * Copyright (C) 2014 Bryance Oyang
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "compiler.h"

extern void compile();

/*
 * an undo-able buffer here to hold output assembly instructions
 * temporarily before flushing to output file
 *
 * state includes both the buffer and the file reader
 *
 * to save current state
 *	state_t saved_state = get_state();
 *
 * to undo to previously saved state do
 *	set_state(saved_state);
 */
#define BUF_SIZE 10000
static char buf[BUF_SIZE];
static int buf_pos = 0;
struct state {
	long file_pos;
	int buf_pos;
};

static FILE *source; /* input sourcecode */
static FILE *as; /* output assembly */

int main()
{
	int a;
	FILE *gcc;

	/* hardcoded input/output files */
	source = fopen("src/source.b", "r");
	as = fopen("src/asm.s", "w");
	if (source == NULL || as == NULL)
		goto out_err;

	fputs(".global main\nmain:\n", as);

	compile(); /* wheeeee */

	if (fclose(source) != 0 || fclose(as) != 0)
		goto out_err;

	/* use gcc assembler/linker to do assembly --> executable */
	gcc = popen("gcc src/asm.s -o src/a.out", "r");
	while ((a = fgetc(gcc)) != EOF)
		fputc(a, stdout);
	pclose(gcc);

	return 0;

out_err:
	fputs("failed\n", stderr);
	return EXIT_FAILURE;
}

/* probably make this a realloc in the future */
static void buffer_overflow()
{
	fputs("instruction buffer overflow\n", stderr);
	abort();
}

/* print a single instruction */
void print(char *s, ...)
{
	va_list ap;

	va_start(ap, s);
	buf[buf_pos++] = '\t';
	buf_pos += vsprintf(buf + buf_pos, s, ap);
	buf[buf_pos++] = '\n';

	if (buf_pos > BUF_SIZE)
		buffer_overflow();
}

/* flush instruction buffer to file */
void flush()
{
	if (buf_pos > BUF_SIZE)
		buffer_overflow();

	buf[buf_pos] = '\0';
	fputs(buf, as);
	fflush(as);

	buf[0] = '\0';
	buf_pos = 0;
}

/* get current state of instruction buffer and file reader*/
state_t get_state()
{
	return (state_t){.file_pos = ftell(source),
		.buf_pos = buf_pos};
}

/* get current state of instruction buffer only */
state_t get_buf_state()
{
	return (state_t){.file_pos = -1,
		.buf_pos = buf_pos};
}

/* set current state to a previously saved one */
void set_state(state_t *state)
{
	if (state->file_pos >= 0) {
		if (fseek(source, state->file_pos, SEEK_SET) != 0) {
			fputs("invalid state\n", stderr);
			abort();
		}
	}
	if (state->buf_pos > BUF_SIZE)
		buffer_overflow();
	if (state->buf_pos >= 0)
		buf_pos = state->buf_pos;
}

/* read one character */
char get()
{
	return fgetc(source);
}

/* read next non-whitespace character */
char wget()
{
	char a;

	while (isspace((a = get())));

	return a;
}

/* read one character but don't advance */
char look()
{
	char a;

	a = get();

	fseek(source, -1, SEEK_CUR);
	return a;
}

/* read next non-whitespace character but only advance to char before it */
char wlook()
{
	char a;

	a = wget();

	fseek(source, -1, SEEK_CUR);
	return a;
}

/* read one character and check if == s, raise error if not */
void match(char s)
{
	if (s != get())
		error("expected %c", s);
}

/* output error message and exit */
void error(char *s, ...)
{
	va_list ap;

	va_start(ap, s);
	vfprintf(stderr, s, ap);
	fputs("\n", stderr);

	exit(EXIT_FAILURE);
}
