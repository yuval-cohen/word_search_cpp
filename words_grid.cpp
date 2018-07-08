#include <cassert>

#include "gen_defs.h"
#include "word_list.h"
#include "words_grid.h"

// explicit instantiation
template WordsGrid<4,4>;

/*******************************************************************************************************************************************************/
/************************************************************ internal functions definition ************************************************************/
/*******************************************************************************************************************************************************/

/**********************************************************************************************************************
* FUNCTION: init_grid                                                                                                *
*                                                                                                                    *
* DESCRIPTION: initialise grid to init value                                                                         *
*                                                                                                                    *
* PARAMETERS: (in/out) grid                                                                                          *
*                                                                                                                    *
* RETURN: none                                                                                                       *
*                                                                                                                    *
* NOTES:                                                                                                             *
*                                                                                                                    *
**********************************************************************************************************************/
template <int X, int Y> template <class type>
void WordsGrid<X, Y>::init_grid(type grid[][Y], int x_len, type init_val)
{
	int i, j;

	for (i = 0; i < x_len; i++)
	{
		for (j = 0; j < Y; j++)
		{
			grid[i][j] = init_val;
		}
	}
}

/**********************************************************************************************************************
* FUNCTION: grid_copy                                                                                                 *
*                                                                                                                    *
* DESCRIPTION: copy one grid to another                                                                              *
*                                                                                                                    *
* PARAMETERS: (out) grid_dest - destination grid                                                                     *
*             (in)  grid_src - source grid                                                                           *
*                                                                                                                    *
* RETURN: none                                                                                                       *
*                                                                                                                    *
* NOTES:                                                                                                             *
*                                                                                                                    *
*********************************************************************************************************************/
template <int X, int Y> template <class type>
void WordsGrid<X, Y>::grid_copy(type grid_dest[][Y], type grid_src[][Y], int size)
{
	int i, j;

	for (i = 0; i < size; i++)
	{
		for (j = 0; j < Y; j++)
		{
			grid_dest[i][j] = grid_src[i][j];
		}
	}
}

/**********************************************************************************************************************
*                                                                                                                     *
* FUNCTION: output_found_words_from_prefix                                                                                *
*                                                                                                                     *
* DESCRIPTION: output (calls output_func) all found words from grid that start with a prefix                          *
*                                                                                                                     *
* PARAMETERS: words_grid - (in) pointer to words grid w/ word list and a grid e.g.:                                   *
*                                                                                  +-------+                          *
*                                                                                  |a|b|a|n|                          *
*                                                                                  +-------+                          *
*                                                                                  |s|d|f|d|                          *
*                                                                                  +-------+                          *
*                                                                                  |g|h|j|o|                          *
*                                                                                  +-------+                          *
*                                                                                  |k|l|n|z|                          *
*                                                                                  +-------+                          *
*                                                                                                                     *
*             word - (in) prefix string, e.g.: "aband"                                                                *
*                                                                                                                     *
*             i,j - (in) the x,y indices of the last character of prefix, e.g.: (1,3)                                 *
*                                                                                                                     *
*             grid_ctrl - (in) grid control that marks which characters are used (part of prefix) and which aren't    *
*                                                                                  e.g.: (x = used; o = unused)       *
*                                                                                  +-------+                          *
*                                                                                  |x|x|x|x|                          *
*                                                                                  +-------+                          *
*                                                                                  |o|o|o|x|                          *
*                                                                                  +-------+                          *
*                                                                                  |o|o|o|o|                          *
*                                                                                  +-------+                          *
*                                                                                  |o|o|o|o|                          *
*                                                                                  +-------+                          *
*                                                                                                                     *
* RETURN: none                                                                                                        *
*                                                                                                                     *
* NOTES:                                                                                                              *
*                                                                                                                     *
***********************************************************************************************************************/
template <int X, int Y>
void WordsGrid<X, Y>::output_found_words_from_prefix(char *word, int i, int j, bool grid_ctrl[][Y], int size)
{
	bool grid_ctrl_next[X][Y];
	char word_next[(X*Y) + 1];
	int word_found;
	size_t word_len;
	int x, y;

	word_found = word_list.FindWord(word);
	if ((word_found == WordList::WORD_FOUND) || (word_found == WordList::PREFIX_FOUND))
	{
		if (word_found == WordList::WORD_FOUND)
		{
			output_func(word);
			no_found_words++;
		}

		/* init x and y to i and j */
		x = i;
		y = j;

		/* in a loop: 1) find next adjacent cell 2) recursive call to output_found_words_from_prefix */
		while (get_next_adjacent_unused_cell(grid_ctrl, X, i, j, &x, &y))
		{
			/* prepare the next work */
			word_len = strlen(word);
			strcpy(word_next, word);
			word_next[word_len] = grid[x][y];
			word_next[word_len + 1] = '\0';

			/* prepare the next grid ctrl */
			grid_copy(grid_ctrl_next, grid_ctrl, X);
			grid_ctrl_next[x][y] = true; // cell used

			output_found_words_from_prefix(word_next, x, y, grid_ctrl_next, X);
		}
	}
	/* else: NOT_FOUND - no need to check further this prefix */
}

