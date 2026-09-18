/*
------------------------------------------------------------------------------
isaac64.h: definitions for a random number generator
Bob Jenkins, 1996, Public Domain
------------------------------------------------------------------------------
*/


/*
------------------------------------------------------------------------------
 If (flag==TRUE), then use the contents of randrsl[0..255] as the seed.
------------------------------------------------------------------------------
*/
void randinit(/*_ word flag _*/);

void isaac64();


/*
------------------------------------------------------------------------------
 Call rand() to retrieve a single 64-bit random value
------------------------------------------------------------------------------
*/
#define rand64() \
   (!randcnt-- ? (isaac64(), randcnt=RANDSIZ-1, randrsl[randcnt]) : \
                 randrsl[randcnt])


