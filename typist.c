//C program to learn to type on a platform-independent layout by asking for the letters to type

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned char byte;
typedef unsigned uint;
typedef unsigned long ulint;

void *mylloc(size_t size) {
	void *retval = malloc(size);
	if (!retval) {
		fprintf(stderr, "Malloc failed for %ld bytes\n", (long) size);
		exit(1);
	}
	return retval;
}
ulint rand_int(ulint max_exclusive) {
	ulint retval = rand();
	return retval % max_exclusive;
}
//Returns malloced null-terminated array of malloced strings that longer than 2 characters and are words containing only the characters contained in the regex, or all words if space is in the regex.
char **find_words(char *regex, size_t *len);
//Knuth fast shuffle of the array of words so you get fairly random results
void shuffle_words(char **words, size_t len);
//frees result of above
void free_words(char **words);
//returns the next line, excluding the newline, from the heap
char *read_line(FILE *from);

char *f_name;

int main(int argc, char **argv) {
	srand(time(0));
	char *letters = (char *) &letters;

	if (argc == 2) {
		f_name = argv[1];
	} else {
		f_name = "words_alpha.txt";
	}

	while (1) {
		printf("Enter the letters you wish to practice (or nothing to quit, or space for all letters): ");
		letters = read_line(stdin);
		if (!letters[0]) break;

		size_t length;
		char **words_to_practice = find_words(letters, &length);
		if (! (*words_to_practice)) {
			printf("No words were found for %s :( try adding vowels perhaps?\n", letters);
		} else {
			ulint correct = 0;

			printf("Enter nothing to choose new letters to practice!\n");
			shuffle_words(words_to_practice, length);

			ulint i;
			byte this_correct = 1;
			for (i = 0; words_to_practice[i];) {

				printf("%s: ", words_to_practice[i]);
				char *next_word = read_line(stdin);
				if (!next_word[0]) break;
				if (!strcmp(next_word, words_to_practice[i])) {
					i++;
					if (this_correct) correct++;
					this_correct = 1;
				} else {
					printf("No, ");
					this_correct = 0;
				}
				free(next_word);
			}

			if (!words_to_practice[i]) {
				printf("You finished all the possible word combaniations for %s!? ", letters);
			}
			if (i) printf("You scored %lu of %lu\n", correct, i);
		}

		free_words(words_to_practice);
		free(letters);
	}

	free(letters);
}

char *read_line(FILE *f) {
	char *buffer = (char *) mylloc(128);
	size_t max = 128;
	size_t cur = 0;

	char next = fgetc(f);
	while (next != EOF && next != '\n') {
		//I had a nightmare of a time realizing that there were carriage returns in the text file. Why did you do this, evil stranger?
		if (next != '\r')
			buffer[cur++] = next;
		if (cur == max) {
			buffer = (char *) realloc(buffer, (max *= 2));
			if (!buffer) {
				fprintf(stderr, "Failed to realloc for size %ld\n", (long) max);
				exit(1);
			}
		}
		next = fgetc(f);
	}
	buffer[cur++] = 0;
	buffer = realloc(buffer, cur);

	return buffer;
}

char **find_words(char *regex, size_t *len) {
	char **retval = (char **) mylloc(sizeof(*retval) * 128);
	size_t max = 128;
	size_t cur = 0;

	FILE *f = fopen(f_name, "r");

	char *line = read_line(f);
	while (*line) {
		byte is_valid = 1;
		uint i;
		if (regex[0] != ' ') {
			for (i=0; line[i]; i++) {
				if (!strchr(regex, (int) line[i])) {
					is_valid = 0;
					break;
				}
			}
		} else i = strlen(line);
		if (i < 3) is_valid = 0;
		if (!is_valid) {
			free(line);
			line = read_line(f);
			continue;
		}
		retval[cur++] = line;
		if (cur == max) {
			retval = (char **) realloc(retval, (max *= 2) * sizeof(*retval));
			if (!retval) {
				fprintf(stderr, "Failed to realloc for size %ld\n", (long) max * sizeof(*retval));
				exit(1);
			}
		}

		line = read_line(f);
	}
	free(line);
	fclose(f);

	retval[cur++] = 0;
	retval = realloc(retval, cur * sizeof(*retval));

	*len = cur-1;

	return retval;
}

void shuffle_words(char **words, size_t length) {
	ulint i;
	for (i = length-1; i > 0; i--) {
		ulint j = rand_int(i+1);
		char *temp = words[i];
		words[i] = words[j];
		words[j] = temp;
	}
}

void free_words(char **words) {
	char **temp = words;
	while (*words) {
		free(*words);
		words++;
	}
	free(temp);
}
