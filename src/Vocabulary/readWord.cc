
/*Reads a word from file descriptor fin*/
void ReadWord(char *word, FILE *fin) {
	int a = 0, character;
	
	while (!feof(fin)) {
		character = fgetc(fin);

		if (character == 13) //Carriage Return
			continue;

		if ((character == ' ') || (character == '\t') || (character == '\n')) {
			
			if (a > 0) {
		    	if (character == '\n')
		    		ungetc(character, fin); //we don't want the new line char.
		    break;
		  	}

		 	if (character == '\n') { 
			    strcpy(word, (char *)"</s>");  //newline become </s> in corpus
			    return;
		  	}
		 	else
		  		continue;
		}

		word[a] = character;
		a++;

		if (a >= MAX_STRING - 1)
			a--;   // Truncate too long words
	}

	word[a] = '\0';
	return;
}
