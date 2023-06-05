#include"purificator.h"
#include"string_op.h"

extern "C" GET_MOVIE_DATA(GetMovieData)
{
    b32 Result = false;
    
    AppMemory->PlatformMakeHTTPRequest(Thread, HTTPInfo, File, Movie);
    
    b32 IsAMatch          = false;
    b32 FileWriteFlag     = true;
    b32 Scrapping         = true;
    b32 HasFoundMovieList = false;
    int MatchAt           = 0;
    int SearchLen = 0;
    
    SearchLen = str_copy("\"/title/", HTTPInfo->Search);
    IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLen, HTTPInfo->ResponseData, &MatchAt);

    HasFoundMovieList = true;
    char *HTTPPointer = HTTPInfo->ResponseData;

    int MovieCount = str_search_from_start_count("\"/title/", HTTPPointer);
                
    for(int MovieIndex = 0; MovieIndex < MovieCount; ++MovieIndex)
    {
        // TODO(Axel): It smells of a function needed to be created
        // TODO(Axel): At first Index we do a not needed search (we already searched fo it)
        SearchLen = str_copy("/title/", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLen , HTTPPointer, &MatchAt);
        if(!IsAMatch) continue;
        HTTPPointer += MatchAt + SearchLen;

        SearchLen = str_copy("/", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLen, HTTPPointer, &MatchAt);
        str_cut(HTTPPointer, 0, (MatchAt - SearchLen), Movie->IdIMDB);

        SearchLen = str_copy(">", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLen, HTTPPointer, &MatchAt);
        HTTPPointer +=  MatchAt;

        char TitleTemp[sizeof(Movie->Title)];
        SearchLen = str_copy("<", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLen, HTTPPointer, &MatchAt);
        str_cut(HTTPPointer, 0, (MatchAt - SearchLen), TitleTemp);

        HTTPPointer +=  MatchAt;

        // NOTE(Axel): Retrieve movie's year suggested
        // TODO: Check if it's a short movie or not
        SearchLen = str_copy("<span", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLen, HTTPPointer, &MatchAt);
        HTTPPointer += MatchAt;

        SearchLen = str_copy(">20", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLen, HTTPPointer, &MatchAt);
        if(!IsAMatch)
        {
            SearchLen = str_copy(">19", HTTPInfo->Search);
            IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLen, HTTPPointer, &MatchAt);
        }
        HTTPPointer += (MatchAt + 1);
                    
        SearchLen = str_copy("<", HTTPInfo->Search);
        IsAMatch = str_search_from_start(HTTPInfo->Search, SearchLen, HTTPPointer, &MatchAt);
        str_cut_after_from_start(HTTPPointer, HTTPInfo->Search, SearchLen, Movie->Year);
        HTTPPointer += (MatchAt + 1);
                    
        Assert(IsAMatch);

    }

    return Result;
}



