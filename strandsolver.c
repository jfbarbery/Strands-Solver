#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 19
#define MAX_LETTER_SOLUTION 10
#define ROWS 8
#define COLS 6
#define NUM_DIGITS 7
#define NUM_WORDS 274927

char** read_board(char* argv[]);
char* read_word(FILE* file);
char** strand_solver(char** board);
void backtrack(char** board, char** colors, int n, int r, int c, int** selected);
int word_exists(char* target);
int coords_in_selected(int r, int c, int** selected, int n);
void print_board(char** board, char** colors, int** selected, int n);
char* get_selected_str(char** board, int** selected, int n);
char** set_colors();
void remove_nl(char* word);
int formatted_str_to_int(char* str);
int tentopower(int n);

int main(int argc, char* argv[])
{
	char** board = read_board(argv);
	strand_solver(board);
	for (int i = 0; i < ROWS; i++)
		free(board[i]);
	free(board);
	return 0;
}

// Reads the command line Strands board parameter
char** read_board(char* argv[])
{
	int offset = ROWS;
	char** board = (char**) malloc(sizeof(char*) * ROWS);
	int i = 0;
	while (i < ROWS)
	{
		board[i] = (char*) malloc(sizeof(char) * (COLS+1));
		i++;
	}
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			board[r][c] = argv[0][(r*COLS)+c+offset];
		}
		board[r][ROWS] = '\0';
	}
	return board;
}

// Reads a single word from the file
char* read_word(FILE* file)
{
	char* word = (char*) malloc(sizeof(char) * (MAX_WORD_LENGTH+1));
	int i = 0;
	char ch;
	while ((ch = fgetc(file)) != '\n')
	{
		if (ch == EOF)
		{
			ch = '\0';
			break;
		}
		if (i >= MAX_WORD_LENGTH)
		{
			printf("We have ran into a word that is the maximum length! This word might be too long.\n");
			ch = '\0';
			break;
		}
		word[i] = ch;
		i++;
	}
	word[i] = ch;
	return word;
}

// Takes in a 2D array of characters that represent a game of Strands.
// Returns a possible list of words that may be correct words in the game.
char** strand_solver(char** board)
{
	char** colors = set_colors();
	
	// Uses backtracking and binary searching in a dictionary to come up with possible words
	// For this purpose, a "full solution" is the case when a word is found in the dictionary
	print_board(board, colors, NULL, 0);
	// Initialize selected letters in partial solution
	int** selected = (int**) malloc(sizeof(int*) * MAX_LETTER_SOLUTION); // Max selected letters
	for (int i = 0; i < MAX_LETTER_SOLUTION; i++)
	{
		selected[i] = (int*) malloc(sizeof(int) * 2); // Coordinate pair, row and col
	}
	backtrack(board, colors, 0, 0, 0, selected);
	for (int i = 0; i < MAX_LETTER_SOLUTION; i++)
	{
		free(selected[i]);
	}
	free(selected);
	for (int i = 0; i < 7; i++)
	{
		free(colors[i]);
	}
	free(colors);
	return NULL;
}

void backtrack(char** board, char** colors, int n, int r, int c, int** selected)
{
	sleep(1);
	print_board(board, colors, selected, n);
	// Here is where we try and see if our current partial solution could be a real word
	char* string = get_selected_str(board, selected, n);
	if (word_exists(string))
	{
		printf("Could this be a word? |%s|\n", string);
	}
	
	free(string);
	// Base case
	if (n == MAX_LETTER_SOLUTION) return;
	
	// Try to go in every direction
	
	// North
	if (r-1 >= 0 && !coords_in_selected(r-1, c, selected, n))
	{
		selected[n][0] = r-1;
		selected[n][1] = c;
		backtrack(board, colors, n+1, r-1, c, selected);
	}
	// East
	if (c+1 < 6 && !coords_in_selected(r, c+1, selected, n))
	{
		selected[n][0] = r;
		selected[n][1] = c+1;
		backtrack(board, colors, n+1, r, c+1, selected);
	}
	// South
	if (r+1 < 8 && !coords_in_selected(r+1, c, selected, n))
	{
		selected[n][0] = r+1;
		selected[n][1] = c;
		backtrack(board, colors, n+1, r+1, c, selected);
	}
	// West
	if (c-1 >= 0 && !coords_in_selected(r, c-1, selected, n))
	{
		selected[n][0] = r;
		selected[n][1] = c-1;
		backtrack(board, colors, n+1, r, c-1, selected);
	}
	// North East
	if (r-1 >= 0 && c+1 < 6 && !coords_in_selected(r-1, c+1, selected, n))
	{
		selected[n][0] = r-1;
		selected[n][1] = c+1;
		backtrack(board, colors, n+1, r-1, c+1, selected);
	}
	// South East
	if (r+1 < 8 && c+1 < 6 && !coords_in_selected(r+1, c+1, selected, n))
	{
		selected[n][0] = r+1;
		selected[n][1] = c+1;
		backtrack(board, colors, n+1, r+1, c+1, selected);
	}
	// South West
	if (r+1 < 8 && c-1 >= 0 && !coords_in_selected(r+1, c-1, selected, n))
	{
		selected[n][0] = r+1;
		selected[n][1] = c-1;
		backtrack(board, colors, n+1, r+1, c-1, selected);
	}
	// North West
	if (r-1 >= 0 && c-1 >= 0 && !coords_in_selected(r-1, c-1, selected, n))
	{
		selected[n][0] = r-1;
		selected[n][1] = c-1;
		backtrack(board, colors, n+1, r-1, c-1, selected);
	}
}

