/*
 * File:        assert.c
 * Purpose:     Provide macro for software assertions
 *
 * Notes:       ASSERT macro defined in assert.h calls assert_failed()
 */

#include "common.h"

const char ASSERT_FAILED_STR[] = "Assertion failed in %s at line %d\n";

/********************************************************************/
void
assert_failed(char *file, int line)
{
    int i;
    
    printf(ASSERT_FAILED_STR, file, line);

    while (1)
    {

    }
}
/********************************************************************/
