#include "StringMatcher.h"
#include "Levenshtein.h"
#include <vector>
#include <iostream>

namespace FuzzyWuzzy
{
	Triple::Triple ( int i , int j , int k )
	{
		i1 = i ; i2 = j ; i3 = k;
	}

	//---------------------------------------------------------------------------

	int& Triple::operator[] ( int idx )
	{
		switch ( idx )
		{
			case 0 : return i1;
			case 1 : return i2;
			case 2 : return i3;
		}

		return i1;
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	
	int SequenceMatcher::Levenshtein ( const std::string& left , const std::string& right , int cost )
	{
		return lev_edit_distance ( left.length()   , left.c_str() ,
								   right.length()  , right.c_str() ,
						           cost );
	}

	//---------------------------------------------------------------------------

	SequenceMatcher::SequenceMatcher ( const std::string& str1 , const std::string& str2 )
	{
		_str1            = str1;
		_str2            = str2;
		_matching_blocks = NULL;

		_reset_cache();  
	};

	//---------------------------------------------------------------------------

	SequenceMatcher::~SequenceMatcher ( void )
	{
		if ( _matching_blocks != NULL )
		{
			delete _matching_blocks;
			_matching_blocks = NULL;
		}
	}

	//---------------------------------------------------------------------------

	void SequenceMatcher::_reset_cache ( void )
	{
		_ratio = _distance = -1;

		if ( _matching_blocks != NULL )
			delete _matching_blocks;

		_matching_blocks = NULL;
	};

	//---------------------------------------------------------------------------

	std::vector<Triple>* SequenceMatcher::get_matching_blocks ( void )
	{
		if ( _matching_blocks != NULL )
			return _matching_blocks;

		_matching_blocks = new std::vector<Triple>();

		int str1_length = _str1.length();
		int str2_length = _str2.length();
		int str1_idx    = 0;

		bool mustSave = false;

		int str2_idx    = -1;

		int buffer_length = 1;

		while ( str1_idx + buffer_length - 1 < str1_length )
		{
			std::string buffer_1 = _str1.substr ( str1_idx , buffer_length );

			size_t pos = _str2.find ( buffer_1 , str2_idx < 0 ? 0 : str2_idx );

			if ( pos == std::string::npos )
			{
				// Check&save last ocurrence
				if ( str2_idx >= 0 && mustSave )
				{
					Triple ocurrence ( str1_idx , str2_idx , buffer_length - 1 );
					_matching_blocks->push_back ( ocurrence );

					str1_idx += buffer_length - 1;
					str2_idx += buffer_length - 1;

					mustSave = false;
				}
				else
				{
					str1_idx++;
				}

				buffer_length = 1;
			}
			else
			{
				str2_idx = pos;
				buffer_length++;
				mustSave = true;
			}
		}

		if ( str2_idx >= 0 && mustSave )
		{
			Triple ocurrence ( str1_idx , str2_idx , buffer_length - 1 );
			_matching_blocks->push_back ( ocurrence );
		}

		Triple dummy ( str1_length , str2_length , 0 );
		_matching_blocks->push_back ( dummy );

		return _matching_blocks;
	}

	//---------------------------------------------------------------------------

	double SequenceMatcher::ratio ( void )
	{
		if ( _ratio == -1 )
		{
			int lensum = _str1.length() + _str2.length();

			if ( lensum == 0 )
			{
				_ratio = 1.0;
			}
			else
			{
				int ldist = Levenshtein ( _str1 , _str2 , 1 );

				_ratio = (double)(lensum - ldist)/(double)lensum;
			}
		}

		return _ratio;
	}

	//---------------------------------------------------------------------------

	int SequenceMatcher::distance ( void )
	{
		if ( _distance == -1 )
		{
			_distance = Levenshtein ( _str1 , _str2 , 0 );
		}
		return _distance;
	}

	//---------------------------------------------------------------------------

}
