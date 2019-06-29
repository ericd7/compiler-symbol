/**************************************************************************
File name: stable.c
Compiler: MS Visual Studio 2010
Author: Eric Dodds, 040-701-142
Course: CST 8152 – Compilers, Lab Section: 402
Assignment: 2-The Scanner
Date: October 25, 2013
Professor: Sv. Ranev
Purpose: .
Function list: 
STD st_create(int st_size);
int st_install(STD sym_table, char *lexeme, int line);
int st_lookup(STD sym_table,char *lexeme);
int st_update_type(STD sym_table,int vid_offset,char v_type);
int st_update_value(STD sym_table, int vid_offset, InitialValue i_value);
char st_get_type (STD sym_table, int vid_offset);
void st_destroy(STD sym_table);
int st_print(STD sym_table);
static void st_setsize(void);
static void st_incoffset(void);
int st_store(STD sym_table);
int st_sort(STD sym_table, char s_order);
**************************************************************************/

#include "stable.h"
#include "buffer.h"
#include "token.h"
#include <string.h>
#include <stdio.h>

extern STD sym_table; /*External variable for use in void functions and scanner*/

/**************************************************************************
Purpose: Initializes the symbol table
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions:  b_create
Parameters: int st_size
Return value: STD temp
Algorithm: Attempts to allocate dynamic memory to a buffer and the pstvr
**************************************************************************/
STD st_create(int st_size)
{
	STD temp; /*Temporary STD*/
	temp.st_offset = 0; /*Set temps string offset to 0*/
	temp.pstvr = (STVR*)malloc(st_size * sizeof(STVR)); /*Malloc enough space for the pstvr*/
	if(temp.pstvr == NULL) /*Make sure it successfully allocated memory*/
	{
		temp.st_size = 0;
		return temp;
	}
	temp.plsBD = b_create(1000, 15, 'a'); /*Create a temporary buffer*/
	if(temp.plsBD == NULL)
	{
		temp.st_size = 0;
		return temp;
	}
	temp.st_size = st_size;
	return temp;/*return the temp STD*/
}

/**************************************************************************
Purpose: Install a lexeme in the symbol table
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions:  st_lookup() b_addc() b_setmark() b_getmark() st_incoffset()
Parameters: STD sym_table, char *lexeme, int line
Return value: int sym_table.st_offset or int isFound
Algorithm: Attempts to install a lexeme into the symbol table if it is not already found within it
**************************************************************************/
int st_install(STD sym_table, char *lexeme, int line)
{
	int	y; /*For looping*/
	unsigned int x; /*For looping*/ 
	int isFound = 0; /*Stores result of the lookup call*/
	short tempMark; /*stores the mark from the buffer*/
	int stOffset = sym_table.st_offset;

	if(sym_table.st_size == 0)
		return R_FAIL_1;
	
	isFound = st_lookup(sym_table, lexeme); /*Check if the lexeme is in the table already*/
	if(isFound != -1)
		return isFound;

	if(sym_table.st_offset == sym_table.st_size)
	{
		printf("Error: The Symbol Table is full - install failed.\n");
		return R_FAIL_1;
	}
	if(isFound == -1)
	{		
		sym_table.pstvr[stOffset].plex = (sym_table.plsBD->ca_head + sym_table.plsBD->addc_offset);
		
		for(x = 0; x <= strlen(lexeme); x++)
		{
			b_addc(sym_table.plsBD, lexeme[x]);
			if(b_get_r_flag(sym_table.plsBD) == 1)
			{
				sym_table.pstvr[0].plex=sym_table.plsBD->ca_head;
				b_setmark(sym_table.plsBD, strlen(sym_table.pstvr[0].plex) + 1);
				for(x = 1; x < sym_table.st_offset + 1; x++)
				{
					tempMark=b_getmark(sym_table.plsBD);
					sym_table.pstvr[x].plex = &sym_table.plsBD->ca_head[tempMark];
					b_setmark(sym_table.plsBD,tempMark+strlen(sym_table.pstvr[x].plex) + 1);
				}
				return sym_table.st_offset;
			}
		}
		/*Use the appropriate bitwise operations to set the status field of the symbol table position specified by stOffset*/
		sym_table.pstvr[stOffset].o_line = line;
		sym_table.pstvr[stOffset].status_field &= DEFAULTZ;
		sym_table.pstvr[stOffset].status_field |= DEFAULT;

		if(lexeme[(strlen(lexeme)-1)] == '#')
		{
			sym_table.pstvr[stOffset].status_field |= STR_SET;
			sym_table.pstvr[stOffset].status_field |= SET_LSB;
			sym_table.pstvr[stOffset].i_value.str_offset = -1;
		}
		else if(lexeme[0] == 'i' || lexeme[0] == 'o' || lexeme[0] == 'd' || lexeme[0] == 'n')
		{
			sym_table.pstvr[stOffset].status_field |= INT_SET;
			sym_table.pstvr[stOffset].i_value.int_val = 0;
		}
		else
		{
			sym_table.pstvr[stOffset].status_field |= FLT_SET;
			sym_table.pstvr[stOffset].i_value.fpl_val = 0;
		}
		st_incoffset();
		return sym_table.st_offset;
	}
}

