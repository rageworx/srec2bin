#pragma once
#ifndef __SREC_H__
#define __SREC_H__

/*******************************************************************************
**   SREC reader class
** ===========================================================================
**
**   A Motorola's S-record reading class for gcc STL.
**
**   (C)Copyright 2012 Raphael KIM (rageworx@gamil.com)
**
*******************************************************************************/

#include <string>
#include <vector>

using namespace std;

#define SREC_RECORD_BUFFER_SIZE     768
#define SREC_TYPE_OFFSET            1
#define SREC_TYPE_LEN               1
#define SREC_COUNT_OFFSET           2
#define SREC_COUNT_LEN              2
#define SREC_ADDRESS_OFFSET         4
#define SREC_CHECKSUM_LEN           2
#define SREC_MAX_DATA_LEN           64
#define SREC_MAX_ADDRESS_LEN        8
#define SREC_ASCII_HEX_BYTE_LEN     2
#define SREC_START_CODE_OFFSET      0
#define SREC_START_CODE             'S'

#define SREC_TYPE_S0                0
#define SREC_TYPE_S1                1
#define SREC_TYPE_S2                2
#define SREC_TYPE_S3                3
#define SREC_TYPE_S4                4
#define SREC_TYPE_S5                5
#define SREC_TYPE_S6                6
#define SREC_TYPE_S7                7
#define SREC_TYPE_S8                8
#define SREC_TYPE_S9                9

#define SREC_TYPE_BLOCK_HEADER      '0'
#define SREC_TYPE_DATA_SEQUENCE16   '1'
#define SREC_TYPE_DATA_SEQUENCE24   '2'
#define SREC_TYPE_DATA_SEQUENCE32   '3'
#define SREC_TYPE_RECORD_COUNT      '5'
#define SREC_TYPE_END_BLOCK32       '7'
#define SREC_TYPE_END_BLOCK24       '8'
#define SREC_TYPE_END_BLCOK16       '9'

typedef struct _SRECItem
{
    int             type;
    int             dataLen;
    unsigned int    address;
    unsigned char   data[SREC_MAX_DATA_LEN / 2];
    unsigned char   checkSum;
}SRECItem;

class SRECReader
{
    public:
        SRECReader();
        virtual ~SRECReader();

    public:
        bool Load(string &fileName);
        bool Load(vector<char> &charArray);
        bool Load(char* charPtr, int charSize);
        bool Unload();

    public:
        SRECItem* GetRecord(unsigned int index);

    public:
        char* GetLastError();

    public:
        unsigned int GetRecordSize();
        unsigned int GetDataSize();
        unsigned int GetDataStartIndex() { return indexDataStart; }
        unsigned int GetDataEndIndex() { return indexDataEnd; }
        unsigned int GetMinimumAddress() { return minimumAddress; }
        unsigned int GetMaximumAddress() { return maximumAddress; }

    protected:
        unsigned int        indexDataStart;
        unsigned int        indexDataEnd;
        unsigned int        minimumAddress;
        unsigned int        maximumAddress;
        vector<SRECItem>    items;
        string              lastErrorStr;

    protected:
        bool parse(vector<string> &srcTextLines);

    private:

};

#endif // of __SREC_H__
