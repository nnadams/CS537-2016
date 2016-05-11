#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define error(M, ...) fprintf(stderr, M, ##__VA_ARGS__);
#define die(M, ...) { error(M, ##__VA_ARGS__); exit(1); }

#define NDIRECT 12
#define BSIZE 512

#define T_INVALID 0
#define T_DIR     1
#define T_FILE    2
#define T_DEV     3

typedef struct _superblock {
    uint size;
    uint nblocks;
    uint ninodes;
} superblock;

typedef struct _dinode {
    short type;
    short major;
    short minor;
    short nlink;
    uint size;
    uint addrs[NDIRECT+1];
} dinode;

typedef struct _dirent {
    ushort inum;
    char name[14];
} dirent;

// Inodes per block
#define IPB                 (BSIZE / sizeof(struct _dinode))
// Block containing inode i
#define IBLOCK(i)           ((i) / IPB + 2)
// Bitmap bits per block
#define BPB                 (BSIZE*8)
// Block containing bit for block b
#define BBLOCK(b, ninodes)  (b/BPB + (ninodes)/IPB + 3)

//#define DBLOCK(b, ninodes, nblocks)  (((nblocks)/BPB + (ninodes)/IPB + 3))

void *img;
superblock *sb;

static inline short isDataBlockValid(uint block) {
    uint *valid = img + (BBLOCK(block, sb->ninodes) * BSIZE);
    valid += block / 32;
    return ((*valid) >> (block % 32)) & 1;
}

int main(int argc, char **argv) {
    int i, j, rc, fd;
    short valid = 0;
    uint *addr;
    struct stat sbuf;

    if (argc != 2)
        die("bad args\n");

    fd = open(argv[1], 0);
    if (fd < 0)
        die("image not found\n");

    rc = fstat(fd, &sbuf);
    assert(rc == 0);

    img = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(img != MAP_FAILED);

    sb = (superblock *)(img + BSIZE);
    printf("%d %d %d\n", sb->size, sb->nblocks, sb->ninodes);

    dinode *dip = (dinode *)(img + 2*BSIZE);
    for (i = 0; i < sb->ninodes; i++) {
        if (i == 1 && dip->type != T_DIR)
            die("root directory does not exist\n");

        if (dip->type != T_INVALID) {
            if (dip->type > 3 || dip->type < 0)
                die("bad inode\n");

            printf("%d type: %d\n", i, dip->type);

            if (dip->type == T_DEV) continue;

            for (j = 0; j < NDIRECT; j++) {
                if (dip->addrs[j] >= sb->size)
                    die("bad address in inode\n");

                valid = isDataBlockValid(dip->addrs[j]);
                if (!valid)
                    die("address used by inode but marked free in bitmap\n");

                printf("%d DPTR(%d) -> %d (%d)\n", i, j, dip->addrs[j], valid);
            }

            if (dip->addrs[j] >= sb->size) {
                die("bad address in inode\n");
            }
            else if (dip->addrs[j] != 0) {
                printf("%d IPTR -> %d\n", i, dip->addrs[j]);
                addr = img + (((dip->addrs[j]) * BSIZE));
                for (j = 0; j < BSIZE/4; j++) {
                    if (addr[j] >= sb->size)
                        die("bad address in inode\n");

                    valid = isDataBlockValid(addr[j]);
                    if (!valid)
                        die("address used by inode but marked free in bitmap\n");

                    printf("%d  DPTR(%d) -> %d (%d)\n", i, j+12, addr[j], valid);
                }
            }

            /*if (dip->type == T_DIR) {

            }*/
        }

        dip++;
    }

    return 0;
}
