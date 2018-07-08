#include <iostream>

#include "gen_defs.h"
#include "words_grid.h"

//#define _MY_DEBUG_

const int GRID_X_LEN = 4;
const int GRID_Y_LEN = 4;


static void words_grid_output_func (const char *word);

static void words_grid_output_func (const char *word)
{
	using std::cout;
	using std::endl;

	cout << word << endl;
}

int main (int argc, char* argv[])
{
	using std::cout;
	using std::cerr;
	using std::endl;

	WordsGrid<GRID_X_LEN,GRID_Y_LEN> words_grid;
	return_code ret_code;

#ifndef _MY_DEBUG_
	if ((argc != 3) || (strlen(argv[2]) != (GRID_X_LEN * GRID_Y_LEN)))
	{
		cout << "Usage: word_search.exe <word-list-file> <gird-as-" << GRID_X_LEN * GRID_Y_LEN << "-chars-string>\n";
	}
	else
#endif
	{
#ifndef _MY_DEBUG_
		ret_code = words_grid.InitWordList(argv[1]);
#else
		ret_code = words_grid.InitWordList("word_x.list");
#endif
		if (ret_code != RC_FILE_NOT_FOUND)
		{
			if (ret_code == RC_EOF)
			{
#ifndef _MY_DEBUG_
				words_grid.SetGrid(argv[2]);
#else
				words_grid.SetGrid("mikayuvaaahlmich"); // more examples: aahebcidbengmika, abombanilenesess, mikayuvaaahlmich
#endif

				words_grid.SetOutputFunc(words_grid_output_func);
				words_grid.OutputFoundWords();
				cout << words_grid.GetNoFoundWords() << " words found\n";
			 
				ret_code = RC_NO_ERROR;
			}
		}

		if (ret_code != RC_NO_ERROR)
		{
			cerr << "ERROR: error code #" << ret_code << endl;
		}
	}

	return 0;
}
