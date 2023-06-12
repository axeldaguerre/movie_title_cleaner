
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <assert.h>
#include <wininet.h>

#include"purificator_platform.h"
#include"win32_purificator.h"

PLATFORM_MESSAGE_VALIDATION(PlatformMessageValidation)
{
    b32 Result = true;
    
    char TextMessageBox[300];
    wsprintf(TextMessageBox, "File Name: %s\n Movie title suggestion: %s (%s)",
             Movie->IdFile, Movie->Title, Movie->Year);
    
    int Answer  = MessageBox( NULL, TextMessageBox, "Is the Movie Title right?",
                              MB_YESNOCANCEL|MB_TOPMOST);
    
    if(Answer == IDOK)
    {
        Result = 1;
    }
    if(Answer == IDNO)
    {
        Result = 0;
    }
    //TODO(Axel): should be able to stop the program
    if(Answer == IDCANCEL)
    {
        Result = 3;
    }
    return Result;
}

PLATFORM_MOVE_FILE(PlatformMoveFile)
{
    b32 Result = true;

    if(MoveFile(FilePath, NewFilePath))
    {
    }
    else
    {
        Result = false;
    }
    
    return Result;
}

PLATFORM_CREATE_FOLDER(PlatformCreateFolder)
{             
    b32 Result = false;
    if(CreateDirectory(FolderPath, 0))
    {
        Result = true;
    }
    else
    {
        
        if(GetLastError() == ERROR_PATH_NOT_FOUND)
        {
            MessageBox( NULL, "Couldn't create the directory\n Path doesn't exists",
                        "Error Movie File Folder Creation", MB_OK);
            AppState->GlobalRunning = false;
            Result = false;
        }
        else
        {
            Result = true;
        }
    }

    return Result;   
}

PLATFORM_FREE_MEMORY(PlatformFreeMemory)
{
    if(Memory)
    {   
        VirtualFree(Memory, 0, MEM_RELEASE);
    }
}

PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile)
{
    // STUDY(Axel): Use HeapAlloc()?     
    debug_read_file_result Result = {};
    
    HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ,
                                    FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if(GetFileSizeEx(FileHandle, &FileSize))
        {
            u32 FileSize32 = safe_truncate_u64(FileSize.QuadPart);
            Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE|MEM_COMMIT,
                                           PAGE_READWRITE);
            if(Result.Contents)
            {
                DWORD BytesRead;
                if(ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0) &&
                   (FileSize32 == BytesRead))
                {
                    Result.ContentsSize = FileSize32;
                }
                else
                {
                    PlatformFreeMemory(Thread, Result.Contents);
                    Result.Contents = 0;
                }
            }
            else
            {
            }
        }
        else
        {

        }
        CloseHandle(FileHandle);
    }
    else
    {
    }
    return Result;
}

PLATFORM_WRITE_ENTIRE_FILE(PlatformWriteEntireFile)
{
    b32 Result = false;

    HANDLE FileHandle = CreateFileA(FileName, FILE_APPEND_DATA, 0, 0, OPEN_ALWAYS, 0, 0);

    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesWritten;
        
        if(WriteFile(FileHandle, Memory, MemorySize, &BytesWritten, 0))
        {
            Result = (BytesWritten == MemorySize);
        }
    }
    else
    {            
    }
    CloseHandle(FileHandle);
    
    return(Result);
}

inline win32_app_code Win32LoadAppCode(char *SourceDLLName)
{
    win32_app_code Result = {};

    Result.DLLAppCode = LoadLibraryA(SourceDLLName);
    if(Result.DLLAppCode)
    {
        Result.GetMovieData = (get_movie_data *)GetProcAddress(Result.DLLAppCode, "GetMovieData");
        Result.CleanFileMovie = (clean_file_movie *)GetProcAddress(Result.DLLAppCode, "CleanFileMovie");
        Result.AskPickMovies = (ask_pick_movies *)GetProcAddress(Result.DLLAppCode, "AskPickMovies");
        Result.DEBUGCheckUserPicksFile = (debug_check_user_picks_file *)GetProcAddress(Result.DLLAppCode, "DEBUGCheckUserPicksFile");
    }
    
    Result.IsValid = (Result.GetMovieData && Result.CleanFileMovie);

    if(!Result.IsValid)
    {
        Result.GetMovieData = 0;
    }
    return Result;
}

