//======== Copyright (c) 2022, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - test suite
//
// $NoKeywords: $ivs_project_1 $white_box_tests.cpp
// $Author:     Dalibor Kalina <xkalin16@stud.fit.vutbr.cz>
// $Date:       $2023-03-07
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Dalibor Kalina
 * 
 * @brief Implementace testu hasovaci tabulky.
 */

#include <vector>

#include "gtest/gtest.h"

#include "white_box_code.h"

using namespace ::testing;

// Trida pro testovani vsech funkci nad prazdnou i neprazdnou tabulkou krom ctor a dtor
class HashTable : public Test{
    protected:
        hash_map_t* map;
};
// Trida pro testovani ctor a dtor nad prazdnou tabulkou
class EmptyHashTable : public Test{
    protected:
        hash_map_t* map;
};

TEST_F(EmptyHashTable, hash_map_ctor){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    
    EXPECT_EQ(map->allocated, HASH_MAP_INIT_SIZE);
    EXPECT_EQ(map->used, 0);
    EXPECT_TRUE(map->first == NULL);
    EXPECT_TRUE(map->last == NULL);
    //EXPECT_TRUE(map->index == NULL);

    auto temp = map;
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    EXPECT_EQ(map->allocated, HASH_MAP_INIT_SIZE);
    EXPECT_EQ(map->used, 0);
    EXPECT_TRUE(map->first == NULL);
    EXPECT_TRUE(map->last == NULL);

    EXPECT_FALSE(map == temp);

    hash_map_dtor(map);
    hash_map_dtor(temp);
}

TEST_F(EmptyHashTable, hash_map_dtor){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    const char* key = "key";

    hash_map_dtor(map);
    EXPECT_EQ(map->allocated, 0);
    EXPECT_EQ(map->used, 0);
    EXPECT_TRUE(map->last == NULL);
    //EXPECT_TRUE(map->dummy == NULL);
}

TEST_F(HashTable, hash_map_ctor){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    const char* key = "key";

    hash_map_put(map,key,1);
    EXPECT_EQ(map->allocated, HASH_MAP_INIT_SIZE);
    EXPECT_EQ(map->used, 1);

    auto temp = map;
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    EXPECT_EQ(map->allocated, HASH_MAP_INIT_SIZE);
    EXPECT_EQ(map->used, 0);
    EXPECT_TRUE(map->first == NULL);
    EXPECT_TRUE(map->last == NULL);

    EXPECT_FALSE(map == temp);
    EXPECT_EQ(temp->allocated, HASH_MAP_INIT_SIZE);
    EXPECT_EQ(temp->used, 1);
    EXPECT_TRUE(hash_map_contains(temp,key));


    hash_map_dtor(map);
    hash_map_dtor(temp);
}

TEST_F(HashTable, hash_map_dtor){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    const char* key = "key";

    hash_map_put(map,key,1);
    EXPECT_EQ(map->allocated, HASH_MAP_INIT_SIZE);
    EXPECT_EQ(map->used, 1);

    hash_map_dtor(map);
    EXPECT_EQ(map->allocated, 0);
    EXPECT_EQ(map->used, 0);
    EXPECT_TRUE(map->last == NULL);
    //EXPECT_TRUE(map->dummy == NULL);
}

TEST_F(HashTable, hash_map_clear){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    const char* key = "key";

    hash_map_clear(map);
    EXPECT_EQ(hash_map_size(map), 0);
    EXPECT_EQ(hash_map_capacity(map), HASH_MAP_INIT_SIZE);

    hash_map_put(map, key, 1);
    hash_map_clear(map);
    EXPECT_EQ(hash_map_size(map), 0);
    EXPECT_EQ(hash_map_capacity(map), HASH_MAP_INIT_SIZE);
    EXPECT_TRUE(map->first == NULL);
    EXPECT_TRUE(map->last == NULL);

    hash_map_dtor(map);
}

TEST_F(HashTable, hash_map_reserve){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    const char* key = "key";
    
    hash_map_state_code_t can_we_reserve = hash_map_reserve(map, 2*HASH_MAP_INIT_SIZE);
    EXPECT_EQ(can_we_reserve, OK);
    EXPECT_EQ(hash_map_capacity(map), 2*HASH_MAP_INIT_SIZE);

    hash_map_put(map, "1", 1);
    hash_map_put(map, "2", 2);
    hash_map_put(map, "3", 3);

    EXPECT_EQ(hash_map_capacity(map),2*HASH_MAP_INIT_SIZE);
    can_we_reserve = hash_map_reserve(map, 2);
    EXPECT_EQ(can_we_reserve, VALUE_ERROR);
    EXPECT_EQ(hash_map_capacity(map),2*HASH_MAP_INIT_SIZE);

    can_we_reserve = hash_map_reserve(map,4*HASH_MAP_INIT_SIZE);
    EXPECT_EQ(can_we_reserve, OK);
    EXPECT_EQ(hash_map_capacity(map),4*HASH_MAP_INIT_SIZE);

    int value;
    EXPECT_TRUE(hash_map_contains(map,"1"));
    EXPECT_TRUE(hash_map_contains(map,"2"));
    EXPECT_TRUE(hash_map_contains(map,"3"));
    hash_map_get(map, "1", &value);
    EXPECT_EQ(value,1);
    hash_map_get(map, "2", &value);
    EXPECT_EQ(value,2);
    hash_map_get(map, "3", &value);
    EXPECT_EQ(value,3);
    

    hash_map_dtor(map);
}