/***********************************************************************************************************************
*                                                                                                                     *
* FUNCTION: get_next_adjacent_unused_cell                                                                                 *
*                                                                                                                     *
* DESCRIPTION: get next adjecent unused cell                                                                          *
*                                                                                                                     *
* PARAMETERS: grid_ctrl - (in) grid control that marks which characters are used and which aren't e.g.:               *
*                           (x = used; o = unused):                                                                   *
*                           +-------+                                                                                 *
*                           |x|x|x|x|                                                                                 *
*                           +-------+                                                                                 *
*                           |o|o|o|x|                                                                                 *
*                           +-------+                                                                                 *
*                           |o|o|o|o|                                                                                 *
*                           +-------+                                                                                 *
*                           |o|o|o|o|                                                                                 *
*                           +-------+                                                                                 *
*                                                                                                                     *
*             i,j - (in) the x,y indices of the character to find the next adjacent unused cell from                  *
*                                                                                                                     *
*             x,y - (out) the indices of next unused adjacent character                                               *
*                                                                                                                     *
* RETURN: 0 - cell not found                                                                                          *
*         1 - cell found                                                                                              *
*                                                                                                                     *
* NOTES:                                                                                                              *
*                                                                                                                     *
***********************************************************************************************************************/
template <int X, int Y>
int WordsGrid<X, Y>::get_next_adjacent_unused_cell(bool grid_ctrl[][Y], int size, int i, int j, int *x, int *y)
{
	while (((*x) != (i - 1)) || ((*y) != (j - 1)))
	{
		/*
		check adjacent cell in the following order from X = the current cell.
		(i,j) - current cell
		(x,y) - current/next adjacent cell - initialised to (i,j)
		-------------
		| 8 | 1 | 2 |
		-------------
		| 7 | X | 3 |
		-------------
		| 6 | 5 | 4 |
		-------------
		*/
		if (((*x) == i) && ((*y) == j))
		{
			/* current -> 1 */
			(*x) = (i - 1);
		}
		else if (((*x) == (i - 1)) && ((*y) == j))
		{
			/* 1 -> 2 */
			(*y) = (j + 1);
		}
		else if (((*x) == (i - 1)) && ((*y) == (j + 1)))
		{
			/* 2 -> 3 */
			(*x) = i;
		}
		else if (((*x) == i) && ((*y) == (j + 1)))
		{
			/* 3 -> 4 */
			(*x) = (i + 1);
		}
		else if (((*x) == (i + 1)) && ((*y) == (j + 1)))
		{
			/* 4 -> 5 */
			(*y) = j;
		}
		else if (((*x) == (i + 1)) && ((*y) == j))
		{
			/* 5 -> 6 */
			(*y) = (j - 1);
		}
		else if (((*x) == (i + 1)) && ((*y) == (j - 1)))
		{
			/* 6 -> 7 */
			(*x) = i;
		}
		else /* (((*x) = i) && ((*y) = (j-1))) */
		{
			assert(((*x) == i) && ((*y) == (j - 1)));

			/* 7 -> 8 */
			(*x) = (i - 1);
		}

		if (((*x) >= 0) && ((*x) < size) && ((*y) >= 0) && ((*y) < Y) && (grid_ctrl[*x][*y] == false))
		{
			/* cell exists and unused */
			return 1;
		}
	}

	return 0;
}

