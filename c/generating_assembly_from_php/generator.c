/*
    * Projekt: Implementace překladače imperativního jazyka IFJ22
    *
    * @brief generátor cílového kódu implementace
    *
    * @author   Kryštof Andrýsek        xandry12
    * @author   Ondřej Dacík            xdacik01
    * @author   Dalibor Kalina          xkalin16
    * @author   Jan Kvapil              xkvapi19
    *
*/

// Detailnější popis funkcí je v hlavičkovém souboru

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "stack.h"
#include "generator.h"
#include "synan.h"

#define TRUE 1
#define FALSE 0
#define NUM_OF_REGS 6
#define MAX_VARNAME_SIZE 256
#define MAX_STACK_SIZE 100
#define MAX_CODE_LENGTH 5000

#define ADD 1
#define SUB 2
#define MUL 3
#define DIV 4
#define IDIV 5
#define LT 6
#define GT 7
#define EQ 8
#define NEQ 9
#define EXIT 10
#define DPRINT 11
#define I2F 12
#define F2I 13
#define I2C 14

// Globalni promenne
bool inFunc = false;
char *tempOut;
int tempOutSize = 200;
char *tempOut2;
int tempOut2Size = 200;
char *codeOut;
int codeOutSize = 200;
struct g_Stack* if_stack;
struct g_Stack* while_stack;
int if_count = 0;
int if_vnoreni = 0;
int while_count = 0;
int while_vnoreni = 0;
bool while_preparation = false;
int num_of_params = 0;
bool ord_func_exist = false;
bool substring_func_exist = false;
bool floatval_func_exist = false;
bool intval_func_exist = false;
bool strval_func_exist = false;
bool concat_datatype_exist = false;
bool ltgt_datatype_exist = false;
int datatype_conversion = 0;

node_t * global_frame = NULL;
node_t * local_frame = NULL;

// Obdoba funkce strcat() ze string.h knihovny.
// Funkce alokuje víc místa pro cílovou proměnnou, pokud v ní dostatek místa není
void generate(char **destination, char *source)
{
    int *destSize;
    if(*destination == codeOut) destSize = &codeOutSize;
    else if(*destination == tempOut) destSize = &tempOutSize;
    else if(*destination == tempOut2) destSize = &tempOut2Size;
    
    bool resize = false;
    while((int)(strlen(source) + strlen(*destination)) >= (*destSize) - 5)
    {
        resize = true;
        (*destSize) = (*destSize) * 2;
    }
    if(resize)
    {
        *destination = realloc(*destination, (*destSize));
        if(*destination == NULL)
            error(ERROR_INTERNAL, "Chybka pri realokaci pameti v generatoru");
    }
    
    strcat(*destination, source);
}


// Následující 3 funkce slouží pro převod INT hodnot do konkrétních STRING hodnot.
//  get_name    převádí na aritmetické a relační operace a pár dalších instrukcí
//  get_func    vrátí GF nebo LF dle toho, zda se překlad aktuálně nachází v deklaraci funkce či ne
//  get_type    převede na datový typ používaného tokenu
const char *get_name(int value) {
    switch(value) {
    case ADD:
        return "ADD";
    case SUB:
        return "SUB";
    case MUL:
        return "MUL";
    case DIV:
        return "DIV";
    case IDIV:
        return "IDIV";
    case LT:
        return "LT";
    case GT:
        return "GT";
    case EQ:
        return "EQ";
    case NEQ:
        return "NEQ";
    case EXIT:
        return "EXIT";
    case DPRINT:
        return "DPRINT";
    case I2F:
        return "INT2FLOAT";
    case F2I:
        return "FLOAT2INT";
    case I2C:
        return "INT2CHAR";
    default:
        return "ERROR";
    }
}
const char *get_func(bool value) {
    if (value)
        return "LF@";
    else
        return "GF@";
}
const char *get_type(char value) {
    switch (value){
        case 'i':
            return "int@";
        case 'f':
            return "float@";
        case 'b':
            return "bool@";
        case 's':
            return "string@";
        case 'n':
            return "nil@";
        default:
            return "ERROR";
    }
    
}

// Několik funkcí pro tvorbu a užití jednosměrně vázaného seznamu,
// který se používá na uchování deklarovaných proměnných a jejich datového typu
// (struktura pro tyto funkce se nachází v generator.h)

void list_print(node_t * head) {
    node_t * current = head;
    while (current != NULL) {
        printf("%s\t%c\t%s\n", current->data,current->type,current->temp ? "TEMPORARY VARIABLE" : "");
        current = current->next;
    }
}

void list_push(node_t * head, char *data,char type) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = (node_t *) malloc(sizeof(node_t));
    if (current->next == NULL){
        error(ERROR_INTERNAL, "\tMALLOC ERROR V LIST PUSH S DATA = %s", data);
    }
    current->next->data = malloc(1024);
    if (current->next->data == NULL){
        error(ERROR_INTERNAL, "\tMALLOC ERROR V LIST PUSH S DATA = %s", data);
    }
    strcpy(current->next->data,data);
    current->next->next = NULL;
    current->next->type = type;
    current->next->temp = FALSE;
}

char* list_pop(node_t ** head) {
    char* retdata = "-1";
    node_t * next_node = NULL;

    if (*head == NULL) {
        return "-1";
    }

    next_node = (*head)->next;
    retdata = (*head)->data;
    free(*head);
    *head = next_node;

    return retdata;
}

char* list_remove_by_value(node_t ** head, char *data) {
    node_t *previous, *current;

    if (*head == NULL) {
        return "-1";
    }

    if (strcmp(data,(*head)->data)==0) {
        return list_pop(head);
    }

    previous = *head;
    current = (*head)->next;
    while (current) {
        if (strcmp(data,current->data) == 0) {
            previous->next = current->next;
            free(current);
            return data;
        }

        previous = current;
        current  = current->next;
    }
    return "-1";
}

void list_delete(node_t *head) {
    node_t  *current = head, 
            *next = head;

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }
}

int list_exist(node_t *head, char* data){
    node_t  *current = head;
    int retval = 0;
    while (current) {
        if (strcmp(data,current->data)==0) {
            return retval;
        }
        current = current->next;
        retval++;
    }
    return -1;
}
// Konec funkcí pro tvorbu seznamu      ;


// Inicializace potřebných proměnných pro generátor

int gen_init(){
    // - seznam obsahující proměnné deklarované v hlavním těle ifj22
    global_frame = (node_t *) malloc(sizeof(node_t));
    if (global_frame == NULL){
        error(ERROR_INTERNAL, "Malloc chyba v generatoru - gen_init\n");
        return -1;
    }
    global_frame->data = "";
    global_frame->type = '\0';
    global_frame->next = NULL;
    global_frame->temp = false;

    // - seznam obsahující promměnné deklarované ve funkcích v ifj22
    // - seznam je mazán před a po vstupu do deklarace dané funkce 
    local_frame = (node_t *) malloc(sizeof(node_t));
    if (local_frame == NULL){
        error(ERROR_INTERNAL, "Malloc chyba v generatoru - gen_init\n");
        return -1;
    }
    local_frame->data = "";
    local_frame->type = '\0';
    local_frame->next = NULL;
    local_frame->temp = false;

    // - zásobníky pro správnou generaci if a while (resp. pro jejich názvy návěští a skoky)
    if_stack = g_createStack(MAX_STACK_SIZE);
    while_stack = g_createStack(MAX_STACK_SIZE);

    // - alokace místa pro buffery, využívané k ukládání výstupu generátoru
    codeOut = malloc(codeOutSize);
    tempOut = malloc(tempOutSize);
    tempOut2 = malloc(tempOut2Size);
    if(codeOut == NULL || tempOut == NULL || tempOut2 == NULL)
        error(ERROR_INTERNAL, "Chyba pri alokaci pameti v generatoru");
    strcpy(codeOut, "");
    strcpy(tempOut, "");
    strcpy(tempOut2, "");

    return 1;
}

// Výsledný tisk výsledku generátoru na standartní výstup 

void gen_output(){
    // Generace vestavěných funkcí a funkcí pro kontorlu datového typu
    // Generují se jen ty, které jsou potřeba (resp. ty, které jsou potřeba/byly použity)
    if (ord_func_exist)
        gen_func_ord();
    if (substring_func_exist)
        gen_func_substring();
    if (floatval_func_exist)
        gen_func_floatval();
    if (intval_func_exist)
        gen_func_intval();
    if (strval_func_exist)
        gen_func_strval();
    if (concat_datatype_exist)
        gen_concat_datatype();
    if (ltgt_datatype_exist)
        gen_ltgt_datatype();
    if ((if_count>0)||(while_count>0))
        gen_ifwhile_datatype();
    
    // Generace chybových výstupů
    generate(&codeOut,"\n");
    gen_jump("error_skip");
    gen_label("error_func_7");
    gen_exit("i","7");
    gen_jump("error_skip");
    gen_label("error_func_4");
    gen_exit("i","4");
    gen_jump("error_skip");
    gen_label("error_func_5");
    gen_exit("i","5");
    gen_jump("error_skip");
    gen_label("error_skip");

    // Finální tisk na stdout
    printf("%s\n", codeOut);
}

// Uvolnění alokovaných zdrojů generátoru
void gen_cleanup()
{
    free(codeOut);
    free(tempOut);
    free(tempOut2);
    list_delete(local_frame);
    list_delete(global_frame);

    while (!(g_isEmpty(if_stack)))
        g_pop(if_stack);
    while (!(g_isEmpty(while_stack)))
        g_pop(while_stack);

}


// 
// Generace hlavičky IFJcode22 a deklarace proměnné pro kontrolu datového typu proměnných

void gen_header(){
    generate(&codeOut,".IFJcode22\n\n");
    gen_variable("v","!temp_typeCheck");
}

// Deklarace nových proměnných a přiřazování do proměnných
int gen_variable(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);

    // Určení, do jakého rámce se proměnné deklarují/na jakém rámci se nachází
    char *name = va_arg(args,char*);
    node_t *inFunc_tempframe = NULL;
    if (inFunc){
        inFunc_tempframe = local_frame;
    }
    else {
        inFunc_tempframe = global_frame;
    }

    // Pokud proměnná není deklarovaná, vytvoří se
    node_t *current = inFunc_tempframe;
    int pos = list_exist(inFunc_tempframe,name);
    if (pos == -1){   
        // - správné uložení do seznamu
        if (strlen(fmt)>1)
            list_push(inFunc_tempframe,name,fmt[1]);
        else{
            list_push(inFunc_tempframe,name,'v');
        }
        pos = list_exist(inFunc_tempframe,name);
        for (int i = 0; i<pos; i++){
            current = current->next;
        }
        if (*fmt == 't'){
            current->temp = true;
        }
        strcpy(current->data,name);

        // - generace instrukce
        char temp[13 + strlen(name)];
        sprintf(temp,"DEFVAR\t%s%s\n",current->temp ? "TF@" : get_func(inFunc),name);
        generate(&codeOut,temp);
    }
    else {
        for (int i = 0; i<pos; i++){
            current = current->next;
        }
    }

    // Pokud má proběhnout, přiřazení do proměnné (buď při její deklaraci nebo kdykoliv za překladu)
    if (strlen(fmt)>1){
        ++fmt;
        char *value = va_arg(args,char*);
        
        //kontrola, jestli je $promenna definovana
        if(*fmt == 'v' && value[0] == '$')
        {
            char tempAA[100];
            char tempBB[100];
            sprintf(tempAA,"TYPE\tGF@!temp_typeCheck\t%s%s\n",get_func(inFunc), value);
            sprintf(tempBB,"JUMPIFEQ\terror_func_5\tGF@!temp_typeCheck\tstring@\n");
            if (while_preparation){
                generate(&tempOut2,tempAA);
                generate(&tempOut2,tempBB);
            }
            else if ((if_vnoreni > 0)||(while_vnoreni > 0)){
                generate(&tempOut,tempAA);
                generate(&tempOut,tempBB);
            }
            else{
                generate(&codeOut,tempAA);
                generate(&codeOut,tempBB);
            }
        }
        //konec kontroly

        // Přiřazení do proměnné
        char temp2[64 + strlen(name) + strlen(value)];
        if (*fmt == 'v'){           // R hodnota je jiná proměnná (kontrola existence této proměnné)
            pos = list_exist(inFunc_tempframe,value);
            if (pos != -1){
                node_t *current_temp = inFunc_tempframe;
                for (int i = 0; i<pos; i++){
                    current_temp = current_temp->next;
                }
                current->type = current_temp->type;
                sprintf(temp2,"MOVE\t%s%s\t%s%s\n",current->temp ? "TF@" : get_func(inFunc),name,current_temp->temp ? "TF@" : get_func(inFunc),value);
            }
            else {
                gen_exit("i","5");
                va_end(args);
                return -1;
            }
        }
        else if (*fmt == 'f'){      // R hodnota je typu float (má speciální zápis)
            double conv_value = atof(value);
            sprintf(temp2,"MOVE\t%s%s\t%s%a\n",current->temp ? "TF@" : get_func(inFunc),name,get_type(*fmt),conv_value);
        }
        else if (*fmt == 's'){      // R hodnota je typu string (má speciální zápis)
            sprintf(temp2,"MOVE\t%s%s\t%s%s\n",current->temp ? "TF@" : get_func(inFunc),name,get_type(*fmt),convertString(value));
        }
        else {                      // R hodnota je jiný libovolný datový typ
            sprintf(temp2,"MOVE\t%s%s\t%s%s\n",current->temp ? "TF@" : get_func(inFunc),name,get_type(*fmt),value);

            current->type = *fmt;
        }

        if (while_preparation)
            generate(&tempOut2,temp2);
        else if ((if_vnoreni > 0)||(while_vnoreni > 0))
            generate(&tempOut,temp2);
        else
            generate(&codeOut,temp2);
    }
    else {
        // deklarace nebo znovudeklarace
    }

    va_end(args);
    return 1;
}

