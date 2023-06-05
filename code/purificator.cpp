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

// NOTE(Axel): IMDB use inline <script> tags, it means the html is messy and huge, because of
//             that the substring search algorithms need to be performant in order to avoid the
//             user to wait too long at start (fetch for all movies in the folder at once was the
//             User experience wanted.
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

    // NOTE(Axel): HTML bloated with script, search is not performant. we skip content (dumb optimization)
    char *HTTPPointer = HTTPInfo->ResponseData + HTTM_CHAR_SKIP_OPTIMIZATION;
    
    SearchLength = str_copy("\"/ti", HTTPInfo->Search);
    str_search_from_start(HTTPInfo->Search, SearchLength, HTTPPointer, &MatchAt);
    HTTPPointer += MatchAt;

    Assert(HTTPPointer);

    //  remove_script_tags();
    
    SearchLength = str_copy("\"/ti", HTTPInfo->Search);
    // TODO(Axel): Current performance is not acceptable, implement a non-brute force algorithm
    IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength, HTTPInfo->ResponseData, &MatchAt);
    int StartCutIndex = MatchAt;
    Assert(IsAMatch);
    HTTPPointer += MatchAt;
    
   
    SearchLength = str_copy("<button", HTTPInfo->Search);
    IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength, HTTPInfo->ResponseDataPointer, &MatchAt);
    int EndCutIndex = MatchAt;

    int MovieCount = str_search_from_start_count("\"/ti", HTTPPointer);
                
    for(int MovieIndex = 0; MovieIndex < MovieCount; ++MovieIndex)
    {
        // TODO(Axel): It smells of a function needed to be created
        // TODO(Axel): At first Index we do a not needed search (we already searched fo it)
        SearchLength = str_copy("/ti", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength , HTTPPointer, &MatchAt);
        if(!IsAMatch) continue;
        HTTPPointer += MatchAt + SearchLength;

        SearchLength = str_copy("/", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength, HTTPPointer, &MatchAt);
        str_cut(HTTPPointer, 0, (MatchAt - SearchLength), Movie->IdIMDB);

        SearchLength = str_copy(">", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength, HTTPPointer, &MatchAt);
        HTTPPointer +=  MatchAt;

        char TitleTemp[sizeof(Movie->Title)];
        SearchLength = str_copy("<", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength, HTTPPointer, &MatchAt);
        str_cut(HTTPPointer, 0, (MatchAt - SearchLength), TitleTemp);

        HTTPPointer +=  MatchAt;

        // NOTE(Axel): Retrieve movie's year suggested
        // TODO: Check if it's a short movie or not
        SearchLength = str_copy("<span", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength, HTTPPointer, &MatchAt);
        HTTPPointer += MatchAt;

        SearchLength = str_copy(">20", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength, HTTPPointer, &MatchAt);
        if(!IsAMatch)
        {
            SearchLength = str_copy(">19", HTTPInfo->Search);
            IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength, HTTPPointer, &MatchAt);
        }
        HTTPPointer += (MatchAt + 1);
                    
        SearchLength = str_copy("<", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLength, HTTPPointer, &MatchAt);
        str_cut_after_from_start(HTTPPointer, HTTPInfo->Search, SearchLength, Movie->Year);
        HTTPPointer += (MatchAt + 1);
                    
        Assert(IsAMatch);

    }

    return Result;
}



