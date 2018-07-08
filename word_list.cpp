#include <cassert>

#include "gen_defs.h"
#include "word_list.h"

/*
 CONCEPT: word list as a tree of character nodes

 e.g. word list with the following words (. means IS_WORD is true; -> mean NEXT pointer; | (n times) means ADJACENT pointer): 
 aa
 aah
 aahed
 aahing
 aahs
 aal
 aalii
 aaliis

 Head->a->a.->h.->e->d.
			  |   |
			  |   i->n->g.
			  |   |
			  |   s.
			  |
			  l.->i->i.->s.    
 */

/*******************************************************************************************************************************************************/
/************************************************************** WordList private functions *************************************************************/
/*******************************************************************************************************************************************************/

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: add_word_to_char_tree                                                                                      *
 *                                                                                                                     *
 * DESCRIPTION: Add new word to character tree                                                                         *
 *              (called in a loop for every word read from word list file to be added to the character tree)           *
 *                                                                                                                     *
 * PARAMETERS: char_tree - (in/out) pointer to character tree pointer                                                  *
 *             word  - (in) word to be added                                                                           *
 *             allocated_nodes - (out) number of new nodes allocated as a result                                       *
 *                                                                                                                     *
 * RETURN: RC_NO_ERROR - no error                                                                                      *
 *         RC_NO_MEM - error, no memory (nodes could not be allocated)                                                 *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
return_code WordListTree::WordList::add_word_to_char_tree (const string& word)
{
   CharNode *nxt_search = char_tree;
   CharNode *nxt_search_prev = NULL;
   CharNode *adj_search;
   CharNode *adj_adj_search;
   CharNode *char_node;
   size_t i, word_len;
   bool is_word, char_added_to_adjacent;

   word_len = word.length();
   for (i = 0; i < word_len; i++)
   {
	  is_word = (i == (word_len - 1));

	  /* no charaters exist in this depth */
	  if (nxt_search == NULL)
	  {
		 char_node = new CharNode(word[i], is_word);
		 if (char_node == NULL)
		 {
			return RC_NO_MEM;
		 }
		allocated_nodes++;

		nxt_search = char_node;
		 
		 /* initialise word list if the very first character */
         if (char_tree == NULL)
		 {
            char_tree = char_node;
		 }
		 else
		 {
			assert(nxt_search_prev);

			nxt_search_prev->next = nxt_search;
		 }

		 nxt_search_prev = nxt_search;
		 nxt_search = nxt_search->next;
	  }
	  
	  /* characters do exist in this depth */
	  else
	  {
		 char_added_to_adjacent = false;
		 
		 /* search ADJACENT list to find the correct spot to add character to list (if character does not exist) - ADJACENT list is kept sorted by character value */
		 for (adj_search = nxt_search; !char_added_to_adjacent; adj_search = adj_search->adjacent)
		 {
			assert(adj_search);
			adj_adj_search = adj_search->adjacent;
			
			if (adj_search->ch == (word[i]))
			{
			   nxt_search_prev = adj_search;
			   nxt_search = nxt_search_prev->next;
			   char_added_to_adjacent = true; /* character already exists - do nothing */
			}
			
			else if (adj_search->ch < (word[i]))
			{
			   if (adj_adj_search == NULL)
			   {
				  /* add character to the end of ADJACENT list */
				  char_node = new CharNode(word[i], is_word);
				  if (char_node == NULL)
				  {
					 return RC_NO_MEM;
				  }
				  allocated_nodes++;

				  adj_search->adjacent = char_node;

				  nxt_search_prev = char_node;
				  nxt_search = nxt_search_prev->next;
				  char_added_to_adjacent = true;

			   }
			   else if (adj_adj_search->ch > (word[i]))
			   {
				  /* add character between adj_search and adj_adj_search */
				  char_node = new CharNode(word[i], is_word);
				  if (char_node == NULL)
				  {
					 return RC_NO_MEM;
				  }
				  allocated_nodes++;

				  adj_search->adjacent = char_node;
				  char_node->adjacent = adj_adj_search;

				  nxt_search_prev = char_node;
				  nxt_search = nxt_search_prev->next;
				  char_added_to_adjacent = true;
			   }
			   /* else (adj_adj_search->ch <= (word[i])) - to be handled in the next loop iteration */
			}
			else /* (adj_search->ch > (word[i])) */
			{
			   /* add character at the start of ADJACENT list */
			   char_node = new CharNode(word[i], is_word);
			   if (char_node == NULL)
			   {
				  return RC_NO_MEM;
			   }
			   allocated_nodes++;

			   nxt_search_prev->next = char_node;
			   char_node->adjacent = nxt_search;

			   nxt_search_prev = char_node;
			   nxt_search = nxt_search_prev->next;
			   char_added_to_adjacent = true;
			}
		 }
	  }

   }

   return RC_NO_ERROR;
}

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordList_FindWord                                                                                         *
 *                                                                                                                     *
 * DESCRIPTION: search to find a word in a word list's character tree                                                  *
 *                                                                                                                     *
 * PARAMETERS: char_tree (in) - pointer to character tree of word list to search the word from                         *
 *             word (in) - word to search/find                                                                         *
 *                                                                                                                     *
 * RETURN: WORD_FOUND - word is found                                                                                  *
 *         PREFIX_FOUND - word is found as a prefix of another word (e.g. "aband" as a prefix of "abandon")            *
 *         NOT_FOUND - word not found (also not found as a prefix)                                                     *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
