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
	FILE* words_ptr = fopen("./en.txt", "r");
	if (words_ptr == NULL) return 0;
	FILE* offset_ptr = fopen("./preprocessed.txt", "r");
	if (offset_ptr == NULL) return 0;
	// Binary Search On Line Number
	int low = 0;
	int high = NUM_WORDS-1;
	int mid = (low+high)/2;
	char target[] = "r";
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
	int res = 0;
	while (!res)
	{
		if (guesses > 12) sleep(1);
		int res = strcmp(target, guess);
		printf("-------------------\n\n");
		printf("target: %s\n", target);
		printf("guess:  %s\n", guess);
		printf("res:    %d\n\n", res);
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
		if (mid == newmid)
		{
			// Try and look at the word after to see if it had any further spellings
			int offset_after_guess = (mid+1) * (NUM_DIGITS+1);
			fseek(offset_ptr, offset_after_guess, SEEK_SET);
			offset = (char*) malloc(sizeof(char) * 9);
			fgets(offset, 9, offset_ptr);
			remove_nl(offset);
			int int_offset = formatted_str_to_int(offset);
			free(offset);
			fseek(words_ptr, int_offset, SEEK_SET);
			fgets(guess, 30, words_ptr);
			remove_nl(guess);
			res = strcmp(target, guess);
			free(guess);
			// If the difference between the string compares is greater than 96,
			// that means that they were the same spelling but one of them had more letters
			// meaning there are still potential words to look for
			if (res < -96)
			{
				printf("There is a word to keep looking for!\n");
				return 1;
			}
			printf("There is no possible word that could be formed from this\n");
			return 0;
		}
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