TEST_F(HashTable, hash_map_reserve_negative){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    const char* key = "key";

    hash_map_state_code_t can_we_reserve = hash_map_reserve(map, -1);
    EXPECT_EQ(can_we_reserve, VALUE_ERROR); // can_we_reserve == MEMORY_ERROR
    EXPECT_EQ(hash_map_capacity(map),HASH_MAP_INIT_SIZE);

    hash_map_dtor(map);
}

TEST_F(HashTable, hash_map_size_capacity){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    const char* key = "key";

    EXPECT_EQ(hash_map_capacity(map),HASH_MAP_INIT_SIZE);
    EXPECT_EQ(map->allocated,HASH_MAP_INIT_SIZE);
    EXPECT_EQ(hash_map_size(map), 0);
    EXPECT_EQ(map->used, 0);

    hash_map_put(map, key, 1);
    EXPECT_EQ(hash_map_size(map), 1);
    EXPECT_EQ(hash_map_capacity(map),HASH_MAP_INIT_SIZE);
    hash_map_remove(map, key);
    //EXPECT_EQ(hash_map_size(map), 0); 
    EXPECT_EQ(hash_map_capacity(map),HASH_MAP_INIT_SIZE);

    hash_map_dtor(map);
    map = hash_map_ctor();  // znovu vytvoreni, protoze map->used se nesnizuje
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    const char* keys[] = {"1","2","3","4","5","6","7","8"};
    const char* keyz[] = {"a","b","c","d","e","f","g","h"};

    EXPECT_EQ(hash_map_capacity(map), HASH_MAP_INIT_SIZE);
    for (int i = 0; i<=7; i++){
        hash_map_put(map, keys[i], i);
    }
    EXPECT_EQ(hash_map_size(map),8);
    EXPECT_EQ(hash_map_capacity(map), 2 * HASH_MAP_INIT_SIZE);
    for (int i = 0; i<=7; i++){
        hash_map_put(map, keyz[i], i);
    }
    EXPECT_EQ(hash_map_size(map),16);
    EXPECT_EQ(hash_map_capacity(map), 4 * HASH_MAP_INIT_SIZE);

    hash_map_clear(map);
    EXPECT_EQ(hash_map_capacity(map), 4 * HASH_MAP_INIT_SIZE);

    hash_map_dtor(map);
    EXPECT_EQ(hash_map_capacity(map),0);
}

TEST_F(HashTable, hash_map_contains){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    
    EXPECT_FALSE(hash_map_contains(map, "one"));
    EXPECT_FALSE(hash_map_contains(map, "two"));
    EXPECT_FALSE(hash_map_contains(map, "ten"));

    hash_map_put(map, "one", 1);
    hash_map_put(map, "two", 2);
    hash_map_put(map, "ten", 10);

    EXPECT_TRUE(hash_map_contains(map, "one"));
    EXPECT_TRUE(hash_map_contains(map, "two"));
    EXPECT_TRUE(hash_map_contains(map, "ten"));

    hash_map_clear(map);

    EXPECT_FALSE(hash_map_contains(map, "one"));
    EXPECT_FALSE(hash_map_contains(map, "two"));
    EXPECT_FALSE(hash_map_contains(map, "ten"));

    hash_map_dtor(map);
}

