/*
    * Projekt: Implementace překladače imperativního jazyka IFJ22
    *
    * @brief hlavičkový soubor pro generátor cílového kódu implementace
    *
    * @author   Kryštof Andrýsek        xandry12
    * @author   Ondřej Dacík            xdacik01
    * @author   Dalibor Kalina          xkalin16
    * @author   Jan Kvapil              xkvapi19
    *
*/
#ifndef GENERATOR_H_INCLUDED
#define GENERATOR_H_INCLUDED

/*
*       Definice fmt[]
*   =====================
*       Jedná se o řetězec obsahující znaky (přesněji písmena), které určují datový typ
*       ostatních parametrů, které funkce dostává. Možnými hodnotami jsou:
*           v - variable = jedná se o proměnnou nějakého datového typu
*           i - integer
*           f - float
*           s - string
*           b - bool
*           n - nil
*       Délka řetězce určuje počet předaných parametrů (to se týká zejména gen_variable()).
*/

/*
*       Funkce pro inicializaci generátoru a výsledný výpis.
*/
int gen_init();
void gen_output();

/*
*       Funkce uvolňující alokovanou paměť, volaná jen jednou na konci
*/
void gen_cleanup();
/*
*       Funkce vypisující úvodní řádek, volaná jen jednou na začátku překladu
*/
void gen_header();

/*
*       Funkce pro deklaraci, inicializaci a změnu proměnné
*   ===========================================================
*       *fmt - řetězec určující počet argumentů a jejich datový typ (viz začátek tohoto souboru)
*       ...  - proměnný počet parametrů (1 = pouze deklarace; 2 = deklarace a přiřazení/změna hodnoty)
*
*       První předaná argument (nepočítaje *fmt) je proměnná, do které se přiřadí druhý předaný
*       argument. Pokud první argument neexistuje, bude vytvořen. Pokud druhý argument není zadán,
*       dojde pouze k deklaraci proměnné v prvním parametru.
*       Proměnná je deklarovaná do globálního (resp. lokálního) rámce, pokud se nacházíme mimo
*       funkci (resp. v ní).
*/
int gen_variable(const char* fmt, ...);


/*
*       Funkce pro deklaraci funkcí v ifj22
*   ==========================================
*       gen_func_begin()
*           Upraví rámce, vygeneruje skok na návěští za koncem funkce a počáteční návěští.
*           Následně deklaruje návratovou hodnotu a iniciaizuje ji na "nil".
*       gen_add_param()
*           Do právě vygenerované proměnné jménem name[] uloží cnt-tý argument,
*           který funkce při jejím volání obdrží.
*       gen_func_end()
*           Vygeneruje konečný label a upraví rámce. retvalVoid udává, zda jsme se dostali
*           na konec ne-void-ovovské funkce bez návratové hodnoty. Tento případ vede na
*           chybu číslo 4.
*
*       Každý return z funkce musí bát volán zvlášť pomocí gen_return() (viz sekse "Volání funkcí").           
*/
int gen_func_begin(char name[]);
void gen_add_param(char fmt,char name[],int cnt, bool is_nullable);
int gen_func_end(bool retvalVoid);

/*
*       Volání funkcí
*   ====================
*       gen_call()
*           Předávané argumenty je nutné vytvořit zvlášť dle příslušných pravidel.
*           Pokud je volanou funkcí 'write', generuje "WRITE" místo "JUMP"
*           name[]      - jméno funkce, na kterou se bude skákat
*           param_count - počet parametrů, které má volaná funkce obdržet. Je vytvořeno tolik proměnných
*                         v dočasném rámci a do nich přiřazeny příslušné hodnoty.
*           n           - pomocná proměnná pro indexaci předávaných parametrů udávající hloubku zanoření
*
*       gen_return()
*           Pokud je tato funkce zavolaná mimo funkci v jazyku ifj22, tak se program ukončí s návratovou
*           hodnotou 0.
*           not_end     - parametr určující, zda se před vygenerování "RETURN" má do návratové proměnné 
*                         "%retval_from_func" uložit návratová hodnota z "!temp_exp_result"
*
*/
int gen_call(char name[], int param_count, int n);
void gen_return(char fmt, bool return_value, bool is_nullable);

/*
*       Instrukce pro generaci příkazu 'if'
*   ==========================================
*       gen_if_begin()
*           Všechny výpisy generátoru jsou pozastaveny (ukládány do bufferu).
*           Generují se pouze deklarace proměnných vytvořených v celém 'if-else' výrazu.
*           Do bufferu se vytvoří podmíněný skok na "else" (porovnávající "!temp_exp_result" a "false").
*       gen_else_begin()
*           Prvně se do bufferu vytvoří skok na konec 'if-else' výrazu (aby se po provedení 'if' části
*           neprovedla i 'else' část) a nakonec návěští "else".
*       gen_else_end()
*           Do bufferu vytvoří návěští značící konec 'if-else'. Pokud se po této instrukci nenacházíme
*           v jiném 'if-else' nebo ve 'while', celý obsah bufferu vypíše. V opačném případě bude do
*           bufferu dále zapisovat, doku se všechny 'if-else' a 'while' neukončí.
*/
void gen_if_begin();
void gen_else_begin();
void gen_else_end();

