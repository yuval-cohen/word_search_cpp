#ifndef _WORD_LIST_H_
#define _WORD_LIST_H_

#include <iostream>
#include <fstream>
#include <string>
#include "gen_defs.h"

using std::ifstream;
using std::string;

namespace WordListTree 
{
	/* word list class */
	class WordList
	{
	private:

		/* character node struct */
		class CharNode
		{
		public:
			char ch;
			bool is_word;

			CharNode *adjacent;
			CharNode *next;
			CharNode(char _ch, bool _is_word) : ch(_ch), is_word(_is_word) { adjacent = next = NULL; }
		};

		/* word list character tree */
		CharNode *char_tree;

		/* for statistics */
		size_t no_of_words;
		size_t allocated_nodes;
		size_t freed_nodes;

		return_code add_word_to_char_tree(const string& word);
		int find_word(CharNode *search_tree, const char *word) const;
		size_t free_char_tree(CharNode *char_tree);

	public:

		enum { NOT_FOUND, WORD_FOUND, PREFIX_FOUND };

		WordList();
		~WordList();
		return_code BuildWorldList(const char *file_name);
		int FindWord(const char *word) const;
	};

}

#include <unordered_set>
#include <algorithm>

namespace WordListSet
{
	using std::unordered_set;

	class WordList
	{
	private:
		unordered_set<string> list;

		/* for statistics */
		size_t no_of_words;

	public:

		enum { NOT_FOUND, WORD_FOUND, PREFIX_FOUND };

		WordList() : no_of_words(0) {}
		~WordList() {}
		return_code BuildWorldList(const char *file_name);
		int FindWord(const char *word) const;
	};
}

#endif // _WORD_LIST_H_