int WordListTree::WordList::find_word (CharNode *search_tree, const char *word) const
{
   size_t word_len;
   CharNode *search;
   
   word_len = strlen(word);

   assert(word_len);
   
   for (search = search_tree; search != NULL; search = search->adjacent)
   {
	  if (search->ch == word[0])
	  {
		 if (word_len == 1)
		 {
			 if (search->is_word)
			 {
				 return WORD_FOUND;
			 }
			 else 
			 {
				 return PREFIX_FOUND;
			 }
		 }
		 else
		 {
			return find_word(search->next, word+1);
		 }
	  }
	  else if (search->ch > word[0])
	  {
		 return NOT_FOUND;
	  }
	  /* else - search->ch < word[0] - continue */
   }

   return NOT_FOUND;
}

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordList_FreeCharTree                                                                                     *
 *                                                                                                                     *
 * DESCRIPTION: free memory dynamically allocated for word list's character tree                                       *
 *                                                                                                                     *
 * PARAMETERS: char_tree (in) - pointer to character tree                                                              *
 *                                                                                                                     *
 * RETURN: none                                                                                                        *
 *                                                                                                                     *
 * NOTES: must be called to free memory at the end of word list life cycle                                             *
 *                                                                                                                     *
 ***********************************************************************************************************************/
size_t WordListTree::WordList::free_char_tree (CharNode *char_tree)
{
	size_t no_of_freed_nodes = 0;

	if (char_tree != NULL)
	{
		if (char_tree->next != NULL)
		{
			no_of_freed_nodes += free_char_tree(char_tree->next);
		}

		if (char_tree->adjacent != NULL)
		{
			no_of_freed_nodes += free_char_tree(char_tree->adjacent);
		}

		char_tree->next = NULL;
		char_tree->adjacent = NULL;

		delete char_tree;

		no_of_freed_nodes++;
	}

	return no_of_freed_nodes;
}


/*******************************************************************************************************************************************************/
/************************************************************ external functions definition ************************************************************/
/*******************************************************************************************************************************************************/

WordListTree::WordList::WordList()
{
	char_tree = NULL;

	no_of_words = 0;
	allocated_nodes = 0;
	freed_nodes = 0;
}

WordListTree::WordList::~WordList()
{
	freed_nodes = free_char_tree(char_tree);
}

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: BuildWorldList                                                                                            *
 *                                                                                                                     *
 * DESCRIPTION: Build character tree for word list from a word list input file                                         *
 *                                                                                                                     *
 * PARAMETERS: word_list (in/out) - pointer to word list to build its character tree                                   *
 *             file - (in) pointer to (already open) input file to read word list from                                 *
 *                          convernsion: <word><LF><word><LF><word><LF>...                                             *
 *                                                                                                                     *
 * RETURN:    RC_NO_MEM - no memory                                                                                    *
 *            RC_EOF - no error (end of file)                                                                          *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
