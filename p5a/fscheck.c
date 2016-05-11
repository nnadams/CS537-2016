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

//#define DEBUG
#ifdef DEBUG
# define DEBUG_PRINT(x, ...) printf(x, ##__VA_ARGS__)
#else
# define DEBUG_PRINT(x, ...) do {} while (0)
#endif

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
#define BBLOCK(b, ninodes)  ((b)/BPB + (ninodes)/IPB + 3)

//#define DBLOCK(b, ninodes, nblocks)  (((nblocks)/BPB + (ninodes)/IPB + 3))

void *img;
superblock *sb;

const char dot[] = ".";
const char dotdot[] = "..";

static inline short isDataBlockValid(uint block) {
    uint *valid = img + (BBLOCK(block, sb->ninodes) * BSIZE);
    valid += block / 32;
    return ((*valid) >> (block % 32)) & 1;
}

void chkParentDir(uint inode, uint parent) {
    short i, j;
    dirent *ent;
    dinode *dip = (dinode *)(img + 2*BSIZE);
    for (i = 0; i < sb->ninodes; i++) {
        if (dip->type == T_DIR && i == parent) {
            for (j = 0; j < NDIRECT; j++) {
                ent = img + (((dip->addrs[j]) * BSIZE));

                while (ent->inum != 0) {
                    if (ent->inum == inode) {
                        if (ent->inum != parent || parent == 1)
                            return;
                    }
                    ent++;
                }
            }
        }
        dip++;
    }
    die("ERROR: parent directory mismatch.\n");
}

void chkInodeUsed(uint inode) {
    short i, j;
    dirent *ent;
    dinode *dip = (dinode *)(img + 2*BSIZE);

    if (inode == 1) return;

    for (i = 0; i < sb->ninodes; i++) {
        if (dip->type == T_DIR) {
            for (j = 0; j < NDIRECT; j++) {
                ent = img + (((dip->addrs[j]) * BSIZE));

                while (ent->inum != 0) {
                    if (ent->inum == inode) {
                        return;
                    }
                    ent++;
                }
            }
        }
        dip++;
    }
    die("ERROR: inode marked use but not found in a directory.\n");
}

void chkDirOnlyOnce(uint inode) {
    short used = 0, i, j;
    dirent *ent;
    dinode *dip = (dinode *)(img + 2*BSIZE);
    for (i = 0; i < sb->ninodes; i++) {
        if (dip->type == T_DIR) {
            for (j = 0; j < NDIRECT; j++) {
                ent = img + (((dip->addrs[j]) * BSIZE));

                while (ent->inum != 0) {
                    if (ent->inum == inode && ent->inum != 1) {
                        if (strncmp(ent->name, dotdot, 3) != 0 && strncmp(ent->name, dot, 2) != 0)
                        {
                            if (++used > 1) {
                                die("ERROR: directory appears more than once in file system.\n");
                            }
                        }
                    }
                    ent++;
                }
            }
        }
        dip++;
    }
}

void chkBlockOnlyOnce(uint block) {
    short used = 0, i, j;
    dinode *dip = (dinode *)(img + 2*BSIZE);

    if (block == 0) return;

    for (i = 0; i < sb->ninodes; i++) {
        for (j = 0; j < NDIRECT; j++) {
            if (dip->addrs[j] == block) used++;
            if (used > 1)
                die("ERROR: address used more than once.\n");
        }
        dip++;
    }
}

void chkCorrectLinkCount(uint inode, uint nlink) {
    uint links = 0, i, j;
    dirent *ent;
    dinode *dip = (dinode *)(img + 2*BSIZE);
    for (i = 0; i < sb->ninodes; i++) {
        if (dip->type == T_DIR) {
            for (j = 0; j < NDIRECT; j++) {
                ent = img + (((dip->addrs[j]) * BSIZE));

                while (ent->inum != 0) {
                    if (ent->inum == inode) {
                        links++;
                    }
                    ent++;
                }
            }
        }
        dip++;
    }

    if (links != nlink)
        die("ERROR: bad reference count for file.\n");
}

