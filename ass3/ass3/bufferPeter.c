/*
File name: buffer.c
Compiler: Microsoft Visual Studio 2012 
Author: Peter Roy 040704738
Course: CST 8152 – Compilers, Lab Section: 401 
Assignment: 1 
Date: September 25 
Professor: Sv. Ranev
Purpose: To build a buffer to read in files
Function list: Buffer * b_create
Buffer *b_addc
int b_reset
void b_destroy
int b_isfull
short b_getsize
short b_getcapacity
int b_setmark
short b_getmark
int b_load
int b_isempty
int b_eob
char b_getc
int b_print
Buffer *b_park
char b_get_r_flag
int b_retract
short b_get_getc_offset
int b_set_getc_offset
char * b_get_chmemloc
*/


#include "buffer.h"

#define FAIL 0

 /********************************************************************
Function Name: 	b_create
Purpose: 		To create the buffer
In parameters:  init_capacity,inc_factor,o_mode
Out parameters:	buffer pointer temp
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/
 Buffer * b_create(short init_capacity,char inc_factor,char o_mode){
	 
	 if (init_capacity<0){
		  return NULL;
	 }
	 else{
		 Buffer * temp;
		 temp =(Buffer *) calloc(1,sizeof(Buffer));/*Calloc temp as a Buffer*/
		 temp->ca_head = (char*) malloc(init_capacity);/*malloc: allocate memory for ca_head*/

		 if (o_mode == 'a' && ((unsigned char)inc_factor>=1 && (unsigned char)inc_factor<=255)){/*mode check for the buffer*/
			 temp->mode = 1;
			 temp->inc_factor = inc_factor;
			
		 }
		 else if (o_mode == 'm' && ((unsigned char)inc_factor>=1 && (unsigned char)inc_factor<=100)){
			 temp->mode= -1;
			 temp->inc_factor = inc_factor;
			 
		 }
		 else if(o_mode== 'f'){
			 temp->mode = 0;
			 temp->inc_factor = 0;
		 }
		 else{
			 return NULL;
		 }
	 
		 
		 temp->capacity=init_capacity;/*initialize the starting capacity of the buffer*/
		 return temp;
	 }

 }
 /********************************************************************
Function Name: 	b_addc
Purpose: 		This function resets the r_flag and tries to add a symbol into the buffer.
In parameters:  pBD,symbol
Out parameters:	Buffer pointer to pBD
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/
 Buffer *b_addc(Buffer *const pBD, char symbol){
		short tcap;/* initializing a temporary capacity*/
		char*nloc;/* initializing a temporary location for ce_head*/
		char*temp;/* initializing a temporary array to reallocate ca_head*/
		short avail;/* initializing a temporary short for the calculation*/
		short inc;/* initializing a temporary short for the calculation*/
		short ncap;/* initializing a temporary short for the calculation*/


	 if(pBD!=NULL){/*check that the buffer is valid*/
		 pBD->r_flag=0;
		 if(pBD && !b_isfull(pBD)){/*check for validity and see if buffer is full*/
			 nloc = pBD->ca_head +pBD->addc_offset;
			 *nloc = symbol;
			 pBD->addc_offset++;/*above lines are used to add symbol into the buffer and then increment the location for next symbol */
		 }
		 else{
		 switch(pBD->mode){
			 case 0:/*used for fixed-size mode*/
				 return NULL;/*return NULL on error*/
				 break;
			 case 1:/*used for additive self-incrementing mode*/
				 tcap = pBD->capacity+(short)pBD->inc_factor;/*casting inc_factor to unsigned char for result*/
				 pBD->capacity=tcap;
				 if(pBD->capacity>0){/*capacity check to make sure it is positive*/
					 temp = (char*) realloc (pBD->ca_head,pBD->capacity);/*reallocating size of ca_head*/
					 if(pBD->ca_head!=temp) {/*check to ensure ca_head is being pointed to*/
						 pBD->r_flag = SET_R_FLAG;
						 pBD->ca_head= temp;
					 }
					 nloc = pBD->ca_head +pBD->addc_offset;
					 *nloc = symbol; 
					 pBD->addc_offset++;/*above lines are used to add symbol into the buffer and then increment the location for next symbol */
					 if(temp==NULL)
						 return NULL;/*return NULL on error*/
				 }
				 else{
					 return NULL;/*return NULL on error*/
				 }
					 break;
			 case -1:/*used for multiplicative self-incrementing mode*/
				 if(pBD->capacity == SHRT_MAX){
					 return NULL;/*return NULL on error*/
				 }
				 else{
					 avail = SHRT_MAX - pBD->capacity;/*SHRT_MAX is total available memory*/
					 inc = avail*(short)pBD->inc_factor/100;/*casting inc_factor to short for result*/
					 ncap= pBD->capacity+inc;
					 if(pBD->capacity<SHRT_MAX && ncap>SHRT_MAX){/*check to see if new capacity will go over max*/
						 pBD->capacity = SHRT_MAX;
					 }
					 else {
						 pBD->capacity = ncap;
					 }
					 
					 temp = (char*) realloc (pBD->ca_head,pBD->capacity);/*reallocating size of ca_head*/
					 if(pBD->ca_head!=temp) {/*check to ensure ca_head is being pointed to*/
						 pBD->r_flag = SET_R_FLAG;
						 pBD->ca_head= temp;
					 }
						 nloc = pBD->ca_head +pBD->addc_offset;
					 *nloc = symbol;
					 pBD->addc_offset++;/*above lines are used to add symbol into the buffer and then increment the location for next symbol */
					 if(temp==NULL)
						 return NULL;/*return NULL on error*/
				 }
				 break;
			 default:
				 return (NULL);/*return NULL on error*/
				 break;
		 }
	 }
		 return (pBD);/*return pointer to pBD*/
	 } 
	 else{
		 return NULL;/*return NULL on error*/
	 }
 
 
 }


  /********************************************************************
Function Name: 	b_reset
Purpose: 		to re initialize variables in buffer without losing the pointer to ca_head 
In parameters:  pBD
Out parameters:	int 1 for SUCCESS -1 for FAIL
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/
 int b_reset(Buffer * const pBD){
	 
	 if(pBD!=NULL){/*check that the buffer is valid*/
		 pBD->addc_offset=0;
		 pBD->eob=0;
		 pBD->getc_offset=0;
		 pBD->mark_offset=0;
		 pBD->r_flag=0;
		 return(SUCCESS);
	 }

	 else{
		 return R_FAIL_1;
	 }

 }



  /********************************************************************
Function Name: 	b_destroy
Purpose: 		Free the memory stored in pBD(the Buffer)
In parameters:  pBD
Out parameters:	none
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/
 void b_destroy(Buffer *const pBD){
	 if (pBD!=NULL){/*check that the buffer is valid*/
	 free(pBD->ca_head);/*freeing allocated memory stored in ca_head*/
	 free(pBD);/*freeing allocated memory stored in Buffer*/
	 }
 }

  /********************************************************************
Function Name: 	b_isfull
Purpose: 		System check to see if the Buffer is full yet
In parameters:  pBD
Out parameters:	int 1 for full and 0 for not full, -1 on error
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/


 int b_isfull(Buffer*const pBD){
	 if (pBD!=NULL){/*check that the buffer is valid*/
	 
		 if(pBD->capacity<=pBD->addc_offset){
			 return(SUCCESS);
		}
		else {
			return(FAIL);
		} 
	 }
	 else{
		 return R_FAIL_1;
	 }
 }

  /********************************************************************
Function Name: 	b_getsize
Purpose: 		This Function returns the current size of the character buffer
In parameters:  pBD
Out parameters:	Size of the character buffer in short form.
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/


 short b_getsize(Buffer*const pBD){
	 if(pBD!=NULL){/*check that the buffer is valid*/
		 return (pBD->addc_offset);/*return current size*/
	 }
	 else{
		 return R_FAIL_1;
	 }
 }

  /********************************************************************
Function Name: 	b_getcapacity
Purpose: 		This function returns the current capacity of the character buffer 
In parameters:  pBD
Out parameters:	short- capacity of the buffer
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/

 short b_getcapacity(Buffer*const pBD){
	 if (pBD!=NULL){/*check that the buffer is valid*/
	 return(pBD->capacity);/*return current capacity*/
	 }
	 else {
		 return R_FAIL_1;
	 }
 }


  /********************************************************************
Function Name: 	b_setmark
Purpose: 		The function sets mark_offset to mark
In parameters:  pBD,mark
Out parameters:	int mark
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/


 int b_setmark(Buffer*const pBD, short mark){
	 if(pBD!=NULL){/*check that the buffer is valid*/
		if(mark<=pBD->capacity && mark > 0){
			pBD->mark_offset=mark;/*set mark_offset by given short mark*/
		}
		return(mark);/*return the mark*/
	 }
	 else{
		 return R_FAIL_1;
	 }
 }

  /********************************************************************
Function Name: 	b_getmark
Purpose: 		This function returns the current mark_offset
In parameters:  pBD
Out parameters:	short returns mark_offset
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/
 short b_getmark(Buffer *const pBD){
	 if(pBD!=NULL){/*check that the buffer is valid*/
	 return(pBD->mark_offset);/*return current mark_offset*/
	 }
	 else{
		 return R_FAIL_1;
	 }
 }

  /********************************************************************
Function Name: 	b_getmode
Purpose: 		
In parameters:  pBD
Out parameters:	int value for mode 0 for fixed-size, 1 for additive self-incrementing, -1 for multiplicative self incrementing
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/

  int b_getmode(Buffer *const pBD){
	  if(pBD!=NULL){/*check that the buffer is valid*/
		return(pBD->mode);/*return the current mode*/
	  }
	  else{
		  return R_FAIL_1;
	  }


  }

   /********************************************************************
Function Name: 	b_load
Purpose: 		this funtion loads each character from the current file
In parameters:  fi,pBD
Out parameters:	int count
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/

  int b_load (FILE*const fi,Buffer *const pBD){
	  int count = 0;/*temporary int to count the amount of chars loaded*/
	  int t = 0;/*temporary int to hold the chars from the file*/
	  if(pBD!=NULL){/*check that the buffer is valid*/
		  t=fgetc(fi);/*gets first character from the file*/ 
		  while(!feof(fi)){/*untill end of file*/
			  
				  if(b_addc(pBD, (char)t) == NULL){/*casts t to char to use this function*/
					 
					  return LOAD_FAIL;
				  }
			  count++;/*count increments to account how many chars were loaded*/
			  t=fgetc(fi);/*gets following chars from file in th while loop*/
			
		  }
		  return (count);

	  }
	  else{
		  return (R_FAIL_1);
	  }
  }

   /********************************************************************
Function Name: 	b_isempty
Purpose: 		this function determines if the Buffer is empty or not
In parameters:  pBD
Out parameters:	int 1 on SUCCESS 0 or -1 on FAIL
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/

  int b_isempty(Buffer *const pBD){
	  if(pBD!=NULL){	/*check that the buffer is valid*/
			if(pBD->addc_offset==0){/**/
				return(SUCCESS);
			}
			else{
				return(FAIL);
			}
		}
	  else{
		  return R_FAIL_1;
	  }
  }

   /********************************************************************
Function Name: 	b_eob
Purpose: 		This function returns eob value
In parameters:  pBD
Out parameters:	int returns eob(end of buffer) 
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/

  int b_eob(Buffer*const pBD){
	  if(pBD==NULL)/*check that the buffer is valid*/
		  return(R_FAIL_1);
	  
	  
	  return pBD->eob;
	  
  }

   /********************************************************************
Function Name: 	b_getc
Purpose: 		this function checks the argument for validity changes the eob to 1 if buffer is full and -1 for any failure
In parameters:  pBD
Out parameters:	char the placement of the current char in the getc_offset
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/

  char b_getc(Buffer * const pBD){
	  char* place;
	  if(pBD!=NULL){/*check that the buffer is valid*/
		  if (pBD->getc_offset == pBD->addc_offset){/*if these two values are equal the buffer is at the end*/
			  pBD->eob=1;/*Buffer is set to end*/
			  return(R_FAIL_1);
		  }
		  else{
			  place =pBD->ca_head+pBD->getc_offset;/*cycle to next position of the Buffer*/
			  pBD->eob=0;/*Buffer is not at the end*/

			  pBD->getc_offset++; /*increment to get to new place in buffer*/

				  return (*place);/*return the new place in getc_offset*/
		  }
	  
	  }
	  else{
		  return(R_FAIL_2);
	  }

  }


   /********************************************************************
Function Name: 	b_print
Purpose: 		This function will print the contents of the buffer
In parameters:  pBD
Out parameters:	int 1 on an empty Buffer -1 on FAIL will return the amount of chars printed on a filled Buffer
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/
  int b_print(Buffer *const pBD){
	  int count = 0;
	  char tm;
	  if(pBD!=NULL){/*check that the buffer is valid*/
		  
		  if(b_isempty(pBD)){
		  printf("The buffer is empty.\n");
		  return SUCCESS;
		  }
		  tm=b_getc(pBD);
		  while (b_eob(pBD)!=1){
			  fprintf(stdout, "%c",tm);
			  count++;
			  tm=b_getc(pBD);
		  }
		  fprintf(stdout,"\n");
		  b_set_getc_offset(pBD,0);
		  return(count);
	  }
	  else{
		  return R_FAIL_1;
	  }


  }

   /********************************************************************
Function Name: 	b_pack
Purpose: 		This function will shrink (or expand ) the size of the buffer to match the current amount of variables 
In parameters:  pBD
Out parameters:	pointer to pBD
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/

  Buffer *b_pack(Buffer *const pBD){
	  char* temp1 = "";
	  
	  if(pBD!=NULL){/*check that the buffer is valid*/
		   
		  pBD->capacity=pBD->addc_offset+1;/*sets capacity to the current size of the Buffers contents*/
		  if (pBD->capacity >0)
		  temp1=(char*) realloc (pBD->ca_head,pBD->capacity);/*re allocates the memory for ca_head to match the new capacity*/
		  
		  if(pBD->ca_head!=temp1){/*check to ensure ca_head is being pointed to*/
			  pBD->r_flag=SET_R_FLAG;
			  pBD->ca_head=temp1;
		  } 
		  pBD->eob = 0;/*sets end of buffer variable to 0 to allow for reprinting*/
		  pBD->r_flag=0;
		  return pBD;/*return a pointer to the Buffer*/
	  }
	  else{
		  return NULL;
	  }
  }

   /********************************************************************
Function Name: 	b_get_r_flag
Purpose: 		This function returns the current r_flag
In parameters:  pBD
Out parameters:	char r_flag
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/
  char b_get_r_flag(Buffer *const pBD){
	  if(pBD!=NULL){/*check that the buffer is valid*/
		return(pBD->r_flag);
	  }
	  else{
		  return R_FAIL_1;
	  }
  }

   /********************************************************************
Function Name: 	b_retract
Purpose: 		This function decrements the getc_offset variable by one
In parameters:  pBD
Out parameters:	int 1 on SUCCESS and -1 on FAIL
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/
  int b_retract(Buffer *const pBD){
	  if(pBD!=NULL){/*check that the buffer is valid*/
	  
		  if(pBD->getc_offset!=0){
		  pBD->getc_offset--;/*decrementing getc_offset*/
		  }

		return (SUCCESS);

	  }
	  else{
		  return R_FAIL_1;
	  }
  }

   /********************************************************************
Function Name: 	b_get_getc_offset
Purpose: 		this function returns the current getc_offset
In parameters:  pBD
Out parameters:	short the current getc_offset
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/
  short b_get_getc_offset(Buffer *const pBD){
	  if(pBD!=NULL){/*check that the buffer is valid*/
		  return(pBD->getc_offset);/*returns the current getc_offset*/
	  }
	  else{
		  return R_FAIL_1;
	  }
  }

   /********************************************************************
Function Name: 	b_set_getc_offset
Purpose: 		this function sets the getc_offset to given short
In parameters:  pBD,offset
Out parameters:	int 1 on SUCCESS and -1 on FAIL
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/

  int b_set_getc_offset(Buffer*const pBD, short offset){
	  if(pBD!=NULL){/*check that the buffer is valid*/
		  pBD->getc_offset=offset;/*set the getc_offset by the given short*/
		  return (SUCCESS);
	  }
	  else{
		  return (R_FAIL_1);
	  }
  }

   /********************************************************************
Function Name: 	b_get_chmemloc
Purpose: 		The function returns a pointer to the location of a character in the character buffer
In parameters:  pBD,offset
Out parameters:	char pointer to the curent mem location
Version: 		1.0
Author: 		Peter Roy
**********************************************************************/

  char * b_get_chmemloc(Buffer *const pBD, short offset){
	  if(pBD!=NULL){/*check that the buffer is valid*/
		  return(pBD->ca_head+offset);/*returning the current mem location*/
	  }
	  else{
		  return NULL;
	  }
  }