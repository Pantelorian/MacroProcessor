#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct deftab{
	char* str;
	struct deftab* next;
} deftab;
deftab* dt=NULL;

deftab* getLast() {
	if (dt == NULL) {
    		dt = (deftab*)malloc(sizeof(deftab));
        	dt->next = NULL;
		return dt;
	}	
	deftab* tmp = dt;
    	while (tmp->next!=NULL) {
       		tmp = tmp->next;
    	}
	tmp->next = (deftab*)malloc(sizeof(deftab));
	tmp = tmp->next;
	tmp->next = NULL;
    	return tmp;
}

deftab* dtadd(char* val) {
    	deftab* last = getLast();
	last->str = (char*)malloc(strlen(val)+1);
    	strcpy(last->str, val);
	return last;
}

void clear_deftab(){
	deftab* tmp = dt;
	while(tmp != NULL){
		dt = tmp;
		tmp = tmp->next;
		free(dt->str);
		free(dt);
	}
}

typedef struct argtab{
	char* key;
	char* val;
	struct argtab* next;
}argtab;

argtab* addargs(argtab* at, char* key, char* val){
	argtab* tmp;
	if (at == NULL){
		at = (argtab*)malloc(sizeof(argtab));
		at->next = NULL;
		tmp = at;
	}
	else{
		tmp = (argtab*)malloc(sizeof(argtab));
		tmp->next = at->next;
		at->next = tmp;
	}
	tmp->key = (char*)malloc(strlen(key)+1);
	tmp->val = (char*)malloc(strlen(val)+1);
	strcpy(tmp->key, key);
	strcpy(tmp->val, val);
	return tmp;
	
}

char* getarg(argtab* args, char* key){
	argtab* tmp = args;
	while(tmp != NULL){
		if ((key != NULL) && (tmp->key!=NULL) && (strcmp(tmp->key, key) == 0))
		       return tmp->val;
		tmp = tmp->next;
	}
}

void clear_argtab(argtab* at){
	argtab* tmp = at;
	while(tmp != NULL){
		at = tmp;
		tmp = tmp->next;
		free(at->key);
		free(at->val);
		free(at);
	}
}

typedef struct nametab{
	char* key;
	deftab* begin;
	deftab* end;
	struct nametab* next;
}nametab;
nametab* nt = NULL;

