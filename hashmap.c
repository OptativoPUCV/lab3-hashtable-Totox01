#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
  long index = hash(key, map->capacity);
  
  while(map->buckets[index] != NULL && map->buckets[index]->key != NULL){
    index = (index + 1) % map->capacity; //metodo lineal de resolucion de colisiones
  }
  
  Pair * newPair = createPair(key, value);
  if (newPair == NULL){
    //impresion de error porque createPair no la tiene
    printf("Error al asignar memoria a un nuevo par");
    return;
  }
  
  map->buckets[index] = newPair;
  map->size++;
  map->current = index;
}

void enlarge(HashMap * map) {
  enlarge_called = 1; //no borrar (testing purposes)
  if (map == NULL){
    return;
  }
  
  Pair ** old_buckets = map->buckets;
  map->capacity *= 2;
  map->buckets = (Pair **)malloc(sizeof(Pair *) * map->capacity);
  if (map->buckets == NULL){
    return;
  }

  map->size = 0;
  for (long i = 0; i < map->capacity; i++){
    map->buckets[i] = NULL;
  }
  for (long i = 0; i < map->capacity / 2; i++){
    if (old_buckets[i] != NULL && old_buckets[i]->key != NULL){
      insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
    }
  }
  free(old_buckets);
}


HashMap * createMap(long capacity) {
  HashMap * map = (HashMap *)malloc(sizeof(HashMap));
  if (map == NULL){
    printf("Error al asignar memoria al nuevo mapa");
    return NULL;
  }
  
  map->buckets = (Pair **)malloc(sizeof(Pair *) * capacity);
  if (map->buckets == NULL){
    printf("Error al asignar memoria al arreglo de buckets");
    free(map);
    return NULL;
  }
  
  map->size = 0;
  map->capacity = capacity;
  map->current = -1;
  
  for (int i = 0; i < capacity; i++){
    map->buckets[i] = NULL;
  }
  
  return map;
}

void eraseMap(HashMap * map,  char * key) {
  if (key == NULL || map == NULL){
    return;
  }
  Pair* aux = searchMap(map,  key);
  if(aux == NULL) return;
  aux->key = NULL;
  map->size--;
}

Pair * searchMap(HashMap * map,  char * key) {   
  long index = hash(key, map->capacity);
  long initialIndex = index;
  while (map->buckets[index] != NULL){
    if (map->buckets[index]->key != NULL && strcmp(map->buckets[index]->key, key) == 0){
      map->current = index;
      return map->buckets[index];
    }
    index = (index + 1) % map->capacity;
    if (index == initialIndex){
      break;
    }
  }
  map->current = -1;
  return NULL;
}

Pair * firstMap(HashMap * map) {
  if (map == NULL || map->size == 0){
    return NULL;
  }

  for (long i = 0; i < map->capacity; i++){
    if (map->buckets[i] != NULL && map->buckets[i]->key != NULL){
      map->current = i;
      return map->buckets[i];
    }
  }
  map->current = -1;
  return NULL;
}

Pair * nextMap(HashMap * map) {
  if (map == NULL || map->size == 0 || map->current == -1){
    return NULL;
  }

  for (long i = map->current + 1; i < map->capacity; i++){
    if (map->buckets[i] != NULL && map->buckets[i]->key != NULL){
      map->current = i;
      return map->buckets[i];
    }
  }
  map->current = -1;
  return NULL;
}