PLATFORM_MAKE_HTTP_REQUEST(PlatformMakeHTTPRequest)
{

    b32 Result = true;
    HINTERNET InternetHandle = InternetOpen("IMDB's Connexion", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    if (InternetHandle == NULL)
    {
    }
    else
    {
        HINTERNET ConnectHandle = InternetOpenUrlA(InternetHandle, HTTPInfo->Query,
                                                   "Accept-Language: en-US;",
                                                   0, INTERNET_FLAG_RELOAD, 0);
        if(ConnectHandle == NULL)
        {
            char ErrorText[200];
            
            str_copy(str_len("Couldn't make the internet call, check your internet connexion"),
                             "Couldn't make the internet call, check your internet connexion",
                             str_len(ErrorText), ErrorText);
            int Answer  = MessageBox( NULL, ErrorText, "Internet Error", MB_OK);
            if(Answer == IDOK)
            {
                AppState->GlobalRunning = false;
                return false;
            }
        }
        else
        {                
            DWORD BytesWritten    = 0;
            DWORD BytesRead       = 0;
            DWORD ContentLen = 0;
            char *Pointer = (char*)Memory;
            
            while(InternetReadFile(ConnectHandle, Pointer, MemorySize, &BytesRead)
                  && BytesRead > 0)
            {
                Pointer += BytesRead;
            }
            ++Pointer  = '\0';
            HTTPInfo->ResponseData = (char*)Memory;
        }
    }
    InternetCloseHandle(InternetHandle);
    
    return Result;
}

inline b32 UserConfirmation(movie *Movie)
{
    b32 UserAnswer = false;
    
    char MessageBoxText[100];
    wsprintf(MessageBoxText, "%s (%s)", Movie->Title, Movie->Year);

    UserAnswer = MessageBox( NULL, MessageBoxText, MessageBoxText,
                                 MB_YESNOCANCEL);
    if(UserAnswer == IDNO)
    {
        *Movie->Title = 0;
        *Movie->Year = 0;
    }
    else if(UserAnswer == IDYES)
    {
        UserAnswer = true;
    }
    else if(UserAnswer == IDCANCEL)
    {
        UserAnswer = 3;
    }
    
    return UserAnswer;
}

inline void clean_file_name(char *String, char *Dest)
{
    int StringLength = str_len(String) - 1;

    int DotCount = 0;
    for(char *Scan = String; *Scan; ++Scan)
    {
        if(*Scan == '.')
        {
            ++DotCount;
        }
    }
    
    for(char *Scan = String; *Scan; ++Scan)
    {
        if(DotCount != 0 && *Scan == '.')
        {
            --DotCount;
            if(*(Scan + 1) != ' ')
            {
                *Dest = ' ';
                ++Dest;
            }
        }
        else if(DotCount != 0 && *Scan == ' ')
        {
            if(*(Scan + 1) != ' ')
            {   
                *Dest = *Scan;
                ++Dest;
            }
        }
        else
        {
            *Dest = *Scan;
            ++Dest;
        }
    }
}

inline b32 is_folder(char *Path)
{
    b32 Result = false;
    DWORD FileAtt = GetFileAttributesA(Path);
    if(FileAtt == FILE_ATTRIBUTE_DIRECTORY)
    {
        Result = true;
    }
    if(FileAtt == FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS)
    {
        Result = true;
    }

    DWORD Error = GetLastError();

    return Result;
}

inline b32 is_program_file(char *String)
{
    b32 Result = false;
    char *Extensions[] = {".dll", ".map", "exp",
                          ".lib", ".exe", ".obj",
                          ".pdb", "..", ".vs", ".sln",
                          ".tmp", ".srt"};
    int AtChar = 0;
    
    for(int Index = 0; Index < ArrayCount(Extensions); ++Index)    
    {
        if(str_search_from_end(Extensions[Index], String, &AtChar))
        {
            Result = true;
            break;
        };
    }
    
    return Result;
}

inline void win32_get_exe_file_name(app_state *State )
{
    GetModuleFileName(0, State->EXEPath, sizeof(State->EXEPath));
    State->OnePastLastEXEFileNameSlash = State->EXEPath;
    
    for(char *Scan = State->OnePastLastEXEFileNameSlash; *Scan; ++Scan)
    {
        if(*Scan == '\\')
        {
            State->OnePastLastEXEFileNameSlash = Scan + 1; 
        }
    }
}

inline void win32_build_file_path(app_state *State, char *FileName,
                                  int DestCount, char *Dest)
{
    str_concat(State->OnePastLastEXEFileNameSlash - State->EXEPath, State->EXEPath,
               str_len(FileName), FileName, DestCount, Dest);    
}

inline void win32_get_file_name(app_state *State, char *Name, int DestCount, char *Dest)
{
    char Temp[64];
    wsprintf(Temp, "%s.meta", Name);
    
    win32_build_file_path(State, Temp, str_len(Name), Name);
}

inline void win32_get_file_exe(app_state *State)
{
    DWORD SizeOfFileName = GetModuleFileName(0, State->EXEPath, sizeof(State->EXEPath));
    State->OnePastLastEXEFileNameSlash = State->EXEPath;
    for(char *Scan = State->EXEPath;
        *Scan;
        ++Scan)
    {
        if(*Scan == '\\')
        {
            State->OnePastLastEXEFileNameSlash = Scan + 1;
        }
    }
}

inline void str_cut_after_from_end(char *String, char *Pat, int PatLen)
{
    int AtIndex = 0;
    
    if(!str_search_from_start(Pat, str_len(String), String, &AtIndex)) return;
    String += AtIndex;
    *String = 0;
}


int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{

#if  INTERNAL
    LPVOID BaseAddress = (LPVOID)TeraBytes(2);
#else
    LPVOID BaseAddress = 0;
#endif

    thread_context Thread = {};

    win32_state Win32State = {};
    Win32State.MemorySize = MegaBytes(64);
    Win32State.AppMemory = VirtualAlloc(BaseAddress, (size_t)Win32State.MemorySize,
                                     MEM_RESERVE|MEM_COMMIT,
                                     PAGE_READWRITE);
    
    app_state AppState = {};
    AppState.GlobalRunning = true;
    AppState.FileCount = 1; // NOTE(Axel): Should start at 1 (see ASK_PICK_MOVIES)
    
    win32_get_file_exe(&AppState);

    
    char SourceAppCodeDLLFullPath[MAX_PATH];
    win32_build_file_path(&AppState, "purificator.dll",
                          sizeof(SourceAppCodeDLLFullPath), SourceAppCodeDLLFullPath);

    win32_app_code App = Win32LoadAppCode(SourceAppCodeDLLFullPath);
   
    app_memory AppMemory = {};
    AppMemory.PermanentMemory = Win32State.AppMemory;
    AppMemory.PermanentStorageSize = Win32State.MemorySize;
    AppMemory.PlatformMakeHTTPRequest = PlatformMakeHTTPRequest;
    AppMemory.PlatformFreeMemory = PlatformFreeMemory;
    AppMemory.PlatformWriteEntireFile = PlatformWriteEntireFile;
    AppMemory.PlatformReadEntireFile = PlatformReadEntireFile;
    AppMemory.PlatformCreateFolder = PlatformCreateFolder;
    AppMemory.PlatformMoveFile = PlatformMoveFile;
    AppMemory.PlatformMessageValidation = PlatformMessageValidation;
        
    WIN32_FIND_DATA FileData = {};
    
    char FileSearchQuery[MAX_PATH];
    str_concat((AppState.OnePastLastEXEFileNameSlash - AppState.EXEPath), AppState.EXEPath,
               str_len("*"), "*",
               str_len(FileSearchQuery), FileSearchQuery);
            
    Win32State.FileHandle = FindFirstFileA(FileSearchQuery, &FileData);
    

    char DevFileToDelete[MAX_PATH];
    str_concat((AppState.OnePastLastEXEFileNameSlash - AppState.EXEPath), AppState.EXEPath,
               str_len("user_picks.tmp"), "user_picks.tmp",
               str_len(DevFileToDelete), DevFileToDelete);
    DeleteFile(DevFileToDelete);
    
    str_concat((AppState.OnePastLastEXEFileNameSlash - AppState.EXEPath), AppState.EXEPath,
               str_len("metadata.tmp"), "metadata.tmp",
               str_len(DevFileToDelete), DevFileToDelete);
    DeleteFile(DevFileToDelete);

    while(AppState.GlobalRunning)
    {        
        if(FindNextFileA(Win32State.FileHandle, &FileData))
        {   
            if(is_program_file(FileData.cFileName)) continue;

            app_file File = {};
            str_copy(str_len(FileData.cFileName), FileData.cFileName,
                     str_len(File.FileName), File.FileName);
            // TODO(Axel): Same trick than onePastEXE
            str_copy(str_len(FileData.cFileName), FileData.cFileName,
                     str_len(File.Path), File.Path);
            
            win32_build_file_path(&AppState, FileData.cFileName,
                                  str_len(File.Path), File.Path);
            
            if(is_folder(File.Path)) continue;

            http_info HTTPInfo = {};
            
            char FileNameNoExtension[MAX_PATH];
            str_cut_after_from_end(FileData.cFileName, FileNameNoExtension, ".");
            Assert(str_len(FileNameNoExtension) > 0);

            wsprintf(HTTPInfo.Query, "https://www.rottentomatoes.com/search?search=%s", FileNameNoExtension);

            movie Movie = {};
            wsprintf(Movie.IdFile, "%s_%d", File.FileName, AppState.FileCount); 
            
            if(AppMemory.PermanentMemory)
            {
                if(App.GetMovieData)
                {

                    char IdFileMovie[MAX_PATH];
                    // TODO(Axel): Construct the full path 
                    wsprintf(IdFileMovie, "%s_%d", File.FileName, AppState.FileCount);


                    str_concat(AppState.OnePastLastEXEFileNameSlash - AppState.EXEPath, AppState.EXEPath,
                               str_len("metadata.tmp"), "metadata.tmp",
                               str_len(AppState.MetadataPath), AppState.MetadataPath);

                    App.GetMovieData(&Thread, &File, &Movie, &HTTPInfo, &AppMemory, &AppState);

                }
            }
        }
        else
        {
            break;
        }
    }
    
    str_concat(AppState.OnePastLastEXEFileNameSlash - AppState.EXEPath, AppState.EXEPath,
               str_len("user_picks.tmp"), "user_picks.tmp",
               str_len(AppState.MoviesSelectedPath), AppState.MoviesSelectedPath);

    if(App.AskPickMovies)
    {       
        if(AppState.GlobalRunning && App.AskPickMovies(&Thread, &AppState, &AppMemory))
        {
            if(App.DEBUGCheckUserPicksFile(&Thread, &AppMemory, &AppState))
            {
                if(App.CleanFileMovie)
                {
                    App.CleanFileMovie(&Thread, &AppMemory, &AppState);
                }
            }
            else
            {
                AppState.GlobalRunning = false;
            }
        }
    }

    str_concat((AppState.OnePastLastEXEFileNameSlash - AppState.EXEPath), AppState.EXEPath,
               str_len("user_picks.tmp"), "user_picks.tmp",
               str_len(DevFileToDelete), DevFileToDelete);
    DeleteFile(DevFileToDelete);
    
    str_concat((AppState.OnePastLastEXEFileNameSlash - AppState.EXEPath), AppState.EXEPath,
               str_len("metadata.tmp"), "metadata.tmp",
               str_len(DevFileToDelete), DevFileToDelete);
    DeleteFile(DevFileToDelete);

    
    return 0;
}
