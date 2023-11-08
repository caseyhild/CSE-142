
#include <stdlib.h>
#include<set>
#include<stack>
#include<iostream>
#include<cstring>
#include"ChunkAlloc.hpp"

template<
    class T,          // This is the type we are allocating.  You can assume this is less than or equal to 4kB
    size_t ALIGNMENT  // The alignment at which we much allocate the objects.  You can assume this is less than or equal to 4kB
    >
class AlignedAllocator {
    std::set<T*> chunks; // We store everything we allocated so we can clean up in the destructor.
    std::stack<void*> available;
public:
    typedef T ItemType; // This will make T available as AlignedAllocator::ItemType
    static const size_t Alignment = ALIGNMENT;  // Likewise, we can access the alignment as AlignedAllocator::Alignment
    const uint iterations = sizeof(T) / ALIGNMENT + 1;
    const uint blocksize = iterations * ALIGNMENT;

    AlignedAllocator() {}

    T * alloc() {
        void* p = NULL;
        T* c = NULL;
        if(available.empty()) {
            p = alloc_chunk();
            new (p) T;
            c = reinterpret_cast<T*>(p);
            new (c) T;
            chunks.insert(c);
            uintptr_t n = reinterpret_cast<uintptr_t>(p);
            if(ALIGNMENT < sizeof(T)) {
                for(uint i = 1; i < CHUNK_SIZE / blocksize; i++) {
                    for(uint j = 0; j < iterations; j++) {
                        n += ALIGNMENT;
                    }
                    available.push(reinterpret_cast<void*>(n));
                }
            }
            else{
                for(uint i = 1; i < CHUNK_SIZE / ALIGNMENT; i++) {
                    n += ALIGNMENT;
                    available.push(reinterpret_cast<void*>(n));
                }
            }
        }
        else {
            p = available.top();
            new (p) T;
            available.pop();
            c = reinterpret_cast<T*>(p);
            new (c) T;
        }
        return c;
    }
    
    void free(T * p) {
        memset(p, 0, sizeof(T));
        available.push(reinterpret_cast<void*>(p));
    }

    ~AlignedAllocator() {
        for(auto & p: chunks) {
            free_chunk(reinterpret_cast<void*>(p));
        }
    }
};

template<class T, size_t ALIGNMENT>
const size_t AlignedAllocator<T, ALIGNMENT>::Alignment;

// Cfiddle-signature=c4f58c47bf71ce34db9234fdee0454b7