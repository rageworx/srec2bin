#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>

#include "srec.h"

////////////////////////////////////////////////////////////////////////////////

using namespace std;

////////////////////////////////////////////////////////////////////////////////

#define STR_VERSION         "0.1.0.0"

////////////////////////////////////////////////////////////////////////////////

string str_me;
string opt_srcfile;
string opt_dstfile;
bool   opt_reversefill = false;

////////////////////////////////////////////////////////////////////////////////

int paramcheck( int argc, char** argv )
{
    str_me = argv[0];

    size_t epos = str_me.find_last_of("\\");
    if ( epos == string::npos )
    {
        epos = str_me.find_last_of("/");
    }

    if ( epos != string::npos )
    {
        str_me = str_me.substr( epos + 1 );
    }

    if ( argc > 1 )
    {
        int cnt = 0;

        for( cnt=1; cnt<argc; cnt++ )
        {
            string tmp = argv[ cnt ];

            if ( tmp.size() > 0 )
            {
                if ( ( tmp == "-r" ) || ( tmp == "-R" ) )
                {
                    opt_reversefill = true;
                }
                else
                if ( opt_srcfile.size() == 0 )
                {
                    opt_srcfile = tmp;
                }
                else
                if ( opt_dstfile.size() == 0 )
                {
                    opt_dstfile = tmp;
                }
            }
        }

        if ( opt_srcfile.size() > 0 )
        {
            if ( opt_dstfile.size() == 0 )
            {
                // find ext.
                size_t extpos = opt_srcfile.find_last_of( '.' );
                if ( extpos != string::npos )
                {
                    opt_dstfile = opt_srcfile.substr( 0, extpos );
                }
                opt_dstfile += ".bin";
            }
        }

        return cnt;
    }

    return 0;
}

void printhelp()
{
    printf( "\n" );
    printf( " usages :\n" );
    printf( "\t%s [source file] (option) (output file)\n", str_me.c_str() );
    printf( "\n" );
    printf( " options :\n" );
    printf( "\t-r(-R) : fill zero memory to 0XFF (default is 0x00)\n" );
    printf( "\n" );
}

void process()
{
    SRECReader* srec = new SRECReader();

    if ( srec != NULL )
    {
        printf( "loading srecord : " );

        if ( srec->Load( opt_srcfile ) == true )
        {
            printf( "Ok.\n" );

            unsigned int data_sz  = srec->GetDataSize();
            unsigned int rec_sz   = srec->GetRecordSize();
            unsigned int addr_min = srec->GetMinimumAddress();
            unsigned int addr_max = srec->GetMaximumAddress();

            printf( "record information :\n" );
            printf( "\t-address : 0x%08X ~ 0x%08X\n", addr_min, addr_max );
            printf( "\t-records : %d\n", rec_sz );
            printf( "\t-datas   : %d bytes\n", data_sz );

            unsigned int mapsz = addr_max - addr_min;

            if ( mapsz == 0 )
            {
                printf( "Error: This srecord seems to abnormal.\n" );

                delete srec;

                return;
            }

            unsigned char* datamap = new unsigned char[ mapsz ];

            if ( datamap == NULL )
            {
                printf( "Error: Failed to allocating memory.\n" );

                delete srec;

                return;
            }

            if ( opt_reversefill == false )
            {
                memset( datamap, 0, mapsz );
            }
            else
            {
                memset( datamap, 0xFF, mapsz );
            }

            printf( "Writing to memory ... " );
            for( unsigned cnt=0; cnt<rec_sz; cnt ++ )
            {
                SRECItem* recitem = srec->GetRecord( cnt );

                if ( recitem != NULL )
                {
                    memcpy( &datamap[ recitem->address ],
                            recitem->data,
                            recitem->dataLen );
                }
            }
            printf( "Ok.\n" );

            printf( "Writing to file ... " );
            if ( access( opt_dstfile.c_str(), 0 ) == 0 )
            {
                if ( unlink( opt_dstfile.c_str() ) != 0 )
                {
                    printf("Failure, file already existed and cannot be overwritten.\n" );

                    delete[] datamap;
                    delete srec;

                    return;
                }
            }

            FILE* fp = fopen( opt_dstfile.c_str(), "wb" );
            if ( fp != NULL )
            {
                fwrite( datamap, 1, mapsz, fp );
                fclose( fp );
            }
            else
            {
                printf("Failure, file cannot be created.\n" );

                delete[] datamap;
                delete srec;

                return;
            }

            delete[] datamap;

            printf( "Ok.\n" );
        }
        else
        {
            printf( "Failure by %s\n", srec->GetLastError() );
        }

        delete srec;
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
    printf( "Motorolla srecord to binary, (C)2017 Raph.K. version %s\n", STR_VERSION );

    if ( paramcheck( argc, argv ) > 0 )
    {
        printf( "\n" );
        printf( " source file : %s\n", opt_srcfile.c_str() );
        printf( " output file : %s\n", opt_dstfile.c_str() );
        printf( "\n" );

        process();
    }
    else
    {
        printhelp();
    }

    return 0;
}
