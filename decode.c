#include<stdio.h>
#include "decode.h"
#include<string.h>
#include "types.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)        //Function defintion to read and validate decode arguments
{
    if(strstr(argv[2],".bmp"))
    {
        decInfo->enc_image_fname=argv[2];
    }
    else
    {
        printf("INFO : No input .bmp file passed\n");
        return e_failure;
    }
    return e_success;
}

Status do_decoding(char **argv,DecodeInfo *decInfo)                              //Function definition to do decoding
{
    int ret;
    printf("INFO: Decoding Procedure Started\n");
    ret=open_encoded_file(decInfo);                                         //Function call to open the encoded file
    if(ret == e_failure)
        return e_failure;
    ret=decode_magic_string(decInfo);                                        //Function call to decode the magic string
    if(ret == e_success)
        printf("INFO: Done\n");
    else
        return e_failure;
    ret=decode_secret_file_extn_size(decInfo);                               //Function call to decode secret file extension size
    if(ret == e_failure)
        return e_failure;
    else
        printf("INFO: Done\n");
    ret=decode_secret_file_extn(decInfo);                                      //Function call to decode secret file extension
    if(ret == e_success)
        printf("INFO: Done\n");
    else
        return e_failure;
    ret=open_decoded_file(argv,decInfo);                                       //Function call to open the decoded file
    if(ret == e_success)
        printf("INFO: Done. Opened all required files\n");
    else
        return e_failure;
    ret=decode_secret_file_size(decInfo);                                       //Function call to decode the secret file
    if(ret == e_success)
        printf("INFO: Done\n");
    else
        return e_failure;
    ret=decode_secret_file_data(decInfo);                                       //Function call to decode the secret file data
    if(ret == e_success)
    {
        printf("INFO: Done\n");
        printf("## Decoding Done Successfully ##\n");
    }
    else
        return e_failure;

    return e_success;
}

Status open_encoded_file(DecodeInfo *decInfo)                                 //Function definition to open the encoded file
{
    //Src enc image file
    printf("INFO: Opening required files\n");
    decInfo->fptr_enc_image = fopen(decInfo->enc_image_fname,"r");
    //Error handling
    if(decInfo->fptr_enc_image == NULL)
    {
        perror("Fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",decInfo->enc_image_fname);
        return e_failure;
    }
    printf("INFO: Opened %s\n",decInfo->enc_image_fname);
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)                                  //Function definition to decode the magic string
{
    fseek(decInfo->fptr_enc_image,54,SEEK_SET);
    char key[20];
    printf("Enter the password : ");
    scanf("%s",key);
    printf("INFO: Decoding Magic String Signature\n");
    int len=strlen(key);

    char magic_str[len+1];
    char arr[8];
    for(int i=0; i<len; i++)
    {
        fread(arr,8,1,decInfo->fptr_enc_image);
        magic_str[i]=decode_byte_to_lsb(arr);
    }
    magic_str[len]='\0';
    if(strcmp(magic_str,key))
    {
        printf("Password is wrong !!\n");
        return e_failure;
    }
    else
    {
        printf("Password is matched\n");
        return e_success;
    }

}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)                       //Function definition to decode secret file extension size
{
    printf("INFO: Decoding Output File Extension Size\n");
    char arr[32];
    fread(arr,32,1,decInfo->fptr_enc_image);
    decInfo->file_extn_size=decode_size_to_lsb(arr);
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)                          //Function definition to decode secret file extension
{
    printf("INFO: Decoding Output File Extension\n");
    char arr[8];
    for(int i=0; i<decInfo->file_extn_size; i++)
    {
        fread(arr,8,1,decInfo->fptr_enc_image);
        decInfo->file_extn[i]=decode_byte_to_lsb(arr);
    }
    decInfo->file_extn[decInfo->file_extn_size]='\0';
    return e_success;
}

Status open_decoded_file(char **argv,DecodeInfo *decInfo)                     //Function definition to open decoded file
{
    if(argv[3] == NULL)
    {
        printf("INFO: Output File not mentioned. Creating decoded%s as default\n",decInfo->file_extn);
        strcpy(decInfo->decoded_fname,"decoded");
    }
    else
    {
        strcpy(decInfo->decoded_fname,argv[3]);
    }
    strcat(decInfo->decoded_fname,decInfo->file_extn);
    if((decInfo->fptr_decoded_file=fopen(decInfo->decoded_fname,"w")) != NULL)
        printf("INFO: Opened %s\n",decInfo->decoded_fname);
    else
        return e_failure;

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)                                  //Function definition to decode secret file size
{
    printf("INFO: Decoding %s File Size\n",decInfo->decoded_fname);
    char arr[32];
    fread(arr,32,1,decInfo->fptr_enc_image);
    decInfo->decoded_size_file=decode_size_to_lsb(arr);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)                                    //Function definition to decode secret file data
{
    printf("INFO: Decoding %s File Data\n",decInfo->decoded_fname);
    char arr[8],ch;
    for(int i=0; i<decInfo->decoded_size_file; i++)
    {
        fread(arr,8,1,decInfo->fptr_enc_image);
        ch=decode_byte_to_lsb(arr);
        fwrite(&ch,1,1,decInfo->fptr_decoded_file);
    }
    return e_success;
}

Status decode_byte_to_lsb(char *image_buffer)                                         //Function definition to decode byte to lsb
{
    char ch=0;
    for(int i=0; i<8; i++)
    {
        if(image_buffer[i] & 1)
            ch=ch | (1<<(7-i));
    }
    return ch;
}

Status decode_size_to_lsb(char *image_buffer)                                         //Function definition to decode size to lsb
{
    int data=0;
    for(int i=0; i<32; i++)
    {
        if(image_buffer[i] & 1)
            data= data | (1<<(31-i));
    }
    return data;
}
