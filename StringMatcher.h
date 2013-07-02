#ifndef StringMatcherH
#define StringMatcherH

#include <string>
#include <vector>

namespace FuzzyWuzzy
{

	class Triple
	{
	private :

		int i1,i2,i3;
		size_t t;

	public:

		Triple ( int i , int j , int k );

		int& operator[] (int idx);
	};

	//---------------------------------------------------------------------------

	class SequenceMatcher
	{
	private :

		std::string          _str1  , _str2;
		double               _ratio , _distance;
		std::vector<Triple>* _matching_blocks;

		void _reset_cache ( void );

		static int Levenshtein ( const std::string& s1 , const std::string& str2 , int cost );

	public:

		SequenceMatcher ( const std::string& str1 , const std::string& str2 );
		virtual ~SequenceMatcher ( void );
		
		std::vector<Triple>* get_matching_blocks ( void );


		double ratio    ( void );
		int    distance ( void );
	};

}

#endif