// Generace instrukcí pro práci s rámci
void gen_createframe(){
    if (while_preparation)
        generate(&tempOut2,"CREATEFRAME\n");
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,"CREATEFRAME\n");
    else
        generate(&codeOut,"CREATEFRAME\n");
}

void gen_pushframe(){
    if (while_preparation)
        generate(&tempOut2,"PUSHFRAME\n");
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,"PUSHFRAME\n");
    else
        generate(&codeOut,"PUSHFRAME\n");
}

void gen_popframe(){
    if (while_preparation)
        generate(&tempOut2,"POPFRAME\n");
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,"POPFRAME\n");
    else
        generate(&codeOut,"POPFRAME\n");
}
// Konec instrukcí pro práci s rámci

// Generace volání funkce
int gen_call(char name[], int param_count, int n){
    // Kontrola deklarace předávaných argumentů
    gen_variable("v","symb_1_type");
    for (int i = 0; i<param_count; i++){
        char tempA[100];
        char tempB[100];
        sprintf(tempA,"TYPE\t%ssymb_1_type\t%s!temp_fcall_%d_%d\n",get_func(inFunc),get_func(inFunc),n,i);
        sprintf(tempB,"JUMPIFEQ\terror_func_5\t%ssymb_1_type\tstring@\n",get_func(inFunc));
        if (while_preparation){
            generate(&tempOut2,tempA);
            generate(&tempOut2,tempB);
        }
        else if ((if_vnoreni > 0)||(while_vnoreni > 0)){
            generate(&tempOut,tempA);
            generate(&tempOut,tempB);
        }
        else{
            generate(&codeOut,tempA);
            generate(&codeOut,tempB);
        }
    }

    // Volání některých vestavěných funkcí má speciální pravidla a po jejich provedení se gen_call() ukončí návratovou hodnotou
    if (strcmp(name,"write")==0){
        gen_write(param_count,n);
        return 1;
    }
    if (strcmp(name,"strlen")==0){
        char temp[64];
        sprintf(temp,"STRLEN\t%s!temp_exp_result\t%s!temp_fcall_%d_0\n",get_func(inFunc),get_func(inFunc),n);
        if (while_preparation)
            generate(&tempOut2,temp);
        else if ((if_vnoreni > 0)||(while_vnoreni > 0))
            generate(&tempOut,temp);
        else
            generate(&codeOut,temp);
        return 1;
    }
    if (strcmp(name,"chr")==0){
        char temp[64];
        sprintf(temp,"INT2CHAR\t%s!temp_exp_result\t%s!temp_fcall_%d_0\n",get_func(inFunc),get_func(inFunc),n);
        if (while_preparation)
            generate(&tempOut2,temp);
        else if ((if_vnoreni > 0)||(while_vnoreni > 0))
            generate(&tempOut,temp);
        else
            generate(&codeOut,temp);
        return 1;
    }
    if (strcmp(name,"readi")==0){
        gen_read("!temp_exp_result","int");
        return 1;   
    }
    if (strcmp(name,"readf")==0){
        gen_read("!temp_exp_result","float");
        return 1;
    }
    if (strcmp(name,"reads")==0){
        gen_read("!temp_exp_result","string");
        return 1;
    }

    // Jiné vestavěné funkce, u kterých jsou pravidla volání stejné jako u uživatelsky vytvořených funkcí.
    // Nastavovaná hodnota určuje pozdější generaci této funkce na konec kódu IFJcode22
    if (strcmp(name,"substring")==0)
        substring_func_exist = true;
    if (strcmp(name,"ord")==0)
        ord_func_exist = true;
    if (strcmp(name,"floatval")==0)
        floatval_func_exist = true;
    if (strcmp(name,"intval")==0)
        intval_func_exist = true;
    if (strcmp(name,"strval")==0)
        strval_func_exist = true;
    
    
    // Předání argumentů proměnné (resp. přesun argumentů do dočasného rámce)
    gen_createframe();
    if (param_count != 0){
        for (int i = 0; i<param_count;i++){
            char tempA[64];
            char tempB[64];
            sprintf(tempA,"DEFVAR\tTF@%%%d\n",i);
            sprintf(tempB,"MOVE\tTF@%%%d\t%s!temp_fcall_%d_%d\n",i,get_func(inFunc),n,i);
            if (while_preparation){
                generate(&tempOut2,tempA);
                generate(&tempOut2,tempB);
            }
            else if ((if_vnoreni > 0)||(while_vnoreni > 0)){
                generate(&tempOut,tempA);
                generate(&tempOut,tempB);
            }
            else {
                generate(&codeOut,tempA);
                generate(&codeOut,tempB);
            }
        }
    }
    
    // Zavolání funkce
    char tempB[10 + strlen(name)];
    sprintf(tempB,"CALL\t%s?\n",name);

    // Uložení návratové hodnoty
    char temp1[64];
    sprintf(temp1,"MOVE\t%s!temp_exp_result\tTF@%%retval_from_func\n",get_func(inFunc));
    
    if (while_preparation){
        generate(&tempOut2,tempB);
        generate(&tempOut2,temp1);
    }
    else if ((if_vnoreni > 0)||(while_vnoreni > 0)){
        generate(&tempOut,tempB);
        generate(&tempOut,temp1);
    }
    else {
        generate(&codeOut,tempB);
        generate(&codeOut,temp1);
    }
    return 1;
}

// Ukládání návratové hodnoty funkce do předem určené proměnné ve funkci.
// Tato proměnná pak slouží pro získání návratové hodnoty po zavolání funkce.
void gen_return(char fmt, bool return_value, bool is_nullable){
    // Je-li zavolán return mimo funkci, ukončuje se překlad
    if (!inFunc){
        gen_exit("i","0");
    }
    else {
        char temp[64];
        if (return_value){  // Má-li funkce mít návratovou hodnotu
            
            // Přesun do předem určené proměnné 
            sprintf(temp,"MOVE\tLF@%%retval_from_func\t%s!temp_exp_result\n",get_func(inFunc));
            if (while_preparation)
                generate(&tempOut2,temp);
            else if ((if_vnoreni > 0)||(while_vnoreni > 0))
                generate(&tempOut,temp);
            else
                generate(&codeOut,temp);
            
            // Kontrola datového typu proměnné
            char conv_temp[100];
            gen_variable("v","symb_1_type");
            sprintf(conv_temp,"TYPE\t%ssymb_1_type\tLF@%%retval_from_func\n",get_func(inFunc));
            if (while_preparation)
                generate(&tempOut2,conv_temp);
            else if ((if_vnoreni > 0)||(while_vnoreni > 0))
                generate(&tempOut,conv_temp);
            else
                generate(&codeOut,conv_temp);

            if (!is_nullable){  // Proměnná nemůže být NULL (resp. nejedná se o ?int, ?float...)
                gen_jumpifeq("lvs","error_func_4","symb_1_type","nil");
                switch (fmt){
                    case 'i':
                        sprintf(conv_temp,"int");
                        break;
                    case 'f':
                        sprintf(conv_temp,"float");
                        break;
                    case 's':
                        sprintf(conv_temp,"string");
                        break;
                    case 'b':
                        sprintf(conv_temp,"bool");
                        break;
                    case 'n':
                        sprintf(conv_temp,"nil");
                        break;
                }
                gen_jumpifneq("lvs","error_func_4","symb_1_type",conv_temp);
            }
            else {      // Proměnná může být NULL (resp. jedná se o ?int, ?float...)
                switch (fmt){
                    case 'i':
                        sprintf(conv_temp,"int");
                        break;
                    case 'f':
                        sprintf(conv_temp,"float");
                        break;
                    case 's':
                        sprintf(conv_temp,"string");
                        break;
                    case 'b':
                        sprintf(conv_temp,"bool");
                        break;
                    case 'n':
                        sprintf(conv_temp,"nil");
                        break;
                }
                char conv_temp2[100];
                sprintf(conv_temp2,"!return_param_nil_test_%d",datatype_conversion);
                gen_jumpifeq("lvs",conv_temp2,"symb_1_type",conv_temp);
                gen_jumpifneq("lvs","error_func_4","symb_1_type","nil");
                gen_label(conv_temp2);
                datatype_conversion++;
            }
        }
        gen_popframe();
        if (while_preparation)
            generate(&tempOut2,"RETURN\n");
        else if ((if_vnoreni > 0)||(while_vnoreni > 0))
            generate(&tempOut,"RETURN\n");
        else
            generate(&codeOut,"RETURN\n");
    }
}

// Generování začátku deklarace funkce
int gen_func_begin(char name[]){
    char tempA[8 + strlen(name)];
    char tempB[9 + strlen(name)];
    sprintf(tempA,"JUMP\t%s!\n\n",name);    // Skok na návěští za deklarací funkce
    sprintf(tempB,"LABEL\t%s?\n",name);     // Návěští začátku funkce
    
    if (while_preparation){
        generate(&tempOut2,tempA);
        generate(&tempOut2,tempB);
    }
    else if ((if_vnoreni > 0)||(while_vnoreni > 0)){
        generate(&tempOut,tempA);
        generate(&tempOut,tempB);
    }
    else{
        generate(&codeOut,tempA);
        generate(&codeOut,tempB);
    }
    gen_pushframe();

    // Smazání a inicializace seznamu s lokálním rámcem
    inFunc = true;
    list_delete(local_frame);
    local_frame = (node_t *) malloc(sizeof(node_t));
    if (local_frame == NULL){
        error(ERROR_INTERNAL, "Malloc chyba v generatoru - gen_func_begin\n");
    }
    local_frame->data = malloc(1024);
    if (local_frame->data == NULL){
        error(ERROR_INTERNAL, "Malloc chyba v generatoru - gen_func_begin\n");
    }
    strcpy(local_frame->data,name);
    local_frame->type = 'x';
    local_frame->next = NULL;
    local_frame->temp = false;

    // Deklarace proměnné pro návratovou hodnotu
    gen_variable("vn","%retval_from_func","nil");
    

    return 1;
}

// Generování konce deklarace funkce
int gen_func_end(bool retvalVoid){
    
    if (retvalVoid){
        gen_return('v',false,false);
    }
    else {
        gen_exit("i","4");
    }

    // Návěští za koncem deklarace
    char temp[12 + strlen(local_frame->data)];
    sprintf(temp,"\nLABEL\t%s!\n\n",local_frame->data);
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);

    inFunc = false;

    // Smazání a inicializace seznamu s lokálním rámcem
    list_delete(local_frame);
    local_frame = (node_t *) malloc(sizeof(node_t));
    if (local_frame == NULL){
        error(ERROR_INTERNAL, "Malloc chyba v generatoru - gen_func_end\n");
        return -1;
    }
    local_frame->data = "";
    local_frame->type = '\0';
    local_frame->next = NULL;
    local_frame->temp = false;
    return 1;
}

// Vytvoření proměnné v lokálním rámci a přiřazení hodnoty argumentu
void gen_add_param(char fmt,char name[],int cnt, bool is_nullable){
    // Kontrola správného datového typu
    char conv_temp[100];
    gen_variable("v","symb_1_type");
    sprintf(conv_temp,"TYPE\t%ssymb_1_type\t%s%%%d\n",get_func(inFunc), get_func(inFunc), cnt);
    if (while_preparation)
        generate(&tempOut2,conv_temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,conv_temp);
    else
        generate(&codeOut,conv_temp);
    
    if (!is_nullable){  // Proměnná není nullovatelný typ (?int, ?float...)
        gen_jumpifeq("lvs","error_func_4","symb_1_type","nil");
        switch (fmt){
            case 'i':
                sprintf(conv_temp,"int");
                break;
            case 'f':
                sprintf(conv_temp,"float");
                break;
            case 's':
                sprintf(conv_temp,"string");
                break;
            case 'b':
                sprintf(conv_temp,"bool");
                break;
            case 'n':
                sprintf(conv_temp,"nil");
                break;
        }
        gen_jumpifneq("lvs","error_func_4","symb_1_type",conv_temp);
    }
    else {      // Proměnná je nullovatelný typ (?int, ?float...)
        switch (fmt){
            case 'i':
                sprintf(conv_temp,"int");
                break;
            case 'f':
                sprintf(conv_temp,"float");
                break;
            case 's':
                sprintf(conv_temp,"string");
                break;
            case 'b':
                sprintf(conv_temp,"bool");
                break;
            case 'n':
                sprintf(conv_temp,"nil");
                break;
        }
        char conv_temp2[100];
        sprintf(conv_temp2,"!add_param_nil_test_%d",datatype_conversion);
        gen_jumpifeq("lvs",conv_temp2,"symb_1_type",conv_temp);
        gen_jumpifneq("lvs","error_func_4","symb_1_type","nil");
        gen_label(conv_temp2);
        datatype_conversion++;
    }

    // Deklarace a přiřazení do nové proměnné
    char temp[40];
    gen_variable("v",name);
    sprintf(temp,"MOVE\tLF@%s\tLF@%%%d\n",name,cnt);
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
}


