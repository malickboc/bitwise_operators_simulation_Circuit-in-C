/************************************************************************************
 a5.c

 CPS 360
 implementation of assignment 5
 author: Malick Bocoum

 version 1.0
 Last updated: Dec 17, 2018
 compile:  gcc a5.c -o a5
 Usage:    ./a5 <hex1> <hex2> <hex-cin>
 ex:    ./a5 1b 2f 1
 ./a5 4f 3d 1
 ./a5 4f01 3d02 0   (for 32 bit values)

#include <stdlib.h>
#include <stdio.h>

/* global variables */
int debug = 0; /* 0: set it to 0 for the final submission;


/* needed for printing bits */

/* global functions */
void printbits(int var, char* var_name);
void halfaddr(int a, int b, int *sumptr, int *carryptr);
void fulladdr(int a, int b, int incarry, int *sumptr, int *carryptr);
void mux2x1(int a, int b, int select, int* out);
void add4(int a, int b, int incarry, int* sumptr, int* carryoutptr);
void parity4(int a, int *outparity);
void add16(int a, int b, int incarry, int *sum, int *outcary);
void magnitude4(int a, int *rslt);
int main(int argc, char *argv[]) {
	/* a5 variables */
	int i, x, y, carryin, sum, selectbit, carry, result, carryout;

	printf("DEBUG=%d\n", debug);

	/* print arguments as strings */
	if (debug >= 1) {
		printf("ARGUMENTS: (argc=%d) ", argc);
		for (i = 0; i < argc; i++)
			printf("%s ", argv[i]);

		printf("\n");
		
	}

	/* executable name is the first command-line argument */
	if (argc < 4) {
		fprintf(stderr, "Usage: %s <hex1> <hex2> <hex-cin>\n", argv[0]);
		exit(1);
	}

	/* get arguments */
	x = (int) strtol(argv[1], NULL, 16);
	y = (int) strtol(argv[2], NULL, 16);
	carryin = strtol(argv[3], NULL, 16);

	if (debug >= 1) {
		printf("x=%d, y=%d, carryin=%d\n", x, y, carryin);
	}

	/* print bits of variable value  */
	printf("\n");
	printbits(x, "x");
	printbits(y, "y");
	printbits(carryin, "carryin");
	printf("\n");

	/* perform half adder to the LSBs of x and y */
	halfaddr(x, y, &sum, &carry);

	/* print half asdder results */
	printf("HalfAdder(x,y) results:\n");
	printbits(sum, "sum");
	printbits(carry, "carry");
	printf("\n");

	fulladdr(x, y, carryin, &sum, &carry);
	printf("FullAdder(x,y) results:\n");
	printbits(sum, "sum");
	printbits(carry, "carry");
	printf("\n");
	/* ADD YOUR program code HERE */

	/* test mux2x1 */
	selectbit = 0;
	mux2x1(x, y, selectbit, &result);
	printbits(result, "mux with sel=0");
	selectbit = 1;
	mux2x1(x, y, selectbit, &result);
	printbits(result, "mux with sel=1");
	printf("\n");

	/* perform 4 bit adder to the LSBs of x and y */
	add4(x, y, carryin, &sum, &carry);

	/* print 4-bit adder results */
	printf("RippleCarry-4bitAdder(x,y,cin) results:\n");
	printbits(sum, "sum");
	printbits(carry, "carryo");
	printf("\n");

	parity4(x, &carryout);
	printf("parity4 results:\n");
	printbits(carryout, "parity");
	printf("\n");

	add16(x, y, carryin, &sum, &carry);
	printf("RippleCarry-16bitAdder(x,y,cin) results:\n");
	printbits(sum, "sum");
	printf("\n");

	magnitude4(x, &result);
	printf("maginitude4 results:\n");
	printbits(result, "magnitude");
	printf("\n");
	exit(0);
}

/* prints bits of variable byte by byte,
 also prints the name of variable if provided */
void printbits(int var, char* var_name) {
	const int SHIFT = 8 * sizeof(unsigned) - 1;
	const unsigned int MASK = 1 << SHIFT;
	int i;

	if (debug == 3) {
		printf("MASK=%d, SHIFT=%d\n", MASK, SHIFT);
	}

	if (var_name)
		printf("%s\t", var_name);

	for (i = 1; i <= SHIFT + 1; i++) {
		printf("%c", (var & MASK ? '1' : '0'));
		var <<= 1;

		if (i % 8 == 0)
			printf(" ");
	}

	printf("\n");

} /* end-printbits */

/* adds the LSBs of a and b and places the results into *sumptr and *carryptr */
void halfaddr(int a, int b, int *sumptr, int *carryptr) {
	int x, y, s, c;

	x = a & 1;
	y = b & 1; /* get the last bit, LSB */
	s = x ^ y;
	c = x & y;

	*sumptr = s;
	*carryptr = c;
}
void fulladdr(int a, int b, int incarry, int *sumptr, int *carryptr) {
	int x, y, s, cin, cout;
	x = a & 1;
	y = b & 1;
	cin = incarry & 1;
	s = x ^ y ^ cin;
	cout = (x & y) | (cin & (x ^ y));

	*sumptr = s;
	*carryptr = cout;
}
void mux2x1(int a, int b, int select, int* out) {
	int x, y = select;
	x = a & 1;
	y = b & 1;
	select = select & 1;
	if (select) {
		*out = x;
	} else {
		*out = y;
	}
}
void add4(int a, int b, int incarry, int *sumptr, int *carryoutptr) {
	int i, x, y, c, s, co, sum;

	sum = 0;
	x = a;
	y = b;
	c = incarry; /* initially */

	for (i = 0; i < 4; i++) {
		/* add LSB of x and y with cin */
		fulladdr(x, y, c, &s, &co);

		/* update x, y, c, sum */
		x = x >> 1;
		y = y >> 1;

		sum = sum + (s << i); /* move the FA sum to the right by one bit position */
		c = co;
	}

	*sumptr = sum;
	*carryoutptr = co;
}

/* Uses 4 add4s to compute 16 bit sum*/
void add16(int a, int b, int incarry, int *sumptr, int *carryoutptr){
	int i, x, y, c, s, co, sum;
	sum = 0;
	x = a;
	y = b;
	c = incarry;
	for (i = 0; i < 16; i++) {
		/* add LSB of x and y with cin */
		fulladdr(x, y, c, &s, &co);
		/* update x, y, c, sum */
		x = x >> 1;
		y = y >> 1;
		sum = sum + (s << i); /* move the FA sum to the right by one bit position */
		c = co;
	}
	*sumptr = sum;
	*carryoutptr = c;
}
