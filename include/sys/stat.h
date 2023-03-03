#ifndef STAT_H
#define STAT_H

#include <sys/types.h>

#if 0
struct stat {
	dev_t st_dev;
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	off_t st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
};
#endif

#define S_IFMT 		0170000		// type of file mask
#define S_IFBLK		0060000		// block special
#define S_IFCHR		0020000		// character special
#define S_IFIFO		0010000		// named pipe(fifo)
#define S_IFREG		0100000		// regular
#define S_IFDIR		0040000		// directory
#define S_IFLNK		0120000		// symbolic link

#define S_IRWXU		0000700		// RWX mask for owner
#define S_IRUSR		0000400		// R for owner
#define S_IWUSR		0000200		// W for owner
#define S_IXUSR		0000100		// X for owner

#define S_IRWXG		0000070		// RWX mask for group
#define S_IRGRP		0000040		// R for group
#define S_IWGRP		0000020		// W for group
#define S_IXGRP		0000010		// X for group

#define S_IRWXO		0000007		// RWX mask for other
#define S_IROTH		0000004		// R for other
#define S_IWOTH		0000002		// W for other
#define S_IXOTH		0000001		// X for other

#define S_ISUID		0004000		// set user id on execution
#define S_ISGID		0002000		// set group id on execution
#define S_ISVTX		0001000		// sticky bit

#define S_ISBLK(m)		((m & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)		((m & S_IFMT) == S_IFCHR)
#define S_ISFIFO(m)		((m & S_IFMT) == S_IFIFO)
#define S_ISREG(m)		((m & S_IFMT) == S_IFREG)
#define S_ISLNK(m)		((m & S_IFMT) == S_ISLNK)

# if 0
int chmod(const char *, mode_t);
int fchmod(int, mode_t);
int fstat(int, struct stat *);
int lstat(const char *, struct stat *);
int mkfifo(const char *, mode_t);
int mknod(const char *, mode_t, dev_t);
int stat(const char *, struct stat *);
mode_t umask(mode_t);
#endif
int mkdir(const char *pathname, mode_t mode);

#endif
