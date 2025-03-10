#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Vsechny struktury v tomto programu.
 * Struktury s ukazatelem na dalsi struktury se alokuji.
 * Nejdulezitejsi struktura je struktura everything obsahujici vsechny nad ni (od com_type po set).
 */

typedef struct set
{
    char set_words[32];
} set;

typedef struct uni_type
{
    int numberOfWords;
    struct set *uni_struct;
} uni_type;

typedef struct set_type
{
    int lineNumber;
    int numberOfWords;
    struct set *set_struct;
} set_type;

typedef struct rel
{
    char rel_words[32];
    char rel_words2[32];
} rel;

typedef struct rel_type
{
    int lineNumber;
    int numberOfRels;
    struct rel *rel_struct;
} rel_type;

typedef struct com
{
    char com_words[16];
} com;

typedef struct com_type
{
    int lineNumber;
    int numberOfWords;
    struct com com_struct[6];
} com_type;

typedef struct everything
{
    int linesInSet;
    int linesInRel;
    int linesInCom;
    set_type *set;
    rel_type *rel;
    com_type *com;
} everything;

/**
 * Funkce Errors vypisuje na stderr podle x chybovou hlasku.
 * @param x rozlisuje chyby
 */

void Errors(int x)
{
    switch (x)
    {
    case 1:
        fprintf(stderr, "Soubor nenalezen\n");
        break;
    case 2:
        fprintf(stderr, "Prvni pismeno na radku 1 musi byt U\n");
        break;
    case 3:
        fprintf(stderr, "Prvni znak na radku 2 a dale musi byt S, R nebo C\n");
        break;
    case 4:
        fprintf(stderr, "Alokovani pameti selhalo\n");
        break;
    case 5:
        fprintf(stderr, "V mnozine nebo relaci je vic slov nez v univerzu\n");
        break;
    case 6:
        fprintf(stderr, "Alespon jedno slovo v univerzu, mnozine nebo relaci se opakuje\n");
        break;
    case 7:
        fprintf(stderr, "Alespon jeden prvek nepatri do univerza\n");
        break;
    case 8:
        fprintf(stderr, "Alespon jedno slovo presahuje maximalni delku (30)\n");
        break;
    case 9:
        fprintf(stderr, "Prvky univerza musi obsahovat jen mala nebo velka pismena anglicke abecedy\n");
        break;
    case 10:
        fprintf(stderr, "Alespon jedno slovo v univerzu patri mezi zakazana slova\n");
        break;
    case 11:
        fprintf(stderr, "Alespon jedna funkce je spatne napsana\n");
        break;
    case 12:
        fprintf(stderr, "Alespon jeden prikaz odkazuje na spatny radek\n");
        break;
    case 13:
        fprintf(stderr, "Nedodrzeny spravny syntax (U, S nebo R, C)\n");
        break;
    case 14:
        fprintf(stderr, "Maximalni pocet radku je 1000\n");
        break;
    case 15:
        fprintf(stderr, "V prikazu nesmi byt pismena vyjma identifikatoru\n");
        break;
    case 16:
        fprintf(stderr, "Nedovolena mezera\n");
        break;
    case 17:
        fprintf(stderr, "Alespon jeden znak v relaci nespada do alespon jedne z mnozin\n");
        break;
    case 18:
        fprintf(stderr, "Neznamy pocet argumentu\n");
        break;
    default:
        break;
    }
}

/**
 * Funkce Command_Check kontroluje, jestli je radek typu C napsan spravne.
 * Na vstup bere slovo za C (napr. union) a pocet slov, podle toho vraci urcitou hodnotu.
 * @param line je slovo na vstupu
 * @param numberOfWords je pocet slov v prikazu vyjma C na zacatku
 * @return 1 je chybny vystup
 * @return 0 je bezchybny vystup
 */

int Command_Check(char line[], int numberOfWords)
{
    if ((strcmp(line, "empty") == 0 || strcmp(line, "card") == 0 || strcmp(line, "complement") == 0) && numberOfWords == 2)
        return 0;
    else if ((strcmp(line, "union") == 0 || strcmp(line, "intersect") == 0 || strcmp(line, "minus") == 0 || strcmp(line, "subseteq") == 0 || strcmp(line, "subset") == 0 || strcmp(line, "equals") == 0) && numberOfWords == 3)
        return 0;
    else if ((strcmp(line, "reflexive") == 0 || strcmp(line, "symmetric") == 0 || strcmp(line, "antisymmetric") == 0 || strcmp(line, "transitive") == 0 || strcmp(line, "function") == 0 || strcmp(line, "domain") == 0 || strcmp(line, "codomain") == 0) && numberOfWords == 2)
        return 0;
    else if ((strcmp(line, "injective") == 0 || strcmp(line, "surjective") == 0 || strcmp(line, "bijective") == 0) && numberOfWords == 4)
        return 0;
    return 1;
}

/**
 * Funkce Forbidden_Words kontroluje, jestli se v univerzu nenachazi alespon jedno ze zakazanych slov.
 * Podobne jako u funkce Command_Check bere slovo (nebo znak) a podle neho vraci urcitou hodnotu.
 * @param line je slovo na vstupu
 * @return 1 je chybny vystup
 * @return 0 je bezchybny vystup
 */