// Generace začátku podmínky
void gen_if_begin(){
    // Kontrola a konverze datového typu výrazu
    char temp[60];
    gen_createframe();
    sprintf(temp,"MOVE\tTF@%%0\t%s!temp_exp_result\n",get_func(inFunc));
    generate(&tempOut,"DEFVAR\tTF@%0\n");
    generate(&tempOut,temp);
    generate(&tempOut,"CALL\tifwhile_datatype_kontrola\n");
    sprintf(temp,"MOVE\t%s!temp_exp_result\tTF@%%retval_from_func\n",get_func(inFunc));
    generate(&tempOut,temp);
    // konec kontroly

    if_vnoreni++;

    // Skok na else-návěští
    sprintf(temp,"JUMPIFEQ\telse?_%d\t%s!temp_exp_result\tbool@false\n",if_count,get_func(inFunc));
    generate(&tempOut,temp);
    g_push(if_stack,if_count);
    if_count++;
}

// Generace začátku else-návěští
void gen_else_begin(){
    int if_num = g_pop(if_stack);
    char temp[32];
    sprintf(temp,"JUMP\telse_end?_%d\n",if_num);
    generate(&tempOut,temp);
    sprintf(temp,"LABEL\telse?_%d\n",if_num);
    generate(&tempOut,temp);
    g_push(if_stack,if_num);
}

// Generace konce podmínky
void gen_else_end(){
    char temp[32];
    sprintf(temp,"LABEL\telse_end?_%d\n",g_pop(if_stack));
    generate(&tempOut,temp);
    if_vnoreni--;
    if ((if_vnoreni == 0)&&(while_vnoreni == 0)){
        generate(&codeOut,tempOut);
        strcpy(tempOut,"\0");
    }
        
}

// Generace začátku výpočtu výrazu pro cyklus
void gen_while_begin1(){
    while_vnoreni++;
    while_preparation = true;
}

// Konec výpočtu výrazu pro cyklus a začátek deklarace cyklu
void gen_while_begin2(){
    // Kontrola a konverze datového typu výrazu
    char temp[60];
    gen_createframe();
    sprintf(temp,"MOVE\tTF@%%0\t%s!temp_exp_result\n",get_func(inFunc));
    generate(&tempOut2,"DEFVAR\tTF@%0\n");
    generate(&tempOut2,temp);
    generate(&tempOut2,"CALL\tifwhile_datatype_kontrola\n");
    sprintf(temp,"MOVE\t%s!temp_exp_result\tTF@%%retval_from_func\n",get_func(inFunc));
    generate(&tempOut2,temp);
    // konec kontroly

    while_preparation = false;

    // Generace návěští začátku cyklu
    sprintf(temp,"LABEL\twhile?_%d\n",while_count);
    generate(&tempOut,temp);
    generate(&tempOut,tempOut2);
    // Generace skoku před výpočet výrazu
    sprintf(temp,"JUMPIFEQ\twhile!_%d\t%s!temp_exp_result\tbool@false\n",while_count,get_func(inFunc));
    generate(&tempOut,temp);

    g_push(while_stack,while_count);
    while_count++;
    strcpy(tempOut2,"\0");
}

// Generování konce deklarace cyklu
void gen_while_end(){
    char temp[20];
    int i = g_pop(while_stack);
    sprintf(temp,"JUMP\twhile?_%d\n",i);    
    generate(&tempOut,temp);
    sprintf(temp,"LABEL\twhile!_%d\n",i);    
    generate(&tempOut,temp);
    while_vnoreni--;
    if ((if_vnoreni == 0)&&(while_vnoreni == 0)){
        generate(&codeOut,tempOut);
        strcpy(tempOut,"\0");
    }
}


void gen_insert(char *str)
{
    generate(&tempOut, str);
}

void gen_for_break(long breakLevel){
    char temp[25];
    int i = g_peek(while_stack);
    if(while_stack->top + 1 < breakLevel)
        error(ERROR_SEMANTIC, "Break mimo cyklus");
    sprintf(temp,"JUMP\twhile!_%d\n",i - (int)breakLevel + 1);    
    generate(&tempOut,temp);
}

void gen_for_continue(long continueLevel){
    char temp[25];
    int i = g_peek(while_stack);
    if(while_stack->top + 1 < continueLevel)
        error(ERROR_SEMANTIC, "Continue mimo cyklus");
    sprintf(temp,"JUMP\twhile?_%d\n",i - (int)continueLevel + 1);    
    generate(&tempOut,temp);
}


// Generace všech aritmetických operací
int gen_ari(char fmt[],char var[],char A[], char B[],int op){
    // Kontrola datových typů 
        char conv_temp[100];

        generate(&codeOut,"\n# Kontrola datovych typu pro aritmeticke operace\n\n");

        gen_type("vv","symb_1_type",A);
        gen_type("vv","symb_2_type",B);
        gen_jumpifeq("lvs","error_func_5","symb_1_type","");
        gen_jumpifeq("lvs","error_func_5","symb_2_type","");
        sprintf(conv_temp,"correct_type_symb1_%d",datatype_conversion);
        gen_jumpifeq("lvs",conv_temp,"symb_1_type","int");
        gen_jumpifeq("lvs",conv_temp,"symb_1_type","float");
        gen_jumpifneq("lvs","error_func_7","symb_1_type","nil");
        gen_variable("vi",A,"0");
        gen_type("vv","symb_1_type",A);
        gen_label(conv_temp);
        sprintf(conv_temp,"correct_type_symb2_%d",datatype_conversion);
        gen_jumpifeq("lvs",conv_temp,"symb_2_type","int");
        gen_jumpifeq("lvs",conv_temp,"symb_2_type","float");
        gen_jumpifneq("lvs","error_func_7","symb_2_type","nil");
        gen_variable("vi",B,"0");
        gen_type("vv","symb_2_type",B);

        gen_label(conv_temp);
        if ((op==ADD)||(op==MUL)||(op==SUB)){
            sprintf(conv_temp,"no_conversion_needed_%d",datatype_conversion);
            gen_jumpifeq("lvv",conv_temp,"symb_1_type","symb_2_type");
            sprintf(conv_temp,"conversion_of_symb1_%d",datatype_conversion);
            gen_jumpifeq("lvs",conv_temp,"symb_1_type","int");
            gen_int2float("vv",B,B);
            sprintf(conv_temp,"no_conversion_needed_%d",datatype_conversion);
            gen_jump(conv_temp);
            sprintf(conv_temp,"conversion_of_symb1_%d",datatype_conversion);
            gen_label(conv_temp);
            gen_int2float("vv",A,A);
            sprintf(conv_temp,"no_conversion_needed_%d",datatype_conversion);
            gen_label(conv_temp);
        }
        else if (op==DIV){
            sprintf(conv_temp,"conversion_of_symb2_%d",datatype_conversion);
            gen_jumpifeq("lvs",conv_temp,"symb_1_type","float");
            gen_int2float("vv",A,A);
            gen_label(conv_temp);
            sprintf(conv_temp,"no_conversion_needed_%d",datatype_conversion);
            gen_jumpifeq("lvs",conv_temp,"symb_2_type","float");
            gen_int2float("vv",B,B);
            gen_label(conv_temp);
        }
        else if (op==IDIV){
            sprintf(conv_temp,"conversion_of_symb2_%d",datatype_conversion);
            gen_jumpifeq("lvs",conv_temp,"symb_1_type","int");
            gen_float2int("vv",A,A);
            gen_label(conv_temp);
            sprintf(conv_temp,"no_conversion_needed_%d",datatype_conversion);
            gen_jumpifeq("lvs",conv_temp,"symb_2_type","int");
            gen_float2int("vv",B,B);
            gen_label(conv_temp);
        }
        datatype_conversion++;
        generate(&codeOut,"\n# Konec kontroly datovych typu pro aritmeticke operace\n\n");
    // Konec kontroly
    
    // Určení aktuálního rámce (resp. zda se nacházíme ve funkci)
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }
    
    node_t *current = act_frame;
    int pos = list_exist(act_frame,var);
    if (pos==-1) {  // Proměnná pro výsledek není deklarovaná -> deklaruje se
        gen_variable("v",var);
        pos= list_exist(act_frame,var);
        for (int i = 0; i<pos; i++){
            current = current->next;
        }
        current->type = 'i';
    }
    else {
        for (int i = 0; i<pos; i++){
            current = current->next;
        }
        if (current->type != 'f')
            current->type = 'i';
    }

    // Generace konkrétní instrukce
    
    char temp[30 + strlen(var) + strlen(A) + strlen(B)];
    if ((fmt[1] != 'v')&&(fmt[2] != 'v')){  // Oba operandy nejsou proměnná. Je nutné rozlišovat mezi int a float
        if (fmt[1] == 'f'){
            double conv_value_A = atof(A);
            if (fmt[2]  == 'f'){
                double conv_value_B = atof(B);
                sprintf(temp,"%s\t%s%s\t%s%a\t%s%a\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),conv_value_A, get_type(fmt[2]),conv_value_B);
            }
            else {
                sprintf(temp,"%s\t%s%s\t%s%a\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),conv_value_A, get_type(fmt[2]),B);
            }
        }
        else if (fmt[2]  == 'f'){
            double conv_value_B = atof(B);
            sprintf(temp,"%s\t%s%s\t%s%s\t%s%a\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),A, get_type(fmt[2]),conv_value_B);
        }
        else{
            sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),A, get_type(fmt[2]),B);
        }
        if ((fmt[1] == 'f')||(fmt[2] == 'f'))
            current->type = 'f';
    }
    else if ((fmt[1] == 'v')&&(fmt[2] != 'v')){     // Jeden operand je proměnná (musí být existovat) a druhý je int nebo float
        if (list_exist(act_frame,A)==-1){
            gen_exit("i","5");
            return -1;
        }
        node_t *var_A_temp = act_frame;
        pos= list_exist(act_frame,A);
        for (int i = 0; i<pos; i++){
            var_A_temp = var_A_temp->next;
        }
        if ((var_A_temp->type == 'f')||(fmt[2] == 'f'))
            current->type = 'f';

        if (fmt[2]  == 'f'){
            double conv_value_B = atof(B);
            sprintf(temp,"%s\t%s%s\t%s%s\t%s%a\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_func(inFunc),A, get_type(fmt[2]),conv_value_B);
        }
        else 
            sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, var_A_temp->temp ? "TF@" : get_func(inFunc),A, get_type(fmt[2]),B);
    }
    else if ((fmt[1] != 'v')&&(fmt[2] == 'v')){ // Jeden operand je proměnná (musí být existovat) a druhý je int nebo float
        if (list_exist(act_frame,B)==-1){
            gen_exit("i","5");
            return -1;
        }
        node_t *var_B_temp = act_frame;
        pos= list_exist(act_frame,B);
        for (int i = 0; i<pos; i++){
            var_B_temp = var_B_temp->next;
        }
        if ((var_B_temp->type == 'f')||(fmt[1] == 'f'))
            current->type = 'f';
        
        if (fmt[1]  == 'f'){
            double conv_value_A = atof(A);
            sprintf(temp,"%s\t%s%s\t%s%a\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),conv_value_A, get_func(inFunc),B);
        }
        else
            sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),A, var_B_temp->temp ? "TF@" : get_func(inFunc),B);
    }
    else {      // Oba opoerandy jsou proměnné (musí existovat)
        if (list_exist(act_frame,A)==-1){
            gen_exit("i","5");
            return -1;
        }
        if (list_exist(act_frame,B)==-1){
            gen_exit("i","5");
            return -1;
        }
        node_t *var_A_temp = act_frame;
        node_t *var_B_temp = act_frame;
        pos= list_exist(act_frame,A);
        for (int i = 0; i<pos; i++){
            var_A_temp = var_A_temp->next;
        }
        pos= list_exist(act_frame,B);
        for (int i = 0; i<pos; i++){
            var_B_temp = var_B_temp->next;
        }
        if ((var_A_temp->type == 'f')||(var_B_temp->type == 'f'))
            current->type = 'f';
        sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, var_A_temp->temp ? "TF@" : get_func(inFunc),A, var_B_temp->temp ? "TF@" : get_func(inFunc),B);
    }
    // Konec generace instrukce

    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
    return 1;
}


