#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#define CACHE_CONTROL_FLUSH_CACHES _IO('q', 1)

void enable_prefetcher(int flags) {

        int cpus = get_nprocs_conf();
        char buf[1024];
        for(int i = 0;i < cpus; i++) {
                //https://software.intel.com/en-us/articles/disclosure-of-hw-prefetcher-control-on-some-intel-processors
                sprintf(buf, "wrmsr -p %d 0x1a4 %d", i, ~flags & 15); // in the register, a 1 disables the register.  So invert and mask out the highorder bits.

                //std::cerr << buf << "\n";
                int r = system(buf);
                if (r != 0) {
                        std::cerr << "Couldn't set prefetcher flags or core " << i << " to " << flags << "\n";
                }
        }
}

 
void flush_caches() {
       if( access( "/dev/cache_control", R_OK|W_OK ) != 0) {
                std::cerr << "Couldn't open '/dev/cache_control'.  Not flushing caches.\n";
                return;
        }

        int fd = open("/dev/cache_control", O_RDWR);
        if (fd == -1) {
                std::cerr << "Couldn't open '/dev/cache_control' to flush caches: " << strerror(errno) << std::endl;
                return;
        }

        int r = ioctl(fd, CACHE_CONTROL_FLUSH_CACHES);
        if (r == -1) {
                std::cerr << "Flushing caches failed: " << strerror(errno) << std::endl;
                exit(1);
        }
}

#endif
