#include "purificator.h"
#define HTML_CHAR_SKIP_OPTIMIZATION 50000

// NOTE(Axel): Linux, Raspberry and other is more permissive than Windows on file path
//             we disallow the same as Windows for all OS and get the same behaviour on
//             all platforms.
// NOTE(Axel): Omitting the char not allowed is enought, because the application is only
//             formatting the files's name, the media software (i.e. Kodi) will scrap the
//             movie for you and find a match even without the special charactere
// NOTE(Axel): We replace them by a ' ' in order to avoid to empty the string before 
inline void remove_disallowed_chars(int StringCount, char *String, char *Dest)
{
    static char *DisallowChars[]= {"<", ">", ":", "\"", "/", "\\", "|", "?", "*", "\n", "\'"};

    for(char *Scan = String; *Scan; ++Scan)
    {
        b32 Skip = false;
        
        for(int Index = 0; Index <= (ArrayCount(DisallowChars) - 1); ++Index)
        {
            char Temp[1];
            str_copy(1, DisallowChars[Index], 1, Temp); 
            if(str_are_equal(1, Temp, 1, Scan))
            {
                Skip = true;
                break;
            }
        }
        
        if(!Skip)
        {
            *Dest++ = *Scan;
        }
        else
        {
            *Dest++ = ' ';
        }
    }
    *Dest = 0;
}

inline void build_file_path(app_state *AppState, char *FileName, char* Dest)
{
    str_concat((AppState->OnePastLastEXEFileNameSlash - AppState->EXEPath), AppState->EXEPath,
               str_len(FileName), FileName,
               str_len(Dest), Dest);
}

extern "C" SHOW_FILES_FAILED(ShowFilesFailed)
{
    b32 Result = false;
    
    if(AppMemory->PlatformReadEntireFile)
    {
        debug_read_file_result FileRead = AppMemory->PlatformReadEntireFile(Thread,
                                                                            AppState->FilesFailedPath);
        if(FileRead.ContentsSize != 0)
        {
            app_file *File = (app_file*)FileRead.Contents;

            char Text[MAX_PATH_APP];
            char Title[100];
            
            if(AppMemory->PlatformMessageValidation)
            {
                Result = AppMemory->PlatformMessageValidation(Thread, Title, Text);
            }
        }
    }
    
    return Result;
}

extern "C" DEBUG_CHECK_USER_PICKS_FILE(DEBUGCheckUserPicksFile)
{
    b32 Result = true;
    if(AppMemory->PlatformReadEntireFile)
    {
        debug_read_file_result FileRead = AppMemory->PlatformReadEntireFile(Thread,
                                                                            AppState->MoviesSelectedPath);        
        // NOTE(Axel): All movies are in the correct order, we only need to check the next movie
        char PreviousFileId[100] ;
        if(FileRead.ContentsSize != 0)
        {
            movie *Movie = (movie*)FileRead.Contents;

            str_copy(str_len(Movie->IdFile), Movie->IdFile,
                     str_len(PreviousFileId), PreviousFileId);
            
            int MovieCount =  FileRead.ContentsSize / sizeof(movie);
            for(int IndexMovie = 1; IndexMovie <= MovieCount-1; ++IndexMovie)
            {
                ++Movie;
                // TODO(Axel): We should have a bug when string is 0
                if(str_are_equal(str_len(Movie->IdFile), Movie->IdFile,
                                 str_len(PreviousFileId), PreviousFileId))
                {
                    //TODO(Axel): Custom Message Dialog
                    char Text[150];
                    char Title[150];
                    str_copy(str_len("The file of the movies selected are corrupted, cancel it"), "The file of the movies selected are corrupted, cancel it",
                             str_len(Text), Text);
                    str_copy(str_len("File Corrupted"), "File Corrupted",
                             str_len(Title), Title);
                    Result = false;
                    AppState->GlobalRunning = false;
                    AppMemory->PlatformMessageValidation(Thread, Text, Title);
                    break;
                }

            }
        }
    }
    return Result;    
}

