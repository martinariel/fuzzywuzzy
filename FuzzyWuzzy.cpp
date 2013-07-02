#include "FuzzyWuzzy.h"
#include "StringMatcher.h"
#include "RegularExpressions/regexp/Matcher.h"
#include "RegularExpressions/regexp/Pattern.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>

namespace FuzzyWuzzy
{
	const std::string REG_TOKEN = "[\\w\\d]+";
	Pattern* p = Pattern::compile ( REG_TOKEN );

	//---------------------------------------------------------------------------
	
	double ratio ( const std::string& s1 , const std::string& s2 )
	{
		SequenceMatcher m ( s1 , s2 );
		return  100.0 * m.ratio();
	}

	//---------------------------------------------------------------------------

	double partial_ratio ( const std::string& s1 , const std::string& s2 )
	{
		std::string shorter, longer;

		if ( s1.length() <= s2.length())
		{
			shorter = s1; longer = s2;
		}
		else
		{
			shorter = s2; longer = s1;
		}

		SequenceMatcher m ( shorter, longer);

		std::vector<Triple>* blocks = m.get_matching_blocks();

		/* each block represents a sequence of matching characters in a string
		* of the form (idx_1, idx_2, len)
		* the best partial match will block align with at least one of those blocks
		*   e.g. shorter = "abcd", longer = XXXbcdeEEE
		*   block = (1,3,3)
		*   best score === ratio("abcd", "Xbcd")
		*/
		double max = -1.0;

		for ( std::vector<Triple>::const_iterator it = blocks->begin(); it != blocks->end() ; ++it )
		{
			Triple block = *it;

			int         long_start  = ( block[1] - block[0] > 0 ) ? block[1] - block[0] : 0;
			int         long_end    = long_start + shorter.length();
			std::string long_substr = longer.substr ( long_start , long_end - long_start );

			SequenceMatcher m2 ( shorter , long_substr );
			
			double r = m2.ratio();

			if ( r > 0.995 )
			{
				return 100.0;
			}
			else if ( r > max || max < 0 )
			{
				max = r;
			}
		}

		return  max * 100.0;
	}

	//---------------------------------------------------------------------------

	void sorted ( std::vector<std::string>& vector )
	{
		sort ( vector.begin() , vector.end() );
	}

	//---------------------------------------------------------------------------

	std::string trim ( const std::string& str,
					   const std::string& whitespace = " \t")
	{
		const auto strBegin = str.find_first_not_of(whitespace);
		if (strBegin == std::string::npos)
			return ""; // no content

		const auto strEnd   = str.find_last_not_of(whitespace);
		const auto strRange = strEnd - strBegin + 1;

		return str.substr(strBegin, strRange);
	}

	//---------------------------------------------------------------------------

	std::string join ( const std::vector<std::string>& v , const std::string& token = " " )
	{
		std::string result;
		
		for (std::vector<std::string>::const_iterator i = v.begin(); i != v.end(); i++)
		{
			if (i != v.begin()) result += token;
			result += trim ( *i );
		}

		return trim ( result );
	}

	//---------------------------------------------------------------------------

	/* Sorted Token
	*   find all alphanumeric tokens in the string
	*   sort those tokens and take ratio of resulting joined strings
	*   controls for unordered string elements
	*/

	double _token_sort ( const std::string& s1 , const std::string& s2 , bool partial )
	{
		Matcher* m1 = p->createMatcher ( s1 );
		Matcher* m2 = p->createMatcher ( s2 );

		// pull tokens
		std::vector<std::string> tokens1 = m1->findAll();
		std::vector<std::string> tokens2 = m2->findAll();

		delete m1;
		delete m2;

		// sort tokens and join
		sorted ( tokens1 );
		sorted ( tokens2 );

		std::string sorted1 = trim ( join ( tokens1 ) );
		std::string sorted2 = trim ( join ( tokens2 ) );

		return partial ?
			partial_ratio ( sorted1 , sorted2 ) :
			        ratio ( sorted1 , sorted2 );

	}

	//---------------------------------------------------------------------------

	double token_sort_ratio ( const std::string& s1 , const std::string& s2 )
	{
		return _token_sort ( s1 , s2 , false );
	}

	//-------------------------------------------------------------------------

	double partial_token_sort_ratio ( const std::string& s1 , const std::string& s2 )
	{
		return _token_sort ( s1 , s2 , true );
	}

	//-------------------------------------------------------------------------

	void _set ( std::vector<std::string>& v )
	{
		std::set<std::string> s;
		unsigned size = v.size();
		for( unsigned i = 0; i < size; ++i ) s.insert( v[i] );
		v.assign( s.begin(), s.end() );
	}