return_code WordListTree::WordList::BuildWorldList(const char *file_name)
{
	return_code ret_code = RC_EOF;
	ifstream file;
	file.open(file_name);
	if (file.is_open() == false)
	{
		std::cerr << "Can't open file: " << file_name << std::endl;
		return RC_FILE_NOT_FOUND;
	}

	string word;
	int count = 0;
	getline(file, word);
	while(file) // ToDo: check read failures
	{
		ret_code = add_word_to_char_tree(word);
		if (ret_code != RC_NO_ERROR)
			break;
		else
			ret_code = RC_EOF;

		no_of_words++;
		getline(file, word);
	}
	file.close();

	return ret_code;
}

/***********************************************************************************************************************
 *                                                                                                                     *
 * FUNCTION: WordList_FindWord                                                                                         *
 *                                                                                                                     *
 * DESCRIPTION: search to find a word in a word list's character tree                                                  *
 *                                                                                                                     *
 * PARAMETERS: char_tree (in) - pointer to character tree of word list to search the word from                         *
 *             word (in) - word to search/find                                                                         *
 *                                                                                                                     *
 * RETURN: WORD_FOUND - word is found                                                                                  *
 *         PREFIX_FOUND - word is found as a prefix of another word (e.g. "aband" as a prefix of "abandon")            *
 *         NOT_FOUND - word not found (also not found as a prefix)                                                     *
 *                                                                                                                     *
 * NOTES:                                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/
int WordListTree::WordList::FindWord (const char *word) const
{
   size_t word_len;
   CharNode *search;
   
   word_len = strlen(word);

   assert(word_len);
   
   for (search = char_tree; search != NULL; search = search->adjacent)
   {
	  if (search->ch == word[0])
	  {
		 if (word_len == 1)
		 {
			 if (search->is_word)
			 {
				 return WORD_FOUND;
			 }
			 else 
			 {
				 return PREFIX_FOUND;
			 }
		 }
		 else
		 {
			return find_word(search->next, word+1);
		 }
	  }
	  else if (search->ch > word[0])
	  {
		 return NOT_FOUND;
	  }
	  /* else - search->ch < word[0] - continue */
   }

   return NOT_FOUND;
}

/***********************************************************************************************************************
*                                                                                                                     *
* FUNCTION: BuildWorldList                                                                                            *
*                                                                                                                     *
* DESCRIPTION: Build word list as vector<string> from a word list input file                                          *
*                                                                                                                     *
* PARAMETERS: file_name (in) - word list file name                                                                    *
*                                                                                                                     *
* RETURN:    RC_FILE_NOT_FOUND - file not found                                                                       *
*            RC_EOF - no error (end of file)                                                                          *
*                                                                                                                     *
* NOTES:                                                                                                              *
*                                                                                                                     *
***********************************************************************************************************************/
return_code WordListSet::WordList::BuildWorldList(const char* file_name)
{
	ifstream file;
	file.open(file_name);
	if (file.is_open() == false)
	{
		std::cerr << "Can't open file: " << file_name << std::endl;
		return RC_FILE_NOT_FOUND;
	}

	string word;
	int count = 0;
	getline(file, word);
	while (file) // while input is good
	{
		list.insert(word);
		no_of_words++;
		getline(file, word);
	}
	file.close();

	return RC_EOF;
}

/***********************************************************************************************************************
*                                                                                                                     *
* FUNCTION: FindWord                                                                                                  *
*                                                                                                                     *
* DESCRIPTION: find a word in a word list                                                                             *
*                                                                                                                     *
* PARAMETERS: word (in) - word to search/find                                                                         *
*                                                                                                                     *
* RETURN: WORD_FOUND - word is found                                                                                  *
*         PREFIX_FOUND - word not found (might be found as a prefix)                                                  *
*                                                                                                                     *
* NOTES:                                                                                                              *
*                                                                                                                     *
***********************************************************************************************************************/
int WordListSet::WordList::FindWord(const char *word) const
{
	if (list.find(string(word)) != list.end())
		return WORD_FOUND;

	return PREFIX_FOUND;
}