int Forbidden_Words(char line[])
{
    if (strcmp(line, "U") == 0 || strcmp(line, "S") == 0 || strcmp(line, "R") == 0 || strcmp(line, "C") == 0 || strcmp(line, "true") == 0 || strcmp(line, "false") == 0)
        return 1;
    else if (strcmp(line, "empty") == 0 || strcmp(line, "card") == 0 || strcmp(line, "complement") == 0 || strcmp(line, "union") == 0 || strcmp(line, "intersect") == 0 || strcmp(line, "minus") == 0 || strcmp(line, "subseteq") == 0 || strcmp(line, "subset") == 0 || strcmp(line, "equals") == 0)
        return 1;
    else if (strcmp(line, "reflexive") == 0 || strcmp(line, "symmetric") == 0 || strcmp(line, "antisymmetric") == 0 || strcmp(line, "transitive") == 0 || strcmp(line, "function") == 0 || strcmp(line, "domain") == 0 || strcmp(line, "codomain") == 0)
        return 1;
    else if (strcmp(line, "injective") == 0 || strcmp(line, "surjective") == 0 || strcmp(line, "bijective") == 0)
        return 1;
    return 0;
}

/**
 * Funkce Empty na mnozinach vypisuje true/false pokud je mnozina prazdna.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Empty(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int isCorrect = 0;
    for (int i = 0; i < ev.linesInSet; i++)
    {
        if (ev.set[i].lineNumber == command)
        {
            isCorrect++;
            if (ev.set[i].numberOfWords != 0)
                return 0;
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 1;
}

/**
 * Funkce Card na mnozinach vypisuje pocet prvku v dane mnozine A.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 0 je bezchybny vystup
 * @return -1 je chybny vystup
 */