// Checks the word list for the string parameter
int word_exists(char* target)
{
	FILE* words_ptr = fopen("./en.txt", "r");
	if (words_ptr == NULL) return 0;
	FILE* offset_ptr = fopen("./preprocessed.txt", "r");
	if (offset_ptr == NULL) return 0;
	// Binary Search On Line Number
	int low = 0;
	int high = NUM_WORDS-1;
	int mid = (low+high)/2;
	// Get the middle offset by multiplying the middle line number * num digits per line (+1 for nl)
	int offset_for_offset = mid * (NUM_DIGITS+1);
	fseek(offset_ptr, offset_for_offset, SEEK_SET);
	char* offset = (char*) malloc(sizeof(char) * (NUM_DIGITS+2));
	fgets(offset, 9, offset_ptr);
	remove_nl(offset);
	int int_offset = formatted_str_to_int(offset);
	free(offset);
	fseek(words_ptr, int_offset, SEEK_SET);
	char* guess = (char*) malloc(sizeof(char) * 30);
	fgets(guess, 30, words_ptr);
	remove_nl(guess);
	int guesses = 0;
	int res = 0;
	while (!res)
	{
		int res = strcmp(target, guess);
		// Target is lexographically before this guess
		if (res < 0)
		{
			high = mid;
		}
		// Target is lexographically after this guess
		else if (res > 0)
		{
			low = mid;
		}
		// Found the word
		else
		{
			free(guess);
			fclose(words_ptr);
			fclose(offset_ptr);
			return 1;
		}
		int newmid = (low + high)/2;
		// We couldn't find the word
		if (mid == newmid) return 0;
		mid = newmid;
		// Go to the line number in our offset file
		offset_for_offset = mid * (NUM_DIGITS+1);
		fseek(offset_ptr, offset_for_offset, SEEK_SET);
		// Get the string version of the offset for the word
		offset = (char*) malloc(sizeof(char) * 9);
		fgets(offset, 9, offset_ptr);
		// Remove the newline from fgets
		remove_nl(offset);
		// Turn the word offset (a string of decimal digits) into an int type
		int int_offset = formatted_str_to_int(offset);
		free(offset);
		// Go to the offset in the words file
		fseek(words_ptr, int_offset, SEEK_SET);
		// Erase the last guess
		free(guess);
		guess = (char*) malloc(sizeof(char) * 30);
		fgets(guess, 30, words_ptr);
		remove_nl(guess);
		guesses++;
	}
	free(guess);
	fclose(words_ptr);
	fclose(offset_ptr);
}

// Returns 1 if r and c appear as a coordinate pair in selected, 0 otherwise
int coords_in_selected(int r, int c, int** selected, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (selected[i][0] == r && selected[i][1] == c) return 1;
	}
	return 0;
}

void print_board(char** board, char** colors, int** selected, int n)
{
	printf("%s+", colors[4]);
	printf("%s-----------%s+\n", colors[5], colors[4]);
	for (int r = 0; r < ROWS; r++)
	{
		printf("%s|", colors[5]);
		for (int c = 0; c < COLS; c++)
		{
			int color_to_print = 2;
			if (n != 0 && coords_in_selected(r, c, selected, n)) color_to_print = 0;
			printf("%s%c", colors[color_to_print], board[r][c]);
			if (c == COLS - 1) continue;
			printf(" ");
		}
		printf("%s|", colors[5]);
		printf("\n");
	}
	printf("%s+", colors[4]);
	printf("%s-----------%s+\n", colors[5], colors[4]);
	char* str = get_selected_str(board, selected, n);
	printf("currently selected letters: %s\n", str);
	free(str);
}

char* get_selected_str(char** board, int** selected, int n)
{
	char* str = (char*) malloc(sizeof(char) * (n+1));
	for (int i = 0; i < n; i++)
	{
		str[i] = board[ (selected[i][0]) ][ (selected[i][1]) ];
	}
	str[n+1] = '\0';
	return str;
}

char** set_colors()
{
	char** colors = (char**) malloc(sizeof(char*) * 7);
	for (int i = 0; i < 7; i++)
	{
		colors[i] = (char*) malloc(sizeof(char) * 10);
	}
	strcpy(colors[0], "\033[31m"); // red
	strcpy(colors[1], "\033[32m"); // green
	strcpy(colors[2], "\033[33m"); // yellow
	strcpy(colors[3], "\033[34m"); // dark blue
	strcpy(colors[4], "\033[35m"); // purple
	strcpy(colors[5], "\033[36m"); // aqua
	strcpy(colors[6], "\033[37m"); // white
	return colors;
}

// Removes the newline character from the return of fgets
void remove_nl(char* word)
{
	if (word == NULL) return;
	int i = 0;
	while (word[i] != '\0')
	{
		if (word[i] == '\n')
		{
			word[i] = '\0';
			return;
		}
		i++;
	}
}

// Takes a string that represents an integer and returns the int primitive
int formatted_str_to_int(char* str)
{
	if (str == NULL) return 0;
	int ret = 0;
	long unsigned int len = strlen(str);
	int digits[len];
	for (int i = 0; i < len; i++)
	{
		int single_digit = (int)(str[i] - 48);
		digits[i] = single_digit;
		ret = ret + (digits[i] * (tentopower(len - i-1)));
	}
	return ret;
}

// Returns ten to power of n
int tentopower(int n)
{
	if (n == 0) return 1;
	int ret = 1;
	int i = 0;
	while (i < n)
	{
		ret *= 10;
		i++;
	}
	return ret;
}