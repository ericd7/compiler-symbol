/*
File name: stable.c
Compiler: Microsoft Visual Studio 2012 
Author: Peter Roy 040704738
Course: CST 8152 – Compilers, Lab Section: 401 
Assignment: 3
Date: November 15 
Professor: Sv. Ranev
Purpose: To build a symbol table
Function list:
*/

#define _CRT_SECURE_NO_WARNINGS

#include "stable.h"
#include "buffer.h"
#include "token.h"
#include <string.h>
#include <stdio.h>

extern STD sym_table;

STD st_create(int st_size){
	
	STD temp;
	temp.st_offset = 0;
	temp.pstvr =  (STVR*) (malloc(st_size * sizeof(STVR)));
	if(temp.pstvr==NULL)
	{
		temp.st_size = 0;
		return temp;
	}
	temp.plsBD = b_create(1000,15,'a');
	if(temp.plsBD == NULL)
	{
		temp.st_size = 0;
		return temp;
	}

	temp.st_size = st_size;
	return temp;
}

int st_install(STD sym_table, char *lexeme, int line)
{
	int oset = sym_table.st_offset;
	unsigned int i;
	int j;
	int find = st_lookup(sym_table,lexeme);

	if(sym_table.st_size == 0)
		return R_FAIL_1;

	if(sym_table.st_size == sym_table.st_offset)
		return R_FAIL_1;

	if(find == R_FAIL_1)
	{
		sym_table.pstvr[oset].plex = (sym_table.plsBD->ca_head + sym_table.plsBD->addc_offset);
		
		for(i=0; i<=strlen(lexeme);i++)
		{
			b_addc(sym_table.plsBD, lexeme[i]);
			if(b_get_r_flag(sym_table.plsBD))
			{
				sym_table.pstvr[0].plex = b_get_chmemloc(sym_table.plsBD,0);
				for(j=1;j<=sym_table.st_offset;j++)
					sym_table.pstvr[j].plex = sym_table.pstvr[j-1].plex + (strlen(sym_table.pstvr[j-1].plex)+1);
			}
		}

		sym_table.pstvr[oset].o_line = line;

		sym_table.pstvr[oset].status_field = sym_table.pstvr[oset].status_field&DEFAULTZ;
		sym_table.pstvr[oset].status_field = sym_table.pstvr[oset].status_field|DEFAULT;

		if(lexeme[strlen(lexeme)-1]=='#'){
			sym_table.pstvr[oset].status_field = sym_table.pstvr[oset].status_field|STR_S;
			sym_table.pstvr[oset].status_field = sym_table.pstvr[oset].status_field|SET_LSB;
			sym_table.pstvr[oset].i_value.str_offset = -1;
		}
		else if(lexeme[0]=='i'||lexeme[0]=='o'||lexeme[0]=='d'||lexeme[0]=='n'){
			sym_table.pstvr[oset].status_field = sym_table.pstvr[oset].status_field|INT_S;
			sym_table.pstvr[oset].i_value.fpl_val = 0;
		}
		else{
			sym_table.pstvr[oset].status_field = sym_table.pstvr[oset].status_field|FLT_S;
			sym_table.pstvr[oset].i_value.fpl_val = 0;
		}
		st_incoffset();
		return sym_table.st_offset;

	}
	else{
		return find;
	}
}

int st_lookup(STD sym_table,char *lexeme)
{
	int i;
	if(sym_table.st_size == 0)
		return R_FAIL_1;

	for(i=sym_table.st_offset-1;i>=0;i--)
	{
		if(strcmp(lexeme,sym_table.pstvr[i].plex)==0)
			return i;
	}

	return R_FAIL_1;

}

int st_update_type(STD sym_table,int vid_offset,char v_type)
{
	if(sym_table.st_size == 0)
		return R_FAIL_1;

	if((sym_table.pstvr[vid_offset].status_field & CHK_LSB) == SUCCESS)
		return R_FAIL_1;

	if(v_type == 'F'){
		sym_table.pstvr[vid_offset].status_field |= FLT_S;
		sym_table.pstvr[vid_offset].status_field |= SET_LSB; 
	}

	if(v_type == 'I'){
		sym_table.pstvr[vid_offset].status_field |= INT_S;
		sym_table.pstvr[vid_offset].status_field |= SET_LSB;
	}
	return vid_offset;
}

int st_update_value(STD sym_table, int vid_offset, InitialValue i_value){

	if(sym_table.st_size == 0)
		return R_FAIL_1;

	sym_table.pstvr[vid_offset].i_value.str_offset = i_value.str_offset;
	sym_table.pstvr[vid_offset].i_value.fpl_val = i_value.fpl_val;
	sym_table.pstvr[vid_offset].i_value.int_val = i_value.int_val;

	return vid_offset;

}

char st_get_type (STD sym_table, int vid_offset){
	if(sym_table.st_size == 0)
		return R_FAIL_1;

	if ((sym_table.pstvr[vid_offset].status_field & INT_S) && (sym_table.pstvr[vid_offset].status_field & FLT_S))
		return 'S';
	if ((sym_table.pstvr[vid_offset].status_field & INT_S))
		return 'I';
	if ((sym_table.pstvr[vid_offset].status_field & FLT_S))
		return 'F';

	return R_FAIL_1;
}

void st_destroy(STD sym_table){

	b_destroy(sym_table.plsBD);

	if(sym_table.pstvr!=NULL)
		free(sym_table.pstvr);

	st_setsize();
}

int st_print(STD sym_table)
{
	int i;

	if(sym_table.st_size == 0)
		return R_FAIL_1;

	printf("\nSymbol Table\n");
	printf("____________\n\n");
	printf("Line Number Variable Identifier\n");

	for(i=0;i<sym_table.st_offset;i++)
		printf("%2d          %s\n",sym_table.pstvr[i].o_line , sym_table.pstvr[i].plex);
	return i;

}

static void st_setsize(void)
{
	sym_table.st_size = 0;
}

static void st_incoffset(void)
{
	sym_table.st_offset++;
}

int st_store(STD sym_table){
	/* Define a file handle - similar to buffer handle */
	FILE *stf; 
	int size = sym_table.st_size;
	int i, length, line;
	unsigned short int status;
	char type;
	char * lex;
	char *fname = "$stable.ste";
	
	if(sym_table.st_size == 0)
		return R_FAIL_1;

	if ((stf = fopen(fname,"wt")) == NULL)
		return R_FAIL_1;

	fprintf(stf,"%d ",size);

	for(i = 0; i < sym_table.st_offset; i++)
	{
		status = sym_table.pstvr[i].status_field;
		length = strlen(sym_table.pstvr[i].plex);
		line = sym_table.pstvr[i].o_line;
		lex = sym_table.pstvr[i].plex;
		type = st_get_type(sym_table,i);

		switch (type)
		{
		case 'F':
			fprintf(stf," %hX %d %s %d %.2d", status,length,lex,line,sym_table.pstvr[i].i_value.fpl_val);
			break;
		case 'I':
			fprintf(stf," %hX %d %s %d %d", status,length,lex,line,sym_table.pstvr[i].i_value.int_val);
			break;
		case 'S':
			fprintf(stf," %hX %d %s %d %d", status,length,lex,line,sym_table.pstvr[i].i_value.str_offset);
			break;
		}
	}							  									 
	fclose(stf);
	printf("\nSymbol Table stored.\n");
	return 0;
}

int st_sort(STD sym_table, char s_order)
{	
	if(sym_table.st_size == 0)
		return R_FAIL_1;

	return 0;
}