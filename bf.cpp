#include <string.h>
#include<random>
#include<iostream>
#include<set>
#include <ctime>
#include"hash.h"
#define fun_num 6
using namespace std;
char* stringRandom(int length);
int intRandom(int seed);

set<char *> true_check;
const int using_fun_num = 1;
/* ------------- bloom types and funcs --------------- */
const unsigned char masks[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

typedef unsigned (*hash_func_ptr)(const char *buffer, unsigned size);
//布隆过滤器的结构
struct __bloom_filter
{
    unsigned n;
    unsigned size;//数组的规模
    unsigned char *bits;//位数组
    hash_func_ptr hash[fun_num];//选取的哈希函数
};
typedef struct __bloom_filter* bloom_filter;

bloom_filter bloom_init (unsigned n, hash_func_ptr hash[]);//申请那么大的空间数组，并且初始化
int bloom_insert(bloom_filter b, void *data, unsigned size);//插入了size的一半的数
int bloom_check(bloom_filter b, void *data, unsigned size);//检查元素是否被插入
void bloom_destroy(bloom_filter b);
/* ------------- end of bloom types and funcs --------------- */

int main()
{
    const int size = 655371;//位数组长度
    hash_func_ptr hash[fun_num] = {sdbmhash,jshash,bkdrhash,aphash,djbhash,rshash};
    bloom_filter b1 = bloom_init(size, hash);//创建一个bf结构
    for (int i = 0; i < size / 2; i ++)
    {
        // char *s=stringRandom(15);字符串

        //整型
        int num = intRandom(i);
        char s[15];
        itoa(num, s, 15);

        true_check.insert(s);
        if (!bloom_insert(b1,s, sizeof(s)))
        {
            fprintf(stderr, "err insert %d\n", i);
            exit(1);
        }
    }
    printf("insert ok \n");

    int tp = 0, tn = 0, fp = 0, fn = 0;
    for (int i = 0; i < size; i++)
    {
        // char *s=stringRandom(15);字符串

        //整型
        int num = intRandom(i);
        char s[15];
        itoa(num, s, 15);

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

int intRandom(int seed)
{
    long long k = seed * 655371 * 655371;
    srand( k & 0x7fffffff);  // 产生随机种子  把0换成NULL也行
    int ret = rand();
    // cout << ret << endl;
    return ret;
}

char* stringRandom(int length)
{
    //生成长度为length的随记字符串
    char *str;
    int flag;
    if((str=(char*)malloc(length+1)) == NULL)
    {//分配内存如果失败
        printf("分配内存失败\n");
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
    //最后一定要加上'\0'
    str[length]='\0';
    return str;
}


bloom_filter bloom_init (unsigned n, hash_func_ptr hash[])
{
    bloom_filter b = (bloom_filter)malloc(sizeof(__bloom_filter));
    if (b == NULL)
    {
        fprintf(stderr, "bloom_init: err malloc bloom_filter\n");
        return NULL;
    }

    b->n = n;
    b->size = (n + 7) / 8;
    // *b->hash = *hash;
    // memcpy(b->hash,hash,sizeof(hash_func_ptr));
    for(int i = 0; i < using_fun_num; i++)
    {
        b->hash[i] = hash[i];
    }
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
    for(int i = 0; i < using_fun_num; i++)
    {
        unsigned h = b->hash[i]((const char *)data, size) % (b->n);
        unsigned idx = h / 8;
        if (idx >= b->size)
        {
            fprintf(stderr, "bloom_insert: hash value overflow\n");
            return 0;
        }
        b->bits[idx] |= masks[h % 8];
    }
    //printf("h = %2d, idx = %2d, bit = %2d\n", h, idx, h % 8);
    return 1;
}

int bloom_check(bloom_filter b, void *data, unsigned size)
{
    bool ret = 1;
    for(int i = 0; i < using_fun_num; i++)
    {
        unsigned h = b->hash[i]((const char *)data, size) % (b->n);
        unsigned idx = h / 8;
        if (idx >= b->size)
        {
            fprintf(stderr, "bloom_insert: hash value overflow\n");
            exit(1);
        }
        ret = (!!(b->bits[idx] & masks[h % 8])) & ret;
    }    
    return ret;
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