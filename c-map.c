#include "./include/c-map.h"

t_int64 cmap_hash(const t_char* key)
{
	t_int64 hash = 0;

	while(*key)
	{
		hash += ((hash * 31) + hash) + *key++;
	}

	return hash;
}

void cmap_find_item_index_by_hash(const t_cmap* self, t_int64 hash, t_int64* start)
{
	t_int64 probe_count = 0;
	t_int64 index       = *start;
	
	while(probe_count < self->capacity) 
	{
		if(index == self->capacity)
		{
			index = 0;
		}
		
		if(self->items[index].hash == hash) 
		{
			*start = index;
			break;
		}
		
		probe_count++;
		index++;
	}
	
	if(probe_count == self->capacity)
	{
		*start = -1;
	}
}

void cmap_find_free_index(const t_cmap_item* items, t_int64 size, t_int64* start)
{
	t_int64 probe_count = 0;
	t_int64 index       = *start;

	while(probe_count < size)
	{
		if(index >= size)
		{
			index = 0;
		}
		
		if(!items[index].hash)
		{
			*start = index;
			break;
		}

		probe_count++;
		index++;
	}

	if(probe_count == size)
	{
		*start = -1;
	}
}

t_cmap_exec_code cmap_dinit(t_cmap* self, t_int64 capacity, t_int8 is_resizable)
{
	CMAP_FAIL(self == NULL,                 CMAP_ERR_NULL_PTR);
	CMAP_FAIL(capacity > CMAP_MAX_CAPACITY, CMAP_ERR_INCORRECT_MAP_CAPACITY);

	self->items = (t_cmap_item*)malloc(capacity * sizeof(t_cmap_item));
	CMAP_FAIL(self->items == NULL, CMAP_ERR_MEM_ALLOCATION);

	memset(self->items, 0, capacity * sizeof(t_cmap_item));

	self->is_resizable = is_resizable;
	self->mem_loc= ON_THE_HEAP;
	self->size = 0;
	self->capacity = capacity;
	
	return CMAP_SUCCESS;
}

t_cmap_exec_code cmap_sinit(t_cmap* self, t_cmap_item* buff, t_int64 capacity, t_int8 is_resizable)
{
	CMAP_FAIL(self == NULL || buff == NULL,     CMAP_ERR_NULL_PTR);
	CMAP_FAIL(capacity > CMAP_MAX_CAPACITY, CMAP_ERR_INCORRECT_MAP_CAPACITY);

	self->size         = 0;
	self->items        = buff;
	self->mem_loc      = ON_THE_STACK;
	self->capacity     = capacity;
	self->is_resizable = is_resizable;
		
	return CMAP_SUCCESS;
}

t_bool cmap_has(const t_cmap* self, const t_char* key)
{
	t_int64 index = 0;
	t_int64 hash = 0;

	hash = cmap_hash(key);
	index = hash % self->capacity;

	if(!self->items[index].hash)
	{
		return false;
	}

	if(self->items[index].hash == hash)
	{
		return true;
	}

	if(self->items[index].hash != hash)
	{
		cmap_find_item_index_by_hash(self, hash, &index);

		return !(index == -1);
	}

	return false;
}

t_cmap_exec_code cmap_set(t_cmap* self, const t_char* key, t_any value)
{
	CMAP_FAIL(self == NULL,                                              CMAP_ERR_NULL_PTR);
	CMAP_FAIL(*key == '\0',                                              CMAP_ERR_INCORRECT_KEY);
	CMAP_FAIL(self->capacity == 0,                                     CMAP_ERR_INCORRECT_MAP_CAPACITY);
	CMAP_FAIL(self->size == self->capacity && !self->is_resizable, CMAP_ERR_MAP_IS_FULL_AND_NOT_RESIZABLE);

	t_int64 hash  = 0;
	t_int64 index = 0;

	hash = cmap_hash(key);
	index = hash % self->capacity;

	
	if(self->items[index].hash)
	{
		// The collision is occur, first we need to check if is no element with the same
		// hash in the map.
		cmap_find_item_index_by_hash(self, hash, &index);
		
		if(index == -1)
		{
			// The element with the same hash doesn't exist in the map, find free index for new element
			// and save data.
			index = hash % self->size;
			cmap_find_free_index(self->items, self->size, &index);

			if(index == -1)
			{
				return CMAP_ERR_FREE_INDEX_NOT_FOUND;
			}
			
			self->items[index].hash = hash;
			self->items[index].key = (t_char*)key;
			self->items[index].value = value;
			
			self->size++;
		}
		else 
		{
			// The element with the same hash was found, override the old value with new value.
			self->items[index].value = value;
		}
	} 
	else
	{
		// There is no element with the same hash.
		self->items[index].hash  = hash;
		self->items[index].key   = (t_char*)key;
		self->items[index].value = value;
		
		self->size++;
	}
	
	if(SHOULD_MAP_GROWTH(self))
	{
		CMAP_SAFE_CALL(cmap_resize(self, MAP_GROWTH));
	}

	return CMAP_SUCCESS;
}