nametab* ntgetptr(char* key){
	nametab* tmp = nt;
	while (tmp != NULL){
		if (strcmp(tmp->key, key) == 0)
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

deftab* ntg(nametab* p, int flag){
	if (flag == 0) 	return p->begin;
	else		return p->end;
}

void ntadd(char* key){
	nametab* tmp = ntgetptr(key);
	if (tmp != NULL)
		return;
	tmp = (nametab*)malloc(sizeof(nametab));
	tmp->key = (char*)malloc(strlen(key)+1);
	strcpy(tmp->key, key);
	tmp->next = nt;
	nt = tmp;
}
void ntedit(char* key, int flag, deftab* ptr){
	nametab* tmp = ntgetptr(key);
	if (flag == 1) tmp->end = ptr;
	else if (flag == 0) tmp->begin = ptr;
}

void clear_nametab(){
	nametab* tmp = nt;
	while(tmp != NULL){
		nt = tmp;
		tmp = tmp->next;
		free(nt->key);
		free(nt);
	}
}

char* separators = " :,\t\n";
int status = 0;
char* nowmacro;

void define(char* str);

void expand(char* str, argtab* at);

void processline(char* str){
	if (status > 0) define(str);
	else {
		if (str != NULL){
			char* st = (char*)malloc(strlen(str)+1);
			char* istr;
			char* tmp;
			strcpy(st, str);
			istr = strtok(st, separators);
			tmp = (char*)malloc(strlen(istr)+1);
			strcpy(tmp, istr);
			istr = strtok(NULL, separators);
			if ((istr != NULL) && (strcmp(istr, "macro") == 0)){
				ntadd(tmp);
				nowmacro = (char*)malloc(strlen(tmp)+1);
				strcpy(nowmacro, tmp);
				define(str);
			}
			else if (ntgetptr(tmp) != NULL){
				argtab* at = NULL;
				expand(str, at);
			} 
			else retstr(str);
			free(st);
			free(tmp);		
		}
	}
}

void define(char* str){
	char* st=(char*)malloc(strlen(str)+1);
	strcpy(st, str);
	char* istr = strtok(st, separators);
	char* tmp = (char*)malloc(strlen(istr)+1);
	strcpy(tmp, istr);
	istr = strtok(NULL, separators);
	if ((tmp!=NULL) && (strcmp(tmp, "endm") == 0))
		status--;
	else if ((istr!=NULL) && (strcmp(istr, "macro") == 0))
		status++;
	if (status > 0){
		if ((status == 1) && ((istr!=NULL)&&(strcmp(istr, "macro") == 0)))
			ntedit(nowmacro, 0, dtadd(str));
		else
			dtadd(str);
	}
	else {
		ntedit(nowmacro, 1, dtadd(str));
		free(nowmacro);
	}
	free(tmp);
	free(st);
}


void replaceSubstring(char *str, const char *old, const char *new) {
    char *pos = strstr(str, old);
    if (pos != NULL) {
        int len_old = strlen(old);
        int len_new = strlen(new);
        if (len_new != len_old) {
            memmove(pos + len_new, pos + len_old, strlen(pos + len_old) + 1);
        }
        memcpy(pos, new, len_new);
    }
}


void expand(char* str, argtab* at){
	char* context_strtok1;
	char* context_strtok2;
	char* st = (char*)malloc(strlen(str)+1);
	strcpy(st, str);
	char* istr = strtok_r(st, separators, &context_strtok1);
	deftab* lines = ntg(ntgetptr(istr), 0);
	deftab* endlines = ntg(ntgetptr(istr), 1);
	char* macroopr = (char*)malloc(strlen(lines->str)+1);
	strcpy(macroopr, lines->str);
	char* macrooprc = (char*)malloc(strlen(lines->str)+1);
	strcpy(macrooprc, lines->str);
	char* iistr = strtok_r(macrooprc, separators, &context_strtok2);
	iistr = strtok_r(NULL, separators, &context_strtok2);
	if (istr != NULL){
		istr = strtok_r(NULL, separators, &context_strtok1);
		iistr = strtok_r(NULL, separators, &context_strtok2);
		while( (istr != NULL)&&(iistr !=NULL)){
			if (at == NULL)
				at = addargs(at, iistr, istr);
			else
				addargs(at, iistr, istr);
			istr = strtok_r(NULL, separators, &context_strtok1);
			iistr = strtok_r(NULL, separators, &context_strtok2);
		}
	}
	free(st);
	free(macroopr);
	free(macrooprc);
	lines = lines->next;
	while(lines != endlines){
		if (status > 0) define(lines->str);
		else if (strstr(lines->str, "macro")!=NULL){
			char* strc = (char*)malloc(strlen(lines->str)+1);
			strcpy(strc, lines->str);
			char* macroname = strtok(strc, separators);
			ntadd(macroname);
			nowmacro = (char*)malloc(strlen(macroname)+1);
			strcpy(nowmacro, macroname);
			define(lines->str);
			free(strc);
		}
		else{
			char* str_to_return = (char*)malloc(strlen(lines->str)+1);
			strcpy(str_to_return, lines->str);	
			char* context_strtok3;
			char* copystr = (char*)malloc(strlen(lines->str)+1);
			strcpy(copystr, lines->str);
			char* pstr = strtok_r(copystr, separators, &context_strtok3);
			while(pstr != NULL){
				char* arg = getarg(at, pstr);
				if (arg!=NULL) {
					replaceSubstring(str_to_return, pstr, arg);
				}
				pstr = strtok_r(NULL, separators, &context_strtok3);
			}	
			free(copystr);
			char* strc = (char*)malloc(strlen(str_to_return)+1);
			strcpy(strc, str_to_return);
			if (ntgetptr(strtok(strc, separators))!=NULL){
				argtab* ata = NULL;
				expand(str_to_return, ata);
			}
			else{
				retstr(str_to_return);
			}
			free(str_to_return);
			free(strc);
		}
		lines = lines->next;
	}
	if (status > 0)
		define(endlines->str);
	clear_argtab(at);
}
