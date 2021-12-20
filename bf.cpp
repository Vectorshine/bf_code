/*
（转自: http://www.felix021.com/blog/read.php?1882）

布隆过滤器的详细介绍和典型用途，可参见
Wikipedia：http://en.wikipedia.org/wiki/Bloom_filter
谷歌黑板报（数学之美）：http://www.google.cn/ggblog/googlechinablog/2007/07/bloom-filter_7469.html

下面是一个简单的布隆过滤器的C/C++实现，以及使用例程。使用sdbmhash字符串hash方法来进行hash。
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<random>
#include<iostream>
#include<set>
using namespace std;
char* stringRandom(int length);

set<char *> true_check;
unsigned int jshash(const char *s, unsigned size);
unsigned int sdbmhash(const char *s, unsigned size);

/* ------------- bloom types and funcs --------------- */
const unsigned char masks[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

typedef unsigned (*hash_func_ptr)(const char *buffer, unsigned size);
//布隆过滤器的结构
struct __bloom_filter
{
    unsigned n;
    unsigned size;//数组的规模
    unsigned char *bits;//位数组
    hash_func_ptr hash;//选取的哈希函数
};
typedef struct __bloom_filter* bloom_filter;

bloom_filter bloom_init (unsigned n, hash_func_ptr hash);//申请那么大的空间数组，并且初始化
int bloom_insert(bloom_filter b, void *data, unsigned size);//插入了size的一半的数
int bloom_check(bloom_filter b, void *data, unsigned size);//检查元素是否被插入
void bloom_destroy(bloom_filter b);
/* ------------- end of bloom types and funcs --------------- */

int main()
{
    const int size = 655371;
    bloom_filter b1 = bloom_init(size, sdbmhash);//创建一个bf结构
    for (int i = 0; i < size / 2; i += 2)
    {
        char *s=stringRandom(15);
        true_check.insert(s);
        if (!bloom_insert(b1,s, sizeof(s)))
        {
            fprintf(stderr, "err insert %d\n", i);
            exit(1);
        }
    }
    printf("insert ok \n");

    int cnt = 0;
    
    // set<char *>::iterator iter = true_check.begin();
    // while (iter != true_check.end())
    // {
    //     cout<<*iter;
    //     cout << bloom_check(b1, *iter, sizeof(*iter)) << endl;
    //     iter++;
    // } 
    int tp = 0, tn = 0, fp = 0, fn = 0;
    for (int i = 0; i < size; i++)
    {
        char *s=stringRandom(15);
        int bf_res = bloom_check(b1, s,sizeof(s));
        int set_res = (true_check.find(s) != true_check.end());
        // printf("%d %d\n", set_res, bf_res);
        if(bf_res & set_res)
            tp++;//存在，且bf检测中也存在
        else if((!bf_res) & set_res)
            tn++;//存在，但是bf检测不存在  永远是0
        else if(bf_res & (!set_res))
            fp++;//不存在，但是bf检测存在
        else
            fn++;//不存在，bf检测也不存在
    }
    printf("tp : %lf \n", tp*1.0/size);
    printf("tn : %lf \n", tn*1.0/size);
    printf("fp : %lf \n", fp*1.0/size);
    printf("fn : %lf \n", fn*1.0/size);
    return 0;
}

char* stringRandom(int length)
{
    //生成长度为length的随记字符串
    char *str;
    int flag;
    if((str=(char*)malloc(length+1)) == NULL)
    {//分配内存如果失败
        printf("ai si bi\n");
        return NULL;
    }	
    for (int i = 0; i < length; i++)
    {//开始添加随记数
        flag = rand()%2;
        if(flag)
        {
            str[i] = 'A' + rand()%26;//这里对大小写的处理
        }else
        {
            str[i] = 'a' + rand()%26; 
        }
    }
    //注意在最后一定要加上'\0'注意斜杠的方向
    str[length]='\0';
    return str;
}


bloom_filter bloom_init (unsigned n, hash_func_ptr hash)
{
    bloom_filter b = (bloom_filter)malloc(sizeof(__bloom_filter));
    if (b == NULL)
    {
        fprintf(stderr, "bloom_init: err malloc bloom_filter\n");
        return NULL;
    }

    b->n = n;
    b->size = (n + 7) / 8;
    b->hash = hash;

    b->bits = (unsigned char *)malloc(b->size);
    memset(b->bits, 0, b->size);
    if (b->bits == NULL)
    {
        fprintf(stderr, "bloom_init: err malloc bits\n");
        return NULL;
    }
    return b;
}

int bloom_insert(bloom_filter b, void *data, unsigned size)
{
    unsigned h = b->hash((const char *)data, size) % (b->n);
    unsigned idx = h / 8;
    if (idx >= b->size)
    {
        fprintf(stderr, "bloom_insert: hash value overflow\n");
        return 0;
    }
    b->bits[idx] |= masks[h % 8];
    //printf("h = %2d, idx = %2d, bit = %2d\n", h, idx, h % 8);
    return 1;
}

int bloom_check(bloom_filter b, void *data, unsigned size)
{
    unsigned h = b->hash((const char *)data, size) % (b->n);
    unsigned idx = h / 8;
    if (idx >= b->size)
    {
        fprintf(stderr, "bloom_insert: hash value overflow\n");
        exit(1);
    }
    return !!(b->bits[idx] & masks[h % 8]);
}

void bloom_destroy(bloom_filter b)
{
    if (b != NULL)
    {
        if (b->bits != NULL)
            free(b->bits);
        free(b);
    }
}

//-----------------------------------------------

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
    // printf("%s  %d\n", s, s);
    while (len < size)
    {
        hash = (hash << 6) + (hash << 16) - hash + s[len];
        len++;
    }
    return (hash & 0x7fffffffl);
}