// Aritmetické instrukce
void gen_add(char fmt[], char var[], char A[], char B[]){
    gen_ari(fmt,var,A,B,ADD);
}

void gen_sub(char fmt[],char var[], char A[], char B[]){
    gen_ari(fmt,var,A,B,SUB);
}

void gen_mul(char fmt[],char var[], char A[], char B[]){
    gen_ari(fmt,var,A,B,MUL);
}

void gen_div(char fmt[],char var[], char A[], char B[]){
    gen_ari(fmt,var,A,B,DIV);
}

void gen_idiv(char fmt[],char var[], char A[], char B[]){
    gen_ari(fmt,var,A,B,IDIV);
}
// Konec aritmetických instrukcí




// Generace všech relačních operací
int gen_rel(char fmt[],char var[], char A[], char B[],int op){
    // Určení aktuálního rámce (resp. zda se nacházíme ve funkci nebo ne)
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }

    int pos = list_exist(act_frame,var);
    node_t *current = act_frame;
    if (pos==-1) {  // Proměnná do které se ukládá není deklarovaná -> vytvoří se
        gen_variable("v",var);
        pos = list_exist(act_frame,var);
        for (int i = 0; i<pos; i++){
            current = current->next;
        }
        current->type = 'b';
    }
    else{
        for (int i = 0; i<pos; i++){
            current = current->next;
        }
    }

    // Generace konkrétní instukce

    char temp[20 + strlen(var) + strlen(A) + strlen(B)];
        if ((fmt[1] != 'v')&&(fmt[2] != 'v')){   // Oba operandy nejsou proměnná. Je nutné rozlišovat mezi datovýmy tyoy
            if (fmt[1] == 'f'){
                double conv_value_A = atof(A);
                if (fmt[2]  == 'f'){
                    float conv_value_B = atof(B);
                    sprintf(temp,"%s\t%s%s\t%s%a\t%s%a\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),conv_value_A, get_type(fmt[2]),conv_value_B);
                }
                else if (fmt[2] == 's'){
                    sprintf(temp,"%s\t%s%s\t%s%a\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),conv_value_A, get_type(fmt[2]),convertString(B));
                }
                else {
                    sprintf(temp,"%s\t%s%s\t%s%a\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),conv_value_A, get_type(fmt[2]),B);
                }
            }
            else if (fmt[2]  == 'f'){
                double conv_value_B = atof(B);
                if (fmt[1] == 's')
                    sprintf(temp,"%s\t%s%s\t%s%s\t%s%a\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),convertString(A), get_type(fmt[2]),conv_value_B);
                else
                    sprintf(temp,"%s\t%s%s\t%s%s\t%s%a\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),A, get_type(fmt[2]),conv_value_B);
            }
            else{
                if (fmt[1] == 's'){
                    if (fmt[2] == 's')
                        sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),convertString(A), get_type(fmt[2]),convertString(B));
                    else
                        sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),convertString(A), get_type(fmt[2]),B);
                }
                else if (fmt[2] == 's')
                    sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),A, get_type(fmt[2]),convertString(B));
                else
                    sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),A, get_type(fmt[2]),B);
            }
        }
        else if ((fmt[1] == 'v')&&(fmt[2] != 'v')){     // Jeden operand je proměnná (musí být deklarovaná) a druhý je jiného datového typu 
            if (list_exist(act_frame,A)==-1){
                gen_exit("i","5");
                return -1;
            }
            node_t *var_A_temp = act_frame;
            pos= list_exist(act_frame,A);
            for (int i = 0; i<pos; i++){
                var_A_temp = var_A_temp->next;
            }
            if (fmt[2]  == 'f'){
                double conv_value_B = atof(B);
                sprintf(temp,"%s\t%s%s\t%s%s\t%s%a\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_func(inFunc),A, get_type(fmt[2]),conv_value_B);
            }
            else if (fmt[2] == 's'){
                sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, var_A_temp->temp ? "TF@" : get_func(inFunc),A, get_type(fmt[2]),convertString(B));
            }
            else 
                sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, var_A_temp->temp ? "TF@" : get_func(inFunc),A, get_type(fmt[2]),B);
        }
        else if ((fmt[1] != 'v')&&(fmt[2] == 'v')){     // Jeden operand je proměnná (musí být deklarovaná) a druhý je jiného datového typu 
            if (list_exist(act_frame,B)==-1){
                gen_exit("i","5");
                return -1;
            }
            node_t *var_B_temp = act_frame;
            pos= list_exist(act_frame,A);
            for (int i = 0; i<pos; i++){
                var_B_temp = var_B_temp->next;
            }
            if (fmt[1]  == 'f'){
                double conv_value_A = atof(A);
                sprintf(temp,"%s\t%s%s\t%s%a\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),conv_value_A, get_func(inFunc),B);
            }
            else if (fmt[1] == 's')
                sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),convertString(A), var_B_temp->temp ? "TF@" : get_func(inFunc),B);
            else
                sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),var, get_type(fmt[1]),A, var_B_temp->temp ? "TF@" : get_func(inFunc),B);
        }
        else if ((fmt[1] == 'v')&&(fmt[2] == 'v')){     // Oba operandy jsou proměnné (musí být deklarované)
            if (list_exist(act_frame,A)==-1){
                gen_exit("i","5");
                return -1;
            }
            if (list_exist(act_frame,B)==-1){
                gen_exit("i","5");
                return -1;
            }
            node_t *var_A_temp = act_frame;
            node_t *var_B_temp = act_frame;
            pos= list_exist(act_frame,A);
            for (int i = 0; i<pos; i++){
                var_A_temp = var_A_temp->next;
            }
            pos= list_exist(act_frame,B);
            for (int i = 0; i<pos; i++){
                var_B_temp = var_B_temp->next;
            }
            sprintf(temp,"%s\t%s%s\t%s%s\t%s%s\n", get_name(op), current->temp ? "TF@" : get_func(inFunc),var, var_A_temp->temp ? "TF@" : get_func(inFunc),A, var_B_temp->temp ? "TF@" : get_func(inFunc),B);
        }
    // Konec generace instrukce

    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
    return 1;
}


// Relační operace se svou příslušnou datovou kontrolou a implicitní konverzí
void gen_lt(char fmt[],char var[], char A[], char B[]){
    if (while_preparation){
        char dtype_temp[60+strlen(A)+strlen(B)];
        gen_createframe();
        generate(&tempOut2,"DEFVAR\tTF@%0\n");
        sprintf(dtype_temp,"MOVE\tTF@%%0\t%s%s\n",get_func(inFunc),A);
        generate(&tempOut2,dtype_temp);
        generate(&tempOut2,"DEFVAR\tTF@%1\n");
        sprintf(dtype_temp,"MOVE\tTF@%%1\t%s%s\n",get_func(inFunc),B);
        generate(&tempOut2,dtype_temp);
        generate(&tempOut2,"CALL\t?ltgt_datatype_control\n");
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_1\n",get_func(inFunc),A);
        generate(&tempOut2,dtype_temp);
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_2\n",get_func(inFunc),B);
        generate(&tempOut2,dtype_temp);
    }
    else if ((if_vnoreni > 0)||(while_vnoreni > 0)){
        char dtype_temp[60+strlen(A)+strlen(B)];
        gen_createframe();
        generate(&tempOut,"DEFVAR\tTF@%0\n");
        sprintf(dtype_temp,"MOVE\tTF@%%0\t%s%s\n",get_func(inFunc),A);
        generate(&tempOut,dtype_temp);
        generate(&tempOut,"DEFVAR\tTF@%1\n");
        sprintf(dtype_temp,"MOVE\tTF@%%1\t%s%s\n",get_func(inFunc),B);
        generate(&tempOut,dtype_temp);
        generate(&tempOut,"CALL\t?ltgt_datatype_control\n");
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_1\n",get_func(inFunc),A);
        generate(&tempOut,dtype_temp);
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_2\n",get_func(inFunc),B);
        generate(&tempOut,dtype_temp);
    }
    else {
        char dtype_temp[60+strlen(A)+strlen(B)];
        gen_createframe();
        generate(&codeOut,"DEFVAR\tTF@%0\n");
        sprintf(dtype_temp,"MOVE\tTF@%%0\t%s%s\n",get_func(inFunc),A);
        generate(&codeOut,dtype_temp);
        generate(&codeOut,"DEFVAR\tTF@%1\n");
        sprintf(dtype_temp,"MOVE\tTF@%%1\t%s%s\n",get_func(inFunc),B);
        generate(&codeOut,dtype_temp);
        generate(&codeOut,"CALL\t?ltgt_datatype_control\n");
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_1\n",get_func(inFunc),A);
        generate(&codeOut,dtype_temp);
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_2\n",get_func(inFunc),B);
        generate(&codeOut,dtype_temp);
    }
    ltgt_datatype_exist = true;

    gen_rel(fmt,var,A,B,LT);
}

void gen_gt(char fmt[],char var[], char A[], char B[]){
    if (while_preparation){
        char dtype_temp[60+strlen(A)+strlen(B)];
        gen_createframe();
        generate(&tempOut2,"DEFVAR\tTF@%0\n");
        sprintf(dtype_temp,"MOVE\tTF@%%0\t%s%s\n",get_func(inFunc),A);
        generate(&tempOut2,dtype_temp);
        generate(&tempOut2,"DEFVAR\tTF@%1\n");
        sprintf(dtype_temp,"MOVE\tTF@%%1\t%s%s\n",get_func(inFunc),B);
        generate(&tempOut2,dtype_temp);
        generate(&tempOut2,"CALL\t?ltgt_datatype_control\n");
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_1\n",get_func(inFunc),A);
        generate(&tempOut2,dtype_temp);
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_2\n",get_func(inFunc),B);
        generate(&tempOut2,dtype_temp);
    }
    else if ((if_vnoreni > 0)||(while_vnoreni > 0)){
        char dtype_temp[60+strlen(A)+strlen(B)];
        gen_createframe();
        generate(&tempOut,"DEFVAR\tTF@%0\n");
        sprintf(dtype_temp,"MOVE\tTF@%%0\t%s%s\n",get_func(inFunc),A);
        generate(&tempOut,dtype_temp);
        generate(&tempOut,"DEFVAR\tTF@%1\n");
        sprintf(dtype_temp,"MOVE\tTF@%%1\t%s%s\n",get_func(inFunc),B);
        generate(&tempOut,dtype_temp);
        generate(&tempOut,"CALL\t?ltgt_datatype_control\n");
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_1\n",get_func(inFunc),A);
        generate(&tempOut,dtype_temp);
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_2\n",get_func(inFunc),B);
        generate(&tempOut,dtype_temp);
    }
    else {
        char dtype_temp[60+strlen(A)+strlen(B)];
        gen_createframe();
        generate(&codeOut,"DEFVAR\tTF@%0\n");
        sprintf(dtype_temp,"MOVE\tTF@%%0\t%s%s\n",get_func(inFunc),A);
        generate(&codeOut,dtype_temp);
        generate(&codeOut,"DEFVAR\tTF@%1\n");
        sprintf(dtype_temp,"MOVE\tTF@%%1\t%s%s\n",get_func(inFunc),B);
        generate(&codeOut,dtype_temp);
        generate(&codeOut,"CALL\t?ltgt_datatype_control\n");
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_1\n",get_func(inFunc),A);
        generate(&codeOut,dtype_temp);
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!ret_symb_2\n",get_func(inFunc),B);
        generate(&codeOut,dtype_temp);
    }
    ltgt_datatype_exist = true;
    
    gen_rel(fmt,var,A,B,GT);
}

void gen_eq(char fmt[],char var[], char A[], char B[]){
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }
    int pos = list_exist(act_frame,var);
    node_t *current = act_frame;
    if (pos==-1) {
        gen_variable("v",var);
        pos = list_exist(act_frame,var);
        for (int i = 0; i<pos; i++){
            current = current->next;
        }
        current->type = 'b';
    }
    else{
        for (int i = 0; i<pos; i++){
            current = current->next;
        }
    }
    generate(&codeOut,"\n# Kontrola datovych typu pro relacni operace\n\n");
    char conv_temp[100];
    sprintf(conv_temp,"v%c",fmt[1]);
    gen_type(conv_temp,"symb_1_type",A);
    sprintf(conv_temp,"v%c",fmt[2]);
    gen_type(conv_temp,"symb_2_type",B);
    gen_jumpifeq("lvs","error_func_5","symb_1_type","");
    gen_jumpifeq("lvs","error_func_5","symb_2_type","");
    sprintf(conv_temp,"rel_datatype_ok_%d",datatype_conversion);
    gen_jumpifeq("lvs",conv_temp,"symb_1_type","nil");
    gen_jumpifeq("lvs",conv_temp,"symb_2_type","nil");
    gen_jumpifeq("lvv",conv_temp,"symb_1_type","symb_2_type");
    sprintf(conv_temp,"MOVE\t%s%s\tbool@false\n",get_func(inFunc),var);
    if (while_preparation)
        generate(&tempOut2,conv_temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,conv_temp);
    else
        generate(&codeOut,conv_temp);
    sprintf(conv_temp,"JUMP\tEQ_skip_%d\n",datatype_conversion);
    if (while_preparation)
        generate(&tempOut2,conv_temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,conv_temp);
    else
        generate(&codeOut,conv_temp);
    sprintf(conv_temp,"rel_datatype_ok_%d",datatype_conversion);
    gen_label(conv_temp);
    generate(&codeOut,"\n# Konec kontroly datovych typu pro relacni operace\n\n");
    

    gen_rel(fmt,var,A,B,EQ);
    sprintf(conv_temp,"LABEL\tEQ_skip_%d\n",datatype_conversion);
    if (while_preparation)
        generate(&tempOut2,conv_temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,conv_temp);
    else
        generate(&codeOut,conv_temp);
    datatype_conversion++;
}