t_cmap_exec_code cmap_resize(t_cmap* self, t_uint8 direction)
{	
	CMAP_FAIL(self == NULL, CMAP_ERR_NULL_PTR);

	t_int64 old_capacity  = self->capacity;
	t_int64 new_hash = 0;
	t_int64 new_index = 0;
	t_cmap_item* new_items = NULL;

	self->capacity = direction == MAP_GROWTH ? 
		self->capacity * 2 : 
		self->capacity / 2;
	new_items = (t_cmap_item*)malloc(self->capacity * sizeof(t_cmap_item));
	CMAP_FAIL(new_items == NULL, CMAP_ERR_NULL_PTR);

	memset(new_items, 0, self->capacity * sizeof(t_cmap_item));

	for(t_int64 index = 0; index < old_capacity; index++)
	{
		if(self->items[index].hash)
		{
			new_hash = cmap_hash(self->items[index].key);
			new_index = new_hash % self->size;
			
			if(new_items[new_index].hash)
			{
				cmap_find_free_index(new_items, self->size, &new_index);

				if(new_index == -1)
				{
					return CMAP_ERR_FREE_INDEX_NOT_FOUND;;
				}
				else
				{
					CMAP_MOVE_ITEM(new_items[new_index], self->items[index], new_hash);
				}
			}
			else
			{
				CMAP_MOVE_ITEM(new_items[new_index], self->items[index], new_hash);
			}
		}
	}
	
	if(self->mem_loc == ON_THE_HEAP)
	{
		free(self->items);
		self->items = NULL;
	}
	
	self->items = new_items;

	return CMAP_SUCCESS;
}

t_cmap_exec_code cmap_get(const t_cmap* self, t_cmap_item** item, const t_char* key)
{
	CMAP_FAIL(self == NULL || item == NULL || key == NULL, CMAP_ERR_NULL_PTR);
	CMAP_FAIL(*key == '\0',                                CMAP_ERR_INCORRECT_KEY);
	
	t_int64 hash  = 0;
	t_int64 index = 0;
	
	hash = cmap_hash(key);
	index = hash % self->capacity;

	if(self->items[index].hash == hash)
	{
		*item = &self->items[index];
	}
	else
	{
		cmap_find_item_index_by_hash(self, hash, &index);

		if(index == -1) 
		{
			return CMAP_ERR_ITEM_NOT_FOUND;
		}
		else 
		{
			*item = &self->items[index];
		}
	}

	return CMAP_SUCCESS;
}

t_cmap_exec_code cmap_delete(t_cmap* self, const t_char* key)
{
	CMAP_FAIL(self == NULL || key == NULL, CMAP_ERR_NULL_PTR);
	CMAP_FAIL(*key == '\0',                CMAP_ERR_INCORRECT_KEY);

	t_int64 hash  = 0;
	t_int64 index = 0;

	hash = cmap_hash(key);
	index = hash % self->size;

	if(self->items[index].hash == hash)
	{
		self->items[index].hash  = 0;
		self->items[index].key   = NULL;
		self->items[index].value = NULL;
			
		self->size--;
	}
	else
	{
		cmap_find_item_index_by_hash(self, hash, &index);

		if(index == -1) 
		{
			return CMAP_ERR_ITEM_NOT_FOUND;
		}
		else 
		{
			self->items[index].hash = 0;
			self->items[index].key   = NULL;
			self->items[index].value = NULL;

			self->size--;
		}
	}

	if(SHOULD_MAP_SHRINK(self))
	{
		CMAP_SAFE_CALL(cmap_resize(self, MAP_SRINK));
	}

	return CMAP_SUCCESS;
}

t_cmap_exec_code cmap_clear(t_cmap* self)
{
	CMAP_FAIL(self == NULL, CMAP_ERR_NULL_PTR);

	self->capacity = 0;
	self->is_resizable = 0;
	self->mem_loc = 0;
	self->size = 0;
	
	free(self->items);

	self->items = NULL;

	return CMAP_SUCCESS;
}