int main(int argc, char **argv) {
    int i, j, k, rc, fd;
    short valid = 0;
    uint *addr;
    struct stat sbuf;
    dirent *ent;
    //char buf[BSIZE];

    if (argc != 2)
        die("bad args\n");

    fd = open(argv[1], 0);
    if (fd < 0)
        die("image not found.\n");

    rc = fstat(fd, &sbuf);
    assert(rc == 0);

    img = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(img != MAP_FAILED);

    sb = (superblock *)(img + BSIZE);
    DEBUG_PRINT("%d %d %d\n", sb->size, sb->nblocks, sb->ninodes);

    dinode *dip = (dinode *)(img + 2*BSIZE);
    for (i = 0; i < sb->ninodes; i++) {
        if (i == 1 && dip->type != T_DIR)
            die("ERROR: root directory does not exist.\n");

        if (dip->type != T_INVALID) {
            if (dip->type > 3 || dip->type < 0)
                die("ERROR: bad inode.\n");

            DEBUG_PRINT("\n%d type: %d\n", i, dip->type);

            if (dip->type == T_DEV) continue;

            chkInodeUsed(i);
            for (j = 0; j < NDIRECT; j++) {
                if (dip->addrs[j] >= sb->size)
                    die("ERROR: bad address in inode.\n");

                valid = isDataBlockValid(dip->addrs[j]);
                if (!valid)
                    die("ERROR: address used by inode but marked free in bitmap.\n");

                chkBlockOnlyOnce(dip->addrs[j]);

                DEBUG_PRINT("%d DPTR(%d) -> %d (%d)\n", i, j, dip->addrs[j], valid);
            }

            if (dip->addrs[j] >= sb->size) {
                die("ERROR: bad address in inode.\n");
            }
            else if (dip->addrs[j] != 0) {
                DEBUG_PRINT("%d IPTR -> %d\n", i, dip->addrs[j]);
                addr = img + (((dip->addrs[j]) * BSIZE));
                for (j = 0; j < BSIZE/4; j++) {
                    if (addr[j] >= sb->size)
                        die("ERROR: bad address in inode.\n");

                    valid = isDataBlockValid(addr[j]);
                    if (!valid)
                        die("ERROR: address used by inode but marked free in bitmap.\n");

                    if (addr[j] == 0) break;
                    //DEBUG_PRINT("%d  DPTR(%d) -> %d (%d)\n", i, j+12, addr[j], valid);
                }
            }

            if (dip->type == T_FILE) {
                chkCorrectLinkCount(i, dip->nlink);
            }

            if (dip->type == T_DIR) {
                chkDirOnlyOnce(i);

                short dots = 0;
                for (j = 0; j < NDIRECT; j++) {
                    ent = img + (((dip->addrs[j]) * BSIZE));

                    while (ent->inum != 0) {
                        if (strncmp(ent->name, dot, 2) == 0) {
                            dots++;
                            if (ent->inum != i) {
                                if (i == 1) {
                                    die("ERROR: root directory does not exist.\n");
                                }
                                else {
                                    die("ERROR: directory not properly formatted.\n");
                                }
                            }
                        }
                        else if (strncmp(ent->name, dotdot, 3) == 0) {
                            dots++;
                            if (i == 1) { // is root
                                if (ent->inum != 1) {
                                    die("ERROR: root directory does not exist.\n");
                                }
                            }
                            else {
                                chkParentDir(i, ent->inum);
                            }

                        }
                        else {
                            dinode *tmp = (dinode *)(img + 2*BSIZE);
                            int n;
                            for (n = 0; n < sb->ninodes; n++) {
                                if (n == ent->inum) {
                                    if (tmp->type < 0 || tmp->type > 3) {
                                        die("ERROR: bad inode.\n");
                                    }
                                    else if (tmp->type == 0) {
                                        die("ERROR: inode referred to in directory but marked free.\n");
                                    }
                                    else {
                                        break;
                                    }
                                }
                                tmp++;
                            }
                        }

                        DEBUG_PRINT("%d  %s (%d)\n", i, ent->name, ent->inum);
                        ent++;
                    }
                }

                if (dots != 2) {
                    die("ERROR: directory not properly formatted.\n");
                }
            }
        }

        dip++;
    }

    uint *v;
    i = BBLOCK(sb->nblocks - 1, sb->ninodes) + 1;
    for (; i < sb->nblocks; i++) {
        v = img + (BBLOCK(i, sb->ninodes) * BSIZE);
        v += i / 32;
        if ((((*v) >> (i % 32)) & 1) == 1) {
            dinode *dip = (dinode *)(img + 2*BSIZE);
            for (j = 0; j < sb->ninodes; j++) {
                for (k = 0; k < NDIRECT; k++) {
                    if (dip->addrs[k] == i) goto nxtBlock;
                }
                if (dip->addrs[k] != 0) {
                    if (i == dip->addrs[k]) goto nxtBlock;
                    addr = img + (((dip->addrs[k]) * BSIZE));
                    for (k = 0; k < BSIZE/4; k++) {
                        if (addr[k] == i) goto nxtBlock;
                    }
                }
                dip++;
            }
        }
        else {
            continue;
        }
        die("ERROR: bitmap marks block in use but it is not in use.\n");

        nxtBlock: ;
    }

    return 0;
}
