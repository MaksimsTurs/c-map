#include "include/c_map.h"

static inline __attribute__((always_inline)) 
t_exec_code 
cmap_gen_hash(t_uint32* hash, const t_char* key) 
{
	CMAP_FAIL_IF(hash == NULL || key == NULL, CMAP_ERROR_INVALID_PTR);
	// Local hash with initialized magic num.
	t_uint32      l_hash  = 0x811c9dc5;
	t_uint8       ch      = 0;
	const t_char* key_tmp = key;

	while((ch = *key++) && (key - key_tmp) < 4) 
	{
		l_hash = (l_hash << 5) + ch;
	}

	*hash = l_hash;

	return CMAP_SUCCESS_EXECUTED;
}


static inline __attribute__((always_inline)) 
t_exec_code 
cmap_find_free_index(t_uint32* item_index, const s_cmap_item** items, t_uint32 size) 
{
	CMAP_FAIL_IF(item_index == NULL || items == NULL, CMAP_ERROR_INVALID_PTR);
	CMAP_FAIL_IF(size == 0                          , CMAP_ERROR_INVALID_MAP_SIZE);

	t_uint32 l_item_index = *item_index;
	t_uint32 index        = 0;

	if(size <= CMAP_SMALL_SIZE) 
	{
		// Linear probe.
		while(items[l_item_index] != NULL) 
		{
			l_item_index++;
			
			if(l_item_index == size)
				l_item_index = 0;
		}	
	} 
	else 
	{
		// Quadratic probe.
		while(items[l_item_index] != NULL) 
		{			
			l_item_index = (l_item_index + (index * index)) % size;
			l_item_index++;

			if(l_item_index == size) 
				l_item_index = 0;
		}	
	}

	*item_index = l_item_index;

	return CMAP_SUCCESS_EXECUTED;
}

static inline __attribute__((always_inline)) 
t_exec_code cmap_find_index_by_hash(t_uint32* item_index, t_uint32 hash, const s_cmap_item** items, t_uint32 size) 
{
	CMAP_FAIL_IF(item_index == NULL || items == NULL, CMAP_ERROR_INVALID_PTR);
	// Used for finding and saving index for item.
	t_uint32 l_item_index = *item_index;
	// Used for probe algorithm.
	t_uint32 index        = 0;
	t_uint32 probe_count  = 0;

	// In worst case, if we have two different keys with two identical hashes,
	// this loop will go through the entire.
	if(size <= CMAP_SMALL_SIZE) 
	{
		while(probe_count < size) 
		{
			if(items[l_item_index] != NULL && (items[l_item_index]->m_hash == hash)) 
			{
				*item_index = l_item_index;
				return CMAP_SUCCESS_ITEM_FOUND;
			}
	
			l_item_index++;
			probe_count++;

			if(l_item_index == size) 
				l_item_index = 0;
		}
	} 
	else 
	{
		while(probe_count < size) 
		{
			if((items[l_item_index] != NULL) && (items[l_item_index]->m_hash == hash)) 
			{
				*item_index = l_item_index;
				return CMAP_SUCCESS_ITEM_FOUND;
			}
			
			l_item_index = (l_item_index + (index * index)) % size;
			index++;
			probe_count++;

			if(l_item_index == size) 
				l_item_index = 0;
		}
	}

	return CMAP_ERROR_ITEM_NOT_FOUND;
}

static inline __attribute__((always_inline))
t_exec_code cmap_resize(s_cmap_map* this, t_uint8 direction) 
{
	CMAP_FAIL_IF(this == NULL, CMAP_ERROR_INVALID_PTR);

	t_uint32      old_size   = this->m_size;
	t_uint32      index      = 0;
	t_uint32      item_index = 0;

	if(direction == CMAP_KEY_GROWTH_SIZE) 
	{
		this->m_size  = CMAP_GET_PRIME_FROM(this->m_size);
		this->m_items = (s_cmap_item**)realloc(this->m_items, this->m_size * sizeof(s_cmap_item*));
		CMAP_FAIL_IF(this->m_items == NULL, CMAP_ERROR_MEMORY_ALLOCATION);
		for(index = old_size; index < this->m_size; index++)
			this->m_items[index] = NULL;	

		for(index = 0; index < old_size; index++) 
		{
			if(this->m_items[index] != NULL) {
				item_index = this->m_items[index]->m_hash % this->m_size;			
					
				if((this->m_items[item_index] != NULL && this->m_items[index] != NULL) && 
					 (this->m_items[index]->m_hash != this->m_items[item_index]->m_hash))
					CMAP_SAFE_CALL(cmap_find_free_index(&item_index, (const s_cmap_item**)this->m_items, this->m_size));
				else if((this->m_items[item_index] != NULL && this->m_items[index] != NULL) &&
								(this->m_items[index]->m_hash == this->m_items[item_index]->m_hash))
					continue;
		
				this->m_items[item_index] = this->m_items[index];
				this->m_items[index]      = NULL;
			}
		}		
	}	
	else if(direction == CMAP_KEY_SHRINK_SIZE) 
	{
		this->m_size  = CMAP_GET_PRIME_FROM(this->m_occupied);

		for(index = 0; index < old_size; index++) 
		{
			if(this->m_items[index] != NULL) {
				item_index = this->m_items[index]->m_hash % this->m_size;			
					
				if((this->m_items[item_index] != NULL && this->m_items[index] != NULL) && 
					 (this->m_items[index]->m_hash != this->m_items[item_index]->m_hash))
					CMAP_SAFE_CALL(cmap_find_free_index(&item_index, (const s_cmap_item**)this->m_items, this->m_size));
				else if((this->m_items[item_index] != NULL && this->m_items[index] != NULL) &&
								(this->m_items[index]->m_hash == this->m_items[item_index]->m_hash))
					continue;
		
				this->m_items[item_index] = this->m_items[index];
				this->m_items[index]      = NULL;
			}
		}	

		this->m_items = (s_cmap_item**)realloc(this->m_items, this->m_size * sizeof(s_cmap_item*));
		CMAP_FAIL_IF(this->m_items == NULL, CMAP_ERROR_MEMORY_ALLOCATION);
	}	else
		return CMAP_ERROR_INVALID_RESIZE_DIRECTION;

	return CMAP_SUCCESS_EXECUTED;
}

