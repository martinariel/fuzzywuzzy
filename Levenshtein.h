/**
* @see https://github.com/miohtama/python-Levenshtein
*/

#ifndef LevenshteinH
#define LevenshteinH

#include <string.h>
#include <stdlib.h>
#include <cstdlib.h>

namespace FuzzyWuzzy
{
	size_t lev_edit_distance ( size_t len1  , const char* string1,
							   size_t len2  , const char* string2,
							   int    xcost );
}
#endif
