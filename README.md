# C Map

C Map is a very simple Hash Table implemintation in C.

## [Table of Contents](#table-of-contents)
  + [Documentation](#documentation)
  + [Examples](#examples)
    + [Init Map](#init-map)
    + [Clear Map](#clear-map)
    + [Set Item](#set-item)
    + [Has Item](#has-item)
    + [Get Item](#get-item)
    + [Delete Item](#delete-item)
    + [Error Handling](#error-handling)

## [Documentation](#documentation)
Checks if the element exist in the map.
```c
t_bool cmap_has(const t_cmap* self, const t_char* key);
```
Initialize a stack based Map that work with the buffer that **must** be allocated on the stack, when the Map does not have more place and `is_resizable` parameters is `0` the `cmap_set` function will return an error code and do nothing. By passing a `1` the Map will switch from stack memory to heap memory and save all items on the heap. 
```c
t_cmap_exec_code cmap_sinit(t_cmap* self, t_cmap_item* buff, t_int64 size, t_int8 should_expand);
```
Initialize memory on the heap, when the Map does not have more place and `is_resizable` parameters was passed a `0` the `cmap_set` function will return a error code and do nothing. By passing a `1` the map will allocate more memory.
```c
t_cmap_exec_code cmap_dinit(t_cmap* self, t_int64 size, t_int8 should_expand);
```
Set a new value or overwrite the old when the existing key was passed.
```c
t_cmap_exec_code cmap_set(t_cmap* self, const t_char* key, t_any value);
```
Save a map item data in the `cmap_item` structure, return error code when item with the passed key does not exist.
```c
t_cmap_exec_code cmap_get(const t_cmap* self, t_cmap_item** item, const t_char* key);
```
Delete a item with specific key.
```c
t_cmap_exec_code cmap_delete(t_cmap* self, const t_char* key);
```
Delete all items in the map.
```c
t_cmap_exec_code cmap_clear(t_cmap* self);
```

## [Examples](#examples)
Some examples how the library can be used.
### [Init Map](#init-map)
```c
  // inlcude the header "c-map.h"
  #define MAP_SIZE 10

  int main(void)
  {
    t_cmap map = {0};
    t_cmap_item buffer[MAP_SIZE] = {0};

    // Init the memory on the heap (not resizable).
    cmap_dinit(&map, MAP_SIZE, 0);
    // Init the memory on the stack (not resizable).
    cmap_dinit(&map, buffer, MAP_SIZE, 0);
  }
```
### [Clear Map](#clear-map)
```c
  // ...includes and defines.
  int main(void)
  {
    // ...init map and other stuff.
    cmap_clear(&map);
  }
```
### [Set Item](#set-item)
```c
  // ...includes and defines.
  int main(void)
  {
    // ...init map and other stuff.
    cmap_set(&map, "Key", "Value");
  }
```
### [Has Item](#has-item)
```c
  // ...includes and defines.
  int main(void)
  {
    // ...init map and other stuff.
    if(cmap_has(&map, "Key"))
    {
      // ...do stuff
    }
  }
```
### [Get Item](#get-item)
```c
  // ...includes and defines.
  int main(void)
  {
    // ...init map and other stuff.
    t_cmap_item* item = NULL;

    cmap_get(&map, &item, "Key");
  }
```
### [Delete Item](#delete-item)
```c
  // ...includes and defines.
  int main(void)
  {
    // ...init map and other stuff.
    cmap_delete(&map, "Key");
  }
```
### [Error Handling](#error-handling)
```c
  // ...includes and defines.
  int main(void)
  {
    // ...init map and other stuff.
    t_cmap_exec_code exec_code = 0;

    if((exec_code = cmap_set(&map, "Key", "Value")) != CMAP_SUCCESS)
    {
      printf("%s", cmap_exec_code_to_string(exec_code));
      // ...do stuff
    }
  }

  const char* cmap_exec_code_to_string(t_cmap_exec_code exec_code)
  {
    switch(exec_code)
    {
      case CMAP_ERR_ITEM_NOT_FOUND:
        return "Item not found";
      // ...other exec codes.
    }
  }
```