int Card(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int isCorrect = 0;
    for (int i = 0; i < ev.linesInSet; i++)
    {
        if (ev.set[i].lineNumber == command)
        {
            isCorrect++;
            printf("%d", ev.set[i].numberOfWords);
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Complement na mnozinach vypisuje doplnek dane mnoziny A.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 0 je bezchybny vystup
 * @return -1 je chybny vystup
 */

int Complement(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int isCorrect = 0;
    int check = 0;
    printf("S");
    for (int i = 0; i < ev.linesInSet; i++)
    {
        if (ev.set[i].lineNumber == command)
        {
            isCorrect++;
            for (int j = 0; j < ev.set[0].numberOfWords; j++)
            {
                for (int k = 0; k < ev.set[i].numberOfWords; k++)
                {
                    if (strcmp(ev.set[i].set_struct[k].set_words, ev.set[0].set_struct[j].set_words) == 0)
                    {
                        check++;
                        break;
                    }
                }
                if (check == 0)
                    printf(" %s", ev.set[0].set_struct[j].set_words);
                check = 0;
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Union na mnozinach vypisuje sjednoceni mnoziny A a B.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 0 je bezchybny vystup
 * @return -1 je chybny vystup
 */

int Union(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int command2 = atoi(ev.com[numberOfLineInCom].com_struct[2].com_words);
    int isCorrect = 0;
    int check = 0;
    printf("S");
    for (int i = 0; i < ev.linesInSet; i++)
    {
        for (int j = 0; j < ev.linesInSet; j++)
        {
            if (ev.set[i].lineNumber == command && ev.set[j].lineNumber == command2)
            {
                isCorrect++;
                for (int k = 0; k < ev.set[i].numberOfWords; k++)
                {
                    for (int l = 0; l < ev.set[j].numberOfWords; l++)
                    {
                        if (strcmp(ev.set[i].set_struct[k].set_words, ev.set[j].set_struct[l].set_words) == 0)
                        {
                            check++;
                            break;
                        }
                    }
                    if (check == 0)
                        printf(" %s", ev.set[i].set_struct[k].set_words);
                    check = 0;
                }
                for (int k = 0; k < ev.set[j].numberOfWords; k++)
                    printf(" %s", ev.set[j].set_struct[k].set_words);
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Intersect na mnozinach vypisuje prunik mnozin A a B.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 0 je bezchybny vystup
 * @return -1 je chybny vystup
 */

int Intersect(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int command2 = atoi(ev.com[numberOfLineInCom].com_struct[2].com_words);
    int isCorrect = 0;
    printf("S");
    for (int i = 0; i < ev.linesInSet; i++)
    {
        for (int j = 0; j < ev.linesInSet; j++)
        {
            if (ev.set[i].lineNumber == command && ev.set[j].lineNumber == command2)
            {
                isCorrect++;
                for (int k = 0; k < ev.set[i].numberOfWords; k++)
                {
                    for (int l = 0; l < ev.set[j].numberOfWords; l++)
                    {
                        if (strcmp(ev.set[i].set_struct[k].set_words, ev.set[j].set_struct[l].set_words) == 0)
                            printf(" %s", ev.set[i].set_struct[k].set_words);
                    }
                }
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Minus na mnozinach vypisuje rozdil mnozin A a B.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 0 je bezchybny vystup
 * @return -1 je chybny vystup
 */

int Minus(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int command2 = atoi(ev.com[numberOfLineInCom].com_struct[2].com_words);
    int isCorrect = 0;
    int check = 0;
    printf("S");
    for (int i = 0; i < ev.linesInSet; i++)
    {
        for (int j = 0; j < ev.linesInSet; j++)
        {
            if (ev.set[i].lineNumber == command && ev.set[j].lineNumber == command2)
            {
                isCorrect++;
                for (int k = 0; k < ev.set[i].numberOfWords; k++)
                {
                    for (int l = 0; l < ev.set[j].numberOfWords; l++)
                    {
                        if (strcmp(ev.set[i].set_struct[k].set_words, ev.set[j].set_struct[l].set_words) == 0)
                        {
                            check++;
                            break;
                        }
                    }
                    if (check == 0)
                        printf(" %s", ev.set[i].set_struct[k].set_words);
                    check = 0;
                }
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Subseteq na mnozinach vypisuje true/false pokud je mnozina A podmnozinou mnoziny B.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Subseteq(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int command2 = atoi(ev.com[numberOfLineInCom].com_struct[2].com_words);
    int isCorrect = 0;
    int check = 0;
    for (int i = 0; i < ev.linesInSet; i++)
    {
        for (int j = 0; j < ev.linesInSet; j++)
        {
            if (ev.set[i].lineNumber == command && ev.set[j].lineNumber == command2)
            {
                isCorrect++;
                for (int k = 0; k < ev.set[i].numberOfWords; k++)
                {
                    for (int l = 0; l < ev.set[j].numberOfWords; l++)
                    {
                        if (strcmp(ev.set[i].set_struct[k].set_words, ev.set[j].set_struct[l].set_words) == 0)
                            check++;
                    }
                }
                if (check == ev.set[i].numberOfWords)
                    return 1;
                else
                    return 0;
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Subset na mnozinach vypisuje true/false pokud je mnozina A vlastni podmnozinou mnoziny B.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Subset(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int command2 = atoi(ev.com[numberOfLineInCom].com_struct[2].com_words);
    int isCorrect = 0;
    int check = 0;
    for (int i = 0; i < ev.linesInSet; i++)
    {
        for (int j = 0; j < ev.linesInSet; j++)
        {
            if (ev.set[i].lineNumber == command && ev.set[j].lineNumber == command2)
            {
                isCorrect++;
                for (int k = 0; k < ev.set[i].numberOfWords; k++)
                {
                    for (int l = 0; l < ev.set[j].numberOfWords; l++)
                    {
                        if (strcmp(ev.set[i].set_struct[k].set_words, ev.set[j].set_struct[l].set_words) == 0)
                            check++;
                    }
                }
                if (check == ev.set[i].numberOfWords && ev.set[i].numberOfWords != ev.set[j].numberOfWords)
                    return 1;
                else
                    return 0;
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Equals na mnozinach vypisuje true/false pokud je mnozina A rovna mnozine B.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Equals(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int command2 = atoi(ev.com[numberOfLineInCom].com_struct[2].com_words);
    int isCorrect = 0;
    int check = 0;
    for (int i = 0; i < ev.linesInSet; i++)
    {
        for (int j = 0; j < ev.linesInSet; j++)
        {
            if (ev.set[i].lineNumber == command && ev.set[j].lineNumber == command2)
            {
                isCorrect++;
                for (int k = 0; k < ev.set[i].numberOfWords; k++)
                {
                    for (int l = 0; l < ev.set[j].numberOfWords; l++)
                    {
                        if (strcmp(ev.set[i].set_struct[k].set_words, ev.set[j].set_struct[l].set_words) == 0)
                            check++;
                    }
                }
                if (check == ev.set[i].numberOfWords && ev.set[i].numberOfWords == ev.set[j].numberOfWords)
                    return 1;
                else
                    return 0;
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Reflexive na relacich vypisuje true/false pokud je relace R reflexivni.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Reflexive(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int isCorrect = 0;
    int check = 0;
    for (int i = 0; i < ev.linesInRel; i++)
    {
        if (ev.rel[i].lineNumber == command)
        {
            isCorrect++;
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[j].rel_words2) == 0)
                    check++;
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    if (check == ev.set->numberOfWords)
        return 1;
    return 0;
}

/**
 * Funkce Symmetric na relacich vypisuje true/false pokud je relace R symetricka.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Symmetric(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int isCorrect = 0;
    int check = 0;
    int numberOfRels = 0;
    for (int i = 0; i < ev.linesInRel; i++)
    {
        if (ev.rel[i].lineNumber == command)
        {
            isCorrect++;
            numberOfRels = ev.rel[i].numberOfRels;
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                for (int k = 0; k < ev.rel[i].numberOfRels; k++)
                {
                    if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[k].rel_words2) == 0 && strcmp(ev.rel[i].rel_struct[k].rel_words, ev.rel[i].rel_struct[j].rel_words2) == 0)
                        check++;
                }
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    if (check == numberOfRels)
        return 1;
    return 0;
}

/**
 * Funkce Antisymmetric na relacich vypisuje true/false pokud je relace R antisymetricka.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Antisymmetric(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int isCorrect = 0;
    for (int i = 0; i < ev.linesInRel; i++)
    {
        if (ev.rel[i].lineNumber == command)
        {
            isCorrect++;
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                for (int k = 0; k < ev.rel[i].numberOfRels; k++)
                {
                    if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[k].rel_words2) == 0 && strcmp(ev.rel[i].rel_struct[k].rel_words, ev.rel[i].rel_struct[j].rel_words2) == 0 && strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[j].rel_words2) != 0)
                        return 0;
                }
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 1;
}

/**
 * Funkce Transitive na relacich vypisuje true/false pokud je relace R tranzitivni.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Transitive(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int isCorrect = 0;
    int check = 0;
    for (int i = 0; i < ev.linesInRel; i++)
    {
        if (ev.rel[i].lineNumber == command)
        {
            isCorrect++;
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                for (int k = 0; k < ev.rel[i].numberOfRels; k++)
                {
                    if (strcmp(ev.rel[i].rel_struct[j].rel_words2, ev.rel[i].rel_struct[k].rel_words) == 0)
                    {
                        check++;
                        for (int l = 0; l < ev.rel[i].numberOfRels; l++)
                        {
                            if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[l].rel_words) == 0 && strcmp(ev.rel[i].rel_struct[k].rel_words2, ev.rel[i].rel_struct[l].rel_words2) == 0)
                            {
                                check--;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    if (check == 0)
        return 1;
    return 0;
}

/**
 * Funkce Function na relacich vypisuje true/false pokud je relace R funkci.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Function(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int numberOfRels = 0;
    int isCorrect = 0;
    int check = 0;
    int pyramid = 1;
    for (int i = 0; i < ev.linesInRel; i++)
    {
        if (ev.rel[i].lineNumber == command)
        {
            isCorrect++;
            numberOfRels = ev.rel[i].numberOfRels;
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                for (int k = 0; k < pyramid; k++)
                {
                    if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[k].rel_words) == 0 && j != k)
                        break;
                    else if (k + 1 == pyramid)
                        check++;
                }
                pyramid++;
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    if (check == numberOfRels)
        return 1;
    return 0;
}

/**
 * Funkce Domain na relacich vypisuje definicni obor funkce R.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 0 je bezchybny vystup
 * @return -1 je chybny vystup
 */

int Domain(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int isCorrect = 0;
    int pyramid = 1;
    printf("S");
    for (int i = 0; i < ev.linesInRel; i++)
    {
        if (ev.rel[i].lineNumber == command)
        {
            isCorrect++;
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                for (int k = 0; k < pyramid; k++)
                {
                    if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[k].rel_words) == 0 && j != k)
                        break;
                    else if (k + 1 == pyramid)
                        printf(" %s", ev.rel[i].rel_struct[j].rel_words);
                }
                pyramid++;
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Codomain na relacich vypisuje obor hodnot funkce R.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 0 je bezchybny vystup
 * @return -1 je chybny vystup
 */

int Codomain(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int isCorrect = 0;
    int pyramid = 1;
    printf("S");
    for (int i = 0; i < ev.linesInRel; i++)
    {
        if (ev.rel[i].lineNumber == command)
        {
            isCorrect++;
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                for (int k = 0; k < pyramid; k++)
                {
                    if (strcmp(ev.rel[i].rel_struct[j].rel_words2, ev.rel[i].rel_struct[k].rel_words2) == 0 && j != k)
                        break;
                    else if (k + 1 == pyramid)
                        printf(" %s", ev.rel[i].rel_struct[j].rel_words2);
                }
                pyramid++;
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    return 0;
}

/**
 * Funkce Injective na relacich vypisuje true/false pokud je relace R injektivni.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Injective(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int command2 = atoi(ev.com[numberOfLineInCom].com_struct[2].com_words);
    int command3 = atoi(ev.com[numberOfLineInCom].com_struct[3].com_words);
    int tempRel = 0;
    int tempSet = 0;
    int domain = 0;
    int isCorrect = 0;
    int pyramid = 1;
    for (int i = 0; i < ev.linesInRel; i++)
    {
        for (int j = 0; j < ev.linesInSet; j++)
        {
            for (int k = 0; k < ev.linesInSet; k++)
            {
                if (ev.rel[i].lineNumber == command && ev.set[j].lineNumber == command2 && ev.set[k].lineNumber == command3)
                {
                    isCorrect++;
                    tempRel = ev.rel[i].numberOfRels;
                    tempSet = ev.set[j].numberOfWords;
                }
            }
        }
        if (ev.rel[i].lineNumber == command)
        {
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                for (int k = 0; k < pyramid; k++)
                {
                    if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[k].rel_words) == 0 && j != k)
                        break;
                    else if (k + 1 == pyramid)
                        domain++;
                }
                pyramid++;
            }
        }
    }
    if (isCorrect == 0)
    {
        printf("%d", ev.rel[5].numberOfRels);
        Errors(12);
        return -1;
    }
    if (domain == tempRel && domain == tempSet)
        return 1;
    return 0;
}

/**
 * Funkce Surjective na relacich vypisuje true/false pokud je relace R surjektivni.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Surjective(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int command2 = atoi(ev.com[numberOfLineInCom].com_struct[2].com_words);
    int command3 = atoi(ev.com[numberOfLineInCom].com_struct[3].com_words);
    int tempRel = 0;
    int tempSet2 = 0;
    int codomain = 0;
    int isCorrect = 0;
    int pyramid = 1;
    for (int i = 0; i < ev.linesInRel; i++)
    {
        for (int j = 0; j < ev.linesInSet; j++)
        {
            for (int k = 0; k < ev.linesInSet; k++)
            {
                if (ev.rel[i].lineNumber == command && ev.set[j].lineNumber == command2 && ev.set[k].lineNumber == command3)
                {
                    isCorrect++;
                    tempRel = ev.rel[i].numberOfRels;
                    tempSet2 = ev.set[k].numberOfWords;
                }
            }
        }
        if (ev.rel[i].lineNumber == command)
        {
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                for (int k = 0; k < pyramid; k++)
                {
                    if (strcmp(ev.rel[i].rel_struct[j].rel_words2, ev.rel[i].rel_struct[k].rel_words2) == 0 && j != k)
                        break;
                    else if (k + 1 == pyramid)
                        codomain++;
                }
                pyramid++;
            }
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    if (codomain == tempRel && codomain == tempSet2)
        return 1;
    return 0;
}

/**
 * Funkce Bijective na relacich vypisuje true/false pokud je relace R bijektivni.
 * @param ev je hlavni struktura
 * @param numberOfLineInCom je cislo radku na nemz je prikaz
 * @return 1 je false
 * @return 0 je true
 * @return -1 je chybny vystup
 */

int Bijective(everything ev, int numberOfLineInCom)
{
    int command = atoi(ev.com[numberOfLineInCom].com_struct[1].com_words);
    int command2 = atoi(ev.com[numberOfLineInCom].com_struct[2].com_words);
    int command3 = atoi(ev.com[numberOfLineInCom].com_struct[3].com_words);
    int tempRel = 0;
    int tempSet = 0;
    int tempSet2 = 0;
    int domain = 0;
    int codomain = 0;
    int isCorrect = 0;
    int pyramid = 1;
    for (int i = 0; i < ev.linesInRel; i++)
    {
        for (int j = 0; j < ev.linesInSet; j++)
        {
            for (int k = 0; k < ev.linesInSet; k++)
            {
                if (ev.rel[i].lineNumber == command && ev.set[j].lineNumber == command2 && ev.set[k].lineNumber == command3)
                {
                    if (ev.set[j].numberOfWords == 0 && ev.set[k].numberOfWords == 0)
                        return 1;
                    isCorrect++;
                    tempRel = ev.rel[i].numberOfRels;
                    tempSet = ev.set[j].numberOfWords;
                    tempSet2 = ev.set[k].numberOfWords;
                }
            }
        }
        if (ev.rel[i].lineNumber == command)
        {
            for (int j = 0; j < ev.rel[i].numberOfRels; j++)
            {
                for (int k = 0; k < pyramid; k++)
                {
                    if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[k].rel_words) == 0 && j != k)
                        break;
                    else if (k + 1 == pyramid)
                        domain++;
                }
                pyramid++;
            }
            pyramid = 1;
        }
        for (int j = 0; j < ev.rel[i].numberOfRels; j++)
        {
            for (int k = 0; k < pyramid; k++)
            {
                if (strcmp(ev.rel[i].rel_struct[j].rel_words2, ev.rel[i].rel_struct[k].rel_words2) == 0 && j != k)
                    break;
                else if (k + 1 == pyramid)
                    codomain++;
            }
            pyramid++;
        }
    }
    if (isCorrect == 0)
    {
        Errors(12);
        return -1;
    }
    if (domain == tempRel && domain == tempSet && codomain == tempRel && codomain == tempSet2)
        return 1;
    return 0;
}

/**
 * Funkce Functions tridi nazev identifikatoru prikazu do spravne funkce.
 * Funkce maji 2 typy, kdy vypisuji true/false nebo vypisuji cislo/prvky na mnozine.
 * @param ev je hlavni struktura
 * @return 0 je bezchybny vystup
 * @return 1 je chybny vystup
 */

int Functions(everything ev)
{
    for (int i = 0; i < ev.linesInCom; i++)
    {
        if (strcmp(ev.com[i].com_struct->com_words, "empty") == 0)
        {
            switch (Empty(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "card") == 0)
        {
            if (Card(ev, i) == -1)
                return 1;
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "complement") == 0)
        {
            if (Complement(ev, i) == -1)
                return 1;
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "union") == 0)
        {
            if (Union(ev, i) == -1)
                return 1;
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "intersect") == 0)
        {
            if (Intersect(ev, i) == -1)
                return 1;
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "minus") == 0)
        {
            if (Minus(ev, i) == -1)
                return 1;
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "subseteq") == 0)
        {
            switch (Subseteq(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "subset") == 0)
        {
            switch (Subset(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "equals") == 0)
        {
            switch (Equals(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "reflexive") == 0)
        {
            switch (Reflexive(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "symmetric") == 0)
        {
            switch (Symmetric(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "antisymmetric") == 0)
        {
            switch (Antisymmetric(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "transitive") == 0)
        {
            switch (Transitive(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "function") == 0)
        {
            switch (Function(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "domain") == 0)
        {
            if (Domain(ev, i) == -1)
                return 1;
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "codomain") == 0)
        {
            if (Codomain(ev, i) == -1)
                return 1;
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "injective") == 0)
        {
            switch (Injective(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "surjective") == 0)
        {
            switch (Surjective(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        else if (strcmp(ev.com[i].com_struct->com_words, "bijective") == 0)
        {
            switch (Bijective(ev, i))
            {
            case -1:
                return 1;
            case 0:
                printf("false");
                break;
            case 1:
                printf("true");
                break;
            }
        }
        printf("\n");
    }
    return 0;
}

/**
 * Funkce Set_Error_Check zjistuje, jestli jsou vsechny mnoziny sepsany spravne, jinak vraci chybu.
 * @param ev je hlavni struktura
 * @return 0 je bezchybny vystup
 * @return 1 je chybny vystup
 */

int Set_Error_Check(everything ev)
{
    int check = 0;
    for (int i = 0; i < ev.linesInSet; i++)
    {
        for (int j = 0; j < ev.set[i].numberOfWords; j++)
        {
            for (int k = 0; k < ev.set[i].numberOfWords; k++)
            {
                if (strcmp(ev.set[i].set_struct[j].set_words, ev.set[i].set_struct[k].set_words) == 0)
                    check++;
            }
            if (check != 1)
            {
                Errors(6);
                return 1;
            }
            check = 0;
        }
        for (int j = 0; j < ev.set[i].numberOfWords; j++)
        {
            for (int k = 0; k < ev.set[0].numberOfWords; k++)
            {
                if (strcmp(ev.set[i].set_struct[j].set_words, ev.set[0].set_struct[k].set_words) == 0)
                    check++;
            }
        }
        if (check != ev.set[i].numberOfWords)
        {
            Errors(7);
            return 1;
        }
        check = 0;
    }
    return 0;
}

/**
 * Funkce Rel_Error_Check zjistuje, jestli jsou vsechny relace sepsany spravne, jinak vraci chybu.
 * @param ev je hlavni struktura
 * @return 0 je bezchybny vystup
 * @return 1 je chybny vystup
 */

int Rel_Error_Check(everything ev)
{
    int check = 0;
    for (int i = 0; i < ev.linesInRel; i++)
    {
        for (int j = 0; j < ev.rel[i].numberOfRels; j++)
        {
            for (int k = 0; k < ev.rel[i].numberOfRels; k++)
            {
                if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.rel[i].rel_struct[k].rel_words) == 0 && strcmp(ev.rel[i].rel_struct[j].rel_words2, ev.rel[i].rel_struct[k].rel_words2) == 0)
                    check++;
            }
            if (check != 1)
            {
                Errors(6);
                return 1;
            }
            check = 0;
        }
        for (int j = 0; j < ev.rel[i].numberOfRels; j++)
        {
            for (int k = 0; k < ev.set[0].numberOfWords; k++)
            {
                if (strcmp(ev.rel[i].rel_struct[j].rel_words, ev.set[0].set_struct[k].set_words) == 0)
                    check++;
                if (strcmp(ev.rel[i].rel_struct[j].rel_words2, ev.set[0].set_struct[k].set_words) == 0)
                    check++;
            }
        }
        if (check / 2 != ev.rel[i].numberOfRels)
        {
            Errors(7);
            return 1;
        }
        check = 0;
    }
    return 0;
}

/**
 * Funkce Com_Error_Check zjistuje, jestli jsou vsechny prikazy sepsany spravne, jinak vraci chybu.
 * @param ev je hlavni struktura
 * @return 0 je bezchybny vystup
 * @return 1 je chybny vystup
 */

int Com_Error_Check(everything ev)
{
    for (int i = 0; i < ev.linesInCom; i++)
    {
        if (Command_Check(ev.com[i].com_struct[0].com_words, ev.com[i].numberOfWords) == 1)
        {
            Errors(11);
            return 1;
        }
        for (int j = 1; j < ev.com[i].numberOfWords; j++)
        {
            for (int k = 0; ev.com[i].com_struct[j].com_words[k]; k++)
            {
                if (isdigit(ev.com[i].com_struct[j].com_words[k]) == 0)
                {
                    Errors(15);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/**
 * Funkce Struct_Init vytvari pocet struktur na zaklade poctu S, R a C a pocet slov ve strukturach na zaklade poctu slov v univerzu.
 * @param ev je hlavni struktura
 * @param wordsInUni je pocet slov v univerzu
 * @return 0 je bezchybny vystup
 * @return 1 je chybny vystup
 */

int Struct_Init(everything *ev, int wordsInUni)
{
    ev->set = malloc(sizeof(struct set_type) * ev->linesInSet);
    if (ev->set == NULL)
    {
        Errors(4);
        return 1;
    }
    for (int i = 0; i < ev->linesInSet; i++)
    {
        ev->set[i].lineNumber = 0;
        ev->set[i].numberOfWords = 0;
        ev->set[i].set_struct = malloc(sizeof(struct set) * wordsInUni);
        if (ev->set[i].set_struct == NULL)
        {
            Errors(4);
            return 1;
        }
    }
    if (ev->linesInRel != 0)
    {
        ev->rel = malloc(sizeof(struct rel_type) * ev->linesInRel);
        if (ev->rel == NULL)
        {
            Errors(4);
            return 1;
        }
        for (int i = 0; i < ev->linesInRel; i++)
        {
            ev->rel[i].lineNumber = 0;
            ev->rel[i].numberOfRels = 0;
            ev->rel[i].rel_struct = malloc(sizeof(struct rel) * wordsInUni * wordsInUni);
            if (ev->rel[i].rel_struct == NULL)
            {
                Errors(4);
                return 1;
            }
        }
    }
    ev->com = malloc(sizeof(struct com_type) * ev->linesInCom);
    if (ev->com == NULL)
    {
        Errors(4);
        return 1;
    }
    ev->com->lineNumber = 0;
    ev->com->numberOfWords = 0;
    return 0;
}

/**
 * Funkce Struct_Dtor maze vsechny struktury.
 * Dulezitou poznamkou je to, kdyz je opoznamkovan, tak valgrind nevyhazuje chyby.
 * @param ev je hlavni struktura
 */

void Struct_Dtor(everything *ev)
{
    if (ev->set->set_struct != NULL)
    {
        free(ev->set->set_struct);
        ev->set->set_struct = NULL;
    }
    if (ev->set != NULL)
    {
        free(ev->set);
        ev->set = NULL;
    }

    if (ev->linesInRel != 0)
    {
        if (ev->rel->rel_struct != NULL)
        {
            free(ev->rel->rel_struct);
            ev->rel->rel_struct = NULL;
        }
        if (ev->rel != NULL)
        {
            free(ev->rel);
            ev->rel = NULL;
        }
    }
    if (ev->com != NULL)
    {
        free(ev->com);
        ev->com = NULL;
    }
}

/**
 * Funkce Number_Of_Lines_Check vraci pocet S, R a C a pocet slov v univerzu.
 * @param fileName je nazev existujiciho souboru
 * @param ev je hlavni struktura
 * @param wordsInUni pocet slov v univerzu
 * @return 0 je bezchybny vystup
 * @return 1 je chybny vystup
 */

int Number_Of_Lines_Check(char fileName[], everything *ev, int *wordsInUni)
{
    char c;
    int character = 0;
    int line = 0;
    int space = 0;
    int set = 0, rel = 0, com = 0;

    FILE *file = fopen(fileName, "r");

    if (file == NULL)
    {
        Errors(1);
        return 1;
    }
    while ((c = getc(file)) != EOF)
    {
        if (line == 0 && character == 0 && c != 'U')
        {
            Errors(2);
            return 1;
        }
        else if (line != 0 && character == 0)
        {
            if (c == 'S')
                set++;
            else if (c == 'R')
                rel++;
            else if (c == 'C')
                com++;
            else
            {
                Errors(3);
                return 1;
            }
        }
        if (c == '\n')
        {
            line++;
            character = 0;
        }
        else
        {
            character++;
            if (line == 0)
            {
                if (1 < character && c == ' ')
                    space++;
            }
        }
    }
    fclose(file);
    if (set + rel == 0 || com == 0)
    {
        Errors(13);
        return 1;
    }
    if (set + rel + com + 1 > 1000)
    {
        Errors(14);
        return 1;
    }
    *wordsInUni = space;
    ev->linesInSet = set + 1;
    ev->linesInRel = rel;
    ev->linesInCom = com;
    return 0;
}

/**
 * Funkce Loading_From_File nacita vsechna data ze souboru a uklada je do struktur.
 * @param fileName je nazev existujiciho souboru
 * @param ev je hlavni struktura
 * @return 0 je bezchybny vystup
 * @return 1 je chybny vystup
 */

int Loading_From_File(char fileName[], everything *ev)
{
    char c;
    char type;

    char correctSyntaxCheck = 'U';

    int line = 0;
    int characterInLine = 0;
    int characterInWord = 0;
    int space = 0;

    char helper[32];
    memset(helper, '\0', sizeof(helper));

    int set = 0;
    int rel = 0;
    int com = 0;

    int left = 0;
    int right = 0;

    int leftBracket = 0;
    int rightBracket = 0;

    char relCheck = '.';

    FILE *file = fopen(fileName, "r");

    while ((c = getc(file)) != EOF)
    {
        if (characterInLine == 0)
        {
            type = c;
        }
        if (0 < characterInLine)
        {
            if (line == 0 && type == 'U')
            {
                if (30 < characterInWord)
                {
                    Errors(8);
                    return 1;
                }
                if (ev->set[set].lineNumber == 0)
                    ev->set[set].lineNumber = line + 1;
                if (c == '\n')
                {
                    if (Forbidden_Words(helper) == 1)
                    {
                        Errors(10);
                        return 1;
                    }
                    if (isalpha(helper[30]) != 0)
                    {
                        Errors(8);
                        return 1;
                    }
                    strcpy(ev->set[0].set_struct[space - 1].set_words, helper);
                    memset(helper, '\0', sizeof(helper));
                    if (isalpha(ev->set[0].set_struct[space - 1].set_words[0]) == 0 && space != ev->set[0].set_struct[space - 1].set_words[0])
                    {
                        Errors(16);
                        return 1;
                    }
                    ev->set[0].numberOfWords = space;
                    characterInLine = 0;
                    characterInWord = 0;
                    space = 0;
                    set++;
                }
                else if (c != ' ')
                {
                    if (isalpha(c) == 0)
                    {
                        Errors(9);
                        return 1;
                    }
                    helper[characterInWord] = c;
                    characterInWord++;
                }
                else
                {
                    if (Forbidden_Words(helper) == 1)
                    {
                        Errors(10);
                        return 1;
                    }
                    if (isalpha(helper[30]) != 0)
                    {
                        Errors(8);
                        return 1;
                    }
                    strcpy(ev->set[0].set_struct[space - 1].set_words, helper);
                    memset(helper, '\0', sizeof(helper));
                    space++;
                    characterInWord = 0;
                }
            }
            else if (type == 'S')
            {
                if (isalpha(helper[30]) != 0)
                {
                    Errors(8);
                    return 1;
                }
                if (ev->set[set].lineNumber == 0)
                    ev->set[set].lineNumber = line + 1;
                if (c == '\n')
                {
                    if (ev->set[0].numberOfWords < space)
                    {
                        Errors(5);
                        return 1;
                    }
                    if (correctSyntaxCheck == 'C')
                    {
                        Errors(13);
                        return 1;
                    }
                    correctSyntaxCheck = type;
                    strcpy(ev->set[set].set_struct[space - 1].set_words, helper);
                    memset(helper, '\0', sizeof(helper));
                    if (isalpha(ev->set[set].set_struct[space - 1].set_words[0]) == 0 && space != ev->set[set].set_struct[space - 1].set_words[0])
                    {
                        Errors(16);
                        return 1;
                    }
                    ev->set[set].numberOfWords = space;
                    characterInLine = 0;
                    characterInWord = 0;
                    space = 0;
                    set++;
                }
                else if (c != ' ')
                {
                    if (characterInLine == 1 && c != ' ')
                    {
                        Errors(11);
                        return 1;
                    }
                    helper[characterInWord] = c;
                    characterInWord++;
                }
                else
                {
                    strcpy(ev->set[set].set_struct[space - 1].set_words, helper);
                    memset(helper, '\0', sizeof(helper));
                    space++;
                    characterInWord = 0;
                }
            }
            else if (type == 'R')
            {
                if (isalpha(helper[30]) != 0)
                {
                    Errors(8);
                    return 1;
                }
                if (ev->rel[rel].lineNumber == 0)
                    ev->rel[rel].lineNumber = line + 1;
                ev->rel[rel].numberOfRels = space;
                if ((c == ' ' && relCheck == ')') || characterInLine == 1)
                {
                    strcpy(ev->rel[rel].rel_struct[space - 1].rel_words2, helper);
                    memset(helper, '\0', sizeof(helper));
                    characterInWord = 0;
                    space++;
                    if (leftBracket != rightBracket)
                    {
                        Errors(11);
                        return 1;
                    }
                }
                else if (c == ' ' && isalpha(relCheck) != 0)
                {
                    characterInWord = 0;
                    strcpy(ev->rel[rel].rel_struct[space - 1].rel_words, helper);
                    memset(helper, '\0', sizeof(helper));
                    left++;
                }
                else if (c == '(')
                {
                    left = 0;
                    leftBracket++;
                    right++;
                }
                else if (c == ')')
                {
                    rightBracket++;
                    relCheck = c;
                }
                else if (c == '\n')
                {
                    if (ev->set[0].numberOfWords * ev->set[0].numberOfWords < space)
                    {
                        Errors(5);
                        return 1;
                    }
                    if (correctSyntaxCheck == 'C')
                    {
                        Errors(13);
                        return 1;
                    }
                    correctSyntaxCheck = type;
                    strcpy(ev->rel[rel].rel_struct[space - 1].rel_words2, helper);
                    memset(helper, '\0', sizeof(helper));
                    characterInLine = 0;
                    characterInWord = 0;
                    space = 0;
                    rel++;
                }
                else
                {
                    relCheck = c;
                    if (space % 2 == 0)
                        right = 0;
                    helper[characterInWord] = c;
                    characterInWord++;
                }
            }
            else if (type == 'C')
            {
                if (ev->com[com].lineNumber == 0)
                    ev->com[com].lineNumber = line + 1;
                if (c == '\n')
                {
                    correctSyntaxCheck = type;
                    strcpy(ev->com[com].com_struct[space - 1].com_words, helper);
                    memset(helper, '\0', sizeof(helper));
                    ev->com[com].numberOfWords = space;
                    characterInLine = 0;
                    characterInWord = 0;
                    space = 0;
                    com++;
                }
                else if (c != ' ')
                {
                    helper[characterInWord] = c;
                    characterInWord++;
                }
                else
                {
                    strcpy(ev->com[com].com_struct[space - 1].com_words, helper);
                    memset(helper, '\0', sizeof(helper));
                    space++;
                    characterInWord = 0;
                }
            }
        }
        if (type != 'C')
            printf("%c", c);
        if (c == '\n')
        {
            line++;
            if (characterInLine == 1 && type == 'R')
            {
                if (correctSyntaxCheck == 'C')
                {
                    Errors(13);
                    return 1;
                }
                if (isalpha(helper[30]) != 0)
                {
                    Errors(8);
                    return 1;
                }
                ev->rel[rel].lineNumber=line;
                for (int i = 0; i < ev->linesInRel; i++)
                {
                    if (line == ev->rel[i].lineNumber)
                    {
                        ev->rel[i].numberOfRels = 0;
                        characterInLine = 0;
                        characterInWord = 0;
                        space = 0;
                        rel++;
                    }
                }
            }
            characterInLine = 0;
            characterInWord = 0;
            space = 0;
        }
        else
        {
            characterInLine++;
        }
    }
    fclose(file);
    return 0;
}

int main(int argc, char *argv[])
{
    everything ev;

    if (argc != 2)
    {
        Errors(18);
        return 1;
    }

    int wordsInUni = 0;

    if (Number_Of_Lines_Check(argv[1], &ev, &wordsInUni) == 1)
        return 1;

    Struct_Init(&ev, wordsInUni);

    if (Loading_From_File(argv[1], &ev) == 1 || (ev.linesInSet != 0 && Set_Error_Check(ev) != 0) || (ev.linesInRel != 0 && Rel_Error_Check(ev) != 0) || Com_Error_Check(ev) != 0)
    {
        Struct_Dtor(&ev);
        return 1;
    }

    if (Functions(ev) == 1)
    {
        Struct_Dtor(&ev);
        return 1;
    }

    Struct_Dtor(&ev);

    return 0;
}