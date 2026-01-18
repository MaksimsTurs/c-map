#ifndef H_CMAP
#define H_CMAP

#include "c-types.h"

#include <stdlib.h>
#include <string.h>

typedef enum e_cmap_exec_code {
	CMAP_SUCCESS,

	CMAP_ERR_INCORRECT_MAP_CAPACITY,
	CMAP_ERR_INCORRECT_KEY,
	CMAP_ERR_ITEM_NOT_FOUND,
	CMAP_ERR_FREE_INDEX_NOT_FOUND  ,
	CMAP_ERR_NULL_PTR,
	CMAP_ERR_MEM_ALLOCATION,
	CMAP_ERR_MAP_IS_FULL_AND_NOT_RESIZABLE,
} t_cmap_exec_code;

typedef enum e_cmap_mem_loc {
	ON_THE_STACK,
	ON_THE_HEAP
} t_cmap_mem_loc;

typedef enum e_cmap_resize_dir {
	MAP_GROWTH,
	MAP_SRINK
} t_cmap_resize_dir;

typedef struct s_cmap_item {
	t_int64 hash;
	t_char* key;
	t_any   value;
} t_cmap_item;

typedef struct s_cmap {
	t_cmap_item* items;
	t_int64    	 size;
	t_int64    	 capacity;
	t_int8     	 is_resizable;
	t_int8     	 mem_loc;
} t_cmap;

#define CMAP_VERSION "v0.0.4"

#define CMAP_MAX_CAPACITY CTYPE_INT64_MAX - 10

#define CMAP_FAIL(cond, code) do { \
	if(cond) { \
		return code; \
	} \
} while(0); 
#define CMAP_MOVE_ITEM(new_item, old_item, new_hash) do { \
	new_item.key   = old_item.key; \
	new_item.value = old_item.value; \
	new_item.hash  = new_hash; \
} while(0);
#define CMAP_SAFE_CALL(expr_res) do { \
	t_int64 res = expr_res; \
	if(res < 0) { return res; } \
} while(0);

#define GET_GROWTH_FACTOR(size) (size >= 5000 ? 0.75f : size >= 2500 ? 0.85f : size >= 500 ? 0.90f : 0.75f)
#define GET_SHRINK_FACTOR(size) (size <= 5000 ? 0.35f : size <= 2500 ? 0.45f : size <= 500 ? 0.55f : 0.50f)
#define SHOULD_MAP_GROWTH(map)  map->is_resizable && (((t_float32)(map)->capacity)) / (map)->size >= GET_GROWTH_FACTOR((map)->size)
#define SHOULD_MAP_SHRINK(map)  map->is_resizable && (((t_float32)(map)->capacity)) / (map)->size <= GET_SHRINK_FACTOR((map)->size)

//###########################################
//################# Utility #################
//###########################################

t_int64          cmap_hash(const t_char* key);
t_cmap_exec_code cmap_resize(t_cmap* self, t_uint8 direction);
void             cmap_find_item_index_by_hash(const t_cmap* self, t_int64 hash, t_int64* start);
void             cmap_find_free_index(const t_cmap_item* items, t_int64 size, t_int64* start);


//############################################
//################### Core ###################
//############################################

// Returns a true if a element with the same key exist.
t_bool           cmap_has(const t_cmap* self, const t_char* key);
// Initialize a map with stack allocated buffer.
t_cmap_exec_code cmap_sinit(t_cmap* self, t_cmap_item* buff, t_int64 size, t_int8 should_expand);
// Initialize a map with heap allocated buffer.
t_cmap_exec_code cmap_dinit(t_cmap* self, t_int64 size, t_int8 should_expand);
// Set item with specific key.
t_cmap_exec_code cmap_set(t_cmap* self, const t_char* key, t_any value);
// Get item with specific key.
t_cmap_exec_code cmap_get(const t_cmap* self, t_cmap_item** item, const t_char* key);
// Delete item with specific key.
t_cmap_exec_code cmap_delete(t_cmap* self, const t_char* key);
// Clear the entier map.
t_cmap_exec_code cmap_clear(t_cmap* self);

#endif // H_CMAP