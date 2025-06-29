# C Map
Small and simple C library (3 Kb) for key - value map.
## Errors
Every function return `t_exec_code (int8)` execution code, negative numbers for errors and positive numbers for success codes.
## API
`t_exec_code cmap_init(s_cmap_map* this, t_uint32 size)` initialize a `size` of Map items on the heap.
```c
	s_cmap_map map = {0}

	cmap_init(&map, 10);
```
`t_exec_code cmap_set(s_cmap_map* this, const t_char* key, const t_any value)` When collision is detected we check with function `cmap_find_index_by_hash` is this collision was happen because whe have two equal hashes but unexist key or because whe have passed a key of existed item. Best case is that we have existed key, than whe simple save a new pointer to `m_value`, worst case is that key doesn't exist and we need to find a free space in map for new item. It map is to small it will be resized. Life time must be controlled by user!
```c
	s_cmap_map map = {0}
	
	cmap_init(&map, 10);
	// Alloc memory for new item and save him.
	cmap_set(&map, "Name", "X");
	// Set new `Name` value.
	cmap_set(&map, "Name", "XY");
```
`t_exec_code cmap_get(s_cmap_map this, s_cmap_item** item, t_char* key)` get a item from map by key, when item doesn't exist function save a `NULL`.
```c
	s_cmap_map map = {0}
	s_cmap_item* item = {0};
	
	cmap_init(&map, 10);
	cmap_set(&map, "Name", "X");

	cmap_get(map, &item, "Name"); // { m_value: "X" m_key: "Name" m_hash: xxxx }
	cmap_get(map, &item, "Age"); // NULL
```
`t_exec_code cmap_clear(s_cmap_map* this)` delete all items and reset the map.
```c
	s_cmap_map map = {0}
	t_uint8    age = 20;
	
	cmap_init(&map, 10);
	cmap_set(&map, "F. Name", "X");
	cmap_set(&map, "S. Name", "X");
	cmap_set(&map, "Age",     &age);

	/*
		Map
		m_size:     10
		m_occupied: 3
		m_items:    [
			{ m_key: "F. Name" m_value: "X" m_hash: xxxx }
			{ m_key: "S. Name" m_value: "X" m_hash: xxxx }
			{ m_key: "Age"     m_value: "X" m_hash: xxxx }
		]
	*/

	cmap_clear(&map);

	/*
		Map
		m_size:     0
		m_occupied: 0
		m_items:    []
	*/
```
`t_exec_code cmap_remove(s_cmap_map* this, const t_char* key)` Remove a single item by `key`, when item does exist do nothing. When Map is to have to few items it will be resized.
```c
	s_cmap_map map = {0}
	t_uint8    age = 20;
	
	cmap_init(&map, 10);
	cmap_set(&map, "F. Name", "X");
	cmap_set(&map, "S. Name", "X");
	cmap_set(&map, "Age",     &age);

	/*
		Map
		m_size:     10
		m_occupied: 3
		m_items:    [
			{ m_key: "F. Name" m_value: "X" m_hash: xxxx }
			{ m_key: "S. Name" m_value: "X" m_hash: xxxx }
			{ m_key: "Age"     m_value: "X" m_hash: xxxx }
		]
	*/

	cmap_remove(&map, "F. Name");

	/*
		Map
		m_size:     10
		m_occupied: 2
		m_items:    [
			{ m_key: "S. Name" m_value: "X" m_hash: xxxx }
			{ m_key: "Age"     m_value: "X" m_hash: xxxx }
		]
	*/
```