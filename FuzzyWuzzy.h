/**
* @see https://github.com/seatgeek/fuzzywuzzy
*/

#ifndef FuzzyWuzzyH
#define FuzzyWuzzyH

#include <string>

namespace FuzzyWuzzy
{
	//###########################
	//# Basic Scoring Functions #
	//###########################

	double ratio                    ( const std::string& s1 , const std::string& s2 );
	double partial_ratio            ( const std::string& s1 , const std::string& s2 );

	//##############################
	//# Advanced Scoring Functions #
	//##############################

	double token_sort_ratio         ( const std::string& s1 , const std::string& s2 );
	double partial_token_sort_ratio ( const std::string& s1 , const std::string& s2 );
	double token_set_ratio          ( const std::string& s1 , const std::string& s2 );
	double partial_token_set_ratio  ( const std::string& s1 , const std::string& s2 );

	//###################
	//# Combination API #
	//###################

	// w is for weighted
	double WRatio ( const std::string& s1 , const std::string& s2 );
}

#endif
