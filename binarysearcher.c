#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_WORDS 274927
#define NUM_DIGITS 7

void remove_nl(char* word);
int formatted_str_to_int(char* str);
int tentopower(int n);

int main(void)
{
	printf("Hello, World!\n");
	FILE* words_ptr = fopen("./en.txt", "r");
	if (words_ptr == NULL)
	{
		printf("Error: Couldn't open words file.\n");
		return 1;
	}
	FILE* offset_ptr = fopen("./preprocessed.txt", "r");
	if (offset_ptr == NULL)
	{
		printf("Error: Couldn't open offset file.\n");
		fclose(words_ptr);
		return 1;
	}
	
	// Binary Search On Index
	int low = 0;
	int high = NUM_WORDS-2;
	int mid = (low+high)/2;
	char target[] = "zzzz";
	// total words / 2, then mult by num digits per entry, then fseek there
	int offset_for_offset = mid * (NUM_DIGITS+1);
	fseek(offset_ptr, offset_for_offset, SEEK_SET);
	char* offset = (char*) malloc(sizeof(char) * 9);
	fgets(offset, 9, offset_ptr);
	remove_nl(offset);
	int int_offset = formatted_str_to_int(offset);
	free(offset);
	fseek(words_ptr, int_offset, SEEK_SET);
	char* guess = (char*) malloc(sizeof(char) * 30);
	fgets(guess, 30, words_ptr);
	remove_nl(guess);
	int guesses = 0;
	printf("Initial Guess: |%s|\n", guess);
	int res = 0;
	while (!res)
	{
		printf("-----------------------------\n\n");
		printf("Guessing Index:   %d\n", mid);
		printf("Low Bound Index:  %d\n", low);
		printf("High Bound Index: %d\n", high);
		printf("Guess: |%s|\n\n", guess);
		sleep(1);
		int res = strcmp(target, guess);
		printf("Result of |%s| vs |%s|: %d\n", target, guess, res);
		if (res < 0)
		{
			high = mid;
			if (high == mid)
			{
				high--;
			}
		}
		else if (res > 0)
		{
			low = mid;
			if (low == mid)
			{
				low++;
			}
		}
		else
		{
			printf("It seems we found the word?\n");
			break;
		}
		
		mid = (low + high)/2;
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
		printf("New guess: |%s| Num Guesses: %d\n", guess, guesses);
		if (guesses > 100)
		{
			printf("Something clearly went wrong lol\n");
			break;
		}
	}
	free(guess);
	fclose(words_ptr);
	fclose(offset_ptr);
	
	return 0;
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

// Takes a string that represents and integer and returns the int
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

// Returns ten to power of n (idk why I implemented this lol)
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