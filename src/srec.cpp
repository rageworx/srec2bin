#include "srec.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>

#include "stools.h"

SRECReader::SRECReader()
 :  indexDataStart(0),
    indexDataEnd(0),
    minimumAddress(0),
    maximumAddress(0)
{
    items.clear();
}

SRECReader::~SRECReader()
{
    if ( items.size() > 0 )
    {
        Unload();
    }
}

bool SRECReader::Load(string &fileName)
{
    ifstream fStrm;

    fStrm.open(fileName.c_str(), ios::binary | ios::in | ios::app);

    if ( fStrm.is_open() == true )
    {
        fStrm.seekg(0,ios::end);
        int charDataSize = fStrm.tellg();
        fStrm.seekg(0,ios::beg);

        char* charData = new char[charDataSize];
        fStrm.read(charData, charDataSize);
        fStrm.close();

        bool retB = Load(charData, charDataSize);

        delete[] charData;

        return retB;
    }

    return false;
}

bool SRECReader::Load(vector<char> &charArray)
{
#ifdef __CYGWIN__
    return Load(&charArray.front(), charArray.size());
#else
    return Load(charArray.data(), charArray.size());
#endif
}

bool SRECReader::Load(char* charPtr, int charSize)
{
    string          srcStr = charPtr;
    string          delimiters = "\r\n";
    vector<string>  tokens;

    tokens = tokenize(srcStr, delimiters);

    if ( tokens.size() > 0 )
    {
        return parse(tokens);
    }

    return false;

}

bool SRECReader::Unload()
{
    minimumAddress = 0;
    maximumAddress = 0;
    indexDataStart = 0;
    indexDataEnd   = 0;

    if ( items.size() > 0 )
    {
        items.clear();
    }

    lastErrorStr.clear();

    return true;
}

SRECItem* SRECReader::GetRecord(unsigned int index)
{
    SRECItem* retPtr = NULL;

    if ( index >= items.size() )
        return NULL;

    retPtr = &items[index];

    return retPtr;
}

unsigned int SRECReader::GetRecordSize()
{
    return (unsigned int)items.size();
}

unsigned int SRECReader::GetDataSize()
{
    unsigned int retSize = 0;

    for (unsigned int cnt=0;cnt<items.size();cnt++)
    {
        if ( ( items[cnt].type > 0) && ( items[cnt].type <= 3 ) )
            retSize += items[cnt].dataLen;
    }

    return retSize;
}

