/*
 * File:		io.h
 * Purpose:		Serial Input/Output routines
 *
 */

#ifndef _IO_H
#define _IO_H

/********************************************************************/

char	
in_char(void);

void
out_char(char);

int
char_present(void);

int dprintf (const char *fmt, ...);

int
sprintf(char *, const char *, ... );

#define printf dprintf

/********************************************************************/

#endif
