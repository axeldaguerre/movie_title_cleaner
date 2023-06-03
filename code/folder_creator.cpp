
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <assert.h>
#include <wininet.h>

#include"folder_creator.h"

inline int str_len(char *String)
{
    int Result = 0;

    while(*String != 0)
    {
       ++Result;
       ++String;
    }
    return Result;
}

inline void
str_concat(size_t SourceACount, char *SourceA,
           size_t SourceBCount, char *SourceB,
           size_t DestCount, char *Dest)
{
    
    for(int Index = 0;
        Index < SourceACount;
        ++Index)
    {
        *Dest++ = *SourceA++;
    }

    for(int Index = 0;
        Index < SourceBCount;
        ++Index)
    {
        *Dest++ = *SourceB++;
    }

    *Dest++ = 0;
}

inline char char_at(char *String, int Index)
{
    int IndexChar = 0;
    char Result = 0;
    for(char *Scan = String; *Scan; ++Scan)
    {
        if(IndexChar == Index)
        {
             Result = *Scan;
             break;
        }
        ++IndexChar;
    }
    return Result;
}

inline void str_inverse(char *String, char *Dest)
{
    int StringLen = str_len(String);
    int DestLen = str_len(String);
    Assert(StringLen >= DestLen);
    char *Scan = (String + StringLen - 1);
    for(int Index = 0; Index < StringLen ; ++Index)
    {
        *Dest++ = *Scan--;
    }
    *Dest = 0;
}

// TODO: Make better readibility
inline b32 str_search_from_end(char *Pattern, char *Text, int *AtChar)
{   
    char TextInverse[MAX_PATH];
    str_inverse(Text, TextInverse);
    char PatInverse[MAX_PATH];
    str_inverse(Pattern, PatInverse);
    
    b32 Result    = false;
    *AtChar         = 0;
    int PatLen    = str_len(PatInverse);
    int TextLen   = str_len(TextInverse);
    int IndexText = 0;
    int IndexPat  = 0;

    for(;IndexText < TextLen && IndexPat < PatLen; ++IndexText)
    {
        if(char_at(PatInverse, IndexPat) == char_at(TextInverse, IndexText))
        {
            ++IndexPat;
        }
        else
        {
            IndexText -= IndexPat;
            IndexPat = 0;
        }
    }
    
    if(IndexPat == PatLen)
    {
        *AtChar = TextLen - IndexText - PatLen + 1;
        Result = true;
    }
    
    return Result;
}

// NOTE(Axel): Brute force of substring search in text, return the last char of the match
// TODO(Axel): Implement perf log 
inline b32 str_search_from_start(char *Pattern, char *Text, int *Char)
{
    b32 Result    = false;
    *Char         = 0;
    int PatLen    = str_len(Pattern);
    int TextLen   = str_len(Text);
    int IndexText = 0;
    int IndexPat  = 0;

    for(;IndexText < TextLen && IndexPat < PatLen; ++IndexText)
    {
        if(char_at(Pattern, IndexPat) == char_at(Text, IndexText))
        {
            ++IndexPat;
        }
        else
        {
            IndexText -= IndexPat;
            IndexPat = 0;
        }
    }
    
    if(IndexPat == PatLen)
    {
        *Char = IndexText + PatLen - 1;
        Result = true;
    }
    
    return Result;
}


inline int str_search_from_start_count(char *Pattern, char *Text)
{
    int Result   = 0;
    int PatLen    = str_len(Pattern);
    int TextLen   = str_len(Text);
    int IndexText = 0;
    int IndexPat = 0;
    for(; IndexText < TextLen; ++IndexText)
    {
        if(char_at(Pattern, IndexPat) == char_at(Text, IndexText))
        {
            ++IndexPat;
        }
        else
        {
            IndexPat = 0;
            IndexText -= IndexPat;
        }

        if(IndexPat == PatLen)
        {
            ++Result;
        }
    }
    return Result;
}