t_exec_code cmap_init(s_cmap_map* this, t_uint32 size) 
{
	CMAP_FAIL_IF(this == NULL, CMAP_ERROR_INVALID_PTR);
	CMAP_FAIL_IF(size == 0   , CMAP_ERROR_INVALID_MAP_SIZE);

	this->m_size           = size;
	this->m_occupied       = 0;
	this->m_items          = (s_cmap_item**)calloc(size, sizeof(s_cmap_item));
	CMAP_FAIL_IF(this->m_items == NULL, CMAP_ERROR_MEMORY_ALLOCATION);

	return CMAP_SUCCESS_EXECUTED;
}

t_exec_code cmap_set(s_cmap_map* this, const t_char* key, const t_any value) 
{
	CMAP_FAIL_IF(this == NULL || key == NULL                                  , CMAP_ERROR_INVALID_PTR);
	CMAP_FAIL_IF(this->m_size > CMAP_MAX_SIZE                                 , CMAP_ERROR_OVERFLOW);

	s_cmap_item* item       = NULL;
	t_exec_code  exec_code  = 0;
	t_uint32     hash       = 0;
	t_uint32     item_index = 0;
	
	CMAP_SAFE_CALL(cmap_gen_hash(&hash, key));
	item_index = hash % this->m_size;
	
	// Collision was found.
	if(this->m_items[item_index] != NULL) 
	{
		// Check if there is an element with the same key.
		exec_code = cmap_find_index_by_hash(&item_index, hash, (const s_cmap_item**)this->m_items, this->m_size);
		if(exec_code == CMAP_SUCCESS_ITEM_FOUND) 
		{
			// BEST CASE!
			// Equal hashes was generated because of equivalent keys, copy pointer to the new value.
			this->m_items[item_index]->m_value = value;
			return CMAP_SUCCESS_EXECUTED;
		} 
		else 
		{
			// WORST CASE!
			// Equal hashes was generated because of not much place in map or bad hash functon, 
			// find next free hash for new element.
			CMAP_SAFE_CALL(cmap_find_free_index(&item_index, (const s_cmap_item**)this->m_items, this->m_size));	
		}
	}

	item = (s_cmap_item*)malloc(sizeof(s_cmap_item));
	CMAP_FAIL_IF(item == NULL, CMAP_ERROR_MEMORY_ALLOCATION);
	
	item->m_key   = key;
	item->m_value = value;
	item->m_hash  = hash;
	
	this->m_items[item_index] = item;
	this->m_occupied++;

	if(CMAP_IS_MAP_TO_SMALL(this->m_occupied, this->m_size))
		CMAP_SAFE_CALL(cmap_resize(this, CMAP_KEY_GROWTH_SIZE));
	
	return CMAP_SUCCESS_EXECUTED;
}

t_exec_code cmap_get(s_cmap_map this, s_cmap_item** item, t_char* key) 
{
	CMAP_FAIL_IF(item == NULL || key == NULL, CMAP_ERROR_INVALID_PTR);

	t_uint32    hash       = 0;
	t_uint32    item_index = 0;
	t_exec_code exec_code  = 0;
	
	exec_code = cmap_gen_hash(&hash, key);
	if(exec_code != CMAP_SUCCESS_EXECUTED) 
	{
		*item = NULL;
		return exec_code;
	}

	item_index = hash % this.m_size;
	exec_code = cmap_find_index_by_hash(&item_index, hash, (const s_cmap_item**)this.m_items, this.m_size);
	if(exec_code == CMAP_ERROR_ITEM_NOT_FOUND) 
	{
		*item = NULL;
		return exec_code;
	}
	
	*item = this.m_items[item_index];

	return *item == NULL ? CMAP_ERROR_ITEM_NOT_FOUND : CMAP_SUCCESS_ITEM_FOUND;
}

t_exec_code cmap_clear(s_cmap_map* this) 
{
	for(t_uint32 index = 0; index < this->m_size; index++) 
	{
		free(this->m_items[index]);
		this->m_items[index] = NULL;
	}

	free(this->m_items);

	this->m_items    = NULL;
	this->m_size     = 0;
	this->m_occupied = 0;

	return CMAP_SUCCESS_EXECUTED;
}

t_exec_code cmap_remove(s_cmap_map* this, const t_char* key) 
{
	CMAP_FAIL_IF(this == NULL || key == NULL, CMAP_ERROR_INVALID_PTR);

	t_uint32 hash       = 0;
	t_uint32 item_index = 0;

	CMAP_SAFE_CALL(cmap_gen_hash(&hash, key));
	item_index = hash % this->m_size;
			
	if(cmap_find_index_by_hash(&item_index, hash, (const s_cmap_item**)this->m_items, this->m_size) == CMAP_ERROR_ITEM_NOT_FOUND)
		return CMAP_ERROR_ITEM_NOT_FOUND;		
	
	free(this->m_items[item_index]);
	this->m_items[item_index] = NULL;
	this->m_occupied--;

	if(CMAP_IS_MAP_TO_BIG(this->m_occupied, this->m_size))
		CMAP_SAFE_CALL(cmap_resize(this, CMAP_KEY_SHRINK_SIZE));

	return CMAP_SUCCESS_EXECUTED;
}