/**************************************************************************
Purpose: Check for lexeme in the symbol table
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions:
Parameters: STD sym_table, char *lexeme
Return value: -1 or int x
Algorithm: searches the symbol table for a matching symbol already installed.
**************************************************************************/
int st_lookup(STD sym_table,char *lexeme)
{
	int x; /*For looping*/
	if(sym_table.st_size == 0)
		return R_FAIL_1;

	for(x = sym_table.st_offset-1; x >= 0; x--)
	{
		if(strcmp(lexeme, sym_table.pstvr[x].plex) == 0)
			return x;
	}
	return R_FAIL_1;
}

/**************************************************************************
Purpose: Update the type of an entry
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions: 
Parameters: STD sym_table,int vid_offset,char v_type
Return value: -1 or int vid_offset
Algorithm: Uses bitwise operations to set the type of a symbol at a specified vid_offset
**************************************************************************/
int st_update_type(STD sym_table, int vid_offset, char v_type)
{
	if(sym_table.st_size == 0)
		return R_FAIL_1;

	if((sym_table.pstvr[vid_offset].status_field & SET_LSB) == SUCCESS)
		return R_FAIL_1;

	if(v_type == 'F')
	{
		sym_table.pstvr[vid_offset].status_field |= FLT_SET;
		sym_table.pstvr[vid_offset].status_field |= SET_LSB;
	}
	if(v_type == 'I')
	{
		sym_table.pstvr[vid_offset].status_field |= INT_SET;
		sym_table.pstvr[vid_offset].status_field |= SET_LSB;
	}
	return vid_offset;
}

/**************************************************************************
Purpose: Sets the value of an entry
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions: 
Parameters: STD sym_table, int vid_offset, InitialValue i_value
Return value: -1 or int vid_offset
Algorithm: Sets the i_value of a pstvr given by vid_offset
**************************************************************************/
int st_update_value(STD sym_table, int vid_offset, InitialValue i_value) 
{
	if(sym_table.st_size == 0)
		return R_FAIL_1;

	sym_table.pstvr[vid_offset].i_value.str_offset = i_value.str_offset;
	sym_table.pstvr[vid_offset].i_value.fpl_val = i_value.fpl_val;
	sym_table.pstvr[vid_offset].i_value.int_val = i_value.int_val;

	return vid_offset;
}