void gen_neql(char fmt[],char var[], char A[], char B[]){       // !==      provede se NOT nad výsledkem EQ
    gen_eq(fmt,var,A,B);
    char temp[32 + strlen(var)*2];
    sprintf(temp,"NOT\t%s%s\t%s%s\n",get_func(inFunc),var, get_func(inFunc),var);
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
}

void gen_lteq(char fmt[],char var[], char A[], char B[]){       // <==      provede se OR nad výsledky EQ a LT
    gen_lt(fmt,"!temp_lteq_exec1",A,B);
    gen_eq(fmt,"!temp_lteq_exec0",A,B);
    char temp[100+strlen(var)];
    sprintf(temp,"OR\t%s%s\t%s!temp_lteq_exec0\t%s!temp_lteq_exec1\n",get_func(inFunc),var, get_func(inFunc), get_func(inFunc));
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
}

void gen_gteq(char fmt[],char var[], char A[], char B[]){       // >==      provede se OR nad výsledky EQ a GT
    gen_gt(fmt,"!temp_gteq_exec1",A,B);
    gen_eq(fmt,"!temp_gteq_exec0",A,B);
    char temp[100+strlen(var)];
    sprintf(temp,"OR\t%s%s\t%s!temp_gteq_exec0\t%s!temp_gteq_exec1\n",get_func(inFunc),var, get_func(inFunc), get_func(inFunc));
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
}

// Konec relačních operací




// Vstupně-výstupní instrukce (READ a WRITE)

void gen_read(char name[],char type[]){
    // Určení aktuálního rámce
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }

    // Vytvoření proměnné pro uložení
    int pos = list_exist(act_frame,name);
    if (pos==-1) {
        gen_variable("v",name);
        pos = list_exist(act_frame,name);
    }

    node_t *current = act_frame;
    for (int i = 0; i<pos; i++){
        current = current->next;
    }

    // Určení jaký datový typ se ma číst
    char t_type = current->type;
    char temp[15 + strlen(name)];
    char out_type[7]; 
    if (t_type == 'v'){
        if (strcmp(type,"int")==0)
            t_type = 'i';
        else if (strcmp(type,"float")==0)
            t_type = 'f';
        else if (strcmp(type,"string")==0)
            t_type = 's';
        else if (strcmp(type,"bool")==0)
            t_type = 'b';
        current->type = t_type;
    }
    if (strcmp(type,"int")==0){
        sprintf(out_type,"int");
    }
    else if (strcmp(type,"float")==0){
        sprintf(out_type,"float");
    }
    else if (strcmp(type,"string")==0){
        sprintf(out_type,"string");
    }    
    else if (strcmp(type,"bool")==0){
        sprintf(out_type,"bool");
    }
    else {
        //ERROR - Nemělo by nastat
    }
    
    sprintf(temp,"READ\t%s%s\t%s\n",current->temp ? "TF@" : get_func(inFunc),name,out_type);
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
}

void gen_write(int param_count,int n){
    char temp[64];
    for (int i = 0; i<param_count;i++){
        sprintf(temp,"WRITE\t%s!temp_fcall_%d_%d\n",get_func(inFunc),n,i);
        if (while_preparation)
            generate(&tempOut2,temp);
        else if ((if_vnoreni > 0)||(while_vnoreni > 0))
            generate(&tempOut,temp);
        else
            generate(&codeOut,temp);
    }
}


// Generování návěští
void gen_label(char name[]){
    char temp[8 + strlen(name)];
    sprintf(temp,"LABEL\t%s\n",name);
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
}

// Generování podmíněných a nepodmíněných skoků

// Nepodmíněný skok
void gen_jump(char name[]){
    char temp[7 + strlen(name)];
    sprintf(temp,"JUMP\t%s\n",name);
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
}

// Generace obou podmíněných skoků (JUMPIFEQ a JUMPIFNEQ)
int gen_jumpif(char fmt[], char name[], char A[], char B[],int op){
    // Určení aktuálního rámce
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }
    
    // Generace konkrétní skokové instrukce

    char temp[26 + strlen(name) + strlen(A) + strlen(B)];
    if ((fmt[1] != 'v')&&(fmt[2] != 'v')){      // Oba operandy nejsou proměnná
        if (fmt[1] == 'f'){
            double conv_value_A = atof(A);
            if (fmt[2]  == 'f'){
                double conv_value_B = atof(B);
                sprintf(temp,"JUMPIF%s\t%s\t%s%a\t%s%a\n",get_name(op),name, get_type(fmt[1]),conv_value_A, get_type(fmt[2]),conv_value_B);
            }
            else if (fmt[2] == 's')
                sprintf(temp,"JUMPIF%s\t%s\t%s%a\t%s%s\n",get_name(op),name, get_type(fmt[1]),conv_value_A, get_type(fmt[2]),convertString(B));
            else 
                sprintf(temp,"JUMPIF%s\t%s\t%s%a\t%s%s\n",get_name(op),name, get_type(fmt[1]),conv_value_A, get_type(fmt[2]),B);
        }
        else if (fmt[2]  == 'f'){
            double conv_value_B = atof(B);
            if (fmt[1] == 's')
                sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%a\n",get_name(op),name, get_type(fmt[1]),convertString(A), get_type(fmt[2]),conv_value_B);
            else
                sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%a\n",get_name(op),name, get_type(fmt[1]),A, get_type(fmt[2]),conv_value_B);
        }
        else{
            if (fmt[1] == 's'){
                if (fmt[2] == 's')
                    sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%s\n",get_name(op),name, get_type(fmt[1]),convertString(A), get_type(fmt[2]),convertString(B));
                else
                    sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%s\n",get_name(op),name, get_type(fmt[1]),convertString(A), get_type(fmt[2]),B);
            }
            else if (fmt[2] == 's')
                sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%s\n",get_name(op),name, get_type(fmt[1]),A, get_type(fmt[2]),convertString(B));
            else
                sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%s\n",get_name(op),name, get_type(fmt[1]),A, get_type(fmt[2]),B);
        }
    }
    else if ((fmt[1] == 'v')&&(fmt[2] != 'v')){     // Jeden operand je proměnná (musí být deklarovaná)
        if (list_exist(act_frame,A)==-1){
            gen_exit("i","5");
            return -1;
        }
        node_t *var_A_temp = act_frame;
        int pos= list_exist(act_frame,A);
        for (int i = 0; i<pos; i++){
            var_A_temp = var_A_temp->next;
        }
        if (fmt[2]  == 'f'){
            double conv_value_B = atof(B);
            sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%a\n",get_name(op),name, get_func(inFunc),A, get_type(fmt[2]),conv_value_B);
        }
        else if (fmt[2] == 's')
            sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%s\n",get_name(op),name, var_A_temp->temp ? "TF@" : get_func(inFunc),A, get_type(fmt[2]),convertString(B));
        else 
            sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%s\n",get_name(op),name, var_A_temp->temp ? "TF@" : get_func(inFunc),A, get_type(fmt[2]),B);
    }
    else if ((fmt[1] != 'v')&&(fmt[2] == 'v')) {        // Jeden operand je proměnná (musí být deklarovaná)
        if (list_exist(act_frame,B)==-1){
            gen_exit("i","5");
            return -1;
        }
        node_t *var_B_temp = act_frame;
        int pos= list_exist(act_frame,B);
        for (int i = 0; i<pos; i++){
            var_B_temp = var_B_temp->next;
        }
        if (fmt[1]  == 'f'){
            double conv_value_A = atof(A);
            sprintf(temp,"JUMPIF%s\t%s\t%s%a\t%s%s\n",get_name(op),name, get_type(fmt[1]),conv_value_A, get_func(inFunc),B);
        }
        else if (fmt[1] == 's')
            sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%s\n",get_name(op),name, get_type(fmt[1]),A, var_B_temp->temp ? "TF@" : get_func(inFunc),convertString(B));
        else
            sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%s\n",get_name(op),name, get_type(fmt[1]),A, var_B_temp->temp ? "TF@" : get_func(inFunc),B);
    }
    else if ((fmt[1] == 'v')&&(fmt[2] == 'v')) {        // Oba operandy jsou proměnné (musí být deklarované)
        if (list_exist(act_frame,A)==-1){
            gen_exit("i","5");
            return -1;
        }
        if (list_exist(act_frame,B)==-1){
            gen_exit("i","5");
            return -1;
        }
        node_t *var_A_temp = act_frame;
        int pos= list_exist(act_frame,A);
        for (int i = 0; i<pos; i++){
            var_A_temp = var_A_temp->next;
        }
        node_t *var_B_temp = act_frame;
        pos= list_exist(act_frame,B);
        for (int i = 0; i<pos; i++){
            var_B_temp = var_B_temp->next;
        }
        sprintf(temp,"JUMPIF%s\t%s\t%s%s\t%s%s\n",get_name(op),name, var_A_temp->temp ? "TF@" : get_func(inFunc),A, var_B_temp->temp ? "TF@" : get_func(inFunc),B);
    }
    // Konec generace instrukce

    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
    return 1;
}

// Podmíněnné skoky volající funkci gen_jumpif()
void gen_jumpifeq(char fmt[], char name[], char A[], char B[]){
    gen_jumpif(fmt,name,A,B,EQ);
}

void gen_jumpifneq(char fmt[], char name[], char A[], char B[]){
    gen_jumpif(fmt,name,A,B,NEQ);
}
// Konec skokových instrukcí


// Generace instrukcí EXIT a DPRINT
int gen_exit_and_drpint(char fmt[], char name[], int op){
    // Určení aktuálního rámce
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }
    
    char temp[20 + strlen(name)];
    if (fmt[0] == 'v') {        // Operand je proměnná (musí být deklarovaná)
        int pos = list_exist(act_frame,name);
        node_t *current = act_frame;
        if (pos != -1){
            for (int i = 0; i<pos; i++){
                current = current->next;
            }
            sprintf(temp,"%s\t%s%s\n",get_name(op),current->temp ? "TF@" : get_func(inFunc),name);
        }
        else {
            gen_exit("i","5");
            return -1;
        }
    }
    else {      // Operand není proměnná
        if (fmt[0] == 'f'){
            double conv_value = atof(name);
            sprintf(temp,"%s\t%s%a\n",get_name(op),get_type(fmt[0]),conv_value);
        }
        else if (fmt[0] == 's')
            sprintf(temp,"%s\t%s%s\n",get_name(op),get_type(fmt[0]),convertString(name));
        else
            sprintf(temp,"%s\t%s%s\n",get_name(op),get_type(fmt[0]),name);
    }

    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
    return 1;
}

void gen_exit(char fmt[], char name[]){
    gen_exit_and_drpint(fmt,name,EXIT);
}
void gen_dprint(char fmt[], char name[]){
    gen_exit_and_drpint(fmt,name,DPRINT);
}
// Konec generace instrukcí EXIT a DPRINT


// Generace instrukce BREAK
void gen_break(){
    if (while_preparation)
        generate(&tempOut2,"BREAK\n");
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,"BREAK\n");
    else
        generate(&codeOut,"BREAK\n");
}


// Generování instrukce TYPE

int gen_type(char fmt[], char name[], char A[]){
    // Určení aktuálního rámce
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }

    int pos = list_exist(act_frame,name);
    if (pos==-1) {  // Proměnná, do které se přiřazuje, není deklarovaná -> vytvoří se
        gen_variable("v",name);
        pos = list_exist(act_frame,name);
    }
    node_t *current = act_frame;
    for (int i = 0; i<pos; i++){
        current = current->next;
    }
    current->type = 's';
    char temp[20 + strlen(name) + strlen(A)];
    if (fmt[1] == 'v'){ // Operand je proměnná
        pos = list_exist(act_frame,A);
        node_t *var_A_temp = act_frame;
        for (int i = 0; i<pos; i++){
            var_A_temp = var_A_temp->next;
        }
        if (pos != -1){
            sprintf(temp,"TYPE\t%s%s\t%s%s\n", current->temp ? "TF@" : get_func(inFunc),name ,var_A_temp->temp ? "TF@" : get_func(inFunc),A);
        }
    }
    else {      // Operand není proměnná
        if (fmt[0] == 'f'){
            double conv_value = atof(A);
            sprintf(temp,"TYPE\t%s%s\t%s%a\n", current->temp ? "TF@" : get_func(inFunc),name ,get_type(fmt[1]),conv_value);
        }
        else if (fmt[0] == 's')
            sprintf(temp,"TYPE\t%s%s\t%s%s\n", current->temp ? "TF@" : get_func(inFunc),name ,get_type(fmt[1]),convertString(A));
        else
            sprintf(temp,"TYPE\t%s%s\t%s%s\n", current->temp ? "TF@" : get_func(inFunc),name ,get_type(fmt[1]),A);
    }
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
    return 1;
}