TEST_F(HashTable, hash_map_put){
    
    // Vlozeni jednoho, dvou prvku a prvek na stejny klic

    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    int value;
    const char* key = "key";

    hash_map_state_code_t is_value_overwritten = hash_map_put(map, key, 5);
    EXPECT_TRUE(is_value_overwritten == OK);
    EXPECT_TRUE(hash_map_contains(map,key));
    hash_map_get(map,key,&value);
    EXPECT_EQ(value,5);

    is_value_overwritten = hash_map_put(map,key,10);
    EXPECT_TRUE(is_value_overwritten == KEY_ALREADY_EXISTS);
    EXPECT_TRUE(hash_map_contains(map,key));
    hash_map_get(map,key,&value);
    EXPECT_EQ(value,10);

    is_value_overwritten = hash_map_put(map,"klic",15);
    EXPECT_TRUE(is_value_overwritten == OK);
    EXPECT_TRUE(hash_map_contains(map,"klic"));
    hash_map_get(map,"klic",&value);
    EXPECT_EQ(value,15);

    hash_map_get(map,key,&value);
    EXPECT_EQ(value,10);
    EXPECT_EQ(hash_map_capacity(map), HASH_MAP_INIT_SIZE);
    EXPECT_EQ(hash_map_size(map), 2);

    hash_map_dtor(map);


    // Kontrola zmeny velikosti pri dosahnuti 2/3 kapacity

    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";

    hash_map_put(map, "one"  , 1);
    hash_map_put(map, "two"  , 2);
    hash_map_put(map, "three", 3);
    hash_map_put(map, "four" , 4);
    hash_map_put(map, "five" , 5);
    EXPECT_EQ(hash_map_size(map),5);
    EXPECT_EQ(hash_map_capacity(map),     HASH_MAP_INIT_SIZE);

    hash_map_put(map, "six"  , 6);
    EXPECT_EQ(hash_map_size(map),6);
    EXPECT_EQ(hash_map_capacity(map), 2 * HASH_MAP_INIT_SIZE);

    hash_map_dtor(map);
}

TEST_F(HashTable, hash_map_get){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    int value;
    const char* key = "key";

    hash_map_state_code_t does_value_exist = hash_map_get(map,"random",&value);
    EXPECT_EQ(does_value_exist, KEY_ERROR);

    hash_map_put(map,key,1);
    does_value_exist = hash_map_get(map,key,&value);
    EXPECT_EQ(does_value_exist, OK);
    EXPECT_EQ(value,1);

    does_value_exist = hash_map_get(map,"random",&value);
    EXPECT_EQ(does_value_exist, KEY_ERROR);
    EXPECT_EQ(value,1);

    EXPECT_EQ(hash_map_size(map),1);
    EXPECT_EQ(hash_map_capacity(map), HASH_MAP_INIT_SIZE);

    hash_map_dtor(map);
}

TEST_F(HashTable, hash_map_pop){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    int value;
    const char* key = "key";

    // POP prazdne tabulky
    hash_map_state_code_t does_value_exist = hash_map_pop(map,key,&value);
    EXPECT_EQ(does_value_exist, KEY_ERROR);

    // POP jednoho prvku
    hash_map_put(map,key,1);
    does_value_exist = hash_map_pop(map,key,&value);
    EXPECT_EQ(does_value_exist, OK);
    EXPECT_EQ(value,1);
    EXPECT_FALSE(hash_map_contains(map,key));

    // POP neexistujiciho prvku
    int size = hash_map_size(map);
    does_value_exist = hash_map_pop(map,key,&value);
    EXPECT_EQ(does_value_exist, KEY_ERROR);
    EXPECT_EQ(value,1);
    EXPECT_EQ(hash_map_size(map), size);
    EXPECT_FALSE(hash_map_contains(map,key));

    // POP s vice prvky v tabulce
    hash_map_put(map,key,1);
    hash_map_put(map,"random",2);
    hash_map_pop(map,"random",&value);

    EXPECT_TRUE(hash_map_contains(map,key));
    EXPECT_FALSE(hash_map_contains(map,"random"));
    
    // map->size != 0, hash_map_pop je spatne naimplementovana
    EXPECT_EQ(hash_map_size(map), 0);
    EXPECT_EQ(hash_map_capacity(map),HASH_MAP_INIT_SIZE);

    hash_map_dtor(map);
}

TEST_F(HashTable, hash_map_remove){
    map = hash_map_ctor();
    ASSERT_TRUE(map != NULL) << "Chyba alokace";
    const char* key = "key";

    // REMOVE prazdne tabulky
    hash_map_state_code_t does_value_exist = hash_map_remove(map,key);
    EXPECT_EQ(does_value_exist, KEY_ERROR);

    // REMOVE jednoho prvku
    hash_map_put(map,key,1);
    does_value_exist = hash_map_remove(map,key);
    EXPECT_EQ(does_value_exist, OK);
    EXPECT_FALSE(hash_map_contains(map,key));

    // REMOVE neexistujiciho prvku
    int size = hash_map_size(map);
    does_value_exist = hash_map_remove(map,key);
    EXPECT_EQ(does_value_exist, KEY_ERROR);
    EXPECT_EQ(hash_map_size(map), size);
    EXPECT_FALSE(hash_map_contains(map,key));

    // REMOVE s vice prvky v tabulce
    hash_map_put(map,key,1);
    hash_map_put(map,"random",2);
    hash_map_remove(map,"random");

    EXPECT_TRUE(hash_map_contains(map,key));
    EXPECT_FALSE(hash_map_contains(map,"random"));
    
    // map->size != 0, hash_map_remove je spatne naimplementovana
    EXPECT_EQ(hash_map_size(map), 0);
    EXPECT_EQ(hash_map_capacity(map),HASH_MAP_INIT_SIZE);

    hash_map_dtor(map);
}

/*** Konec souboru white_box_tests.cpp ***/
