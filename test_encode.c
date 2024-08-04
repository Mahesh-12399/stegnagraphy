u#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "decode.h"
#include "common.h"
#include "types.h"

int main(int argc,char *argv[])
{
    int ret;
    if (argc == 1 && (strcmp(argv[0] , "./a.out") == 0))                   //if the user passes only one argument i.e ./a.out
    {
        printf("Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
        printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
        exit(0);
    }
    else if ((argc == 2 && (strcmp(argv[1] , "-e") == 0))  || (argc == 3 && (strstr(argv[2],".bmp")) && (strcmp(argv[1],"-e") == 0))) //Prints the belowerror if the user passes only arg2 and arg3 in encoding part
    {
        printf("Encoding: ./a.out -e <.bmp file> <test file> [output file]\n");
        exit(0);
    }
    else if (argc >= 3 )
    {
        ret = check_operation_type(argv);        //Checks the operation type encode or decode
        if (ret == e_encode)                     //if the condition trues then only encoding part will be selected
        {
            printf("Selected encoding\n");     //Prints the statement
            EncodeInfo encInfo;
            ret = read_and_validate_encode_args(argv,&encInfo); //Read and validate encoding arguments
            do_encoding(&encInfo);
        }
        else if (ret == e_decode)                                        //If the condition trues then only the the decoding part will be selected
        {
            printf("Selected decoding\n");
            DecodeInfo decInfo;             //Declaring the variable of structure DecodeInfo
            ret = read_and_validate_decode_args(argv,&decInfo); //Read and validate decoding arguments
            do_decoding(argv,&decInfo);               //Perform decodingpart
        }

    }
    else if (( argc == 3 || argc == 2)  && (strcmp(argv[1],"-d") == 0))            //Checks whether the arguments 2 is passed or not in decoding part
    {
        printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
        exit(0);
    }
}
