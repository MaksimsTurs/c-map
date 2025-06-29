#ifndef H_CMAP
#define H_CMAP

//=============================Includes=============================//

#include <stdlib.h>
#include <stdio.h>

#include "c_types.h"

//=============================Constants=============================//

// Restriction constants.

#define CMAP_MAX_SIZE                            (t_uint32)CTYPE_UINT32_MAX

// Error and success constants.

#define CMAP_ERROR_MEMORY_ALLOCATION             (t_exec_code)-1
#define CMAP_ERROR_OVERFLOW                      (t_exec_code)-2
#define CMAP_ERROR_ITEM_NOT_FOUND                (t_exec_code)-3
#define CMAP_ERROR_INVALID_RESIZE_DIRECTION      (t_exec_code)-4
#define CMAP_ERROR_INVALID_MAP_SIZE              (t_exec_code)-5
#define CMAP_ERROR_INVALID_PTR                   (t_exec_code)-6
#define CMAP_SUCCESS_EXECUTED                    (t_exec_code)1
#define CMAP_SUCCESS_ITEM_FOUND                  (t_exec_code)2

// Map resize constants.

#define CMAP_KEY_GROWTH_SIZE                     (t_uint8)1
#define CMAP_KEY_SHRINK_SIZE                     (t_uint8)2

// Map factors and size constants.

#define CMAP_SMALL_SIZE                          (t_uint32)2000
#define CMAP_SMALL_GROWTH_AT                     (t_float32)0.7f
#define CMAP_SMALL_SHRINK_AT                     (t_float32)0.4f
#define CMAP_MEDIUM_SIZE                         (t_uint32)2147483647
#define CMAP_MEDIUM_GROWTH_AT                    (t_float32)0.8f
#define CMAP_MEDIUM_SHRINK_AT                    (t_float32)0.5f
#define CMAP_BIG_SIZE                            (t_uint32)CTYPE_UINT32_MAX
#define CMAP_BIG_GROWTH_AT                       (t_float32)0.9f
#define CMAP_BIG_SHRINK_AT                       (t_float32)0.5f

//=============================Macros=============================//

#define CMAP_FAIL_IF(cond, err_code) \
do {                                 \
	if(cond) return err_code;          \
} while(0)
#define CMAP_SAFE_CALL(x)                \
do {                                     \
	t_exec_code exec_code = (x);           \
	if(exec_code != CMAP_SUCCESS_EXECUTED) \
		return exec_code;                    \
} while(0)
#define CMAP_GET_PRIME_FROM(x)               (x * 2) >= CMAP_MAX_SIZE ? CMAP_MAX_SIZE : (x * 2)
#define CMAP_IS_MAP_TO_BIG(occupied, size)   (((t_float32)occupied) / size) <= (CMAP_GET_SHRINK_FACTOR(size))
#define CMAP_IS_MAP_TO_SMALL(occupied, size) (((t_float32)occupied) / size) >= (CMAP_GET_GROWTH_FACTOR(size))
#define CMAP_GET_SHRINK_FACTOR(size)         size >= CMAP_BIG_SIZE    ? CMAP_BIG_SHRINK_AT    :                      \
																		         size >= CMAP_MEDIUM_SIZE ? CMAP_MEDIUM_SHRINK_AT :                      \
																		         size <= CMAP_SMALL_SIZE  ? CMAP_SMALL_SHRINK_AT  : CMAP_SMALL_SHRINK_AT
#define CMAP_GET_GROWTH_FACTOR(size)         size >= CMAP_BIG_SIZE    ? CMAP_BIG_GROWTH_AT    :                      \
																		         size >= CMAP_MEDIUM_SIZE ? CMAP_MEDIUM_GROWTH_AT :                      \
																		         size <= CMAP_SMALL_SIZE  ? CMAP_SMALL_GROWTH_AT  : CMAP_SMALL_GROWTH_AT

//=============================Main API=============================//

typedef struct {
	const t_char*  m_key;
	t_any          m_value;
	// Used for comparing.
	t_uint32       m_hash;
} s_cmap_item;

typedef struct {
	t_uint32      m_size;
	t_uint32      m_occupied;
	s_cmap_item** m_items;
} s_cmap_map;

t_exec_code cmap_init(s_cmap_map* this, t_uint32 size);
t_exec_code cmap_set(s_cmap_map* this, const t_char* key, const t_any value);
t_exec_code cmap_get(s_cmap_map this, s_cmap_item** item, t_char* key);
t_exec_code cmap_clear(s_cmap_map* this);
t_exec_code cmap_remove(s_cmap_map* this, const t_char* key);

//=============================Helper functions=============================//

static inline __attribute__((always_inline))
t_exec_code cmap_gen_hash(t_uint32* hash, const t_char* key);
static inline __attribute__((always_inline))
t_exec_code cmap_find_free_index(t_uint32* item_index, const s_cmap_item** items, t_uint32 size);
static inline __attribute__((always_inline))
t_exec_code cmap_find_index_by_hash(t_uint32* item_index, t_uint32 hash, const s_cmap_item** items, t_uint32 size);
static inline __attribute__((always_inline))
t_exec_code cmap_resize(s_cmap_map* this, t_uint8 direction);

#endif