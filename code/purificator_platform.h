typedef char unsigned u8;
typedef short unsigned u16;
typedef int unsigned u32;
typedef long long unsigned u64;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef s32 b32;

#define internal static 

#define MAX_PATH_SIZE 260

#define KiloBytes(Value) (Value*1024LL)
#define MegaBytes(Value) (KiloBytes(Value)*1024LL)
#define GigaBytes(Value) (MegaBytes(Value)*1024LL)
#define TeraBytes(Value) (GigaBytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

#include"string_op.h"

inline u32 safe_truncate_u64(u64 Value)
{
    Assert(Value <= 0xFFFFFFFF);
    u32 Result = (u32)Value;
    return Result;
}

typedef struct thread_context
{
    int Placeholder;
} thread_context;

typedef struct app_file
{
    char NewDirPath[MAX_PATH_SIZE];
    char NewPath[MAX_PATH_SIZE];

    char FileName[100];
    
    char Path[MAX_PATH_SIZE];
    char MetadataPath[MAX_PATH_SIZE];
} app_file;

typedef struct debug_read_file_result
{
    u32 ContentsSize;
    void *Contents;
} debug_read_file_result;

typedef struct http_info
{
    char Query[60];
    char *ResponseData;
    char *ResponseDataPointer;
    int ResponseDataSize;
    b32 HasFailed;
    u8 MovieCount;
} http_info;

#pragma pack(push,1) // TODO(Axel): Delete if not needed

struct movie
{
    u16 IdFile;
    char Title[100];
    char Year[4];
};
#pragma pack(pop)

struct app_state
{
    u16 FileCount;
    char MoviesSelectedPath[MAX_PATH_SIZE];
    
    char EXEPath[MAX_PATH_SIZE];
    char *OnePastLastEXEFileNameSlash;
};

#define PLATFORM_MAKE_HTTP_REQUEST(name) void name(thread_context *Thread, http_info *HTTPInfo, app_file *File, movie *Movie)
typedef PLATFORM_MAKE_HTTP_REQUEST(platform_make_http_request);

#define PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(thread_context *Thread, char *FileName, u32 MemorySize, void *Memory)
typedef PLATFORM_WRITE_ENTIRE_FILE(platform_write_entire_file);

#define PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context *Thread, char *FileName)
typedef PLATFORM_READ_ENTIRE_FILE(platform_read_entire_file);

#define PLATFORM_FREE_MEMORY(name) void name(thread_context *Thread, void *Memory)
typedef PLATFORM_FREE_MEMORY(platform_free_memory);

#define PLATFORM_CREATE_FOLDER(name) b32 name(thread_context *Thread, char *FileName, app_state *AppState, movie *Movie, app_file *File)
typedef PLATFORM_CREATE_FOLDER(platform_create_folder);

#define PLATFORM_MOVE_FILE(name) b32 name(thread_context *Thread, char *FileName, app_file *File, movie *Movie)
typedef PLATFORM_MOVE_FILE(platform_move_file);

typedef struct app_memory
{
    platform_make_http_request *PlatformMakeHTTPRequest;
    
    platform_write_entire_file *PlatformWriteEntireFile;
    platform_read_entire_file *PlatformReadEntireFile;
    platform_free_memory *PlatformFreeMemory;

    platform_create_folder *PlatformCreateFolder;
    platform_move_file *PlatformMoveFile;
    
    void *PermanentMemory;
} app_memory;

#define GET_MOVIE_DATA(name) b32 name(thread_context *Thread, app_file *File, movie *Movie, http_info *HTTPInfo, app_memory *AppMemory, app_state *AppState)
typedef GET_MOVIE_DATA(get_movie_data);

#define CLEAN_FILE_MOVIE(name) app_file name(thread_context *Thread, app_file *File, app_memory *AppMemory, app_state *AppState, movie *Movie)
typedef CLEAN_FILE_MOVIE(clean_file_movie);