/**************************************************************************
Purpose: Gets the type of an entry
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions: 
Parameters: STD sym_table, int vid_offset
Return value: -1 or char corresponding to type
Algorithm: Returns a char designating what type of entry it is based on a bitwise check
**************************************************************************/
char st_get_type (STD sym_table, int vid_offset) 
{
	if(sym_table.st_size == 0)
		return R_FAIL_1;
	
	if((sym_table.pstvr[vid_offset].status_field & FLT_SET) && (sym_table.pstvr[vid_offset].status_field & INT_SET))
		return 'S';
	if(sym_table.pstvr[vid_offset].status_field & INT_SET)
		return 'I';
	if(sym_table.pstvr[vid_offset].status_field & FLT_SET)
		return 'F';

	return R_FAIL_1;
}
 
/**************************************************************************
Purpose: Frees up the memory allocated
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions: st_setsize()
Parameters: STD sym_table
Return value:
Algorithm: Destroys the buffer before freeing the pstvr if it isn't null then set's the size 
**************************************************************************/
void st_destroy(STD sym_table)
{

	b_destroy(sym_table.plsBD);
	
	if(sym_table.pstvr != NULL)
		free(sym_table.pstvr);

	st_setsize();

}

/**************************************************************************
Purpose: Prints the symbol table
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions:
Parameters: STD sym_table
Return value: -1 or int x
Algorithm: Prints the contents of the symbol table and the line number of each entry
**************************************************************************/
int st_print(STD sym_table)
{
	int x; /*Used for looping*/

	if(sym_table.st_size == 0)
		return R_FAIL_1;

	printf("\nSymbol Table\n");
	printf("____________\n\n");
	printf("Line Number Variable Identifier\n");

	for(x = 0; x < sym_table.st_offset; x++)
		printf("%2d          %s\n",sym_table.pstvr[x].o_line, sym_table.pstvr[x].plex);
	return x;
}

/**************************************************************************
Purpose: Sets the size of the extern sym_table
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions:
Parameters: void
Return value:
Algorithm: Sets the external sym_table.st_size to zero
**************************************************************************/
static void st_setsize(void)
{
	sym_table.st_size = 0;
}

/**************************************************************************
Purpose: Increments the offsetof the extern sym_table
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions:
Parameters: void
Return value:
Algorithm: Increments the offset of the external variable
**************************************************************************/
static void st_incoffset(void) 
{
	sym_table.st_offset++;
}

/**************************************************************************
Purpose: Stores the sym_table into an output file
Author: Eric Dodds
History/Versions: 1.0 November 15, 2013 
Called functions:
Parameters: STD sym_table
Return value: -1 or 0
Algorithm: Dumps the contents of the symbol table into a file
**************************************************************************/
int st_store(STD sym_table) 
{
	FILE *file;
	int x, line, length;
	char type;
	char * lex;
	unsigned short int status;
	char *fname = "$stable.ste"; /*The name of the file to be created*/
	int size = sym_table.st_size;

	if(sym_table.st_size == 0)
		return R_FAIL_1;

	if((file = fopen(fname, "wt")) == NULL)
		return R_FAIL_1;

	fprintf(file, "%d", size); /*Prints the size to the file*/

	for(x = 0; x < sym_table.st_offset; x++)
	{
		status = sym_table.pstvr[x].status_field;
		length = strlen(sym_table.pstvr[x].plex);
		line = sym_table.pstvr[x].o_line;
		lex = sym_table.pstvr[x].plex;
		type = st_get_type(sym_table, x);

		switch (type) /*Prints the appropriate things based on the type of the variable*/
		{
		case 'F':
			fprintf(file, " %hX %d %s %d %.2f", status, length, lex, line, sym_table.pstvr[x].i_value.fpl_val);
			break;
		case 'I':
			fprintf(file, " %hX %d %s %d %d", status, length, lex, line, sym_table.pstvr[x].i_value.int_val);
			break;
		case 'S':
			fprintf(file, " %hX %d %s %d %d", status, length, lex, line, sym_table.pstvr[x].i_value.str_offset);
			break;
		}
	}
	fclose(file); /*close the file once done writing*/
	printf("\nSymbol Table stored.\n");
	return 0;
}

int st_sort(STD sym_table, char s_order) 
{
	if(sym_table.st_size == 0)
		return R_FAIL_1;

	return 0;
}