// Instrukce pro práci s řetězci

int gen_concat(char fmt[], char name[], char A[], char B[]){

    // Volání funkce pro datovou kontrolu a implicitní konverzi
    if (while_preparation){
        char dtype_temp[60+strlen(A)+strlen(B)];
        gen_createframe();
        sprintf(dtype_temp,"MOVE\tTF@%%0\t%s%s\n",get_func(inFunc),A);
        generate(&tempOut2,"DEFVAR\tTF@%0\n");
        generate(&tempOut2,dtype_temp);
        sprintf(dtype_temp,"MOVE\tTF@%%1\t%s%s\n",get_func(inFunc),B);
        generate(&tempOut2,"DEFVAR\tTF@%1\n");
        generate(&tempOut2,dtype_temp);
        generate(&tempOut2,"CALL\t?concat_type_check_func\n");
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!return_var_A\n",get_func(inFunc),A);
        generate(&tempOut2,dtype_temp);
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!return_var_B\n",get_func(inFunc),B);
        generate(&tempOut2,dtype_temp);
    }
    else if ((if_vnoreni > 0)||(while_vnoreni > 0)){
        char dtype_temp[60+strlen(A)+strlen(B)];
        gen_createframe();
        sprintf(dtype_temp,"MOVE\tTF@%%0\t%s%s\n",get_func(inFunc),A);
        generate(&tempOut,"DEFVAR\tTF@%0\n");
        generate(&tempOut,dtype_temp);
        sprintf(dtype_temp,"MOVE\tTF@%%1\t%s%s\n",get_func(inFunc),B);
        generate(&tempOut,"DEFVAR\tTF@%1\n");
        generate(&tempOut,dtype_temp);
        generate(&tempOut,"CALL\t?concat_type_check_func\n");
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!return_var_A\n",get_func(inFunc),A);
        generate(&tempOut,dtype_temp);
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!return_var_B\n",get_func(inFunc),B);
        generate(&tempOut,dtype_temp);}
    else
        {char dtype_temp[60+strlen(A)+strlen(B)];
        gen_createframe();
        sprintf(dtype_temp,"MOVE\tTF@%%0\t%s%s\n",get_func(inFunc),A);
        generate(&codeOut,"DEFVAR\tTF@%0\n");
        generate(&codeOut,dtype_temp);
        sprintf(dtype_temp,"MOVE\tTF@%%1\t%s%s\n",get_func(inFunc),B);
        generate(&codeOut,"DEFVAR\tTF@%1\n");
        generate(&codeOut,dtype_temp);
        generate(&codeOut,"CALL\t?concat_type_check_func\n");
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!return_var_A\n",get_func(inFunc),A);
        generate(&codeOut,dtype_temp);
        sprintf(dtype_temp,"MOVE\t%s%s\tTF@!return_var_B\n",get_func(inFunc),B);
        generate(&codeOut,dtype_temp);}
    // Konec kontroly
    
    concat_datatype_exist = true;
        
        if (fmt[1] != 'v')
            fmt[1] = 's';
        if (fmt[2] != 'v')
            fmt[2] = 's';

    // Určení aktuálního rámce
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }


    int pos = list_exist(act_frame,name);
    if (pos==-1) {  // Proměnná, do které se přiřazuje, není deklarovaná -> vytvoří se
        gen_variable("v",name);
        pos = list_exist(act_frame,name);
    }
    node_t *current = act_frame;
    for (int i = 0; i<pos; i++){
        current = current->next;
    }
    current->type = 's';


    char temp[64+strlen(name)+strlen(A)*3+strlen(B)*3];
    if ((fmt[1] != 'v')&&(fmt[2] != 'v')){  // Oba operandy nejsou proměnná (musí být string)
        if ((fmt[1] == 's')&&(fmt[2] == 's')){
            sprintf(temp,"CONCAT\t%s%s\tstring@%s\tstring@%s\n",get_func(inFunc),name, convertString(A), convertString(B));
        }    
        else {
            //ERROR konkatenace spatnych datovych typu - díky konverzi nenastane
        }
    }
    else if ((fmt[1] == 'v')&&(fmt[2] != 'v')){ // Jeden operand je proměnná typu string (tedy je deklarovaná) a druhý operand je string
        if (fmt[2] == 's'){
            if (list_exist(act_frame,A)==-1){
                gen_exit("i","5");
                return -1;
            }
            node_t *var_A_temp = act_frame;
            pos= list_exist(act_frame,A);
            for (int i = 0; i<pos; i++){
                var_A_temp = var_A_temp->next;
            }
            var_A_temp->type = 's';
            sprintf(temp,"CONCAT\t%s%s\t%s%s\tstring@%s\n",get_func(inFunc),name, var_A_temp->temp ? "TF@" : get_func(inFunc),A, convertString(B));
        }
        else {
            //ERROR konkatenace spatnych datovych typu - díky konverzi nenastane
        }
    }
    else if ((fmt[1] != 'v')&&(fmt[2] == 'v')){ // Jeden operand je proměnná typu string (tedy je deklarovaná) a druhý operand je string
        if (fmt[1] == 's'){
            if (list_exist(act_frame,B)==-1){
                gen_exit("i","5");
                return -1;
            }
            node_t *var_B_temp = act_frame;
            pos= list_exist(act_frame,B);
            for (int i = 0; i<pos; i++){
                var_B_temp = var_B_temp->next;
            }
            var_B_temp->type = 's';
            sprintf(temp,"CONCAT\t%s%s\tstring@%s\t%s%s\n",get_func(inFunc),name, convertString(A), var_B_temp->temp ? "TF@" : get_func(inFunc),B);
        }
        else {
            //ERROR konkatenace spatnych datovych typu - díky konverzi nenastane
        }
    }
    else if ((fmt[1] == 'v')&&(fmt[2] == 'v')){ // Oba operandy jsou proměnné typu string a musí být deklarované
        if (list_exist(act_frame,A)==-1){
                gen_exit("i","5");
                return -1;
            }
            if (list_exist(act_frame,B)==-1){
                gen_exit("i","5");
                return -1;
            }
            node_t *var_A_temp = act_frame;
            pos= list_exist(act_frame,A);
            for (int i = 0; i<pos; i++){
                var_A_temp = var_A_temp->next;
            }
            var_A_temp->type = 's';
            node_t *var_B_temp = act_frame;
            pos= list_exist(act_frame,B);
            for (int i = 0; i<pos; i++){
                var_B_temp = var_B_temp->next;
            }
            var_B_temp->type = 's';
            sprintf(temp,"CONCAT\t%s%s\t%s%s\t%s%s\n",get_func(inFunc),name, var_A_temp->temp ? "TF@" : get_func(inFunc),A, var_B_temp->temp ? "TF@" : get_func(inFunc),B);
    }
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
    return 1;
}

// Generování instrukce STRLEN
int gen_strlen(char fmt[], char name[], char A[]){
    // Určení aktuálního rámce
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }

    int pos = list_exist(act_frame,name);
    if (pos==-1) {  // Proměnná, do které se přiřazuje, není deklarovaná -> vytvoří se
        gen_variable("v",name);
        pos = list_exist(act_frame,name);
    }
    node_t *current = act_frame;
    for (int i = 0; i<pos; i++){
        current = current->next;
    }
    current->type = 'i';

    // Generace instrukce
    char temp[30 + strlen(name) + strlen(A)];
    sprintf(temp,"STRLEN\t%s%s\t%s%s\n", current->temp ? "TF@" : get_func(inFunc),name , get_type(fmt[1]),convertString(A));
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
    return 1;
}

