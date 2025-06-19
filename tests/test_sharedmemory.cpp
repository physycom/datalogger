#include "shared_memory.hpp"
#include "data.hpp"
#include <cassert>

int main(){
#if defined(USE_HOST_MEMORY)
    const char* name = "testmem";
    Data* ptr = static_cast<Data*>(allocate_host_memory(name, sizeof(Data)));
    ptr->d[0] = 42;
    Data* ptr2 = static_cast<Data*>(get_host_allocated_memory(name));
    assert(ptr2->d[0] == 42);
    ptr2->d[0] = 43;
    assert(ptr->d[0] == 43);
    remove_host_memory(name);
#endif
    return 0;
}