/*******************************************************************************************************************************************************/
/************************************************************ external functions definition ************************************************************/
/*******************************************************************************************************************************************************/

template <int X, int Y>
WordsGrid<X, Y>::WordsGrid()
{
	init_grid(grid, X, '\0');
	output_func = NULL;
	no_found_words = 0;
}

template <int X, int Y>
WordsGrid<X, Y>::~WordsGrid()
{
}

template <int X, int Y>
return_code WordsGrid<X, Y>::InitWordList(const char *file_name)
{
	return word_list.BuildWorldList(file_name);
}

/***********************************************************************************************************************
*                                                                                                                     *
* FUNCTION: WordsGrid_SetGrid                                                                                         *
*                                                                                                                     *
* DESCRIPTION: set grid from grid string length [X x Y] (convert to character 2D matrix)            *
*                                                                                                                     *
* PARAMETERS: words_grid - (in/out) pointer to words grid to set its 2D character grid                                *
*             string - (in) string (length X x Y) e.g. (3 x 4): "abcdefghijkl"                      *
*                                                                                                                     *
* RETURN: none                                                                                                        *
*                                                                                                                     *
* NOTES:                                                                                                              *
*                                                                                                                     *
***********************************************************************************************************************/
template <int X, int Y>
void WordsGrid<X, Y>::SetGrid(const char *string)
{
	int i, j, k = 0;

	assert(strlen(string) == (X*Y));

	for (i = 0; i < X; i++)
	{
		for (j = 0; j < Y; j++)
		{
			grid[i][j] = string[k++];
		}
	}
}

/***********************************************************************************************************************
*                                                                                                                     *
* FUNCTION: WordsGrid_SetOutputFunc                                                                                   *
*                                                                                                                     *
* DESCRIPTION: set output function in words grid                                                                      *
*                                                                                                                     *
* PARAMETERS: words_grid - (in/out) pointer to words grid to set its output_func                                      *
*             output_func - (in) pointer to output function                                                           *
*                                                                                                                     *
* RETURN: none                                                                                                        *
*                                                                                                                     *
* NOTES:                                                                                                              *
*                                                                                                                     *
***********************************************************************************************************************/
template <int X, int Y>
void WordsGrid<X, Y>::SetOutputFunc(void(*out_func) (const char *word))
{
	output_func = out_func;
}

/***********************************************************************************************************************
*                                                                                                                     *
* FUNCTION: WordsGrid_OutputFoundWords                                                                                *
*                                                                                                                     *
* DESCRIPTION: output (using output function) found words from word list in grid                                      *
*                                                                                                                     *
* PARAMETERS: words_grid - (in/out) pointer to words grid                                                             *
*                                                                                                                     *
* RETURN: none                                                                                                        *
*                                                                                                                     *
* NOTES: must be called only after SetGrid and SetOutputFunc                   *
*                                                                                                                     *
***********************************************************************************************************************/
template <int X, int Y>
void WordsGrid<X, Y>::OutputFoundWords()
{
	bool grid_ctrl[X][Y];
	char word[(X*Y) + 1];
	int i, j;

	no_found_words = 0;
	for (i = 0; i < X; i++)
	{
		for (j = 0; j < Y; j++)
		{
			init_grid(grid_ctrl, X, false);
			grid_ctrl[i][j] = true; // cell used

			word[0] = grid[i][j];
			word[1] = '\0';

			output_found_words_from_prefix(word, i, j, grid_ctrl, X);
		}
	}
}
