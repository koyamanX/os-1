#ifndef TYPES_H
#define TYPES_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long long size_t;
#endif

#ifndef _SSIZE_T
#define _SSIZE_T
typedef long long ssize_t;
#endif

//typedef short dev_t;
typedef unsigned long ino_t;
typedef unsigned short mode_t;
typedef unsigned long off_t;

typedef unsigned long long u64;
typedef long long int i64;
typedef unsigned int u32;
typedef int i32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;

#endif