/*
*       Instrukce pro generaci příkazu 'while'
*   ==========================================
*       gen_while_begin1()
*           Všechny výpisy generátoru jsou pozastaveny (ukládány do bufferu A).
*           Generují se pouze deklarace proměnných vytvořených v celém 'if-else' výrazu.
*           Do bufferu A se ukládá výraz pro určení podmínky while (resp. zda se má while vykonávat).
*       gen_while_begin2()
*           Do nového bufferu B se vloží návěští začátku cyklu, celý buffer A a pak podmíněný skok na konec 'while'.
*       gen_while_end()
*           Do bufferu vytvoří skok na začátek 'while' a návěští značící konec 'while'.
*           Pokud se po této instrukci nenacházíme v jiném 'while' nebo ve 'if-else', celý obsah bufferu vypíše.
*           V opačném případě bude do bufferu dále zapisovat, dokud se všechny 'while' a 'if-else' neukončí.
*/
void gen_while_begin1();
void gen_while_begin2();
void gen_while_end();

void gen_for_break(long breakLevel);
void gen_for_continue(long continueLevel);
void gen_insert(char *str);

void gen_ifwhile_datatype();

/*
*       Instrukce pro práci s rámci
*/
void gen_createframe();
void gen_pushframe();
void gen_popframe();

/*
*       Aritmetické instrukce
*   =============================
*       fmt[] - řetězec určující počet argumentů a jejich datový typ (viz začátek tohoto souboru) 
*       var[] - název proměnné, do které se MOVEne výsledek operace. Pokud neexistuje, bude vytvořena.
*       A[]   - levý operand
*       B[]   - pravý operand
*       op    - tento parametr má pouze funkce gen_ari(), která je volána všemy ostatními aritmetickými
*               instrukcemi a je univerzální pro jejich výpočet. op může nabývat hodnoty ADD/SUB/MUL/DIV/IDIV,
*               které jsou definované v generator.c jako celočíselné hodnoty. 
*
*       Funkce vypíše příslušnou operaci a následovně var[], A[], B[] přesně v tomto pořadí
*/
int gen_ari(char fmt[],char var[],char A[], char B[],int op);
void gen_add(char fmt[], char var[], char A[], char B[]);
void gen_sub(char fmt[], char var[], char A[], char B[]);
void gen_mul(char fmt[], char var[], char A[], char B[]);
void gen_div(char fmt[], char var[], char A[], char B[]);
void gen_idiv(char fmt[], char var[], char A[], char B[]);

/*
*       Relační instrukce
*   =========================
*       fmt[] - řetězec určující počet argumentů a jejich datový typ (viz začátek tohoto souboru) 
*       var[] - booleovská proměnná, do které se uloží výsledek vyhodnocení operace
*       A[]   - levý operand
*       B[]   - pravý operand
*       op    - tento parametr má pouze funkce gen_rel(), která je volána všemy ostatními relačními
*               instrukcemi a je univerzální pro jejich vyhodnocení. op může nabývat hodnoty LT/GT/EQ,
*               které jsou definované v generator.c jako celočíselné hodnoty.
*
*       Funkce vypíše příslušnou operaci a následovně var[], A[], B[] přesně v tomto pořadí
*/
int gen_rel(char fmt[],char var[], char A[], char B[],int op);
void gen_lt(char fmt[],char var[], char A[], char B[]);
void gen_gt(char fmt[],char var[], char A[], char B[]);
void gen_eq(char fmt[],char var[], char A[], char B[]);
void gen_neql(char fmt[],char var[], char A[], char B[]);
void gen_lteq(char fmt[],char var[], char A[], char B[]);
void gen_gteq(char fmt[],char var[], char A[], char B[]);

/*
*       Instrukce skoků
*   ======================
*       fmt[] - řetězec určující počet argumentů a jejich datový typ (viz začátek tohoto souboru) 
*       name[] - název návěští, na které se skočí/bude se skákat v případě, že platí rovnost/nerovnost
*       A[]   - levý operand
*       B[]   - pravý operand
*       op    - tento parametr má pouze funkce gen_jumpif(), která je volána všemy ostatními skokovými
*               instrukcemi a je univerzální pro jejich výpočet. op může nabývat hodnoty EQ/NEQ,
*               které jsou definované v generator.c jako celočíselné hodnoty.
*/
void gen_jump(char name[]);
int gen_jumpif(char fmt[], char name[], char A[], char B[],int op);
void gen_jumpifeq(char fmt[], char name[], char A[], char B[]);
void gen_jumpifneq(char fmt[], char name[], char A[], char B[]);

