typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;
typedef long long unsigned u64;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef s32 b32;

#define MAX_PATH_SIZE 260

#define KiloBytes(Value) (Value*1024LL)
#define MegaBytes(Value) (KiloBytes(Value)*1024LL)
#define GigaBytes(Value) (MegaBytes(Value)*1024LL)
#define TeraBytes(Value) (GigaBytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

typedef struct thread_context
{
    int Placeholder;
} thread_context;

typedef struct app_file
{
    char Path[260];
} app_file;

typedef struct http_info
{
    char Query[60];
    char *ResponseData;
    char *ResponseDataPointer;
    int ResponseDataSize;
    b32 HasFailed;
} http_info;

struct movie
{
    char IdIMDB[20];
    char Title[100];
    char Year[4];
};

#define PLATFORM_MAKE_HTTP_REQUEST(name) void name(thread_context *Thread, http_info *HTTPInfo, app_file *File, movie *Movie)
typedef PLATFORM_MAKE_HTTP_REQUEST(platform_make_http_request);


#define PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(thread_context *Thread, char *Filename, u32 MemorySize, void *Memory)
typedef PLATFORM_WRITE_ENTIRE_FILE(platform_write_entire_file);

typedef struct app_memory
{
    platform_make_http_request *PlatformMakeHTTPRequest;
    platform_write_entire_file *PlatformWriteEntireFile;
    void *PermanentMemory;
} app_memory;

typedef struct app_state
{
    char ResponseNoJS[500000];
} app_state;

#define GET_MOVIE_DATA(name) b32 name(thread_context *Thread, app_file *File, movie *Movie, http_info *HTTPInfo, app_memory *AppMemory)
typedef GET_MOVIE_DATA(get_movie_data);

