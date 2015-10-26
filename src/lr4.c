#include "lr4.h"
/* This is the main file for the label replacer. */

unsigned long long FILELINE = 0;

/* Engage! */
int main(int argc, char **argv)
{
	/* If the number of arguments isn't 4, someone doesn't know how to use the program. */
	if(argc != 4)
	{
		/* Quit and inform them. */
		puts("This is the label replacer for the Nibble Knowledge 4-bit computer");
		puts("");
		puts("Usage: lr4 definitionsfile inputasmfile outputasmfile");
		return 0;
	}
	else
	{
		/* defs is the defintion file */
		FILE *defs;	
		/* inasm is the input low level assembly that we are replacing the labels of */
		FILE *inasm;
		/* outasm is the final low level assembly file that we produce */
		FILE *outasm;
		/* This is the current line in the file */
		char *line = NULL;
		/* Current length of the line, -1 indicates end of file. */
		ssize_t linelen = -1;
		/* size of the buffer for getline. 0 tells getline to make a new one the right size. */
		size_t len = 0;
		/* our size of delimiters to for strtok() to tell it where to cut tokens */
		const char *delims = " \t";
		/* The list of label replacements */
		replace *replaces = NULL;
		datasize *datasizes = NULL;
		/* Amount of replacements */
		unsigned long numreplaces = 0;
		unsigned long numdatasizes = 0;
		uint16_t currsize = 0;
		/* Generic looping unit */
		unsigned int i = 0;		
		/* If we have an INF, record and output metadata for the assembler. */
		char inf = 0;
		FILE *header = NULL;
		char collectdata = 0;
		unsigned char firstdata = 0;

		/* Open the definitions file... */
		defs = fopen(argv[1], "r");
		if(defs == NULL)
		{
			perror("Could not open definitions file");
			return 3;
		}

		/* First line of the definitions file */		
		FILELINE = 1;
		/* Read the first line */
		linelen = getline(&line, &len, defs);
		/* If linelen is -1, we're at the end of the file. */
		while(linelen > -1)
		{
			/* String used for comparison, writespace removal, and used as the name of the label in replaces */
			char *tempstr = NULL;
			/* The current token */
			char *tokens = strtok(line, delims);
			/* When we try to convert the string to a number, endptr tells us the first invalid character. */
			/* We use this to determine if the offset or memory address is a valid value. */
			char *endptr = NULL;
			/* Address location of the replacement */
			unsigned long address = 0;
			/* Generic looping unit */
			unsigned int j = 0;
			/* What element we are on - either the name of the replacement (1) or the address (2) */
			unsigned char ele = 0;

			/* If tokens is NULL, we have reached the end of the line. */
			while(tokens != NULL)
			{
				i = 0;
				/* Remove the leading whitespace. Could probably be moved into a function. */
				while(isspace((unsigned char)tokens[i]))
				{
					i++;
					/* If we remove all the trailing whitespace and are past the end of the string... the string is empty */
					if(i > strlen(tokens))
					{
						/* ele > 2 will break cause the rest of the code to ignore this string. */
						ele = 3;
						break;
					}
				}
				/* If after removing all the whitespace, the first character is ";" or "#", the character is a comment. */
				if(tokens[i] == ';' || tokens[i] == '#')
				{
					/* Ignore the rest of this line */
					break;
				}
				/* Allocate enough space to store the entire string */
				tempstr = calloc(1, strlen(tokens));
				if(tempstr == NULL)
				{
					perror("Could not allocate memory");
					return 4;
				}
				/* Starting from the first non-whitespace character in tokens, assign the non-whitespace parts of the string to the new string. */
				for(i = i, j = 0; i < strlen(tokens) && j < strlen(tokens); i++, j++)
				{
					/* Remove trailing whitespace by breakout out of the loop as soon as we detect whitespace. */
					if(isspace((unsigned char)tokens[i]))
					{
						break;
					}
					tempstr[j] = tokens[i];
				}
				/* Terminate the string here as it is the end - also eliminates some weirdness */			
				tempstr[j] = '\0';

				ele++;
				/* The first element is the name of the replacement */
				if(ele == 1)
				{
					/* Should be in the form LABEL: where since we removed all the whitespace, ":" must be the last character. */
					if(tempstr[strlen(tempstr) - 1] != ':')
					{
						fprintf(stderr, "Line %llu in definition file: definitions must be declared as \"LABEL: ADDRESS\"\n", FILELINE);			
						return 5;
					}
					/* If the ":", remove it. We'll keep just the name and reference this replacement by it. */
					tempstr[strlen(tempstr) - 1] = '\0';
					/* Make sure the programmer hasn't used this replacement before... */
					for(i = 0; i < numreplaces; i++)
					{
						if(strcmp(replaces[i].name, tempstr) == 0)
						{
							fprintf(stderr, "Line %llu in definition file: label %s already used.\n", FILELINE, tempstr);
							return 7;
						}
					}
					/* If all the above is good, add the new replacement to the list. */
					numreplaces++;
					replaces = realloc(replaces, sizeof(replace) * numreplaces);
					if(replaces == NULL)
					{
						perror("Could not reallocate memory");
						return 6;
					}
					replaces[numreplaces - 1].name = tempstr;
				}
				/* If we're at the second element, we need to save the memory address. */
				else if(ele == 2)
				{
					/* Convert the memory address in the string to a number. */
					address = estrtoul(tempstr, &endptr, STDHEX);
					/* If tempstr and endptr (which points to the first invalid character) point to the same location, the entire number was invalid. */
					if(tempstr == endptr)
					{
						fprintf(stderr, "Line %llu in definition file: invalid address.\n", FILELINE);
						return 8;
					}
					/* If the conversion was good, save the address. */
					replaces[numreplaces - 1].addr = (uint16_t) address;
				}
				/* next token... */
				tokens = strtok(NULL, delims);
			}
			/* Next file line... */
			FILELINE++;
			/* getline allocates a memory buffer it does not free, so make sure we do so. */
			free(line);
			/* get line to NULL and len 0 to tell getline it needs to create a new buffer. */
			line = NULL;
			len = 0;
			linelen = getline(&line, &len, defs);
		}
		/* Free getline's buffer. */
		free(line);
		line = NULL;
		len = 0;
		/* We're done with the definition file. */
		fclose(defs);

		/* Open our input assembly */
		inasm = fopen(argv[2], "r");
		if(inasm == NULL)
		{
			perror("Could not open input assembly file");
			return 1;
		}
		/* Also make sure we have somewhere to write our output. */
		outasm = fopen(argv[3], "w");
		if(outasm == NULL)
		{
			perror("Could not open output assembly file");
			return 2;
		}
		/* Start at the first line... */
		FILELINE = 1;
		/* Get the first line of the file... */	
		linelen = getline(&line, &len, inasm);
		/* If linelen is -1, we are at the end of the file. */
		while(linelen > -1)
		{
			/* String used for comparison, writespace removal, and used as the name of the label in replaces */
			char *tempstr = NULL;
			/* When we try to convert the string to a number, endptr tells us the first invalid character. */
			/* We use this to determine if the offset or memory address is a valid value. */
			char *endptr = NULL;
			/* The current token */
			char *tokens = strtok(line, delims);
			/* What token number we are on for this line. */
			unsigned int tokennum = 1;
			/* If this line has a label at the start of it */
			unsigned char withlabel = 0;
			/* If this line has a .data statement */
			unsigned char dotdata = 0;
			unsigned char dotascii = 0;
			unsigned char dotasciiz = 0;
			/* What size the .data statement is - if we are replacing a label and it is not 4, there's a problem! */
			unsigned long dotdatasize = 0;
			/* The offset from the label location requested. */
			uint16_t offset = 0;
			/* Generic looping unit. */
			unsigned int j = 0;
			/* If this is a comment line. Helps to skip printing useless whitespace. */
			unsigned char comment = 0;
			unsigned char alreadyprinted = 0;

			/* If tokens is NULL, we're at the last token. */
			while(tokens != NULL)
			{
				tempstr = trim(tokens);
				/* If after skipping whitespace, the first character is ";" or "#", this token and all that follow on this line are part of a command. Don't print it or a newline. */
				if(tempstr[0] == ';' || tokens[i] == '#')
				{
					comment = 1;
					break;
				}

				/* If it's not an empty string, we can probably do something with it. */
				if(strlen(tempstr) > 0)
				{
					/* If we're at the first token in the line, and the last character is a ":", it's a label. */
					if(tokennum == 1 && tempstr[strlen(tempstr) - 1] == ':')
					{
						char *oldstr;
						char *newstr = NULL;
						withlabel = 1;
						if(!firstdata)
						{
							tokennum++;
							tokens = strtok(NULL, delims);
							newstr = trim(tokens);
							if(newstr != NULL && strlen(newstr) > 0)
							{
								oldstr = tempstr;
								tempstr = trim(tokens);
								if(strcmp(tempstr, ".data") == 0)
								{
									newstr = realloc(newstr, strlen("D_SEC:\n") + strlen(oldstr));
									sprintf(newstr, "%s\n%s", "D_SEC:", oldstr);
									fputs(newstr, outasm);
									firstdata++;
								}
								else
								{
									fputs(oldstr, outasm);
									/*fputs(" ", outasm);
									fputs(newstr, outasm);*/
								}
							}
							else
							{
								fputs(tempstr, outasm);
								alreadyprinted = 1;
							}
						}
					}
					/* If the first token on this line (or the second if we have a label) is "INF", record this... */
					else if(tokennum == (unsigned int) (1 + withlabel) && strcmp(tempstr, "INF") == 0)
					{
						inf = 1;
						header = fopen("header", "w");
						if(header == NULL)
						{
							perror("Could not open temporary header file");
							return 33;
						}
						fputs("INF\n", header);
					}
					/* If the first token on this line (or the second if we have a label) is ".data", record this... */
					else if(tokennum == (unsigned int) (1 + withlabel) && strcmp(tempstr, ".data") == 0)
					{
						dotdata = 1;
						if(!firstdata)
						{
							fputs("D_SEC:\n", outasm);
							firstdata++;
						}
					}
					/* If the first token on this line (or the second if we have a label) is ".ascii", record this... */
					else if(tokennum == (unsigned int) (1 + withlabel) && strcmp(tempstr, ".ascii") == 0)
					{
						dotascii = 1;
					}
					/* If the first token on this line (or the second if we have a label) is ".asciiz", record this... */
					else if(tokennum == (unsigned int) (1 + withlabel) && strcmp(tempstr, ".asciiz") == 0)
					{
						dotasciiz = 1;
					}
					else if((dotascii || dotasciiz))
					{
						uint16_t backslashes = 0;
						unsigned char foundquote = 0;
						uint16_t totallen = 0;
						
						totallen = strlen(tempstr);
						
						while(foundquote < 2)
						{
							puts(tempstr);
							for(i = 0; i < strlen(tempstr); i++)
							{
								if(tempstr[i] == '\\')
								{
									backslashes++;
									if(tempstr[i+1] == '"')
									{
										i++;
										continue;
									}
								}
								if(tempstr[i] == '"')
								{
									foundquote++;
								}
							}
							if(foundquote == 2)
							{
								break;
							}
							fputs(" ", outasm);
							fputs(tempstr, outasm);
							tokens = strtok(NULL, delims);
							tempstr = trim(tokens);
							totallen += strlen(tempstr) + 1;
						}
						printf("totallen: %hu, backslashes: %hu\n", totallen, backslashes);
						dotdatasize = totallen - 2 + dotasciiz - backslashes;
						addsize(dotdatasize, &currsize, &datasizes, &numdatasizes);
						dotasciiz = 0;
						dotascii = 0;
					}
					else if(inf == 1)
					{
						uint16_t baseaddr = 0;
						
						fputs("PINF\n", header);
						baseaddr = estrtoul(tempstr, &endptr, STDHEX);
						if(tempstr == endptr)
						{
							fprintf(header, "BADR %s\n", findreplace(tempstr, replaces, numreplaces, dotdatasize, dotdata));
						}
						else
						{
							fprintf(header, "BADR 0x%04X\n", baseaddr);
						}
						fputs("EPINF", header);
						fprintf(header, "\nDSEC D_SEC\n");
						inf = 0;
						collectdata = 1;
						FILELINE++;
						/* Free getline's buffer and make sure we tell it to allocate a new one. */
						free(line);
						line = NULL;
						len = 0;
						linelen = getline(&line, &len, inasm);
						tokens = strtok(line, delims);
						tokennum = 1;
						continue;
					}
					else if(inf == 2)
					{
						uint16_t baseaddr = 0;

						if(!strcmp("PINF", tempstr))
						{
							fputs(tempstr, header);
							while(strcmp("EPINF", tempstr))
							{
								tokens = strtok(NULL, delims);
								while(tokens != NULL)
								{
									tempstr = trim(tokens);
									fputs(" ", header);
									fputs(tempstr, header);
									tokens = strtok(NULL, delims);
								}
								fputs("\n", header);
								/* Get the next line of the file... */
								FILELINE++;
								/* Free getline's buffer and make sure we tell it to allocate a new one. */
								free(line);
								line = NULL;
								len = 0;
								linelen = getline(&line, &len, inasm);
								tokens = strtok(line, delims);
								tokennum = 1;
								tempstr = trim(tokens);
								if(tempstr == NULL)
								{
									fprintf(stderr, "Line %llu in input assembly file: invalid INF header.\n", FILELINE);
									fclose(header);
									fclose(outasm);
									remove("header");
									remove(argv[3]);
									exit(34);
								}
								if(!strcmp("BADR", tempstr))
								{
									tokens = strtok(NULL, delims);
									tempstr = trim(tokens);
									if(tempstr == NULL)
									{
										fprintf(stderr, "Line %llu in input assembly file: base base address.\n", FILELINE);
										fclose(header);
										fclose(outasm);
										remove("header");
										remove(argv[3]);
										exit(34);
									}
									baseaddr = estrtoul(tempstr, &endptr, STDHEX);
									if(tempstr == endptr)
									{
										fprintf(header, "BADR %s", findreplace(tempstr, replaces, numreplaces, dotdatasize, dotdata));
									}
									else
									{
										fprintf(header, "BADR 0x%04X", baseaddr);
									}
								}
								else
								{
									tempstr = trim(tokens);
									fputs(tempstr, header);
								}
							}
							FILELINE++;
							/* Free getline's buffer and make sure we tell it to allocate a new one. */
							free(line);
							line = NULL;
							len = 0;
							linelen = getline(&line, &len, inasm);
							FILELINE++;
							/* Free getline's buffer and make sure we tell it to allocate a new one. */
							free(line);
							line = NULL;
							len = 0;
							linelen = getline(&line, &len, inasm);
							tokens = strtok(line, delims);
							tokennum = 1;
							fprintf(header, "\nDSEC D_SEC\n");
							inf = 0;
							collectdata = 1;
							continue;
						}
					}
					/* If this line is a .data statement, the second token (or third if we have a label) is the size of the data section */
					else if(tokennum == (unsigned int) (2 + withlabel) && dotdata)
					{
						/* Store the size to check later */
						dotdatasize = estrtoul(tempstr, &endptr, STDHEX);
						addsize(dotdatasize, &currsize, &datasizes, &numdatasizes);
						/* A zero size data section is simply wrong */
						if(dotdatasize == 0)
						{
							fprintf(stderr, "Line %llu in input assembly file: invalid .data storage size.\n", FILELINE);
							/* Delete our output file. This is because we have no output buffer, so the output file could have partial and incorrect low level assembly in it. */
							fclose(outasm);
							remove(argv[3]);
							return 9;
						}
					}
					/* For an instruction with a label, the second token (third with a label) is possibly the label we want to replace, with a data section it's the third (fourth with a label) */
					else if((tokennum == (unsigned int) (2 + withlabel) && !dotdata) || (tokennum == (unsigned int) (3 + withlabel) && dotdata))
					{
						tempstr = findreplace(tempstr, replaces, numreplaces, dotdatasize, dotdata);
						if(tempstr == NULL)
						{
							fclose(outasm);
							remove(argv[3]);
							exit(14);
						}
					}
					if(inf == 0 && !alreadyprinted)
					{
						/* If we're not the first token, put a space before we write the string. */
						if(tokennum > 1)
						{
							fputs(" ", outasm);
						}
						/* Write the current string to the output file. */
						fputs(tempstr, outasm);
					}
				}
				/* We've finished using this string, free it. */
				free(tempstr);
				/* Get the next token. */
				tokens = strtok(NULL, delims);
				tokennum++;
			}
			/* If this is a comment, and we're at the first token, the entire line is a comment. So don't bother adding useless whitespace. */
			if(!(comment && tokennum == 1))
			{
				fputs("\n", outasm);
			}
			/* Get the next line of the file... */
			FILELINE++;
			/* Free getline's buffer and make sure we tell it to allocate a new one. */
			free(line);
			line = NULL;
			len = 0;
			linelen = getline(&line, &len, inasm);
			if(inf > 0)
			{
				inf++;
			}
		}
		fclose(inasm);
		if(collectdata && (header != NULL))
		{
			for(i = 0; i < numdatasizes; i++)
			{
				fprintf(header, "DNUM 0x%X\nDSIZE 0x%X\n", datasizes[i].num, datasizes[i].size);
			}
			fputs("EINF\n", header);
			fclose(outasm);
			fclose(header);
			rename(argv[3], "texttemp");
			rename("header", argv[3]);
			outasm = fopen("texttemp", "r");
			header = fopen(argv[3], "a");
			free(line);
			line = NULL;
			len = 0;
			linelen = getline(&line, &len, outasm);
			while(linelen > -1)
			{
				if(!isspace((unsigned char)line[0]))
				{
					fputs(line, header);
				}
				free(line);
				line = NULL;
				len = 0;
				linelen = getline(&line, &len, outasm);
			}
			fclose(header);
		}
		/* Free the list of replacements and close all the files - we're done our work here. */		
		for(i = 0; i < numreplaces; i++)
		{
			free(replaces[i].name);
		}
		free(replaces);
		fclose(outasm);
	}

	return 0;
}