	//-------------------------------------------------------------------------

	/* Token Set
	*   find all alphanumeric tokens in each string...treat them as a set
	*   construct two strings of the form
	*       <sorted_intersection><sorted_remainder>
	*   take ratios of those two strings
	*   controls for unordered partial matches
	*/
	double _token_set ( const std::string& s1 , const std::string& s2 , bool partial )
	{
		Matcher* m1 = p->createMatcher ( s1 );
		Matcher* m2 = p->createMatcher ( s2 );

		// pull tokens
		std::vector<std::string> v_tokens1 = m1->findAll();
		std::vector<std::string> v_tokens2 = m2->findAll();

		delete m1;
		delete m2;

		_set   ( v_tokens1 );
		_set   ( v_tokens2 );
		sorted ( v_tokens1 );
		sorted ( v_tokens2 );


		std::vector<std::string>::iterator it;
		std::vector<std::string> intersection ( std::min ( v_tokens1.size() , v_tokens2.size() ) );
		std::vector<std::string> diff1to2     ( v_tokens1.size() );
		std::vector<std::string> diff2to1     ( v_tokens2.size() );

		it = set_intersection   ( v_tokens1.begin() , v_tokens1.end() ,
								  v_tokens2.begin() , v_tokens2.end() ,
								  intersection.begin() );

		it = set_difference 	( v_tokens1.begin() , v_tokens1.end() ,
								  v_tokens2.begin() , v_tokens2.end() ,
								  diff1to2.begin() );

		it = set_difference 	( v_tokens2.begin() , v_tokens2.end() ,
								  v_tokens1.begin() , v_tokens1.end() ,
								  diff2to1.begin()  );

		sorted ( intersection );
		sorted ( diff1to2     );
		sorted ( diff2to1     );

		std::string sorted_sect = join ( intersection );
		std::string sorted_1to2 = join ( diff1to2     );
		std::string sorted_2to1 = join ( diff2to1     );

		std::string combined_1to2 = sorted_sect + " " + sorted_1to2;
		std::string combined_2to1 = sorted_sect + " " + sorted_2to1;

		// Trim
		sorted_sect   = trim ( sorted_sect   );
		combined_1to2 = trim ( combined_1to2 );
		combined_2to1 = trim ( combined_2to1 );

		std::vector<double> pairwise;

		pairwise.push_back ( ratio ( sorted_sect   , combined_1to2 ) );
		pairwise.push_back ( ratio ( sorted_sect   , combined_2to1 ) );
		pairwise.push_back ( ratio ( combined_1to2 , combined_2to1 ) );

		return *max_element ( pairwise.begin() , pairwise.end() );
	}

	//-------------------------------------------------------------------------

	double token_set_ratio ( const std::string& s1 , const std::string& s2 )
	{
		return _token_set ( s1 , s2 , false );
	}

	//-------------------------------------------------------------------------

	double partial_token_set_ratio ( const std::string& s1 , const std::string& s2 )
	{
		return _token_set ( s1 , s2 , true );
	}

	//-------------------------------------------------------------------------
	
	double WRatio ( const std::string& s1 , const std::string& s2 )
	{
		// Validate string
		if ( s1.length() == 0 || s2.length() == 0 )
			return 0;

		// should we look at partials?
		bool   try_partial   = true;
		double unbase_scale  = 0.95;
		double partial_scale = 0.90;

		double base = ratio ( s1 , s2 );

		double len_ratio = (double) std::max ( s1.length() , s2.length() ) /
						   (double) std::min ( s1.length() , s2.length() );

		// if strings are similar length, don't use partials
		if ( len_ratio < 1.5 ) try_partial = false;

		// if one string is much much shorter than the other
		if ( len_ratio > 8 )  partial_scale = 0.6;

		if ( try_partial )
		{
			double partial = partial_ratio            ( s1 , s2 ) * partial_scale;
			double ptsor   = partial_token_sort_ratio ( s1 , s2 ) * unbase_scale * partial_scale;
			double ptser   = partial_token_set_ratio  ( s1 , s2 ) * unbase_scale * partial_scale;

			return std::max (
					 std::max ( base  , partial ) ,
					 std::max ( ptsor , ptser   )
			);
		}
		else
		{
			double tsor = token_sort_ratio ( s1 , s2 ) * unbase_scale;
			double tser = token_set_ratio  ( s1 , s2 ) * unbase_scale;

			return std::max ( tsor , tser );
		}
	}
}

