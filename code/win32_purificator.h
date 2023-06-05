
struct win32_file 
{
    char FileName[MAX_PATH];
};

struct win32_app_code
{
    HMODULE DLLAppCode;
    get_movie_data *GetMovieData;
    b32 IsValid;
};

struct win32_state
{
    u64 MemorySize;
    void *Memory;
    char EXEPath[MAX_PATH];
    char *OnePastLastEXEFileNameSlash;
    HANDLE FileHandle;

};


