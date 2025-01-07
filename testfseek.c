#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int formatted_str_to_int(char* str);
int tentopower(int n);
void remove_nl(char* word);

// Goal: Jump to words using the fseek offset in preprocessed.txt

/*
  Notes:
  fseek indexes starting by 0, so the first position in the file is 0
  as opposed to what my current setup of np++ tells me, which is that the
  first position in the file is 1
  
  Newlines count as part of the file itself, so when you're calculating offset
  The new line has to be added to the total number of digits per entry
*/
int main(void)
{
	FILE* preprocessed_ptr = fopen("preprocessed.txt", "r");
	FILE* words_ptr = fopen("./en.txt", "r");
	
	// 000000
	// 000006
	// 000013
	// 000021
	// 000028
	// 000035
	// 000045
	// 000054
	// 000066
	// 000071
	// 000079
	
	// Jump to the word
	// The offset should be calculated by taking total digits * total words which gets you the total number of chars in the file
	// Then divide by 2 to get half
	int offset = formatted_str_to_int("02133184");
	printf("offset: %d\n", offset);
	fseek(words_ptr, offset, SEEK_SET);
	char* word = (char*) malloc(sizeof(char) * 30);
	fgets(word, 30, words_ptr);
	printf("|%s|\n", word);
	remove_nl(word);
	printf("|%s|\n", word);
	for (int i = 0; word[i] != '\0'; i++)
	{
		printf("word[%d] = |%c|\n", i, word[i]);
	}
	free(word);
	
	fclose(preprocessed_ptr);
	fclose(words_ptr);
	return 0;
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