inline void str_copy(char *String, char *Dest)
{
    for(char *Scan = String; *Scan; ++Scan)
    {
        *Dest++ = *Scan;
    }
    *Dest = 0;
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

inline void str_cut(char *String, int Start, int End, char* Dest)
{
    assert(Start < End);
    String += Start;
    for(int IndexChar = Start; IndexChar <= End; ++IndexChar)
    {
        if(IndexChar == End)
        {
            *Dest = 0;
            break;
        }
        else if(IndexChar >= Start || IndexChar < End)
        {
            *Dest = *String;
        }
        ++String;
        ++Dest;
    }
}

inline b32 str_are_equal(char *StringA, char *StringB)
{
    b32 Result = true;
    for(char *Scan = StringA; *Scan; ++Scan)
    {
        if(*Scan != *StringB++)
        {
            Result = false;
            break;
        }
    }
    return Result;
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
    Assert(Error == 0);
    return Result;
}

inline b32 is_program_file(char *String)
{
    b32 Result = false;
    char *Extensions[] = {".exe", ".obj", ".pdb", "..", ".vs", ".sln"};

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


inline b32 str_cut_after_from_start(char *String, char* Pattern, char *Dest)
{
    b32 Result = false;
    int MatchCharAt = 0;
    if(str_search_from_start(Pattern, String, &MatchCharAt)) return false;
    char *Pointer = Dest;
    
    for(int IndexChar = 0; IndexChar < str_len(String); ++IndexChar)
    {
        if(IndexChar == MatchCharAt)
        {
            *--Pointer = 0;
            Result = true;
            break;
        }
        else
        {
            *Pointer = *String;
            ++Pointer;
            ++String;            
        }
    }
    return Result;
}


inline b32 str_cut_after_from_end(char *String, char* Pattern, char *Dest)
{
    b32 Result = false;
    int MatchCharAt = 0;
    if(!str_search_from_end(Pattern, String, &MatchCharAt)) return false;
    char *Pointer = Dest;
    
    for(int IndexChar = 0; IndexChar < str_len(String); ++IndexChar)
    {
        if(IndexChar == MatchCharAt)
        {
            *--Pointer = 0;
            Result = true;
            break;
        }
        else
        {
            *Pointer = *String;
            ++Pointer;
            ++String;            
        }
    }
    return Result;
}

inline b32 get_movie_data(win32_state *State, file *File, WIN32_FIND_DATA FileData)
{
    b32 Scrapping = true;
    
    b32 Result = false;
    char Query[200];
    char FileNameNoExtension[MAX_PATH];
    str_cut_after_from_end(FileData.cFileName, ".", FileNameNoExtension);

    Assert(str_len(FileNameNoExtension) > 0);
    str_concat(str_len("https://www.imdb.com/find/?q="), "https://www.imdb.com/find/?q=",
               str_len(FileNameNoExtension), FileNameNoExtension,
               str_len(Query), Query);


    
    HINTERNET hInternet = InternetOpen("WinINet Example", INTERNET_OPEN_TYPE_DIRECT,
                                       NULL, NULL, 0);

    if (hInternet == NULL)
    {
        OutputDebugStringA("InternetOpen failed \n");
    }
    else
    {
        HINTERNET hConnect = InternetOpenUrlA(hInternet, Query, NULL, 0,
                                              INTERNET_FLAG_RELOAD, 0);

        if(hConnect == 0)
        {
            OutputDebugStringA("InternetOpenUrlA failed \n");
        }
        else
        {
            DWORD statusCode;
            DWORD statusCodeSize = sizeof(statusCode);
            int MatchAt = 0;
            DWORD BytesWritten = 0;
            DWORD BytesRead = 0;
            char HTTPResponse[200000];
            b32 IsAMatch = false;
            b32 FileWriteFlag = true;
            str_copy("find-result-item fi", State->Search);
            
            HttpQueryInfo(hConnect, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                          &statusCode, &statusCodeSize, NULL);
            int ResLen = 0;
            do
            {
                InternetReadFile(hConnect, HTTPResponse, sizeof(HTTPResponse), &BytesRead);
                 ResLen = str_len(HTTPResponse);
            }
            while(!str_search_from_start(State->Search, HTTPResponse, &MatchAt) &&
                   ResLen > 0);

            if(ResLen == 0) return Result;
            char *HTTPPointer = HTTPResponse;
            
            while(Scrapping)
            {
                
                int MovieCount = str_search_from_start_count("/title/", HTTPResponse);
                for(int MovieIndex = 0; MovieIndex < MovieCount; ++MovieIndex)
                {
                    char TempBuffer[100];
                    str_copy("/title/", State->Search);
                    IsAMatch = str_search_from_start(State->Search, HTTPPointer, &MatchAt);
                    if(!IsAMatch) continue;
                    HTTPPointer += MatchAt;
                
                    str_copy(">", State->Search);
                    IsAMatch = str_search_from_start(State->Search, HTTPPointer, &MatchAt);
                    HTTPPointer +=  MatchAt;

                    str_copy("<", State->Search);
                    str_cut_after_from_start(HTTPPointer, State->Search, TempBuffer);
                    wsprintf(File->Movie.Title, "%s", TempBuffer);
                    IsAMatch = str_search_from_start(State->Search, HTTPPointer, &MatchAt);
                    HTTPPointer +=  MatchAt;

                    HTTPPointer +=  MatchAt;
                    // NOTE(Axel): Retrieve movie's year suggested
                    // TODO: Check if it's a short movie or not
                    str_copy("item__li\"", State->Search);
                    IsAMatch = str_search_from_start(State->Search, HTTPPointer, &MatchAt);
                    assert(IsAMatch);
                    HTTPPointer += MatchAt;
            
                    str_copy(">20", State->Search);
                    IsAMatch = str_search_from_start(State->Search, HTTPPointer, &MatchAt);
                    if(!IsAMatch)
                    {
                        str_copy(">19", State->Search);
                        IsAMatch = str_search_from_start(State->Search, HTTPPointer, &MatchAt);
                    }
                    assert(IsAMatch);
                    HTTPPointer += MatchAt;
                                        
                    str_copy("<", State->Search);
                    str_cut_after_from_start(HTTPPointer, State->Search, TempBuffer);
                    wsprintf(File->Movie.Year, "%s", TempBuffer);
                    IsAMatch = str_search_from_start(State->Search, HTTPPointer, &MatchAt);
                    HTTPPointer += MatchAt;
                                        
                    char MessageBoxText[100];
                    wsprintf(MessageBoxText, "%s (%s)", File->Movie.Title, File->Movie.Year);

                    int UserAnswer = MessageBox( NULL, MessageBoxText, MessageBoxText,
                                                 MB_YESNOCANCEL);            
                    if(UserAnswer == IDNO)
                    {
                        *File->Movie.Title = 0;
                        *File->Movie.Year = 0;
                    }
                    else if(UserAnswer == IDYES)
                    {
                        Result = true;
                        Scrapping = false;
                    }
                    else if(UserAnswer == IDCANCEL)
                    {
                        Scrapping = false;
                        Result = false;
                    }
                }
            }

        }
        
    }
    
    return Result;
}

inline void win32_get_exe_file_name(win32_state *State)
{
    GetModuleFileName(0, State->EXEFileName, sizeof(State->EXEFileName));
    State->OnePastLastEXEFileNameSlash = State->EXEFileName;
    
    for(char *Scan = State->OnePastLastEXEFileNameSlash; *Scan; ++Scan)
    {
        if(*Scan == '\\')
        {
            State->OnePastLastEXEFileNameSlash = Scan + 1; 
        }
    }
}

inline void win32_build_file_path(win32_state *State, char *FileName,
                                  int DestCount, char *Dest)
{
    str_concat(State->OnePastLastEXEFileNameSlash - State->EXEFileName, State->EXEFileName,
               str_len(FileName), FileName,
               DestCount, Dest);    
}


inline void win32_get_file_name(win32_state *State, char *Name, int DestCount, char *Dest)
{
    char Temp[64];
    wsprintf(Temp, "%s.meta", Name);
    
    win32_build_file_path(State, Temp, str_len(Name), Name);
}


inline void win32_begin_file_record(win32_state *State, file *File)
{
    char FileName[MAX_PATH];
    win32_get_file_name(State, File->Movie.Title, str_len(FileName), FileName);
    
    File->RecordHandle = CreateFileA(FileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
}

inline void win32_end_file_record(win32_state *State, file *File)
{
    CloseHandle(File->RecordHandle);
}

inline void str_cut_after_from_end(char *String, char *Pat)
{
    int AtIndex = 0;
    
    if(!str_search_from_start(Pat, String, &AtIndex)) return;
    String += AtIndex;
    *String = 0;
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{

#if INTERNAL
    LPVOID BaseAddress = (LPVOID)TeraBytes(2);
#else
    LPVOID BaseAddress = 0;
#endif
/*
    Win32State.SizeMemory = MegaBytes(64);

    Win32State.Memory = VirtualAlloc(BaseAddress,
                                     (size_t)Win32State.SizeMemory,
                                     MEM_RESERVE|MEM_COMMIT,
                                     PAGE_READWRITE);
*/
    win32_state Win32State = {};
    win32_get_exe_file_name(&Win32State);
    
            
    WIN32_FIND_DATA FileData = {};
    char FileSearchQuery[MAX_PATH];

    str_concat(Win32State.OnePastLastEXEFileNameSlash - Win32State.EXEFileName, Win32State.EXEFileName,
               str_len("*"), "*",
                str_len(FileSearchQuery), FileSearchQuery);
            
    Win32State.FileHandle = FindFirstFileA(FileSearchQuery, &FileData);
    
    for(int FileIndex = 1;; ++FileIndex)
    {        
        file File   = {};
            
        if(FindNextFileA(Win32State.FileHandle, &FileData))
        {   
            if(is_program_file(FileData.cFileName)) continue;

            char Temp[MAX_PATH];
            win32_build_file_path(&Win32State, FileData.cFileName, str_len(Temp), Temp);
            if(is_folder(Temp)) continue;
                
            if(get_movie_data(&Win32State, &File, FileData))
            {
                win32_begin_file_record(&Win32State, &File);
                char DirPathTemp[MAX_PATH];
                str_cut_after_from_start(File.FileName, "\\", DirPathTemp);
            }
        }
        else
        {
            break;
        }
    }
    return 0;
}
