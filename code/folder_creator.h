typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;
typedef long long unsigned u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef s32 b32;

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define KiloBytes(Value) (Value*1024LL)
#define MegaBytes(Value) (KiloBytes(Value)*1024LL)
#define GigaBytes(Value) (MegaBytes(Value)*1024LL)
#define TeraBytes(Value) (GigaBytes(Value)*1024LL)

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

struct movie
{   
    char Title[100];
    char Year[4];
};

struct file 
{
    HANDLE RecordHandle;
    char FileName[MAX_PATH];
    movie Movie;
};

struct win32_state 
{
    HANDLE FileHandle;
    void *Memory;
    int TotalSize;

    char *OnePastLastEXEFileNameSlash;
    char EXEFileName[MAX_PATH];

    char Search[100];
};





