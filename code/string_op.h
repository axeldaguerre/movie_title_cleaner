#if !defined(STRING_OP_H)

#define STRING_OP_H

#define MAX_PATH_APP 260

//TODO(Axel): Be more consistent on function paramaters (Ex: StringCount before Strin)
inline int str_len(char *String)
{
     int Count = 0;
    while(*String++)
    {
        ++Count;
    }
    return(Count);
}

inline void str_concat(size_t SourceACount, char *SourceA,
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
inline b32 str_search_from_end(char *Pattern, char *Text, int *AtChar)
{   
    char TextInverse[MAX_PATH_APP];
    str_inverse(Text, TextInverse);
    char PatInverse[MAX_PATH_APP];
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
// TODO(Axel): respect parameter usual order
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
            *Char = IndexText;

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


inline void remove_char_in_text(char *Text, char *Dest, char *Char)
{
    Assert(str_len(Char) == 1);
    for(char *Scan = Text; *Scan; ++Scan)
    {
        if(*Scan != *Char)
        {
            *Dest++ = *Scan;
        }
    }
    *Dest = 0;
}

inline int str_copy(int StringLength, char *String, int DestLength, char *Dest)
{
    Assert(Dest)
    int Result = 0;
    for(char *Scan = String; *Scan; ++Scan)
    {
        *Dest++ = *Scan;
        ++Result;
    }
    *Dest = 0;
    return Result;
}

inline int str_copy_by_char(char *String, char *Dest, char *FromChar, char *UntilChar)
{
    // TODO(Axel): Find a way to avoid a string or make the algorithm works with string
    Assert(str_len(UntilChar) == 1);
    int Result = 0;
    // TODO(Axel): Algo should handle handle same char
    Assert(FromChar != UntilChar);

    b32 HasReachStart = false;
    for(char *Scan = String; *Scan; ++Scan)
    {
        if(char_at(Scan, 0) == *UntilChar)
        {
            ++Result;
            break;
        }
        else if(char_at(Scan, 0) == *FromChar)
        {
            ++Scan;
            HasReachStart = true;
        }
        
        if(HasReachStart)
        {
            *Dest++ = *Scan;
            ++Result;
        }
    }
    *Dest = 0;
    return Result;
}

inline int str_copy_until(char *String, char *Dest, char *UntilChar)
{
    int Result = 0;
    
    for(char *Scan = String; *Scan; ++Scan)
    {
        if(*UntilChar == *Scan)
        {
            ++Result;
            break;
        }
        *Dest++ = *Scan;
        ++Result;
    }
    *Dest = 0;
    return Result;
}

inline int str_copy(char *String, char *Dest, int Start, char *UntilChar)
{
    int Result = 0;
    String += Start;
    Assert(Start < str_len(String));
    
    for(char *Scan = String; *Scan; ++Scan)
    {
        if(*UntilChar == *Scan)
        {
            ++Dest;
            break;
        }
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
inline void str_cut(char *String, char* Dest, int Start, int End)
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

inline int str_index_of_terminating (int StringCount, char *String)
{
    int Result = 0;
    int Index = 0;
    while(Index < StringCount)
    {
        if(char_at(String, Index))
        {
            Result = Index;    
        }
        ++Index;        
    }
    
    return Result;
}

inline b32 str_are_equal(int StringCountA, char *StringA, int StringCountB, char *StringB)
{
    b32 Result = true;

    for(int Index = 0; Index < StringCountA; ++Index)
    {
        if(*StringA++ != *StringB++)
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
    if(!str_search_from_start(Pattern, PatLen, String, &MatchCharAt)) return false;
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

inline b32 str_cut_after_from_end(char *String,  char *Dest, char* Pattern)
{
    b32 Result = true;
    int AtChar = 0;
    
    if(!str_search_from_end(Pattern, String, &AtChar)) return false;
    
    str_copy(str_len(String), String, str_len(Dest), Dest);
    
    Dest[AtChar] = 0;
    
    return Result;
}

// TODO(Axel): This function has a bug, it doesn't terminate the Dest with 0
inline void str_trim_end(int StringCount, char *String, int DestCount, char *Dest)
{

    int Index = StringCount - 1;
    for(char *Scan = String + StringCount; ;--Scan)
    {
        if(char_at(String, Index) == ' ')
        {
            *Scan = 0;
        }
        else
        {
            break;
        }

        --Index;        
    }
    
    str_copy(StringCount, String, DestCount, Dest);
}

inline void replace_empties(int StringCount, char *String, int DestCount, char *Dest)
{
    // TODO(Axel): Find a way to avoid the dest being too small
    //             to get the String **Trimed**. 
    b32 Result = true;
    // TODO(Axel): What Should we do on the size of the string Temp
    int Index = 0;
    char Temp[100];
    // STUDY(Axel): We should be able to move the pointer of Temp without doing below
    char *Pointer = Temp;
    for(char *Scan = String; *Scan; ++Scan)
    {
        
        if(char_at(String, Index) == ' ')
        {
            
            if(char_at(String, Index+1) == ' ')
            {
                 ++Index;
                 continue;
            }            
        }
        ++Index;
        *Pointer++ = *Scan;
    }
    *Pointer = 0;

    int TempCount = str_len(Temp);
    
    str_trim_end(TempCount, Temp, DestCount, Dest);
}

// TODO(Axel): Issues occured when you want to deal with char '' and char ""
// NOTE(Axel): Works only with ''         
inline b32 str_contains(char *String, char *Char)
{
    b32 Result = false;
    b32 Index = 0;
    for(char *Scan = String; *Scan; ++Scan)
    {
        if(*Scan == *Char)
        {
            Result = Index;
            break;
        }
        ++Index;
    }

    return Result;
}

struct ascii_table
{
    char **Encodings;
    int AsciiCount;
};

// TODO(Axel): Use preprocessor if possible and needed
// NOTE(Axel): Index 0 is a non-existing term, and permit nice
//             initialization for index's logics
static char *Raw[] =
{
    "&#0;", "&#1;", "&#2;", "&#3;", "&#4;", "&#5;", "&#6;", "&#7;",
    "&#8;", "&#9;", "&#10;", "&#11;", "&#12;", "&#13;", "&#14;", "&#15;",
    "&#16;", "&#17;", "&#18;", "&#19;", "&#20;", "&#21;", "&#22;", "&#23;",
    "&#24;", "&#25;", "&#26;", "&#27;", "&#28;", "&#29;", "&#30;", "&#31;",
    "&#32;", "&#33;", "&#34;", "&#35;", "&#36;", "&#37;", "&#38;", "&#39;",
    "&#40;", "&#41;", "&#42;", "&#43;", "&#44;", "&#45;", "&#46;", "&#47;",
    "&#48;", "&#49;", "&#50;", "&#51;", "&#52;", "&#53;", "&#54;", "&#55;",
    "&#56;", "&#57;", "&#58;", "&#59;", "&#60;", "&#61;", "&#62;", "&#63;",
    "&#64;", "&#65;", "&#66;", "&#67;", "&#68;", "&#69;", "&#70;", "&#71;",
    "&#72;", "&#73;", "&#74;", "&#75;", "&#76;", "&#77;", "&#78;", "&#79;",
    "&#80;", "&#81;", "&#82;", "&#83;", "&#84;", "&#85;", "&#86;", "&#87;",
    "&#88;", "&#89;", "&#90;", "&#91;", "&#92;", "&#93;", "&#94;", "&#95;",
    "&#96;", "&#97;", "&#98;", "&#99;", "&#100;", "&#101;", "&#102;", "&#103;",
    "&#104;", "&#105;", "&#106;", "&#107;", "&#108;", "&#109;", "&#110;", "&#111;",
    "&#112;", "&#113;", "&#114;", "&#115;", "&#116;", "&#117;", "&#118;", "&#119;",
    "&#120;", "&#121;", "&#122;", "&#123;", "&#124;", "&#125;", "&#126;", "&#127;",
    "&amp;"
};
   
static char *Replacement[] =
{
    "\0", "\x01", "\x02", "\x03", "\x04", "\x05", "\x06", "\a", "\b", "\t",
    "\n", "\v", "\f", "\r", "\x0E", "\x0F", "\x10", "\x11", "\x12", "\x13",
    "\x14", "\x15", "\x16", "\x17", "\x18", "\x19", "\x1A", "\x1B", "\x1C",
    "\x1D", "\x1E", "\x1F", " ", "!", "\"", "#", "$", "%", "&", "\'", "(", ")",
    "*", "+", ",", "-", ".", "/", "0", "1", "2", "3", "4", "5", "6", "7", "8",
    "9", ":", ";", "<", "=", ">", "?", "@", "A", "B", "C", "D", "E", "F", "G",
    "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
    "W", "X", "Y", "Z", "[", "\\", "]", "^", "_", "`", "a", "b", "c", "d", "e",
    "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
    "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "\x7F", "&"
};

static ascii_table get_ascii_table()
{
    ascii_table Result = {};
    
    Result.AsciiCount = ArrayCount(Raw);
    Result.Encodings = Raw;
    
    return Result;
}

static ascii_table get_replacement_ascii_table()
{
    ascii_table Result = {};
    Result.AsciiCount = ArrayCount(Raw);
    Result.Encodings = Replacement;
    return Result;
}

inline void str_convert_to_ascii(int StringCount, char *String, int DestCount, char *Dest)
{
    ascii_table Ascii = get_ascii_table();
    ascii_table AsciiRep = get_replacement_ascii_table();

    int StringLen = str_len(String);
    int CharAt = 0;
    for(int Index = 0; Index <= StringLen ; ++Index)
    {
        if(*String!= '&')
        {
            *Dest++ = *String++;
            continue;
        }
        else if(str_search_from_start("&", str_len("&"), String, &CharAt))
        {            
            int CharEnd = 0;
            str_search_from_start(";", str_len(";"), String, &CharEnd);
            
            char AsciiTermString[6];
            int AsciiTermStringLen = (CharEnd - CharAt) + 1;
            str_cut(String, AsciiTermString, CharAt, CharEnd);
            
            for(int IndexTable = 0; IndexTable <= Ascii.AsciiCount; ++IndexTable)
            {
                char AsciiTermTable[6];
                char AsciiTermRepl[6];
                int AsciiTermTableLen = str_len(Ascii.Encodings[IndexTable]);
                
                str_copy(AsciiTermTableLen, Ascii.Encodings[IndexTable],
                         str_len(AsciiTermTable), AsciiTermTable);
                
                if(str_are_equal(AsciiTermStringLen, AsciiTermString,
                                 AsciiTermTableLen, AsciiTermTable))
                {
                    str_copy(str_len(AsciiRep.Encodings[IndexTable]), AsciiRep.Encodings[IndexTable],
                             str_len(AsciiTermRepl), AsciiTermRepl);
                    
                    str_copy(str_len(AsciiTermRepl), AsciiTermRepl, str_len(Dest), Dest);
                    
                    ++Dest;
                    String += AsciiTermStringLen;
                    break;
                }
            }
        }
    }
}
#endif