/*
*       Konverzní instrukce
*   ==========================
8       fmt[] - řetězec určující počet argumentů a jejich datový typ (viz začátek tohoto souboru) 
*       name[] - proměnná, do které se uloží výsledek převodu. Pokud neexistuje, bude vytvořena.
*       A[]   - levý operand
*       op    - tento parametr má pouze funkce gen_conversion(), která je volána všemy ostatními konverzními
*               instrukcemi a je univerzální pro jejich chování. op může nabývat hodnoty I2F/F2I/I2C,
*               které jsou definované v generator.c jako celočíselné hodnoty.
*/
int gen_conversion(char fmt[], char name[], char A[],int op);
void gen_int2float(char fmt[], char name[], char A[]);
void gen_float2int(char fmt[], char name[], char A[]);
void gen_int2char(char fmt[], char name[], char A[]);

/*
*       Vstupně-výstupní instrukce
*   ==================================
*       name[] - proměnná, do které se bude číst. Pokud neexistuje, bude vytvořena.
*       type[] - datový typ, který se bude číst
*       param_count - počet parametrů, které mají být vypsané. Je vytvořeno tolik proměnných
*                     v dočasném rámci a do nich přiřazeny příslušné hodnoty.
*       n           - pomocná proměnná pro indexaci vypisovaných parametrů udávající hloubku zanoření
*/
void gen_read(char name[],char type[]);
void gen_write(int param_count,int n);

/*
*       Ladící instukce a instrukce toku programu
*   ================================================
*       name[] - jméno návěští/hodnota návratového kódu/hodnota pro výpis na stderr
*       fmt[]  - řetězec určující počet argumentů a jejich datový typ (viz začátek tohoto souboru)
*       op    - tento parametr má pouze funkce gen_exit_and_dprint(), která je volána gen_exit a gen_dprint
*               instrukcemi a je univerzální pro jejich chování. op může nabývat hodnoty EXIT/DPRINT,
*               které jsou definované v generator.c jako celočíselné hodnoty.
*/
void gen_label(char name[]);
int gen_exit_and_drpint(char fmt[], char name[], int op);
void gen_exit(char fmt[], char name[]);
void gen_dprint(char fmt[], char name[]);
void gen_break();

/*
*       Instrukce pro práci s typy
*   =================================
*       fmt[]  - řetězec určující počet argumentů a jejich datový typ (viz začátek tohoto souboru)
*       name[] - proměnná, do které se uloží výsledek operace. Pokud neexistuje, bude vytvořena.
*       A[]    - proměnná nebo konstanta, u které určujeme její datový typ
*/
int gen_type(char fmt[], char name[], char A[]);


/*
*       Instrukce pro práci s řetězci
*   =================================
*       fmt[]  - řetězec určující počet argumentů a jejich datový typ (viz začátek tohoto souboru)
*       name[] - proměnná, do které se uloží výsledek operace. Pokud neexistuje, bude vytvořena.
*       name[] - proměnná, do které ukládáme výsledek
*       A[]    - levý operand (musí být datového typu string)  
*       B[]    - pravý operand (musí být datového typu string)
*/
int gen_strlen(char fmt[], char name[], char A[]);
int gen_concat(char fmt[], char name[], char A[], char B[]);




// POMOCNÉ FUNKCE PRO SPRÁVNOU FUNKCI GENERÁTORU A USNADNĚNÍ PRÁCE V NĚM

// Funkce fungující jako strcat z knihovny string.h s tím rozdílem, že při nedostatku místa proběhne realokace.
void generate(char **destination, char *source);

// Funkce převádí hodnotu value na string odpovídající významu dané hodnoty (viz začátek generator.c)
const char *get_name(int value);
const char *get_func(bool value);
const char *get_type(char value);

// Definice struktury pro ukládání deklarovaných proměnných z ifj22
typedef struct node {
    char *data;
    char type;
    bool temp;
    struct node * next;
} node_t;

// Funkce pro práci nad danou strukturou
void list_print(node_t * head);
void list_push(node_t * head, char *data,char type);
char* list_pop(node_t ** head);
char* list_remove_by_value(node_t ** head, char *data);
void list_delete(node_t *head);
int list_exist(node_t *head, char* data);

// Funkce konvertující string na správný zápis
char *convertString(char *input);

// Funce generující vestavěné funkce a funkce pro kontrolu datovéých typů/implicitní konverzi
void gen_func_ord();
void gen_func_substring();
void gen_func_floatval();
void gen_func_intval();
void gen_func_strval();
void gen_concat_datatype();
void gen_ltgt_datatype();

#endif