bool SRECReader::parse(vector<string> &srcTextLines)
{
#ifdef _DEBUG
    printf("\n#SRECROD.DEBUG#\n");
#endif
    int lineCount = srcTextLines.size();

#ifdef _DEBUG
    printf(" ... total identied lines = %d\n",lineCount);
#endif

    if ( lineCount == 0 )
        return false;

    items.clear();

    unsigned char byteQueue = 0;

    for ( int cnt=0; cnt<lineCount; cnt++)
    {
#ifdef _DEBUG
        printf("\r ... parsing line : %d/%d   ",cnt+1 , lineCount);
        fflush(stdout);
#endif

        SRECItem    newItem = {0};
        string      tmpLine = srcTextLines[cnt];
        int         tmpLineSize = tmpLine.size();

        byteQueue = 0;

        // check it starts with "S" ..
        if ( tmpLineSize > 0 )
        {
            if ( tmpLine[0] != 'S' )
            {
#ifdef _DEBUG
                printf("first charactor is not 'S' !\n");
#endif
                break;
            }
        }

        byteQueue++;    // = 1

        // a line must be bigger than 6 !
        if ( tmpLineSize > 6 )
        {
            newItem.type = (int)(tmpLine[byteQueue] - SREC_TYPE_BLOCK_HEADER);

            byteQueue++; // = 2

            char byteCount[3] = {0};

            byteCount[0] = tmpLine[byteQueue + 0];
            byteCount[1] = tmpLine[byteQueue + 1];

            byteQueue += 2; // = 4

            int dataLen = 0;

            sscanf(byteCount, "%X", &dataLen);

            newItem.dataLen = dataLen;

            if ( dataLen > 0 )
            {
                // dataLen included with checkSum. real data size is (dataLen - 1).
                int reqSize = ( dataLen * 2 ) + 3;
                if ( reqSize < tmpLineSize )
                {
                    // read address
                    char wordCount[9] = {0};

                    switch(newItem.type)
                    {
                        /// S1 = 2bytes address
                        case 1:
                            memcpy(wordCount,&tmpLine[byteQueue],4);
                            byteQueue += 4;
                            break;

                        /// S2 = 3bytes address
                        case 2:
                            memcpy(wordCount,&tmpLine[byteQueue],6);
                            byteQueue += 6;
                            break;

                        /// S3 = 4byte address
                        case 3:
                            memcpy(wordCount,&tmpLine[byteQueue],8);
                            byteQueue += 8;
                            break;
                    }

                    unsigned int addr = 0;

                    sscanf(wordCount, "%X", &addr);

                    newItem.address = addr;

                    if ( ( newItem.type > 0 ) && ( newItem.type <= 3) )
                    {
                        if ( addr < minimumAddress )
                        {
                            minimumAddress = addr;
                        }
                        else
                        if ( ( addr + dataLen ) > maximumAddress )
                        {
                            maximumAddress = addr + dataLen;
                        }
                    }

                    // read codes/datas (n bytes)
                    unsigned int getCount = dataLen - 3;

                    if ( tmpLine.size() > ( getCount + 8 ) )
                    {
                        static unsigned int scnt;

                        for ( scnt=0; scnt<getCount; scnt++ )
                        {
                            memset(byteCount, 0, 3);

                            byteCount[0] = tmpLine[ byteQueue + (scnt * 2) + 0];
                            byteCount[1] = tmpLine[ byteQueue + (scnt * 2) + 1];

                            unsigned char byteData = 0;

                            sscanf(byteCount, "%X", (unsigned int*)&byteData);

                            newItem.data[scnt] = byteData;
                        }

                        // read check sum
                        memset(byteCount, 0, 3);

                        byteCount[0] = tmpLine[ byteQueue + (scnt * 2) + 0];
                        byteCount[1] = tmpLine[ byteQueue + (scnt * 2) + 1];

                        unsigned char byteCheckSum = 0;

                        sscanf(byteCount, "%X", (unsigned int*)&byteCheckSum);

                        newItem.checkSum = byteCheckSum;

                        items.push_back(newItem);

                    }
#ifdef _DEBUG
                    else
                    {
                        printf("\nError : data length is not enough !\n");
                        printf("   line string : %s\n", tmpLine.c_str());
                        printf("   data length : %d\n", dataLen);
                        printf("   line length : %d\n", tmpLine.size());
                        fflush(stdout);
                    }
#endif
                }
            }
        }
        else
        {
            char tmpStr[80] = {0};
            sprintf(tmpStr,
                    "Line %d is not enought to read as SRECORD = %u",
                    cnt,
                    (unsigned)tmpLine.size());
            lastErrorStr = tmpStr;
        }
    }

    // find first S1/S2/S3 record index.
    int seekSize    = items.size();

    for(int cnt=0;cnt<seekSize;cnt++)
    {
        if ( items[cnt].type > 0 )
        {
            indexDataStart = cnt;
            break;
        }
    }

    for(int cnt=seekSize-1;cnt>=0;cnt--)
    {
        if ( ( items[cnt].type > 0) && ( items[cnt].type <= 3) )
        {
            indexDataEnd = cnt;
            break;
        }
    }

#ifdef _DEBUG
    printf("\n");
    printf("\n");
    printf("SRECROD.DEBUG______________\n");
    printf("   addresse starts in 0x%08X at %d \n", items[0].address, indexDataStart);
    printf("   addresse ends   in 0x%08X at %d \n", items[items.size()-2].address + items[items.size()-1].dataLen, indexDataEnd);
#endif

    return true;
}

char* SRECReader::GetLastError()
{
    return (char*)lastErrorStr.c_str();
}
