#include <stdio.h>
#include<string.h>
#include <stdlib.h>

unsigned int jshash(const char *s, unsigned size);
unsigned int sdbmhash(const char *s, unsigned size);
unsigned int bkdrhash(const char *s, unsigned size);
unsigned int aphash(const char *s, unsigned size);
unsigned int djbhash(const char *s, unsigned size);
unsigned int rshash(const char *s, unsigned size);

//使用jshash方法
unsigned int jshash(const char *s, unsigned size)
{
    int hash = 1315423911;
    unsigned len = 0;
    while (len < size)
    {
        hash ^= (hash << 5) + s[len] + (hash >> 2);
        len++;
    }
    return (hash & 0x7fffffffl);
}
//sdbhash方法
unsigned int sdbmhash(const char *s, unsigned size)
{
    int hash = 0;
    unsigned len = 0;
    while (len < size)
    {
        hash = (hash << 6) + (hash << 16) - hash + s[len];
        len++;
    }
    return (hash & 0x7fffffffl);
}

// BKDR hash Function
unsigned int bkdrhash(const char *s, unsigned size)
{
    unsigned seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    unsigned len = 0;
    while (len < size)
    {
        hash = hash * seed + s[len];
        len++;
    }
    return (hash & 0x7fffffffl);
}

// AP hash Function
unsigned int aphash(const char *s, unsigned size)
{
    unsigned int hash = 0;
    int i;
    int len = 0;
 
    while(len < size)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ (s[len]) ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ (s[len]) ^ (hash >> 5)));
        }
        len++;
    }
 
    return (hash & 0x7fffffffl);
}

// DJB hash Function
unsigned int djbhash(const char *s, unsigned size)
{
    unsigned hash = 5381;
    int len = 0;
    while (len < size)
    {
        hash += (hash << 5) + s[len];
        len++;
    }
    return (hash & 0x7fffffffl);
}

// RS hash Function
unsigned int rshash(const char *s, unsigned size)
{
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
    int len = 0;
 
    while (len < size)
    {
        hash = hash * a + s[len];
        a *= b;
        len++;
    }
 
    return (hash & 0x7fffffffl);
}