extern "C" ASK_PICK_MOVIES(AskPickMovies)
{
    b32 Result = true;

    if(AppMemory->PlatformReadEntireFile)
    {
        debug_read_file_result FileRead =
            AppMemory->PlatformReadEntireFile(Thread, AppState->MetadataPath);
    
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
                        char Title[150];
                        char Year[6];
                        char IdFile[150];
                        char Text[300];
                        char Temp[150];
                        // TITLE
                        str_concat(str_len("File Name: "), "File Name: ",
                                   str_len(Movie->IdFile), Movie->IdFile,
                                   str_len(Title), Title);

                        // YEAR
                        str_concat(str_len("("), "(",
                                   str_len(Movie->Year), Movie->Year,
                                   str_len(Temp), Temp);

                        str_concat(str_len(Temp), Temp,
                                   str_len(")"), ")",
                                   str_len(Year), Year);
            
                        str_concat(str_len("Suggestion: "), "Suggestion: ",
                                   str_len(Movie->Title), Movie->Title,
                                   str_len(Temp), Temp);
                        
                        str_concat(str_len(Temp), Temp,
                                   str_len(Year), Year,
                                   str_len(Text), Text);
                        
                        UserAnswer = AppMemory->PlatformMessageValidation(Thread, Title, Text);
                        
                        if(UserAnswer == 1 && AppMemory->PlatformWriteEntireFile)
                        {                            
                            if(AppMemory->PlatformWriteEntireFile(Thread,
                                                                  AppState->MoviesSelectedPath,
                                                                  sizeof(movie), Movie))
                            {
                                str_copy(str_len(Movie->IdFile), Movie->IdFile,
                                         str_len(IdFile), IdFile);

                                while(ContentsSizeLeft > 0 &&
                                      str_are_equal(str_len(Movie->IdFile), Movie->IdFile,
                                                    str_len(IdFile), IdFile))
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
         // TODO(Axel): Ugly, create the wsprint like function or use sprint
        //              in order to get rid of most Buffers
        u32 ContentsSizeLeft = FileRead.ContentsSize;
        movie *Movie = (movie*)FileRead.Contents;
        char YearWithParenthesis[10];
        char YearWithParenthesis2[10];

        while(ContentsSizeLeft)
        {

            str_concat(str_len("("), "(",
                       str_len(Movie->Year), Movie->Year,
                       str_len(YearWithParenthesis), YearWithParenthesis);

            str_concat(str_len(YearWithParenthesis), YearWithParenthesis,
                       str_len(")"), ")",
                       str_len(YearWithParenthesis2), YearWithParenthesis2);
            
            char Temp[MAX_PATH_APP];
            str_concat(str_len(Movie->Title), Movie->Title,
                       str_len(YearWithParenthesis2), YearWithParenthesis2,
                       str_len(Temp), Temp);

            char FolderName[150];
            remove_disallowed_chars(str_len(Temp), Temp, FolderName);
            
            char FolderPath[MAX_PATH_APP];            
            build_file_path(AppState, FolderName, FolderPath);

            if(AppMemory->PlatformCreateFolder(Thread, AppState, FolderPath))
            {
                char FileName[150];
                str_copy_until(Movie->IdFile, FileName, "_");

                char FilePath[MAX_PATH_APP];
                build_file_path(AppState, FileName, FilePath);
                
                str_concat(str_len(FolderPath), FolderPath,
                           str_len("\\"), "\\",
                           str_len(Temp), Temp);
                
                char NewFilePath[MAX_PATH_APP];                
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
    char Query[150];
    // Removing file's extension and the unique identifier (AppState.FileCount)
    // NOTE(Axel): Removing Year is better for Rotten tomatoes query searching
    // TODO(Axel): Maybe handle the '(' before the year
    // TODO(Axel): Make logic if the file name has no year in the title but number part of the title    
    b32 StartYearIndex = str_first_number_char_from_end(UniqueFileName, '.');
    if(StartYearIndex)
    {    
        // Movie has number in title
        str_cut(UniqueFileName, Query, 0, StartYearIndex);
    }
    else
    {
        str_cut_after_from_end(UniqueFileName, Query, ".");
    }
    
    if(str_len(Query) == 0) return Result;

    // TODO(Axel) Rotten seems to work better without the year of the movie in the search
    http_info HTTPInfo = {};
    str_concat(str_len("https://www.rottentomatoes.com/search?search="), "https://www.rottentomatoes.com/search?search=",
             str_len(Query), Query,
             str_len(HTTPInfo.Query), HTTPInfo.Query);
    
    if(!AppMemory->PlatformMakeHTTPRequest(Thread, &HTTPInfo, UniqueFileName, AppState,
                                       AppMemory->PermanentMemory,
                                          (int)AppMemory->PermanentStorageSize))
    {
        return Result;
    }
    
    if(HTTPInfo.ResponseData)
    {
        // NOTE(Axel): HTML bloated with script, search is not performant. we skip content (dumb optimization)
        char *HTTPPointer = HTTPInfo.ResponseData + HTML_CHAR_SKIP_OPTIMIZATION;
                
        if(HTTPPointer)
        {
            IsAMatch = str_search_from_start("Movies</h2>", str_len("Movies</h2>"), HTTPPointer, &MatchAt);
            HTTPPointer += (MatchAt + 1);

            b32 HasMovieLeft = true;
            for(;;)
            {
                movie Movie = {};
                str_copy(str_len(UniqueFileName), UniqueFileName,
                         str_len(Movie.IdFile), Movie.IdFile);
                // NOTE(Axel): Year
                IsAMatch = str_search_from_start("releaseyear=\"", str_len("releaseyear=\""),
                                                 HTTPPointer, &MatchAt);
                // Assert(IsAMatch);
                if(!IsAMatch)
                {
                    if(AppMemory->PlatformWriteEntireFile)
                    {
                        AppMemory->PlatformWriteEntireFile(Thread, AppState->FilesFailedPath,
                                                           sizeof(movie), &Movie);
                    }
                     
                    break;
                }

                HTTPPointer += (MatchAt + 1);
                
                char Temp[150];
                IsAMatch = str_copy_until(HTTPPointer, Temp, "\"");
                str_trim_end(str_len(Temp), Temp, str_len(Movie.Year), Movie.Year);
                // NOTE(Axel): Title
                str_search_from_start("slot=\"title\">",
                                      str_len("slot=\"title\">"),
                                      HTTPPointer, &MatchAt);
                HTTPPointer +=  MatchAt;                

                //TODO(Axel): This is dumb, find a better way to handle it
                char Temp2[150];
                char Temp3[150];
                str_copy_by_char(HTTPPointer, Temp, ">", "<");

                // NOTE(AXEL): Multiple white space by one space
                replace_empties(str_len(Temp), Temp, str_len(Temp2), Temp2);
                if(str_contains(Temp2, ";"))
                {
                    str_convert_to_ascii(str_len(Temp2), Temp2, str_len(Temp3), Temp3);
                    remove_char_in_text(Temp3, Movie.Title, "\n");
                }
                else
                {
                    remove_char_in_text(Temp2, Movie.Title, "\n");
                }
                
                int IndexUl = 0;
                int IndexSearch = 0;
                str_search_from_start("<search", str_len("<search"), HTTPPointer, &IndexSearch);
                str_search_from_start("</ul", str_len("</ul"), HTTPPointer, &IndexUl);
                
                if(IndexUl < IndexSearch)
                {
                    // NOTE(Axel): We passed the result search section
                    break;
                }
                Assert(IsAMatch);

               if(AppMemory->PlatformWriteEntireFile)
               {                   
                   if(!AppMemory->PlatformWriteEntireFile(Thread, AppState->MetadataPath,
                                                         sizeof(movie), &Movie))
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



