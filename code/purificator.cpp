#include "purificator.h"
#define HTML_CHAR_SKIP_OPTIMIZATION 50000

inline void build_file_path(app_state *AppState, char *FileName, char* Dest)
{
    str_concat((AppState->OnePastLastEXEFileNameSlash - AppState->EXEPath), AppState->EXEPath,
               str_len(FileName), FileName,
               str_len(Dest), Dest);
}

extern "C" DEBUG_CHECK_USER_PICKS_FILE(DEBUGCheckUserPicksFile)
{
    b32 Result = true;
    if(AppMemory->PlatformReadEntireFile)
    {
        debug_read_file_result FileRead = AppMemory->PlatformReadEntireFile(Thread, AppState->MoviesSelectedPath);
        u32 ContentsSizeLeft = FileRead.ContentsSize;
        
        // NOTE(Axel): All movies are in the correct order, we only need to check the next movie
        char PreviousFileId[100] ;
        if(FileRead.ContentsSize != 0)
        {
            movie *Movie = (movie*)FileRead.Contents;
            str_copy(str_len(Movie->IdFile), Movie->IdFile,
                     str_len(PreviousFileId), PreviousFileId);

            while(ContentsSizeLeft)
            {
                ++Movie;
                if(str_are_equal(Movie->IdFile, PreviousFileId))
                {
                    //TODO(Axel): Change the message box type 
                    str_copy(str_len("The file of the movies selected are corrupted, cancel it"), "The file of the movies selected are corrupted, cancel it",
                             str_len(Movie->Title), Movie->Title);
                    Result = false;
                    AppState->GlobalRunning = false;
                    AppMemory->PlatformMessageValidation(Thread, Movie);
                    break;
                };
                ContentsSizeLeft -= sizeof(movie);
            }
        }
    }
    return Result;    
}

// TODO(Axel): Do a check of the movies selected file (movies id should be unique)
extern "C" ASK_PICK_MOVIES(AskPickMovies)
{
    b32 Result = true;

    if(AppMemory->PlatformReadEntireFile)
    {
        debug_read_file_result FileRead = AppMemory->PlatformReadEntireFile(Thread, AppState->MetadataPath);
    
        if(FileRead.ContentsSize != 0)
        {
       
            u32 ContentsSizeLeft = FileRead.ContentsSize;
            b32 UserAnswer = false;
            movie *Movie;
            Movie = (movie*)FileRead.Contents;
            if(AppMemory->PlatformMessageValidation)
            {
                while(ContentsSizeLeft)
                {
                    if(Movie)
                    {
                        UserAnswer = AppMemory->PlatformMessageValidation(Thread, Movie);
                        
                        if(UserAnswer == 1 && AppMemory->PlatformWriteEntireFile)
                        {                            
                            if(AppMemory->PlatformWriteEntireFile(Thread,
                                                                  AppState->MoviesSelectedPath,
                                                                  sizeof(movie), Movie))
                            {
                                char IdFile[100];
                                str_copy(str_len(Movie->IdFile), Movie->IdFile,
                                         str_len(IdFile), IdFile);

                                while(ContentsSizeLeft > 0 && str_are_equal(Movie->IdFile, IdFile))
                                {
                                    ++Movie;
                                    ContentsSizeLeft -= sizeof(movie);
                                }
                            }
                            else
                            {
                                //NOTE(Axel): Error occured during write
                                break;
                            }
                        }
                        else if(UserAnswer == 0)
                        {
                            ++Movie;
                            ContentsSizeLeft -= sizeof(movie);
                        }
                        else if(UserAnswer == 3)
                        {
                            AppState->GlobalRunning = false;
                            Result = false;
                            break;
                        }

                    }
                }
            }
        }
    }
    else
    {
        Result = false;
    }
    
    return Result;
}