// Funkce pro generování všech konverzních instrukcí
int gen_conversion(char fmt[], char name[], char A[],int op){
    // Určení aktuálního rámce
    node_t *act_frame = NULL;
    if (inFunc){
        act_frame = local_frame;
    }
    else {
        act_frame = global_frame;
    }

    int pos = list_exist(act_frame,name);
    if (pos==-1) {  // Proměnná, do které se přiřazuje, není deklarovaná -> vytvoří se
        gen_variable("v",name);
        pos = list_exist(act_frame,name);
    }
    node_t *current = act_frame;
    for (int i = 0; i<pos; i++){
        current = current->next;
    }

    char temp[20 + strlen(name) + strlen(A)];
    if (fmt[1] == 'v'){     // Operand je proměnná
        pos = list_exist(act_frame,A);
        node_t *var_A_temp = act_frame;
        for (int i = 0; i<pos; i++){
            var_A_temp = var_A_temp->next;
        }
        if (pos != -1){
            sprintf(temp,"%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),name ,var_A_temp->temp ? "TF@" : get_func(inFunc),A);
        }
        else {
            gen_exit("i","5");
            return -1;
        }
    }
    else {      // Operand enní proměnná
        if (fmt[0] == 'f'){
            double conv_value = atof(A);
            sprintf(temp,"%s\t%s%s\t%s%a\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),name ,get_type(fmt[1]),conv_value);
        }
        else if (fmt[0] == 's')
            sprintf(temp,"%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),name ,get_type(fmt[1]),convertString(A));
        else
            sprintf(temp,"%s\t%s%s\t%s%s\n",get_name(op), current->temp ? "TF@" : get_func(inFunc),name ,get_type(fmt[1]),A);
    }
    if (while_preparation)
        generate(&tempOut2,temp);
    else if ((if_vnoreni > 0)||(while_vnoreni > 0))
        generate(&tempOut,temp);
    else
        generate(&codeOut,temp);
    return 1;
}

// Konkrétní konverzní instrukce, které volají gen_conversion()
void gen_int2float(char fmt[], char name[], char A[]){
    gen_conversion(fmt,name,A,I2F);
}

void gen_float2int(char fmt[], char name[], char A[]){
    gen_conversion(fmt,name,A,F2I);
}

void gen_int2char(char fmt[], char name[], char A[]){
    gen_conversion(fmt,name,A,I2C);
}


// Konverze řetězce na správný zápis
char *convertString(char *input){
    int len = strlen(input);
    
    //je potřeba víc místa, například kvůli "\" -> "\092"
    char *output = malloc(len * 4 + 1);
    if(output == NULL)
        error(ERROR_INTERNAL, "Chyba alokace paměti v generátoru");

    output[0] = '\0';
    
    
    for(int i = 0; i < len; i++)
    {
        if(input[i] < 32)
            error(ERROR_LEXICAL, "Nevalidní znak ve stringu");
        if(input[i] == ' ')
            strcat(output, "\\032");
        else if(input[i] == '\\')
        {
            char esc[4];
            for(int i = 0; i < 4; i++)
                esc[i] = '\0';

            char *backshlash = "\\092";
            
            i++;
            if(i == len)
            {
                strcat(output, backshlash);
                break;
            }
            else if(input[i] == 'x')
            {
                esc[0] = input[i];
                i++;
                if(i == len)
                {
                    strcat(output, backshlash);
                    strcat(output, esc);
                    break;
                }
                else if((input[i] >= 48 && input[i] <= 57) || (input[i] >= 65 && input[i] <= 70) || (input[i] >= 97 && input[i] <= 102))
                {
                    esc[1] = input[i];
                    i++;
                    if(i == len)
                    {
                        strcat(output, backshlash);
                        strcat(output, esc);
                        break;
                    }
                    else if((input[i] >= 48 && input[i] <= 57) || (input[i] >= 65 && input[i] <= 70) || (input[i] >= 97 && input[i] <= 102))
                    {
                        esc[2] = input[i];
                        char num[3];
                        num[0] = esc[1];
                        num[1] = esc[2];
                        num[2] = '\0';
                        
                        long n = strtol(num, NULL, 16);
                        if(n == 0)
                        {
                            strcat(output, backshlash);
                            strcat(output, esc+1);
                            continue;
                        }
                        char result[10];
                        sprintf(result, "%03ld", n);
                        
                        strcat(output, "\\");
                        strcat(output, result);
                        continue;
                    }
                    else
                    {
                        esc[2] = input[i];
                        strcat(output, backshlash);
                        strcat(output, esc);
                    }
                }
                else
                {
                    esc[1] = input[i];
                    strcat(output, backshlash);
                    strcat(output, esc);
                }
                
            }
            else if(input[i] >= 48 && input[i] <= 51)
            {
                esc[0] = input[i];
                i++;
                if(i == len)
                {
                    strcat(output, backshlash);
                    strcat(output, esc);
                    break;
                }
                else if(input[i] >= 48 && input[i] <= 55)
                {
                    esc[1] = input[i];
                    i++;
                    if(i == len)
                    {
                        strcat(output, backshlash);
                        strcat(output, esc);
                        break;
                    }
                    else if(input[i] >= 48 && input[i] <= 55)
                    {
                        //oktalove done
                        esc[2] = input[i];
                        
                        long n = strtol(esc, NULL, 8);
                        if(n == 0 || n > 255)
                        {
                            strcat(output, backshlash);
                            strcat(output, esc);
                            continue;
                        }
                        char result[10];
                        sprintf(result, "%03ld", n);
                        
                        strcat(output, "\\");
                        strcat(output, result);
                        continue;
                    }
                    else
                    {
                        esc[2] = input[i];
                        strcat(output, backshlash);
                        strcat(output, esc);
                    }
                }
                else
                {
                    esc[1] = input[i];
                    strcat(output, backshlash);
                    strcat(output, esc);
                }
            }
            else if(input[i] == '"')
                strcat(output, "\\034");
            else if(input[i] == 'n')
                strcat(output, "\\010");
            else if(input[i] == 't')
                strcat(output, "\\009");
            else if(input[i] == '\\')
                strcat(output, "\\092");
            else if(input[i] == '$')
                strcat(output, "$");
            else
            {
                esc[0] = input[i];
                strcat(output, backshlash);
                strcat(output, esc);
            }
            
        }
        else
        {
            char temp[2];
            temp[0] = input[i];
            temp[1] = '\0';
            strcat(output, temp);
        }
    }
    return output;
}

// Zbytek souboru jsou generace vestavěných funkcí a funkcí pro kontrolu datových typů/implicitní konverzi.
// Jen ty, které jsou potřeba, se ukáží na výstupu

void gen_func_substring(){
    char temp[] =   "\nJUMP\tsubstring!\n\n"
                    "LABEL\tsubstring?\n"
                    "PUSHFRAME\n"
                    "DEFVAR\tLF@%retval_from_func\n"
                    "MOVE\tLF@%retval_from_func\tnil@nil\n"
                    "DEFVAR\tLF@$s\n"
                    "MOVE\tLF@$s\tLF@%0\n"
                    "DEFVAR\tLF@$i\n"
                    "MOVE\tLF@$i\tLF@%1\n"
                    "DEFVAR\tLF@$j\n"
                    "MOVE\tLF@$j\tLF@%2\n"
                    "DEFVAR\tLF@length\n"
                    "STRLEN\tLF@length\tLF@$s\n"
                    "DEFVAR\tLF@temp\n"
                    "LT\tLF@temp\tLF@$i\tint@0\n"
                    "JUMPIFEQ\treturn_null\tLF@temp\tbool@true\n"
                    "LT\tLF@temp\tLF@$j\tint@0\n"
                    "JUMPIFEQ\treturn_null\tLF@temp\tbool@true\n"
                    "GT\tLF@temp\tLF@$i\tLF@$j\n"
                    "JUMPIFEQ\treturn_null\tLF@temp\tbool@true\n"
                    "GT\tLF@temp\tLF@$j\tLF@length\n"
                    "JUMPIFEQ\treturn_null\tLF@temp\tbool@true\n"
                    "GT\tLF@temp\tLF@$i\tLF@length\n"
                    "JUMPIFEQ\treturn_null\tLF@temp\tbool@true\n"
                    "EQ\tLF@temp\tLF@$i\tLF@length\n"
                    "JUMPIFEQ\treturn_null\tLF@temp\tbool@true\n"
                    "DEFVAR\tLF@temp_string\n"
                    "DEFVAR\tLF@out_string\n"
                    "MOVE\tLF@out_string\tstring@\n"
                    "LABEL\twhile?\n"
                    "JUMPIFEQ\twhile!\tLF@$i\tLF@$j\n"
                    "GETCHAR\tLF@temp_string\tLF@$s\tLF@$i\n"
                    "CONCAT\tLF@out_string\tLF@out_string\tLF@temp_string\n"
                    "ADD\tLF@$i\tLF@$i\tint@1\n"
                    "JUMP\twhile?\n"
                    "LABEL\twhile!\n"
                    "MOVE\tLF@%retval_from_func\tLF@out_string\n"
                    "LABEL\treturn_null\n"
                    "POPFRAME\n"
                    "RETURN\n"
                    "LABEL\tsubstring!\n\n";
    generate(&codeOut,temp);        
}

void gen_func_ord(){
    char temp[] =   "\nJUMP\tord!\n\n"
                    "LABEL\tord?\n"
                    "PUSHFRAME\n"
                    "DEFVAR\tLF@%retval_from_func\n"
                    "MOVE\tLF@%retval_from_func\tint@0\n"
                    "DEFVAR\tLF@length\n"
                    "STRLEN\tLF@length\tLF@%0\n"
                    "JUMPIFEQ\tempty_str\tLF@length\tint@0\n"
                    "STRI2INT\tLF@%retval_from_func\tLF@%0\tint@0\n"
                    "LABEL\tempty_str\n"
                    "POPFRAME\n"
                    "RETURN\n"
                    "\nLABEL\tord!\n\n";
    generate(&codeOut,temp);
}

void gen_func_floatval(){
    char temp[] =   "\nJUMP\tfloatval!\n\n"

                    "LABEL\tfloatval?\n"
                    "PUSHFRAME\n"
                    "DEFVAR\tLF@%retval_from_func\n"
                    "MOVE\tLF@%retval_from_func\tLF@%0\n"

                    "DEFVAR\tLF@datatype\n"
                    "TYPE\tLF@datatype\tLF@%0\n"
                    "JUMPIFEQ\tfval_is_null\tLF@datatype\tstring@nil\n"
                    "JUMPIFEQ\tfval_is_int\tLF@datatype\tstring@int\n"
                    "JUMPIFEQ\tfval_is_float\tLF@datatype\tstring@float\n"
                    "JUMPIFEQ\tfval_is_bool\tLF@datatype\tstring@bool\n"
                    "JUMPIFEQ\tfval_is_string\tLF@datatype\tstring@string\n"

                    "LABEL\tfval_is_null\n"
                    "MOVE\tLF@%retval_from_func\tfloat@0x0p+0\n"
                    "JUMP\tfval_is_float\n"

                    "LABEL\tfval_is_int\n"
                    "INT2FLOAT\tLF@%0\tLF@%0\n"
                    "MOVE\tLF@%retval_from_func\tLF@%0\n"
                    "JUMP\tfval_is_float\n"

                    "LABEL\tfval_is_bool\n"
                    "JUMPIFEQ\tfval_is_true\tLF@%0\tbool@true\n"
                    "MOVE\tLF@%retval_from_func\tfloat@0x0p+0\n"
                    "JUMP\tfval_is_float\n"
                    "LABEL\tfval_is_true\n"
                    "MOVE\tLF@%retval_from_func\tfloat@0x8p-3\n"
                    "JUMP\tfval_is_float\n"

                    "LABEL\tfval_is_string\n"
                    "\t# soucast STRNUM - neresime\n"

                    "LABEL\tfval_is_float\n"

                    "POPFRAME\n"
                    "RETURN\n"
                    "LABEL\tfloatval!\n\n";
    generate(&codeOut,temp);
}

void gen_func_intval(){
    char temp[] =   "\nJUMP\tintval!\n\n"

                    "LABEL\tintval?\n"
                    "PUSHFRAME\n"
                    "DEFVAR\tLF@%retval_from_func\n"
                    "MOVE\tLF@%retval_from_func\tLF@%0\n"

                    "DEFVAR\tLF@datatype\n"
                    "TYPE\tLF@datatype\tLF@%0\n"
                    "JUMPIFEQ\tival_is_null\tLF@datatype\tstring@nil\n"
                    "JUMPIFEQ\tival_is_int\tLF@datatype\tstring@int\n"
                    "JUMPIFEQ\tival_is_float\tLF@datatype\tstring@float\n"
                    "JUMPIFEQ\tival_is_bool\tLF@datatype\tstring@bool\n"
                    "JUMPIFEQ\tival_is_string\tLF@datatype\tstring@string\n"

                    "LABEL\tival_is_null\n"
                    "MOVE\tLF@%retval_from_func\tint@0\n"
                    "JUMP\tival_is_int\n"

                    "LABEL\tival_is_float\n"
                    "FLOAT2INT\tLF@%0\tLF@%0\n"
                    "MOVE\tLF@%retval_from_func\tLF@%0\n"
                    "JUMP\tival_is_int\n"

                    "LABEL\tival_is_bool\n"
                    "JUMPIFEQ\tival_is_true\tLF@%0\tbool@true\n"
                    "MOVE\tLF@%retval_from_func\tint@0\n"
                    "JUMP\tival_is_int\n"
                    "LABEL\tival_is_true\n"
                    "MOVE\tLF@%retval_from_func\tint@1\n"
                    "JUMP\tival_is_int\n"

                    "LABEL\tival_is_string\n"
                    "\t# soucast STRNUM - neresime\n"

                    "LABEL\tival_is_int\n"

                    "POPFRAME\n"
                    "RETURN\n"
                    "LABEL\tintval!\n\n";
    generate(&codeOut,temp);
}

void gen_func_strval(){
    char temp[] =   "\nJUMP\tstrval!\n\n"

                    "LABEL\tstrval?\n"
                    "PUSHFRAME\n"
                    "DEFVAR\tLF@%retval_from_func\n"
                    "MOVE\tLF@%retval_from_func\tLF@%0\n"

                    "DEFVAR\tLF@datatype\n"
                    "TYPE\tLF@datatype\tLF@%0\n"
                    "JUMPIFEQ\tsval_is_null\tLF@datatype\tstring@nil\n"
                    "JUMPIFEQ\tsval_is_int\tLF@datatype\tstring@int\n"
                    "JUMPIFEQ\tsval_is_float\tLF@datatype\tstring@float\n"
                    "JUMPIFEQ\tsval_is_bool\tLF@datatype\tstring@bool\n"
                    "JUMPIFEQ\tsval_is_string\tLF@datatype\tstring@string\n"

                    "LABEL\tsval_is_null\n"
                    "MOVE\tLF@%retval_from_func\tstring@\n"
                    "JUMP\tsval_is_string\n"

                    "LABEL\tsval_is_int\n"
                    "LABEL\tsval_is_float\n"
                    "\t# soucast STRNUM - neresime\n"
                    "JUMP\tsval_is_string\n"

                    "LABEL\tsval_is_bool\n"
                    "JUMPIFEQ\tsval_is_true\tLF@%0\tbool@true\n"
                    "MOVE\tLF@%retval_from_func\tstring@\n"
                    "JUMP\tsval_is_string\n"
                    "LABEL\tsval_is_true\n"
                    "MOVE\tLF@%retval_from_func\tstring@1\n"
                    "JUMP\tsval_is_string\n"

                    "LABEL\tsval_is_string\n"

                    "POPFRAME\n"
                    "RETURN\n"
                    "LABEL\tstrval!\n\n";
    generate(&codeOut,temp);
}

void gen_ifwhile_datatype(){
    char temp[] =   "\nJUMP\tkontrola_skip\n"
                    "\n# Kontrola datovych typu pro if/while\n\n"
                    "LABEL\tifwhile_datatype_kontrola\n"
                    "PUSHFRAME\n"
                    "DEFVAR\tLF@%retval_from_func\n"
                    "MOVE\tLF@%retval_from_func\tLF@%0\n"
                    "DEFVAR\tLF@symb_1_type\n"
                    "TYPE\tLF@symb_1_type\tLF@%retval_from_func\n"
                    "JUMPIFEQ\ttype_is_bool\tLF@symb_1_type\tstring@bool\n"
                    "JUMPIFEQ\ttype_is_int\tLF@symb_1_type\tstring@int\n"
                    "JUMPIFEQ\ttype_is_float\tLF@symb_1_type\tstring@float\n"
                    "JUMPIFEQ\ttype_is_string\tLF@symb_1_type\tstring@string\n"
                    "JUMPIFEQ\ttype_is_nil\tLF@symb_1_type\tstring@nil\n"
                    "LABEL\ttype_is_int\n"
                        "JUMPIFEQ\tint_is_zero\tLF@%retval_from_func\tint@0\n"
                        "MOVE\tLF@%retval_from_func\tbool@true\n"
                        "JUMP\ttype_is_bool\n"
                        "LABEL\tint_is_zero\n"
                        "MOVE\tLF@%retval_from_func\tbool@false\n"
                        "JUMP\ttype_is_bool\n"
                    "LABEL\ttype_is_float\n"
                        "JUMPIFEQ\tfloat_is_zero\tLF@%retval_from_func\tfloat@0x0p+0\n"
                        "MOVE\tLF@%retval_from_func\tbool@true\n"
                        "JUMP\ttype_is_bool\n"
                        "LABEL\tfloat_is_zero\n"
                        "MOVE\tLF@%retval_from_func\tbool@false\n"
                        "JUMP\ttype_is_bool\n"
                    "LABEL\ttype_is_string\n"
                        "JUMPIFEQ\tstring_is_zero\tLF@%retval_from_func\tstring@\n"
                        "JUMPIFEQ\tstring_is_zero\tLF@%retval_from_func\tstring@0\n"
                        "MOVE\tLF@%retval_from_func\tbool@true\n"
                        "JUMP\ttype_is_bool\n"
                        "LABEL\tstring_is_zero\n"
                        "MOVE\tLF@%retval_from_func\tbool@false\n"
                        "JUMP\ttype_is_bool\n"
                    "LABEL\ttype_is_nil\n"
                        "MOVE\tLF@%retval_from_func\tbool@false\n"
                    "LABEL\ttype_is_bool\n"
                    "POPFRAME\n"
                    "RETURN\n"
                    "\n# Konec kontroly datovych typu pro if/while\n\n"
                    "LABEL\tkontrola_skip\n";
    generate(&codeOut,temp);
}

void gen_concat_datatype(){
    char temp[] =   "\nJUMP\t!concat_type_check_func\n"
                    "\n#Kontrola datovych type pro konkatenaci\n\n"
                    "LABEL\t?concat_type_check_func\n"
                    "PUSHFRAME\n"
                    "DEFVAR\tLF@symb_A_type\n"
                    "TYPE\tLF@symb_A_type\tLF@%0\n"
                    "DEFVAR\tLF@symb_B_type\n"
                    "TYPE\tLF@symb_B_type\tLF@%1\n"
                    "JUMPIFEQ\tA_concat_type_nil\tLF@symb_A_type\tstring@nil\n"
                    "JUMPIFEQ\tA_concat_type_bool\tLF@symb_A_type\tstring@bool\n"
                    "JUMPIFEQ\tB_concat_check\tLF@symb_A_type\tstring@string\n"
                    "JUMP\terror_func_7\n"
                    
                    "LABEL\tA_concat_type_nil\n"
                    "MOVE\tLF@%0\tstring@\n"
                    "JUMP\tB_concat_check\n"
                    
                    "LABEL\tA_concat_type_bool\n"
                    "JUMPIFEQ\tA_concat_bool_false\tLF@%0\tbool@false\n"
                    "MOVE\tLF@%0\tstring@1\n"
                    "JUMP\tB_concat_check\n"
                    "LABEL\tA_concat_bool_false\n"
                    "MOVE\tLF@%0\tstring@\n"

                    "LABEL\tB_concat_check\n"

                    "JUMPIFEQ\tB_concat_type_nil\tLF@symb_B_type\tstring@nil\n"
                    "JUMPIFEQ\tB_concat_type_bool\tLF@symb_B_type\tstring@bool\n"
                    "JUMPIFEQ\tconcat_correct_end\tLF@symb_B_type\tstring@string\n"
                    "JUMP\terror_func_7\n"

                    "LABEL\tB_concat_type_nil\n"
                    "MOVE\tLF@%1\tstring@\n"
                    "JUMP\tconcat_correct_end\n"
                    
                    "LABEL\tB_concat_type_bool\n"
                    "JUMPIFEQ\tB_concat_bool_false\tLF@%1\tbool@false\n"
                    "MOVE\tLF@%1\tstring@1\n"
                    "JUMP\tconcat_correct_end\n"
                    "LABEL\tB_concat_bool_false\n"
                    "MOVE\tLF@%1\tstring@\n"

                    "LABEL\tconcat_correct_end\n"

                    "DEFVAR\tLF@!return_var_A\n"
                    "DEFVAR\tLF@!return_var_B\n"
                    "MOVE\tLF@!return_var_A\tLF@%0\n"
                    "MOVE\tLF@!return_var_B\tLF@%1\n"

                    "POPFRAME\n"
                    "RETURN\n"
                    "LABEL\t!concat_type_check_func\n";
    generate(&codeOut,temp);
}

void gen_ltgt_datatype(){
    char temp[] =   "\nJUMP\t!ltgt_datatype_control\n"
                    "\n# Kontrola datovych typu pro relacni operace\n\n"
                    "LABEL\t?ltgt_datatype_control\n"
                    "PUSHFRAME\n"

                    "DEFVAR\tLF@!ret_symb_1\n"
                    "DEFVAR\tLF@!ret_symb_2\n"
                    "MOVE\tLF@!ret_symb_1\tLF@%0\n"
                    "MOVE\tLF@!ret_symb_2\tLF@%1\n"

                    "DEFVAR\tLF@symb_1_type\n"
                    "DEFVAR\tLF@symb_2_type\n"
                    "TYPE\tLF@symb_1_type\tLF@%0\n"
                    "TYPE\tLF@symb_2_type\tLF@%1\n"
                    
                    "JUMPIFEQ\terror_func_5\tLF@symb_1_type\tstring@\n"
                    "JUMPIFEQ\terror_func_5\tLF@symb_2_type\tstring@\n"
                    "JUMPIFEQ\tltgt_same_datatype\tLF@symb_1_type\tLF@symb_2_type\n"

                    "JUMPIFEQ\tltgt_symb1_nil\tLF@symb_1_type\tstring@nil\n"
                    "JUMPIFEQ\tltgt_symb1_int\tLF@symb_1_type\tstring@int\n"
                    "JUMPIFEQ\tltgt_symb1_float\tLF@symb_1_type\tstring@float\n"
                    "JUMPIFEQ\tltgt_symb1_bool\tLF@symb_1_type\tstring@bool\n"
                    "JUMPIFEQ\tltgt_symb1_string\tLF@symb_1_type\tstring@string\n"

                    "LABEL\tltgt_symb1_nil\n"
                    "JUMPIFEQ\tltgt_s1_nil_s2_int\tLF@symb_2_type\tstring@int\n"
                    "JUMPIFEQ\tltgt_s1_nil_s2_float\tLF@symb_2_type\tstring@float\n"
                    "JUMPIFEQ\tltgt_s1_nil_s2_bool\tLF@symb_2_type\tstring@bool\n"
                    "JUMPIFEQ\tltgt_s1_nil_s2_string\tLF@symb_2_type\tstring@string\n"

                    "LABEL\tltgt_s1_nil_s2_int\n"
                        "MOVE\tLF@!ret_symb_1\tint@0\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_nil_s2_float\n"
                        "MOVE\tLF@!ret_symb_1\tfloat@0x0p+0\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_nil_s2_bool\n"
                        "MOVE\tLF@!ret_symb_1\tbool@false\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_nil_s2_string\n"
                        "MOVE\tLF@!ret_symb_1\tstring@\n"
                        "JUMP\tltgt_same_datatype\n"

                    "LABEL\tltgt_symb1_int\n"
                    "JUMPIFEQ\tltgt_s1_int_s2_nil\tLF@symb_2_type\tstring@nil\n"
                    "JUMPIFEQ\tltgt_s1_int_s2_float\tLF@symb_2_type\tstring@float\n"
                    "JUMPIFEQ\tltgt_s1_int_s2_bool\tLF@symb_2_type\tstring@bool\n"
                    "JUMPIFEQ\tltgt_s1_int_s2_string\tLF@symb_2_type\tstring@string\n"

                    "LABEL\tltgt_s1_int_s2_nil\n"
                        "MOVE\tLF@!ret_symb_2\tint@0\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_int_s2_float\n"
                        "INT2FLOAT\tLF@!ret_symb_1\tLF@!ret_symb_1\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_int_s2_bool\n"
                        "JUMPIFEQ\tltgt_s1_int_s2_false\tLF@!ret_symb_2\tbool@false\n"
                        "MOVE\tLF@!ret_symb_2\tint@1\n"
                        "JUMP\tltgt_same_datatype\n"
                        "LABEL\tltgt_s1_int_s2_false\n"
                        "MOVE\tLF@!ret_symb_2\tint@0\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_int_s2_string\n"
                        "JUMP\terror_func_7\n"

                    "LABEL\tltgt_symb1_float\n"
                    "JUMPIFEQ\tltgt_s1_float_s2_nil\tLF@symb_2_type\tstring@nil\n"
                    "JUMPIFEQ\tltgt_s1_float_s2_int\tLF@symb_2_type\tstring@int\n"
                    "JUMPIFEQ\tltgt_s1_float_s2_bool\tLF@symb_2_type\tstring@bool\n"
                    "JUMPIFEQ\tltgt_s1_float_s2_string\tLF@symb_2_type\tstring@string\n"

                    "LABEL\tltgt_s1_float_s2_nil\n"
                        "MOVE\tLF@!ret_symb_2\tfloat@0x0p+0\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_float_s2_int\n"
                        "INT2FLOAT\tLF@!ret_symb_2\tLF@!ret_symb_2\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_float_s2_bool\n"
                        "JUMPIFEQ\tltgt_s1_float_s2_false\tLF@!ret_symb_2\tbool@false\n"
                        "MOVE\tLF@!ret_symb_2\tfloat@0x8p-3\n"
                        "JUMP\tltgt_same_datatype\n"
                        "LABEL\tltgt_s1_float_s2_false\n"
                        "MOVE\tLF@!ret_symb_2\tfloat@0x0p+0\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_float_s2_string\n"
                        "JUMP\terror_func_7\n"

                    "LABEL\tltgt_symb1_bool\n"
                    "JUMPIFEQ\tltgt_s1_bool_s2_nil\tLF@symb_2_type\tstring@nil\n"
                    "JUMPIFEQ\tltgt_s1_bool_s2_int\tLF@symb_2_type\tstring@int\n"
                    "JUMPIFEQ\tltgt_s1_bool_s2_float\tLF@symb_2_type\tstring@float\n"
                    "JUMPIFEQ\tltgt_s1_bool_s2_string\tLF@symb_2_type\tstring@string\n"

                    "LABEL\tltgt_s1_bool_s2_nil\n"
                        "MOVE\tLF@!ret_symb_2\tbool@false\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_bool_s2_int\n"
                        "JUMPIFEQ\tltgt_s2_int_s1_false\tLF@!ret_symb_1\tbool@false\n"
                        "MOVE\tLF@!ret_symb_1\tint@1\n"
                        "JUMP\tltgt_same_datatype\n"
                        "LABEL\tltgt_s2_int_s1_false\n"
                        "MOVE\tLF@!ret_symb_1\tint@0\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_bool_s2_float\n"
                        "JUMPIFEQ\tltgt_s2_float_s1_false\tLF@!ret_symb_1\tbool@false\n"
                        "MOVE\tLF@!ret_symb_1\tfloat@0x8p-3\n"
                        "JUMP\tltgt_same_datatype\n"
                        "LABEL\tltgt_s2_float_s1_false\n"
                        "MOVE\tLF@!ret_symb_1\tfloat@0x0p+0\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_bool_s2_string\n"
                        "JUMPIFEQ\tltgt_s2_string_s1_false\tLF@!ret_symb_1\tbool@false\n"
                        "MOVE\tLF@!ret_symb_1\tstring@1\n"
                        "JUMP\tltgt_same_datatype\n"
                        "LABEL\tltgt_s2_string_s1_false\n"
                        "MOVE\tLF@!ret_symb_1\tstring@\n"
                        "JUMP\tltgt_same_datatype\n"

                    "LABEL\tltgt_symb1_string\n"
                    "JUMPIFEQ\tltgt_s1_string_s2_nil\tLF@symb_2_type\tstring@nil\n"
                    "JUMPIFEQ\tltgt_s1_string_s2_int\tLF@symb_2_type\tstring@int\n"
                    "JUMPIFEQ\tltgt_s1_string_s2_float\tLF@symb_2_type\tstring@float\n"
                    "JUMPIFEQ\tltgt_s1_string_s2_bool\tLF@symb_2_type\tstring@bool\n"

                    "LABEL\tltgt_s1_string_s2_nil\n"
                        "MOVE\tLF@!ret_symb_2\tstring@\n"
                        "JUMP\tltgt_same_datatype\n"
                    "LABEL\tltgt_s1_string_s2_int\n"
                        "JUMP\terror_func_7\n"
                    "LABEL\tltgt_s1_string_s2_float\n"
                        "JUMP\terror_func_7\n"
                    "LABEL\tltgt_s1_string_s2_bool\n"
                        "JUMPIFEQ\tltgt_s1_string_s2_false\tLF@!ret_symb_2\tbool@false\n"
                        "MOVE\tLF@!ret_symb_2\tstring@1\n"
                        "JUMP\tltgt_same_datatype\n"
                        "LABEL\tltgt_s1_string_s2_false\n"
                        "MOVE\tLF@!ret_symb_2\tstring@\n"
                        "JUMP\tltgt_same_datatype\n"

                    "LABEL\tltgt_same_datatype\n"
                    
                    "JUMPIFNEQ\tltgt_same_notnil\tLF@symb_1_type\tstring@nil\n"
                    "JUMPIFNEQ\tltgt_same_notnil\tLF@symb_2_type\tstring@nil\n"
                    "MOVE\tLF@!ret_symb_1\tint@0\n"
                    "MOVE\tLF@!ret_symb_2\tint@0\n"
                    "LABEL\tltgt_same_notnil\n"

                    "POPFRAME\n"
                    "RETURN\n"
                    "LABEL\t!ltgt_datatype_control\n";
    generate(&codeOut,temp);
}