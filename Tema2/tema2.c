#include <stdio.h>
#include <string.h>
#include "util/task_helper.h"
#include <stdlib.h>

#define sep ".,:;[]?{}! "

void replace(char *poem, char *to_replace , char *to_be_replaced)
	{
		/*inlocuiesc cuvintele care nu rimeaza. Copiez tot ce este inainte de cuvand,
		lipesc cuvantul care rimeaza si dupa copiez restul propozitiei*/

		char copy[5000], *p;
		memset(copy, 0, sizeof(copy));
		p = strstr(poem, to_be_replaced);
		memcpy(copy, poem, strlen(poem)-strlen(p));
		strcat(copy, to_replace);
		strcat(copy, p + strlen(to_be_replaced));
		strcpy(poem, copy);

	}

void silly(char *val, char *poem)
{
	/*verific daca valoarea generata random e mai mica decat probabilitatea
	si schimb fac valoarea literei -32 daca este litera mica, respectiv '=32
	daca este litera mare*/

	float p = atof(val), sample;
	int n = strlen(poem);
	for(int i = 0; i <= n; i++){  
		if(*(poem+i) >= 'A' && *(poem+i) <= 'z')  //verific daca este litera
			sample = rand() % 100 / 99.0;

		if(sample < p && *(poem+i) <= 'Z' && *(poem+i) >= 'A')
				*(poem+i)=*(poem+i)+32;
		else if(sample < p && *(poem+i) <= 'z' && *(poem+i) >= 'a')
				*(poem+i)=*(poem+i)-32;
	}
}

void uppercase(char *poem)
{

	if(*poem >= 'a' && *poem <= 'z')  //prima litera din poezie o fac litera mare
		*poem = *poem - 32;
	/*literele care nu sunt dupa \n, adica la inceput de rand le fac mici
	si pe cele care sunt dupa \n le fac mari*/

	for (int i = 1; i <= strlen(poem) ; i++)
		if(*(poem+i-1) == '\n' && *(poem+i) >= 'a' && *(poem+i) <= 'z')
				*(poem+i) = *(poem+i)-32;

		else if(*(poem+i) >= 'A' && *(poem+i) <= 'Z' && *(poem+i-1) != '\n')
				*(poem+i) = *(poem+i)+32;

}

void trimming(char *poem)
{
	char *p, copy[5000];
	strcpy(copy, poem);
	p=strtok(copy, sep);
	strcpy(poem, p);

	/*sterg toate non-caracterele si spatiile, mai putin \n dupa care pun 
	fiecare cuvant, urmat de spatiu*/

	while(p != NULL){
		p=strtok(NULL, sep);

		if(p){
			if(p[0] != '\n')
				strcat(poem, " ");
			strcat(poem, p);
			}}
}

