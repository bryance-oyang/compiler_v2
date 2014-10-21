/*
 * language.c: the silly language definition
 *
 * Copyright (C) 2014 Bryance Oyang
 */
#include <stdio.h>
#include <ctype.h>
#include "compiler.h"

#define TRUE 1
#define FALSE 0
#define END 0

#define IN_F_EVAL 1
#define NO_F_EVAL 0

/*
 * a statement is either a single digit numeric constant or a function
 * evaluation of the form (f args...) lisp style
 */
int statement();

/* a constant is a single digit number that evaluates to itself */
void constant();

/* a f_eval is a function evalution of the form (f args...) lisp style */
void f_eval();

/* built-in arithmetic functions */
void add();
void sub();
void mul();
void div();

/* compile the program and output assembly code */
void compile()
{
	while (statement(NO_F_EVAL));
	
	print("ret");
	flush();
}

int statement(int status)
{
	if (isdigit(wlook()))
		constant();
	else if (wlook() == '(')
		f_eval();
	else if (wlook() == ')')
		if (status == IN_F_EVAL)
			return END;
		else
			error("unexpected ')'");
	else if (wlook() == EOF)
		return END;
	else
		error("expected statement");

	return TRUE;
}


void constant()
{
	int a;

	if (!isdigit((a = wget())))
		error("expected constant");

	a -= '0';
	print("movq\t$%d, %%rax", a);
	flush();
}

void f_eval()
{
	match('(');

	if (isspace(look()))
		error("expected function name after (");

	switch (look()) {
	case '+':
		add();
		break;
	case '-':
		sub();
		break;
	case '*':
		mul();
		break;
	case '/':
		div();
		break;
	default:
		error("undefined function");
	}
}

void add()
{
	int stack_count = 0;

	match('+');

	while (statement(IN_F_EVAL)) {
		print("pushq\t%%rax");
		stack_count++;
	}

	match(')');

	print("xorq\t%%rax, %%rax");
	while (stack_count--) {
		print("popq\t%%rbx");
		print("addq\t%%rbx, %%rax");
	}

	flush();
}

void sub()
{
	match('-');

	if (!statement(IN_F_EVAL)) {
		error("sub: expected statement");
	}
	print("movq\t%%rax, %%rbx");

	if (!statement(IN_F_EVAL)) {
		error("sub: expected statement");
	}

	match(')');

	print("subq\t%%rax, %%rbx");
	print("movq\t%%rbx, %%rax");

	flush();
}

void mul()
{
	int stack_count = 0;

	match('*');

	while (statement(IN_F_EVAL)) {
		print("pushq\t%%rax");
		stack_count++;
	}

	match(')');

	print("movq\t$1, %%rax");
	while (stack_count--) {
		print("popq\t%%rbx");
		print("imulq\t%%rbx, %%rax");
	}

	flush();
}

void div()
{
	match('/');

	if (!statement(IN_F_EVAL)) {
		error("div: expected statement");
	}
	print("movq\t%%rax, %%rbx");

	if (!statement(IN_F_EVAL)) {
		error("div: expected statement");
	}

	match(')');

	print("movq\t%%rax, %%rcx");
	print("movq\t%%rbx, %%rax");
	print("cqo");
	print("idivq\t%%rcx");
}
