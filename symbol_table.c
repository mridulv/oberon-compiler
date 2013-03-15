#include <stdio.h>
#include <malloc.h>
#include "symbol_table.h"
#include "ast.h"

extern unsigned long lineCount;

#define NULL 0

void createSymbolTable(SymbolTable* symbolTable)
{
	int scope = 0;
	tableEntry* owner;
	tableEntry* param;
	
	// // This is a special case, since it's the first entry.
	// symbolTable->first = createTableEntry("ORD",INTEGER,VAL,FUNC_NAME,0,NULL,0,++scope,NULL,0,NULL);
	// symbolTable->last = symbolTable->first;	
	// addFormalParameter(symbolTable, createTableEntry("_ORD",CHAR,VAL,IDENTIFIER,0,NULL,NULL,NULL,NULL,0,NULL));
	
	// // This is how it should normally be, with the optional addition of formal parameters.
	// addSymbolTableEntry(symbolTable, createTableEntry("CHR",CHAR,VAL,FUNC_NAME,0,NULL,0,++scope,NULL,0,NULL));	
	// addFormalParameter(symbolTable, createTableEntry("_CHR",INTEGER,VAL,IDENTIFIER,0,NULL,NULL,NULL,NULL,0,NULL));
	
	// addSymbolTableEntry(symbolTable, createTableEntry("TOINT",INTEGER,VAL,FUNC_NAME,0,NULL,0,++scope,NULL,0,NULL));
	// addFormalParameter(symbolTable, createTableEntry("_TOINT",REAL,VAL,IDENTIFIER,0,NULL,NULL,NULL,NULL,0,NULL));
	
	// addSymbolTableEntry(symbolTable, createTableEntry("TRUE",BOOLEAN,VAL,FUNC_NAME,0,NULL,0,++scope,NULL,0,NULL));
	
	// addSymbolTableEntry(symbolTable, createTableEntry("FALSE",BOOLEAN,VAL,FUNC_NAME,0,NULL,0,++scope,NULL,0,NULL));	
}

void destroySymbolTable(SymbolTable* symbolTable)
{
	tableEntry* te = symbolTable->first;
	tableEntry* last = te;
	
	while (te != NULL)
	{
		last = te;
		te = te->next;
		free(last);
	}	
}

int addSymbolTableEntry(SymbolTable* symbolTable, tableEntry* entry)
{
	if (findEntry(symbolTable, entry->name, entry->scope) != NULL)
	{
		if (entry->mode == PROC_NAME || entry->mode == FUNC_NAME)
		{
			printf("******  Procedure redeclared ******** \n");
		}
		else if (entry->mode == IDENTIFIER)
		{
			printf("******  Identifier Redeclared ******** \n");
		}
		free(entry);
		return 0;
	}
	else
	{
		symbolTable->last->next = entry;
		symbolTable->last = symbolTable->last->next;
		return 1;
	}
}

tableEntry* findEntry(SymbolTable* symbolTable, char* name, int scope)
{
	tableEntry* te = symbolTable->first;
	while (te != NULL)
	{
		if (strcmp(te->name,name) == 0 && te->scope == scope)
		{
			return te;
		}		
		te = te->next;
	}	
	return NULL;
}

void changeVariableType(SymbolTable* symbolTable, type_tableEntry *ty,int type)
{
	tableEntry* te = symbolTable->first;
	
	while (te != NULL)
	{
		if (te->type == NULL)
		{
			te->type = ty;
			te->t = type;
		}
		te = te->next;
	}
}

tableEntry* createTableEntry(char* name, type_tableEntry *type, int passType, int mode, int order,
							tableEntry* owner, int scope, int procScope, tableEntry* formal_params,
							int num_params, tableEntry* next)
{
	tableEntry* te = (tableEntry*)malloc(sizeof(tableEntry));
	if (te == NULL) return NULL;
	
	te->name = name;
	te->type = type;
	te->passType = passType;
	te->mode = mode;
	te->order = order;
	te->owner = owner;
	te->scope = scope;
	te->procScope = procScope;
	te->next = next;
	
	return te;
}

type_tableEntry* createtypeEntry(int type, type_tableEntry* tp,type_tableEntry* formal)
{
	type_tableEntry* te = (type_tableEntry*)malloc(sizeof(type_tableEntry));
	if (te == NULL) return NULL;
	
	te->type = type;
	te->tp = tp;
	te->formal_params = formal;
	
	return te;
}


tableEntry* getOwner(tableEntry* entry)
{
	if (entry == NULL) return NULL;
	
	while(entry->owner != NULL)
	{
		entry = entry->owner;
	}
	
	return entry;
}

tableEntry* getScopeOwner(SymbolTable* symbolTable, int scope)
{
	if (scope == 0) return NULL;
	
	tableEntry* te = symbolTable->first;
	while (te != NULL)
	{
		if (te->procScope == scope) return te;
		
		te = te->next;
	}
	
	return NULL;
}

void add_type_FormalParameter(type_tableEntry *p,tableEntry *t){
	if (p->formal_params == NULL){
		p->formal_params = t;
		t->next = NULL;
	}
	else{
		struct tableEntry *k;
		k = p->formal_params;
		while (k->next != NULL){
			k=k->next;
		}
		k->next = t;
	}
}

void change_type_FormalParamType (type_EntryTable *p , type_EntryTable *t){
	type_tableEntry *k = p->last;
	tableEntry  *u = k->formal_params;
	while (u->next != NULL){
		if (u->type != NULL){
			u->type = t;
		}
	}
}

type_EntryTable *create_typeEntry(){
	type_EntryTable *p = (type_EntryTable*)malloc(sizeof(type_EntryTable));
	p->first = NULL;
	p->last = NULL;
	return p;
}

void insert_last(type_EntryTable *t,type_tableEntry *p){
	if (t->first == NULL)
		t->first = p;
	else{
		type_tableEntry *k = t->first;
		while(k->next != NULL){
			k=k->next;
		}
		k->next = p;
		t->last = p;
	}
}

void remove_last(type_EntryTable *t){
	if (t->first == NULL)
		return;
	if (t->first->next == NULL){
		free(t->first);
		t->first = NULL;
		t->last = NULL;
	}
	else{
		type_tableEntry *k = t->first;
		type_tableEntry *prev = t->first;
		while(k->next != NULL){
			prev = k;
			k=k->next;
		}
		free(k);
		prev->next = NULL;
		t->last = prev;
	}
}

type_tableEntry *type_lookup(SymbolTable *sym,char *look,int scope){
	tableEntry* te = sym->first;
	while (te != NULL)
	{
		if (strcmp(te->name,look) == 0 && te->scope == scope && te->t == TYPE_VALUE)
		{
			return te->type;
		}		
		te = te->next;
	}	
	return NULL;
}

int module_lookup_bool(SymbolTable *sym,char *look){
	tableEntry* te = sym->first;
	while (te != NULL)
	{
		if (strcmp(te->name,look) == 0 && te->t == MODULE_VALUE)
		{
			return 1;
		}		
		te = te->next;
	}	
	return 0;
}

void print_Symbol(SymbolTable* symbolTable){
	tableEntry *te = symbolTable -> first;
	while (te!=NULL){
		printf("***********%s*********** \n",te->name);
		te=te->next;
	}
}