char *trim(char *str)
{
	unsigned int i = 0;
	unsigned int j = 0;
	char empty[1] = {'\0'};
	char *retstr = NULL;

	if(str == NULL)
	{
		return NULL;
	}
	
	while(isspace((unsigned char)str[i]))
	{	
		i++;
		/* If we travel past the end of the string, the string is empty. */
		if(i > strlen(str))
		{
			retstr = empty;
			return retstr;
		}
	}
	retstr = calloc(1, strlen(str));
	if(retstr == NULL)
	{
		perror("Could not allocate memory");
		exit(4);
	}
	/* Trim trailing whitespace. Same process as done above, could really be part of a function. */
	for(i = i, j = 0; i < strlen(str) && j < strlen(str); i++, j++)
	{
		if(isspace((unsigned char)str[i]))
		{
			break;
		}
		retstr[j] = str[i];
	}
	/* Terminate the string here as it is the end, and stop some potential weirdness. */
	retstr[j] = '\0';
	
	return retstr;
}

char *findreplace(char *str, replace *replaces, unsigned long numreplaces, uint16_t dotdatasize, unsigned char dotdata)
{
	char *retstr = NULL;
	char *endptr = NULL;
	unsigned int i = 0;
	unsigned int offset = 0;
	
	/* Check to see if there is a requested offset */
	for(i = 0; i < strlen(str); i++)
	{
		/* If there is a "[", there is */
		if(str[i] == '[')
		{
			/* Try to convert this offset to a number. Handle the fact the macro assembler produces hex values in a non-standard form without "0x". */
			offset = estrtoul((str + i + 1), &endptr, NSTDHEX);
			/* If these two are equal, the value given in the square brackets was simply invalid */
			if((str + i + 1) == endptr)
			{
				fprintf(stderr, "Line %llu in input assembly file: invalid offset from label.\n", FILELINE);
				/* Make sure to remove the output file so we do not leave incorrect or invalid assembly lying around. */
				return NULL;
			}
			/* If the endptr does not point to the end of the current token, or the end of the current token isn't "]", the offset has been given in the incorrect form */
			if((str + strlen(str) - 1) != endptr || str[strlen(str) - 1] != ']')
			{
				fprintf(stderr, "Line %llu in input assembly file: label offsets must be in form LABEL[OFFSET].\n", FILELINE);
				/* Make sure to remove the output file so we do not leave incorrect or invalid assembly lying around. */
				return NULL;
			}
			/* If we do have an offset, terminate the string at the point we found the "[" so we can use the actual name of the label in searching for if we have a replacement address. */
			str[i] = '\0';
		}
	}
	/* Look for a replacement to the current label */
	for(i = 0; i < numreplaces; i++)
	{
		/* if we find one... */
		if(strcmp(replaces[i].name, str) == 0)
		{
			/* Check the data storage size to see if it is correct, if not whine and die */
			if(dotdatasize < 4 && dotdata)
			{
				fprintf(stderr, "Line %llu in input assembly file: .data storage size must be at least 4 nibbles to store a memory address.\n", FILELINE);
				/* Make sure to remove the output file so we do not leave incorrect or invalid assembly lying around. */
				return NULL;
			}
			/* Allocating 20 bytes should hold the maximum possible number on a 64-bit platform, so we should be fine... */
			free(str);
			retstr = calloc(1, 20);
			if(retstr == NULL)
			{
				perror("Could not allocate memory");
				exit(14);
			}
			/* Write the address + the offset to the string to be written to the output file. */		
			sprintf(retstr, "0x%04X", replaces[i].addr + offset);
			return retstr;
		}
	}
	return str;
}

void addsize(uint16_t size, uint16_t *currsize, datasize **datasizes, unsigned long *numdatasizes)
{
	if(size > 0)
	{
		if(datasizes == NULL || currsize == NULL || numdatasizes == NULL)
		{
			return;			
		}
		else if((*datasizes) == NULL)
		{
			(*datasizes) = malloc(sizeof(datasize));
			(*numdatasizes) = 1;
			(*datasizes)[0].size = size;
			(*datasizes)[0].num = 1;
			(*currsize) = size;
			return;
		}
		else if((*currsize) == size)
		{
			(*datasizes)[(*numdatasizes) - 1].num += 1;
		}
		else
		{
			(*numdatasizes) += 1;
			(*datasizes) = realloc((*datasizes), sizeof(datasize) * (*numdatasizes));
			(*datasizes)[(*numdatasizes) - 1].size = size;
			(*datasizes)[(*numdatasizes) - 1].num = 1;
			(*currsize) = size;
		}
	}
}

/* Listo! */
