#include "lr4.h"

int main(int argc, char **argv)
{
	if(argc != 4)
	{
		puts("This is the label replacer for the Nibble Knowledge 4-bit computer");
		puts("");
		puts("Usage: lr4 definitionsfile inputasmfile outputasmfile");
		return 0;
	}
	else
	{
		FILE *defs;	
		FILE *inasm;
		FILE *outasm;
		char *line = NULL;
		ssize_t linelen = -1;
		size_t len = 0;
		const char *delims = " \t";

		inasm = fopen(argv[2], "rb");
		if(inasm == NULL)
		{
			perror("Could not open input assembly file");
			return 1;
		}

		outasm = fopen(argv[3], "wb");
		if(outasm == NULL)
		{
			perror("Could not open input assembly file");
			return 2;
		}

		linelen = getline(&line, &len, inasm);
		while(linelen > -1)
		{
			char *tempstr = NULL;
			char *tokens = strtok(line, delims);
			unsigned int i = 0;
			unsigned int tokennum = 1;

			while(tokens != NULL)
			{
				tempstr = calloc(1, strlen(tokens) + 1);
				if(tempstr == NULL)
				{
					perror("Could not allocate memory");
					return 4;
				}
				memcpy(tempstr, tokens, strlen(tokens));
				for(i = 0; i < strlen(tempstr); i++)
				{
					if(isspace((unsigned char)tempstr[i]))
					{
						tempstr[i] = '\0';
						break;
					}
				}
				if(strlen(tempstr) > 0)
				{
					if(tokennum > 1)
					{
						fputs(" ", outasm);
					}
					printf("%s ", tempstr);
					fputs(tempstr, outasm);
				}
				free(tempstr);
				tokens = strtok(NULL, delims);
				tokennum++;
			}
			fputs("\n", outasm);
			puts("");
			linelen = getline(&line, &len, inasm);
		}
		fclose(inasm);
		fclose(outasm);
	}

	return 0;
}
