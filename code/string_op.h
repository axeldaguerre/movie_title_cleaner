#if !defined(STRING_OP_H)

#define STRING_OP_H
// TODO(Axel): Make the path size macro definition in one place only
#define MAX_PATH_PURI 260

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

// NOTE(Axel): This function is really not optimized for big strings Text
// TODO: Make better readibility
inline b32 str_search_from_end(char *Pattern, char *Text, int *AtChar)
{   
    char TextInverse[MAX_PATH_PURI];
    str_inverse(Text, TextInverse);
    char PatInverse[MAX_PATH_PURI];
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
//             you can optimize its perf by having the start of the pattern the most unique possible,
//             by doing it you minimize the number of match.
// TODO(Axel): Implement perf log
inline b32 str_search_from_start(char *Pattern, int PatLen, char *Text, int *Char)
{
    b32 Result    = false;
    *Char         = 0;
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
        
        if(IndexPat == PatLen)
        {
            if(IndexPat != 0 && IndexText >= PatLen)
            {
                *Char = IndexText - PatLen;
            }
            else
            {
                *Char = 0;
            }
            Result = true;
            break;
        }
    }
    
    return Result;
}

inline b32 str_search_from_start(char *Pattern, int PatLen, char *Text,
                                 int Start, int End, int *Char)
{
    Assert(Start < End);
    
    b32 Result    = false;
    *Char         = 0;
    int TextLen   = str_len(Text) - Start;
    Text += Start;
    int IndexText = Start;
    int IndexPat  = 0;

    for(;IndexText < TextLen && IndexPat < PatLen; ++IndexText)
    {
        if(IndexText == End)
        {
            break;
        }
        
        if(char_at(Pattern, IndexPat) == char_at(Text, IndexText))
        {
            ++IndexPat;
        }
        else
        {
            IndexText -= IndexPat;
            IndexPat = 0;
        }
        
        if(IndexPat == PatLen)
        {
            if(IndexPat !=0)
            {
                *Char = IndexText - PatLen;
            }
            else
            {
                *Char = 0;
            }
            Result = true;
            break;
        }
    }
    
    return Result;
}

inline b32 str_search_from_start(char *Pattern, int PatLen, char *Text,
                                 int Start, int *Char)
{
    b32 Result    = false;
    *Char         = 0;
    int TextLen   = str_len(Text) - Start;
    Text += Start;
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
        
        if(IndexPat == PatLen)
        {
            if(IndexPat !=0)
            {
                *Char = IndexText - PatLen;
            }
            else
            {
                *Char = 0;
            }
            Result = true;
            break;
        }
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

inline int str_copy(char *String, char *Dest)
{
    int Result = 0;
    for(char *Scan = String; *Scan; ++Scan)
    {
        *Dest++ = *Scan;
        ++Result;
    }
    *Dest = 0;
    return Result;
}


inline int str_copy(char *String, char *Dest, int Start, int End)
{
    Assert(Start < End);
    int Result = 0;
    String += Start;
    
    for(int Index = Start; Index < End; ++Index)
    {
        *Dest++ = *String;
        ++Result;
        ++String;
    }
    *Dest = 0;
    return Result;
}

// NOTE(Axel): Start and End char will be in the return
inline void str_cut(char *String, int Start, int End, char* Dest)
{
    Assert(Start < End);
    String += Start;
    for(int IndexChar = Start; IndexChar <= End; ++IndexChar)
    {
        if(IndexChar > End)
        {
            break;
        }
        else if(IndexChar >= Start || IndexChar < End)
        {
            *Dest = *String;
        }
        ++String;
        ++Dest;
    }
    *Dest = 0;

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


// TODO(Axel): name is too long
inline b32 str_cut_after_from_start(char *String, char* Pattern, int PatLen, char *Dest)
{
    b32 Result = false;
    int MatchCharAt = 0;
    if(str_search_from_start(Pattern, PatLen, String, &MatchCharAt)) return false;
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
    b32 Result = true;
    int AtChar = 0;
    
    if(!str_search_from_end(Pattern, String, &AtChar)) return false;
    
    str_copy(String, Dest);
    
    Dest[AtChar] = 0;
    
    return Result;
}

#endif
