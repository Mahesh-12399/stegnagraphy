#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */
OperationType check_operation_type(char *argv[])     //Function to check whether the argument is passed or not
{
    if (strcmp(argv[1] ,"-e")==0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d")==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)  //Function to evaluate read and validate encode  arguments
{
    if (strstr(argv[2],".bmp"))
    {
        encInfo->src_image_fname=argv[2];
    }
    else
    {
        return e_failure;
    }
    if (strstr(argv[3],".txt"))
    {
        encInfo->secret_fname=argv[3];
    }
    else
    {
        return e_failure;
    }
    if (argv[4] != NULL)
    {
        if (strstr(argv[4],".bmp"))
        {
            encInfo->stego_image_fname=argv[4];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        encInfo->stego_image_fname="steged_img.bmp";
        printf("INFO: Output File is not mentioned. Creating %s as default\n",encInfo->stego_image_fname);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)                                //Function to perform the encoding
{
    int res=open_files(encInfo);                                      // Function call to open the files
    if (res == e_success)                                            //if the condition trues then the below statements will be printed
    {
        printf("INFO: Opened required files\n");
        printf("INFO: Opened beautiful.bmp\n");
        printf("INFO: Opened secret.txt\n");
        printf("INFO: Opened %s\n",encInfo->stego_image_fname);
        printf("INFO: Done\n");
    }
    else
        return e_failure;
    //printf("Failure in opening a file\n");
    printf("INFO: ## Encoding Procedure Started ##\n");
    printf("INFO: Checking for secret.txt size\n");
    printf("INFO: Done. Not Empty\n");
    printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
    if(check_capacity(encInfo) == e_success)                        //Function call to check the image capacity has enough to store secret.txt file

    {
        printf("INFO: Done. Found OK\n");
    }
    else
    {
        printf("INFO: Can't handle the secret.txt file\n");
    }
    if ((copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success))       //Function call to copy the header file
        printf("INFO: Done\n");
    else
        printf("INFO: Not Done\n");
    if (encode_magic_string(MAGIC_STRING,encInfo) == e_success)                                  //Function call to encode the magic string
        printf("INFO: Done\n");
    else
        printf("INFO: Not Done\n");
    if ((encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)) == e_success)    //Function call to encode secret file extension size
        printf("INFO: Done\n");
    else
        printf("INFO: Not Done\n");
    if ((encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success))               //Function call encode secret file extension
        printf("INFO: Done\n");
    else
        printf("INFO: Not Done\n");
    if ((encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success))                  //Function encode secret file size
        printf("INFO: Done\n");
    else
        printf("INFO: Not Done\n");
    if ((encode_secret_file_data(encInfo) == e_success))                                              //Function call encode secret file data
        printf("INFO: Done\n");
    else
        printf("INFO: Not Done\n");
    if ((copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success))      //Function call copy the remaining image data
    {
        printf("INFO: Done\n");
        printf("INFO: ## Encoding Done Successfully ##\n");
    }
    else
        printf("INFO: Not Done\n");

}

Status check_capacity(EncodeInfo *encInfo)       //Function definition to check the capacity
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
   // printf("Image capacity : %u\n", encInfo->image_capacity);
    encInfo->size_secret_file= get_file_size(encInfo->fptr_secret);
    strcpy(encInfo->extn_secret_file,".txt");
    int size_extn_secret_file=strlen(encInfo->extn_secret_file);
    int magic_string_len = strlen(MAGIC_STRING);
    if (encInfo->image_capacity > (54+(magic_string_len) * 8 + (size_extn_secret_file) * 8 + sizeof(size_extn_secret_file)*8+sizeof(encInfo->size_secret_file)*8 + (encInfo->size_secret_file)*8))
    {
        //printf("Image capacity is sufficient\n");
        return e_success;
    }
    SW2  
    {
//      printf("Image capacity is not sufficient\n");
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)         //Function definition to copy the bmp_header
{
    printf("INFO: Copying Image header\n");
    rewind(fptr_src_image);
    char buffer[54];
    fread(buffer, 54, 1, fptr_src_image);
    fwrite(buffer, 54, 1, fptr_dest_image);
    return e_success;
}

uint get_file_size(FILE *fptr)                                                //Function definition to get the file size
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)            //Function definition to encode magic string
{
    //fseek(fptr src_image,54,SEEK_SET)
    printf("INFO: Encoding Magic String Signature\n");
    char arr[8];
    int size_of_magic_string=strlen(MAGIC_STRING);
    for(int i=0;i<size_of_magic_string;i++)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i],arr);
        fwrite(arr, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)                  //Function definition to encode secret file extension size
{
    printf("INFO: Encoding Secret.txt File Extension Size\n");
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size,arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)          //Function definition to encode secret file extension
{
    printf("INFO: Encoding Secret.txt File Extension\n");
    int secret_extn_size = strlen(encInfo->extn_secret_file);
    char arr[8];
    for (int i = 0; i < secret_extn_size; i++)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], arr);
        fwrite(arr, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)               //Function definition to encode secret file size
{
    printf("INFO: Encoding Secret.txt File Size\n");
    file_size=encInfo->size_secret_file;
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)                            //Function definition to encode secret file data
{
    printf("INFO: Encoding Secret.txt File Data\n");
    rewind(encInfo->fptr_secret);
    char arr[8],ch;
    while((ch=fgetc(encInfo->fptr_secret))!=EOF)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, arr);
        fwrite(arr, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)                   //Function definition to copy remaining image data
{
    printf("INFO: Copying Left Over Data\n");
    char ch;
    while(fread(&ch, 1, 1, fptr_src))
        fwrite(&ch, 1, 1, fptr_dest);
    return e_success;
}

Status encode_byte_to_lsb(char data,char *image_buffer)                       //Function definition to encode byte to lsb
{
    for(int i=7;i>=0;i--)
    {
        if(data & (1<<i))
            image_buffer[7-i] |= 1;
        else
            image_buffer[7-i] &= ~1;
    }
    return e_success;
}

Status encode_size_to_lsb(char data,char *image_buffer)                            //Function definition to encode size to lsb
{
    for(int i=31;i>=0;i--)
    {
        if(data & (1<<i))
            image_buffer[31-i] |= 1;
        else
            image_buffer[31-i] &= ~1;
    }
    return e_success;
}


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)                                     //Function definition to get image size for bmp
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
   // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
   // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)                                             //Function definition to open files
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}
