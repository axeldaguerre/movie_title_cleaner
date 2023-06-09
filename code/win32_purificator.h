struct win32_app_code
{
    HMODULE DLLAppCode;
    
    get_movie_data *GetMovieData;
    clean_file_movie *CleanFileMovie;
    debug_check_user_picks_file *DEBUGCheckUserPicksFile;
    ask_pick_movies *AskPickMovies;
    
    b32 IsValid;
};

struct win32_state
{
    u64 MemorySize;
    void *AppMemory;
    HANDLE FileHandle;
};
