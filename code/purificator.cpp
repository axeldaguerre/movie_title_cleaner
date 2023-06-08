#include "purificator.h"
#define HTML_CHAR_SKIP_OPTIMIZATION 50000


extern "C" CLEAN_FILE_MOVIE(CleanFileMovie)
{
    app_file Result = {};

    debug_read_file_result FileRead = AppMemory->PlatformReadEntireFile(Thread, File->MetadataPath);

    if(FileRead.ContentsSize != 0)
    {
        movie *MovieExtracted = (movie*)FileRead.Contents;
        u32 ContentsSizeLeft = FileRead.ContentsSize;
       
        while(ContentsSizeLeft)
        {
            AppMemory->PlatformCreateFolder(Thread, Movie->Title, AppState, Movie, File);
            AppMemory->PlatformMoveFile(Thread, Movie->Title, File, Movie);

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

    AppMemory->PlatformMakeHTTPRequest(Thread, HTTPInfo, File, Movie);

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

                IsAMatch = str_copy_until(HTTPPointer, Movie->Year, "\"");

                // NOTE(Axel): Title
                str_search_from_start("slot=\"title\">",
                                      str_len("slot=\"title\">"),
                                      HTTPPointer, &MatchAt);
                HTTPPointer +=  MatchAt;                
                char Temp[150];
                char Temp2[150];
                str_copy_by_char(HTTPPointer, Temp, ">", "<");

                // NOTE(AXEL): Multiple white space by one space
                replace_empties(Temp, Temp2);
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
                   AppMemory->PlatformWriteEntireFile(Thread, File->MetadataPath,
                                                      sizeof(movie), Movie);
               }
            }
        }                    
    }

    return Result;
}