// TODO(Axel): CLEAN_FILES NOT CLEAN_FILE
extern "C" CLEAN_FILE_MOVIE(CleanFileMovie)
{
    app_file Result = {};
    debug_read_file_result FileRead = {};
    FileRead = AppMemory->PlatformReadEntireFile(Thread,
                                                 AppState->MoviesSelectedPath);

    if(FileRead.ContentsSize != 0)
    {

        u32 ContentsSizeLeft = FileRead.ContentsSize;
        movie *Movie = (movie*)FileRead.Contents;
        char FileName[150];
        char FolderName[150];
        char FolderPath[MAX_PATH_APP];
        char FilePath[MAX_PATH_APP];
        char NewFilePath[MAX_PATH_APP];
        while(ContentsSizeLeft)
        {           
            
            str_concat(str_len(Movie->Title), Movie->Title,
                       str_len(Movie->Year), Movie->Year,
                       str_len(FolderName), FolderName);
                
            build_file_path(AppState, FolderName, FolderPath);
            
            if(AppMemory->PlatformCreateFolder(Thread, AppState, FolderPath))
            {                

                str_copy_until(Movie->IdFile, FileName, "_");
            
                build_file_path(AppState, FileName, FilePath);

                char Temp[MAX_PATH_APP];
                str_concat(str_len(FolderPath), FolderPath,
                           str_len("\\"), "\\",
                           str_len(Temp), Temp);
                
                str_concat(str_len(Temp), Temp,
                           str_len(FileName), FileName,
                           str_len(NewFilePath), NewFilePath);
                                
                if(AppMemory->PlatformMoveFile(Thread, FilePath, NewFilePath))
                {
                }
                else
                {
                    //TODO(Axel): Log moving file failed
                }
            }
            ++Movie;
            ContentsSizeLeft -= sizeof(movie);
        }
    }
    
    return Result;
}


inline void remove_script_tag(char *String, char* Dest)
{
    char SearchStart[] = "<script";
    int SearchStartLength = str_len(SearchStart);
    char SearchEnd [] = "/script>";
    int SearchEndLength = str_len(SearchEnd);
    int IndexEnd = 0;
    int IndexStart = 0;
    int MovieCount = 0;

    for(char *Scan = String; *Scan; ++Scan)
    {
        str_search_from_start(SearchEnd, SearchEndLength, Scan, &IndexEnd);
        Scan += (IndexEnd + SearchEndLength + 1);

        str_search_from_start(SearchStart, SearchStartLength, Scan, &IndexStart);

        if(IndexStart != 0)
        {   
            Dest += str_copy(Scan, Dest, 0, IndexStart);
        }
    }
}

extern "C" GET_MOVIE_DATA(GetMovieData)
{
    b32 Result = false;    
    b32 IsAMatch          = false;
    b32 FileWriteFlag     = true;
    b32 Scrapping         = true;
    int MatchAt           = 0;
    int SearchLength      = 0;

    AppMemory->PlatformMakeHTTPRequest(Thread, HTTPInfo, File, Movie, AppState);
    
    ++AppState->FileCount;
    
    if(HTTPInfo->ResponseData)
    {
        // NOTE(Axel): HTML bloated with script, search is not performant. we skip content (dumb optimization)
        char *HTTPPointer = HTTPInfo->ResponseData + HTML_CHAR_SKIP_OPTIMIZATION;
                
        if(HTTPPointer)
        {
            IsAMatch = str_search_from_start("Movies</h2>", str_len("Movies</h2>"), HTTPPointer, &MatchAt);
            HTTPPointer += (MatchAt + 1);

            b32 HasMovieLeft = true;
            while(HasMovieLeft)
            {
                // NOTE(Axel): Year
                IsAMatch = str_search_from_start("releaseyear=\"", str_len("releaseyear=\""),
                                                 HTTPPointer, &MatchAt);
                HTTPPointer += (MatchAt + 1);
                
                char Temp[150];
                IsAMatch = str_copy_until(HTTPPointer, Temp, "\"");
                str_trim_end(str_len(Temp), Temp, str_len(Movie->Year), Movie->Year);
                // NOTE(Axel): Title
                str_search_from_start("slot=\"title\">",
                                      str_len("slot=\"title\">"),
                                      HTTPPointer, &MatchAt);
                HTTPPointer +=  MatchAt;                
                
                char Temp2[150];
                str_copy_by_char(HTTPPointer, Temp, ">", "<");

                // NOTE(AXEL): Multiple white space by one space
                replace_empties(str_len(Temp), Temp, str_len(Temp2), Temp2);
                // TODO(Axel): Trim end of the string
                remove_char_in_text(Temp2, Movie->Title, "\n");
                
                int IndexUl = 0;
                int IndexSearch = 0;
                str_search_from_start("<search", str_len("<search"), HTTPPointer, &IndexSearch);
                str_search_from_start("</ul", str_len("</ul"), HTTPPointer, &IndexUl);
                
                if(IndexUl < IndexSearch)
                {
                    HasMovieLeft = false;
                }
                Assert(IsAMatch);

               ++HTTPInfo->MovieCount;

               if(AppMemory->PlatformWriteEntireFile)
               {                   
                   if(!AppMemory->PlatformWriteEntireFile(Thread, AppState->MetadataPath,
                                                         sizeof(movie), Movie))
                   {
                       // NOTE(Axel): Couldn't write the file.
                       break;
                   }
               }
            }
        }                    
    }

    return Result;
}



