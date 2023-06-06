#include"purificator.h"
#include"string_op.h"

#define HTTM_CHAR_SKIP_OPTIMIZATION 200000

inline void remove_script_tag(char *String, char* Dest)
{
    char SearchStart[] = "<script";
    int SearchStartLength = str_len(SearchStart);
    char SearchEnd [] = "/script>";
    int SearchEndLength = str_len(SearchEnd);
    int IndexEnd = 0;
    int IndexStart = 0;

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

    app_state *State = (app_state*)AppMemory->PermanentMemory;

    if(HTTPInfo->ResponseData)
    {
        // NOTE(Axel): HTML bloated with script, search is not performant. we skip content (dumb optimization)
        char *HTTPPointer = HTTPInfo->ResponseData + HTTM_CHAR_SKIP_OPTIMIZATION;

        b32 HasMovieLeft = true;
        while(HasMovieLeft)
        {
            IsAMatch = str_search_from_start("\"/title/", str_len("\"/title/"), HTTPPointer, &MatchAt);
            HTTPPointer += (MatchAt + 1);
        
            HTTPPointer += str_copy(HTTPPointer, Movie->IdIMDB, 0, "/");

            str_copy_by_char(HTTPPointer, Movie->Title, ">", "<");
            HTTPPointer +=  MatchAt;

            IsAMatch = str_search_from_start("<span", str_len("<span"), HTTPPointer, &MatchAt);
            HTTPPointer += MatchAt;

            IsAMatch = str_search_from_start(">20", str_len(">20"), HTTPPointer, &MatchAt);
            if(!IsAMatch)
            {
                IsAMatch = str_search_from_start(">19", str_len(">19"), HTTPPointer, &MatchAt);
            }
            HTTPPointer += (MatchAt + 1);

            IsAMatch = str_search_from_start("<", str_len("<"), HTTPPointer, &MatchAt);
            HTTPPointer += (MatchAt + 1);
                    
            Assert(IsAMatch);
        }
                    

    }

    return Result;
}