void rhimy(char type[10], char *poem)
{
	char copy[5000], *p, last_word[500][25];
	int i = 0;
	strcat(copy, poem);
	p=strtok(copy, "\n");

	while(p){  //pastrez ultimul cuvant de pe fiecare rand
		if(strrchr(p,' ') != "\n")
		strcpy(last_word[i], strrchr(p,' '));
		i++;
		p = strtok(NULL,"\n");
	}
	
	for(int j = 0; j < i; j++){  //in cazul in care cuvantul contine non-caractere le sterg
		p=strtok(last_word[j], sep);
		if(p)
			strcpy(last_word[j], p);
	}

	if(strstr(type,"imperecheata"))
		for(int j = 0; j < i; j = j+2){

			// pastrez ultima litera din fiecare cuvant si o transform in litera mica

			char **word_list, last_letter1, last_letter2;
			last_letter1 = last_word[j][strlen(last_word[j])-1];
			last_letter2 = last_word[j+1][strlen(last_word[j+1])-1];

			if(last_letter1 <= 'Z' && last_letter2 >= 'A')
				last_letter1 = last_letter1 + 32;
			
			if(last_letter2 <= 'Z' && last_letter2 >= 'A')
				last_letter2 = last_letter2 +32;
			
			int n;

			if(last_letter1 != last_letter2)  //verific daca ultimele litere sunt la fel
				get_synonym(last_word[j], &n,&word_list);

			if(n > 0){  //verific daca primul cuvant are sinonime
				char s[25] = "0";
				for(int k = 0; k < n; k++)

					/*aflu cel mai mic lexicografic sinonim care rimeaza cu
					al doilea cuvant*/

					if(last_letter2 == word_list[k][strlen(word_list[k])-1]){
						if(strlen(s) == 1)
							strcpy(s, word_list[k]);
						if(strcmp(s, word_list[k]) > 0)
							strcpy(s, word_list[k]);  
					}
				if(strlen(s) != 1)
					replace(poem, s, last_word[j]);  //inlocuiesc cuvantul
			}
			else{  //daca primul nu are sinonime, il verific pe al doilea
				get_synonym(last_word[j+1], &n,&word_list);
				if(n > 0){
					char s[25] = "0";

					/*aflu cel mai mic lexicografic sinonim care rimeaza cu
					al primul cuvant*/

					for(int k = 0; k < n; k++)
						if(last_letter1 == word_list[k][strlen(word_list[k])-1]){
							if(strlen(s) == 1)
								strcpy(s, word_list[k]);
							if(strcmp(s, word_list[k]) > 0)
								strcpy(s, word_list[k]);
					}
					if(strlen(s) != 1)
						replace(poem, s, last_word[j+1]);  //inlocuiesc cuvantul
				}
			}
			n = 0;
		}

	if(strstr(type, "incrucisata")){
		int jump = 0;
		for(int j = 0; j < i; j++){
			char **word_list,last_letter1, last_letter2;
			last_letter1 = last_word[j][strlen(last_word[j])-1];
			last_letter2 = last_word[j+2][strlen(last_word[j+2])-1];

			if(last_letter1 <= 'Z' && last_letter2 >= 'A')
				last_letter1 = last_letter1 + 32;
			
			if(last_letter2 <= 'Z' && last_letter2 >= 'A')
				last_letter2 = last_letter2 + 32;
			
			int n;

			if(last_letter1 != last_letter2){
				get_synonym(last_word[j], &n, &word_list);
				if(n > 0){
					char s[25] = "0";
					for(int k = 0; k < n; k++)
						if(last_letter2 == word_list[k][strlen(word_list[k])-1]){
							if(strlen(s) == 1)
								strcpy(s, word_list[k]);
							if(strcmp(s, word_list[k]) > 0)
								strcpy(s, word_list[k]);
						}
					if(strlen(s) != 1)
						replace(poem, s, last_word[j]);
				}
				else{
					get_synonym(last_word[j+2], &n, &word_list);
					if(n > 0){
						char s[25] = "0";
						for(int k = 0; k < n; k++)
							if(last_letter1 == word_list[k][strlen(word_list[k])-1]){
								if(strlen(s) == 1)
									strcpy(s, word_list[k]);
								if(strcmp(s, word_list[k]) > 0)
									strcpy(s, word_list[k]);
						}
						if(strlen(s) != 1)
							replace(poem, s, last_word[j+2]);
					}
			}
		}

		if (jump == 1){
			j = j+2;
			jump = 0;
		}
		else
			jump = 1;
		n = 0;
		}
	}

	if(strstr(type,"imbratisata")){
		int jump = 0;
		for(int j = 0; j < i; j++){
			char **word_list, last_letter1, last_letter2;
			if(j%2 == 0){
				last_letter1 = last_word[j][strlen(last_word[j])-1];
				last_letter2 = last_word[j+3][strlen(last_word[j+3])-1];
				}
			else{
				last_letter1 = last_word[j][strlen(last_word[j])-1];
				last_letter2 = last_word[j+1][strlen(last_word[j+1])-1];
				}

			if(last_letter1 <= 'Z' && last_letter2 >= 'A')
				last_letter1 = last_letter1 + 32;
			
			if(last_letter2 <= 'Z' && last_letter2 >= 'A')
				last_letter2 = last_letter2 + 32;
			
			int n;

			if(last_letter1 != last_letter2){
				get_synonym(last_word[j], &n, &word_list);
				if(n > 0){
					char s[25]="0";
					for(int k = 0; k < n; k++)
						if(last_letter2 == word_list[k][strlen(word_list[k])-1]){
							if(strlen(s) == 1)
								strcpy(s, word_list[k]);
							if(strcmp(s, word_list[k]) > 0)
								strcpy(s, word_list[k]);
						}
					if(strlen(s) != 1)
						replace(poem, s, last_word[j]);
				}
				else {
					if(j%2 == 0)
						get_synonym(last_word[j+3], &n, &word_list);
					else
						get_synonym(last_word[j+1], &n, &word_list);

					if(n > 0){
						char s[25]="0";
						for(int k = 0; k < n; k++)
							if(last_letter1 == word_list[k][strlen(word_list[k])-1]){
								if(strlen(s) == 1)
									strcpy(s, word_list[k]);
								if(strcmp(s, word_list[k]) > 0)
									strcpy(s, word_list[k]);
						}
						if(strlen(s) != 1 && j%2 == 0)
							replace(poem, s, last_word[j+3]);
						else if(strlen(s) != 1 && j%2 !=0)
							replace(poem, s, last_word[j+1]);
					}
				}
			}

		if (jump == 1){
			j = j+2;
			jump = 0;
		}
		else
			jump = 1;

		n = 0;
		}
	}
}

