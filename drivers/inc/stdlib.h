/*
 * File:        stdlib.h
 * Purpose:     Function prototypes for standard library functions
 *
 * Notes:
 */

#ifndef _STDLIB_H
#define _STDLIB_H

/********************************************************************
 * Standard library functions
 ********************************************************************/

int
isspace (int);

int
isalnum (int);

int
isdigit (int);

int
isupper (int);

int
strcasecmp (const char *, const char *);

int
strncasecmp (const char *, const char *, int);

char *
strcat (char *, const char *);



char *
strcpy (char *, const char *);



int
strcmp (const char *, const char *);




void
free (void *);

/********************************************************************/

#endif
