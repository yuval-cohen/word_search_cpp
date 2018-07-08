#ifndef _WORDS_GRID_H_
#define _WORDS_GRID_H_

#include "word_list.h"

// comment in/out one of the following namespaces to compile WordList class from WordListTree or from WordListSet
using namespace WordListSet;
//using namespace WordListTree;

/* words grid struct */
template <int X, int Y>
class WordsGrid
{
private:
	/* word list to search words from */
	WordList word_list;
	
	/* characters grid */
	char grid[X][Y];

	// number of found words
	int no_found_words;
	
	/* output function to be used for all found words in grid */
	void (*output_func) (const char *word);

	template <class type>
	void init_grid (type grid[][Y], int x_len, type init_val);

	template <class type>
	void grid_copy (type grid_dest[][Y], type grid_src[][Y], int size);

	void output_found_words_from_prefix (char *word, int i, int j, bool grid_ctrl[][Y], int size);

	int get_next_adjacent_unused_cell (bool grid_ctrl[][Y], int size, int i, int j, int *x, int *y);

public:

	WordsGrid();
	~WordsGrid();
	return_code InitWordList(const char *file_name);
	void SetGrid(const char *string);
	void SetOutputFunc(void (*out_func) (const char *word));
	void OutputFoundWords();
	int GetNoFoundWords() { return no_found_words; }
};

#endif // _WORDS_GRID_H_
