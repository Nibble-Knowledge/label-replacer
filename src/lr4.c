#include "lr4.h"
/* This is the main file for the label replacer. */

unsigned long long FILELINE = 0;
unsigned char N_DEFINED = 0;
unsigned char N_NEEDED = 0;

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
					/* If the label is "N_", we have the static numbers defined. Used later for optimisation of address-of operations. */
					if(!strcmp("N_", tempstr))
					{
						N_DEFINED = 1;
					}
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
						/* If we are trying to print the label to the data section, we need to check each label until we get there to see if there is a .data after it (because that's where they usually are) */
						if(!firstdata && collectdata)
						{
							tokennum++;
							tokens = strtok(NULL, delims);
							newstr = trim(tokens);
							/* If there is a token after... */
							if(newstr != NULL && strlen(newstr) > 0)
							{
								/* Save the current string to print later... */
								oldstr = tempstr;
								/* Get a new one and check if it is .data */
								tempstr = trim(tokens);
								if(strcmp(tempstr, ".data") == 0)
								{
									/* If it is, it's the first .data location, add the D_SEC label and the label for the .data together. */
									newstr = realloc(newstr, strlen("D_SEC:\n") + strlen(oldstr));
									sprintf(newstr, "%s\n%s", "D_SEC:", oldstr);
									fputs(newstr, outasm);
									/*free(newstr);
									free(oldstr);*/
									firstdata++;
									dotdata = 1;
								}
								else
								{
									fputs(oldstr, outasm);
								}
							}
							else
							{
								/* Otherwise just print whatever is on this line. */
								fputs(tempstr, outasm);
								/* Gets double printed if this isn't here - not sure why... */
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
						if(!firstdata && collectdata)
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
					/* print out and measure the length of the dotascii and dotasciiz sections so we can record them in the header. */
					else if((dotascii || dotasciiz))
					{
						/* Each backslash looks like two characters but it is actually one */
						uint16_t backslashes = 0;
						/* Keep going until the endquote */
						unsigned char foundquote = 0;
						/* total length */
						uint16_t totallen = 0;
						
						/* Start with the length of this string. */
						totallen = strlen(tempstr);
						
						/* Until we find the final quote... */
						while(foundquote < 2)
						{
							/* For each character in this section of the string */
							for(i = 0; i < strlen(tempstr); i++)
							{
								/* If we find a backslash... */
								if(tempstr[i] == '\\')
								{
									/* record it */
									backslashes++;
									/* Skip the next quote if it appears - it's not the endquote. */
									if(tempstr[i+1] == '"')
									{
										i++;
										continue;
									}
								}
								/* If we find the endquote, tell the guard value... */
								if(tempstr[i] == '"')
								{
									foundquote++;
								}
							}
							if(foundquote == 2)
							{
								break;
							}
							/* Each tempstr is actually a space delimited section of a longer string */
							fputs(" ", outasm);
							fputs(tempstr, outasm);
							tokens = strtok(NULL, delims);
							tempstr = trim(tokens);
							totallen += strlen(tempstr) + 1;
						}
						/* The size of this string is the total length minus the quotes, plus the possible zero terminator, and minus the amount of backslashes. */
						dotdatasize = totallen - (unsigned int) 2 + dotasciiz - backslashes;
						addsize(dotdatasize, &currsize, &datasizes, &numdatasizes);
						dotasciiz = 0;
						dotascii = 0;
					}
					/* If inf is one, it is the automatic header specified by INF BASEADDRESS */
					else if(inf == 1)
					{
						uint16_t baseaddr = 0;
						/* Write out the header */
						fputs("PINF\n", header);
						/* The current token should be the base address, as the INF has just been written. */
						/* Try to convert it. If it's an invalid address, it's probably a label. */
						baseaddr = estrtoul(tempstr, &endptr, STDHEX);
						
						if(tempstr == endptr)
						{
							/* If the label is invalid, the assembler will catch it. */
							fprintf(header, "BADR %s\n", findreplace(tempstr, replaces, numreplaces, dotdatasize, dotdata));
						}
						else
						{
							/* Otherwise just write the number. */
							fprintf(header, "BADR 0x%04X\n", baseaddr);
						}
						/* End the executable information header */
						fputs("EPINF", header);
						/* Write the standard data section, we'll add the label later. */
						fprintf(header, "\nDSEC D_SEC\n");
						inf = 0;
						/* make sure the rest of the program reports on the .data sections. */
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
					/* If inf is two, an INF was declared and the specifics of the header are being explicitly specified (instead of the automatic header generation above). */ 
					else if(inf == 2)
					{
						uint16_t baseaddr = 0;
						unsigned char foundbaseaddr = 0;
						/* Right after the INF, we should have the PINF which starts the executable information section. */
						if(!strcmp("PINF", tempstr))
						{
							/* write that to the header. */
							fputs(tempstr, header);
							/* Until we reach the end of the executable information section (EPINF, strcmp("EPINF", tempstr) != 0) */
							while(strcmp("EPINF", tempstr))
							{
								/* Directly write everything we find in here to the header without changing it (except for trimming whitespace), with the exception of the BADR (base address) */
								tokens = strtok(NULL, delims);
								/* Write the entire line */
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
								/* If we loop to the end of the file, this is obviously wrong. */
								if(linelen < 0)
								{
									fprintf(stderr, "Input assembly file: PINF with no EPINF!\n");
									fclose(header);
									fclose(outasm);
									remove("header");
									remove(argv[3]);
									exit(34);
								}
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
								/* If we find the BADR... */
								if(!strcmp("BADR", tempstr))
								{
									/* We found the base address... */
									foundbaseaddr = 1;
									/* Get the next token (which should be the base address itself) */
									tokens = strtok(NULL, delims);
									tempstr = trim(tokens);
									if(tempstr == NULL)
									{
										fprintf(stderr, "Line %llu in input assembly file: invalid base address.\n", FILELINE);
										fclose(header);
										fclose(outasm);
										remove("header");
										remove(argv[3]);
										exit(34);
									}
									/* Try to convert it. If it's an invalid address, it's probably a label. */
									baseaddr = estrtoul(tempstr, &endptr, STDHEX);
									if(tempstr == endptr)
									{
										/* If the label is invalid, the assembler will catch it. */
										fprintf(header, "BADR %s", findreplace(tempstr, replaces, numreplaces, dotdatasize, dotdata));
									}
									else
									{
										/* Otherwise just write the number. */
										fprintf(header, "BADR 0x%04X", baseaddr);
									}
								}
								/* If it's not BADR, just write it. */
								else
								{
									if(foundbaseaddr == 0)
									{
										fprintf(stderr, "Line %llu in input assembly file: BADR must precede all pseudo-instructions!.\n", FILELINE);
										fclose(header);
										fclose(outasm);
										remove("header");
										remove(argv[3]);
										exit(34);
									}
									tempstr = trim(tokens);
									fputs(tempstr, header);
								}
							}
							/* If we find the EPINF, we've written it above. */
							/* The next line must be the EINF, so skip two. */
							FILELINE++;
							/* Free getline's buffer and make sure we tell it to allocate a new one. */
							free(line);
							line = NULL;
							len = 0;
							linelen = getline(&line, &len, inasm);
							/* This is where the EINF should be. */
							if(strcmp("EINF", trim(line)))
							{
								fprintf(stderr, "Line %llu in input assembly file: INF with no EINF!\n", FILELINE);
								
								fclose(header);
								fclose(outasm);
								remove("header");
								remove(argv[3]);
								exit(34);
							}
							FILELINE++;
							/* Free getline's buffer and make sure we tell it to allocate a new one. */
							free(line);
							line = NULL;
							len = 0;
							linelen = getline(&line, &len, inasm);
							tokens = strtok(line, delims);
							tokennum = 1;
							/* Write the boilerplate DSEC to the header. We'll add the label to the assembly later. */
							fprintf(header, "\nDSEC D_SEC\n");
							inf = 0;
							/* Make sure the rest of the program reports on the .data sections. */
							collectdata = 1;
							continue;
						}
						/* If the PINF isn't here, the INF is invalid. */
						else
						{
							fprintf(stderr, "Input assembly file: invalid INF header\n");
							fclose(outasm);
							fclose(header);
							remove("header");
							remove(argv[3]);
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
						if(tempstr[0] == '*')
						{
							for(i = 1; i < strlen(tempstr); i++)
							{
								tempstr[i-1] = tempstr[i];
							}
							tempstr[i-1] = '\0';
						}
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
			if(N_NEEDED == 1 && linelen == -1)
			{
				line = calloc(1, 21);
				strcpy(line, "N_: .data 4 0x0123\n");
				linelen = strlen(line);
				N_NEEDED = 2;
				N_DEFINED = 1;
			}
			else if(N_NEEDED == 2)
			{
				line = calloc(1, 18);
				strcpy(line, ".data 4 0x4567\n");
				linelen = strlen(line);
				N_NEEDED = 3;

			}
			else if(N_NEEDED == 3)
			{
				line = calloc(1, 18);
				strcpy(line, ".data 4 0x89AB\n");
				linelen = strlen(line);
				N_NEEDED = 4;
				N_DEFINED = 1;

			}
			else if(N_NEEDED == 4)
			{
				line = calloc(1, 18);
				strcpy(line, ".data 4 0xCDEF\n");
				linelen = strlen(line);
				N_NEEDED = 5;

			}
			if(inf > 0)
			{
				inf++;
			}
		}
		/* The input assembly file has no more data for us. */
		fclose(inasm);
		/* If collectdata is true, we need to print out the rest of the header. */
		if(collectdata && (header != NULL))
		{
			char *nstartstr = NULL;
			char *N_ = NULL;

			/* Print all the data section sizes */
			for(i = 0; i < numdatasizes; i++)
			{
				fprintf(header, "DNUM 0x%X\nDSIZE 0x%X\n", datasizes[i].num, datasizes[i].size);
			}
			/* If N_DEFINED is 1, put the information for the low level assembler. Allocate 13 plus a little extra... should be enough!*/
			if(N_DEFINED)
			{
				nstartstr = calloc(1, 20);
				N_ = calloc(1, 4);
				memcpy(N_, "N_", 4);
				sprintf(nstartstr, "NSTART %s\n", findreplace(N_, replaces, numreplaces, 0, 0));
				fputs(nstartstr, header);
				free(nstartstr);
			}
			/* End of the header */
			fputs("EINF\n", header);
			/* Instead of making an internal buffer, move the header file to the name of the output file and append the rest of the assembly below it. */
			fclose(outasm);
			fclose(header);
			remove("texttemp");
			rename(argv[3], "texttemp");
			rename("header", argv[3]);
			outasm = fopen("texttemp", "r");
			header = fopen(argv[3], "a");
			free(line);
			line = NULL;
			len = 0;
			linelen = getline(&line, &len, outasm);
			/* Copy every line one by one */
			while(linelen > -1)
			{
				/* If it starts with a space, it's likely an empty line so skip it. */
				if(!isspace((unsigned char)line[0]))
				{
					fputs(line, header);
				}
				free(line);
				line = NULL;
				len = 0;
				linelen = getline(&line, &len, outasm);
			}
			/* If no replaces, the header should be at the end of the file. */
			if(numdatasizes == 0)
			{
				fputs("D_SEC:", header);
			}
			fclose(header);
		}
		/* Make sure to print the N_ if it is defined. This also cleans whitespace... */
		/* This is not the cleanest code to prepend the NSTART, and should probably be combined with the above case. */
		else
		{
			char *nstartstr = NULL;
			char *N_ = NULL;

			fclose(outasm);
			remove("texttemp");
			rename(argv[3], "texttemp");
			outasm = fopen("texttemp", "r");
			header = fopen(argv[3], "a");
			if(N_DEFINED)
			{
				nstartstr = calloc(1, 20);
				N_ = calloc(1, 4);
				memcpy(N_, "N_", 4);
				sprintf(nstartstr, "NSTART %s\n", findreplace(N_, replaces, numreplaces, 0, 0));
				fputs(nstartstr, header);
				free(nstartstr);
			}
			free(line);
			line = NULL;
			len = 0;
			linelen = getline(&line, &len, outasm);
			/* Copy every line one by one */
			while(linelen > -1)
			{
				/* If it starts with a space, it's likely an empty line so skip it. */
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
		remove("texttemp");
	}

	return 0;
}
/* Whitespace trimming function. */
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
	/* Trim leading whitespace */
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
	/* Trim trailing whitespace. */
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
/* This function searches if the label needs replacing, and if it does, returns the replacement */
char *findreplace(char *str, replace *replaces, unsigned long numreplaces, uint16_t dotdatasize, unsigned char dotdata)
{
	char *retstr = NULL;
	char *endptr = NULL;
	unsigned int i = 0;
	unsigned int offset = 0;


	/* Check if this is an address-of operation */
	if(str[0] == '&')
	{
		/* If the static numbers are defined, the low level assembly will handle this. */
		if(!N_DEFINED)
		{
			/* But if they are not, make sure we make them. */
			N_NEEDED = 1;
		}
		return str;
	}	
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
	if(offset != 0)
	{
		retstr = calloc(1, strlen(str) + 10);
		sprintf(retstr, "%s[%x]", str, offset);
		free(str);
	}
	else
	{
		retstr = str;
	}
	return retstr;
}
/* This function add's a dot data's size to the output header. Used for the metadata section. */
void addsize(uint16_t size, uint16_t *currsize, datasize **datasizes, unsigned long *numdatasizes)
{
	if(size > 0)
	{
		if(datasizes == NULL || currsize == NULL || numdatasizes == NULL)
		{
			return;			
		}
		/* If the datasizes list is empty, this is obviously a unique size. */
		else if((*datasizes) == NULL)
		{
			(*datasizes) = malloc(sizeof(datasize));
			(*numdatasizes) = 1;
			(*datasizes)[0].size = size;
			(*datasizes)[0].num = 1;
			(*currsize) = size;
			return;
		}
		/* If the current size input is the same as the size of the dot data we are adding, just increment the num of dot data sections. */
		else if((*currsize) == size)
		{
			(*datasizes)[(*numdatasizes) - 1].num += 1;
		}
		/* Otherwise, we add a new element to the list with one data section of the input size. */
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