void friendly(char *poem)
{
	char *word, *friendly_word, *p, copy[5000];
	strcpy(copy, poem);
	word = strtok(copy, " .,><?!':;{}[]\n");  //separ o copie a propozitiei in cuvinte
	get_friendly_word(word, &friendly_word);
	int a = 0;

	while(word){  //verific fiecare cuvant sa vad daca are diminutiv
		if(friendly_word)
		{  
			/*in cazul in care un cuvant are diminutiv il inlocuiesc. Copiez tot
			ce este inainte de cuvand,lipesc cuvantul care rimeaza si dupa copiez restul propozitiei*/
			
			char copy2[4000];
			memset(copy2, 0, sizeof(copy2));
			p = strstr(poem+a, word);

			//verific daca mi-a gasit cuvantul potrivit si nu o aparitia a lui in alt cuvant

			if(!( (*(p-1) <= 'z' && *(p-1) >= 'A') || (*(p+strlen(word)) <= 'z' && *(p+strlen(word)) >= 'A'))){
				memcpy(copy2, poem, strlen(poem)-strlen(p));
				strcat(copy2, friendly_word);
				strcat(copy2, p+strlen(word));
				strcpy(poem, copy2);}
			else{
				a = strlen(poem) - strlen(p) + 1;
				p = strstr(poem+a, word);
				memcpy(copy2, poem, strlen(poem)-strlen(p));
				strcat(copy2, friendly_word);
				strcat(copy2, p+strlen(word));
				strcpy(poem, copy2);}

			a = strlen(poem) - strlen(p) + 1;  //pastrez pozitia in care am ramas in poezie
		}

		word = strtok(NULL, " .,><?!':;{}[]\n");
		get_friendly_word(word, &friendly_word);
	}
}


int main(void)
{char s[30], poem[5000], *p, path[20];
	srand(42);
	fgets(s, 30, stdin);
	while (!strstr(s,"quit")){  //verific ce e introdus de la tastatura si apelez functia

		if(strstr(s,"uppercase"))
			uppercase(poem);

		if(strstr(s,"load")){
			p = strtok(s," ") ; p = strtok(NULL," ");  //in p retin calea de unde se ia poezia
			strncpy(path, p, strlen(p)-1);
			load_file(path,poem);
			strcat(poem,"\n");}

		if(strstr(s,"trimming"))
			trimming(poem);

		if(strstr(s,"make_it_silly")){
			p = strtok(s," "); p = strtok(NULL," ");  //in p retin probabilitatea de schimbare
			silly(p, poem);
		}

		if(strstr(s,"make_it_rhyme")){
			p = strtok(s," "); p = strtok(NULL," ");  //in p retin tipul de rima
			rhimy(p, poem);
		}

		if(strstr(s,"make_it_friendlier"))
			friendly(poem);

		if(strstr(s,"print"))
			printf("%s\n",poem);

		fgets(s, 30, stdin);
	}
	return 0;
}