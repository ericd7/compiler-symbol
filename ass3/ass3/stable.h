/**************************************************************************
File name: stable.h
Compiler: MS Visual Studio 2010
Author: Svillen Ranev, modified by Eric Dodds, 040701142 
Course: CST8152 – Compilers, Lab Section: 402
Assignment: 3 - Symbol Table
Date: November 15, 2013
Professor: Sv. Ranev
Purpose: Declares constants and functions
**************************************************************************/
#ifndef STABLE_H_
#define STABLE_H_

#include "buffer.h"

#define DEFAULTZ	0x0000	 /* 0000 0000 0000 0000 */
#define DEFAULT     0xFFF8   /* 1111 1111 1111 1000 */
#define SET_LSB     0x0001   /* 0000 0000 0000 0001 */
#define FLT_SET		0x0004	 /* 0000 0000 0000 0100 */
#define INT_SET		0x0002	 /* 0000 0000 0000 0010 */
#define STR_SET		0x0006	 /* 0000 0000 0000 0110 */

typedef union InitialValue { 
 int int_val; /* integer variable initial value */ 
 float fpl_val; /* floating-point variable initial value */ 
 int str_offset; /* string variable initial value (offset) */ 
} InitialValue;

typedef struct SymbolTableVidRecord { 
 unsigned short status_field; /* variable record status field*/ 
 char * plex; /* pointer to lexeme (VID name) in CA */ 
 int o_line; /* line of first occurrence */ 
 InitialValue i_value; /* variable initial value */ 
 size_t ds_offset;/*offset from the beginning of data segment*/ 
} STVR;

typedef struct SymbolTableDescriptor { 
 STVR *pstvr; /* pointer to array of STVR */ 
 int st_size; /* size in number of STVR elements */ 
 int st_offset; /*offset in number of STVR elements */ 
 Buffer *plsBD; /* pointer to the lexeme storage buffer descriptor */ 
} STD;

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

#endif
