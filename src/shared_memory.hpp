#pragma once

#if defined(USE_HOST_MEMORY)
void remove_host_memory(const char*);
void* allocate_host_memory(const char*, size_t);
void* get_host_allocated_memory(const char*);
#endif
