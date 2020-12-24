// 208783522 Amber Weiner

#include <stdio.h>
#include <string.h>
static const int SIZE_OF_CHAR_UTF16 = sizeof(char) * 2;

/**
 * The function compare between the chars and return the significant place
 * @param buffer
 * @param flag_to_compare
 * @return the significant place
 */
int get_significant_place(const char buffer[2],const char flag_to_compare[2]) {
    // big endian case
    if (buffer[0] == flag_to_compare[0])
        return 0;
        // little endian case
    else if (buffer[1] == flag_to_compare[0])
        return 1;
    // problem case- the flag is not existed in the buffer
    return -1;
}
/**
 * The function swap two given values.
 * @param first_num - the first number to swap.
 * @param second_num - the second number to swap.
 */
void swap(char *first_num, char *second_num) {
    char temp = *first_num;
    *first_num = *second_num;
    *second_num = temp;
}

/**
 * The function get a file , and copy it to a new file.
 * @param src_file - the file source to read
 * @param new_file - the file to write to
 * @param swap_flag - if we need to swap it's value 1, or else 0.
 */
void option1(FILE **src_file, FILE **new_file, int swap_flag) {
    char buffer[SIZE_OF_CHAR_UTF16];
    // go over the source file and write it to the new file
    while (0 != fread(buffer, SIZE_OF_CHAR_UTF16, 1, *src_file)) {
        // if no swap is needed
        if (swap_flag == 0)
            fwrite(buffer, SIZE_OF_CHAR_UTF16, 1, *new_file);
        else { // swap case
            swap(buffer, buffer + 1);
            fwrite(buffer, SIZE_OF_CHAR_UTF16, 1, *new_file);
        }
    }
}
/**
 * The function convert from unix/mac to the other type.
 * When we get to a "new line" we send to temp function that return if is
 * big/little endian and return the correct place of the tav.
 * @param src_file- the source file input to read
 * @param new_file- the output file to write
 * @param source_flag - the type to text for the source file
 * @param swap_flag - 1 if we need to do swap, or else 0.
 */
void unix_mac(FILE **src_file, FILE **new_file, char *source_flag, int swap_flag) {
        int valueIndex;
    char buffer[SIZE_OF_CHAR_UTF16], source_sign[1], other_sign[1];
    // check if the source file is in unix
    if (strcmp(source_flag, "-unix") == 0) {
        source_sign[0] = '\n';
        other_sign[0] = '\r';
    } else if (strcmp(source_flag, "-mac") == 0) {
        // check if the source file is in mac
        source_sign[0] = '\r';
        other_sign[0] = '\n';
    }
    // go over the file and read it
    // and when we get to the end of a line, check if swap is needed (little endian case)
    while (0 != fread(buffer, SIZE_OF_CHAR_UTF16, 1, *src_file)) {
        if ((buffer[0] == source_sign[0]) || (buffer[1] == source_sign[0])) {
            //replace the valueIndex to the otherSign
            valueIndex = get_significant_place(buffer,source_sign);
            buffer[valueIndex] = other_sign[0];
        }
        if (swap_flag == 0) {
            fwrite(buffer, SIZE_OF_CHAR_UTF16, 1, *new_file);
        } else {
            //swap case
            swap(buffer, buffer + 1);
            fwrite(buffer,SIZE_OF_CHAR_UTF16,1,*new_file);
        }
    }
}

/**
 *The function convert from win to mac or unix type.
 * We go over the file with two pointers- prev and current.
 * When prev equal to '\r' and current to '\n'.
 * We need to convert this to the new input flag.
 * @param src_file - the source file input to read.
 * @param new_file - the output file to write.
 * @param new_file_flag - the type to change to.
 * @param swap_flag 1- if we need to do swap, or else 0.
 */
void win_to_mac_or_unix(FILE **src_file, FILE **new_file, char *new_file_flag, int swap_flag) {
    int value_index, other_index, flag = 0;
    char current[2], dest_sign[1], temp_flag[1];
    temp_flag[0] = '\n';
    char *prev = NULL;
    // check if the required type of the new file is mac
    if (strcmp(new_file_flag, "-mac") == 0) {
        dest_sign[0] = '\r';
    } else {
        // it is unix
        dest_sign[0] = '\n';
    }
    // go over the file
    // and use prev and current to know when we arrive to the "\r\n"
    while (0 != fread(current, SIZE_OF_CHAR_UTF16, 1, *src_file)) {
        // if not swap case
        if (swap_flag == 0) {
            if ((prev != NULL) && ((current[0] == '\n')||(current[1] == '\n') )) {
                // check the significant place
                value_index = get_significant_place(current,temp_flag);
                current[value_index] = dest_sign[0];
                fwrite(current, sizeof(current), 1, *new_file);
                flag = 1;
            }
            // we go over if the char is '\n'
            if (flag == 1) {
                flag = 0;
            } else {
                if ((current[0] != '\r') && (current[1] != '\r'))
                    fwrite(current, sizeof(current), 1, *new_file);
            }
        } else { //swap case
            if ((prev != NULL) && ((current[0] == '\n')||(current[1] == '\n') )) {
                // check the significant place
                value_index = get_significant_place(current,temp_flag);
                other_index = 1 - value_index;
                //swap the byte
                current[other_index] = dest_sign[0];
                current[value_index] = '\000';
                fwrite(current, sizeof(current), 1, *new_file);
                flag = 1;
            }
            if (flag == 1) {
                flag = 0;
            } else {
                if ((current[0] != '\r') && (current[1] != '\r')) {
                    swap(current, current + 1);
                    fwrite(current,SIZE_OF_CHAR_UTF16,1,*new_file);
                }
            }
        }
        prev = current;
    }
}
/**
 *The function convert from unix or mac to win type.
 * When we read '\n' or '\r' we write '\r\n'
 * and do the opposite if we need to do swap.
 * @param src_file - the source file input to read.
 * @param new_file - the output file to write.
 * @param source_flag - the type of the source file text.
 * @param swap_flag 1- if we need to do swap or else 0.
 */
void unix_or_mac_to_win(FILE** src_file, FILE** new_file, char* source_flag , int swap_flag) {
    int value_index, other_index;
    char buffer[2], source_sign[1];
    // check if the source file text is unix
    if (strcmp(source_flag, "-unix") == 0)
        source_sign[0] = '\n';
    else
        // it is mac type
        source_sign[0] = '\r';
    // go over the file
    while (0 != fread(buffer, SIZE_OF_CHAR_UTF16, 1, *src_file)) {
        // check big endian
        if (swap_flag == 0) {
            // get to the end of a line- big or little endian
            if ((buffer[0] == source_sign[0]) || (buffer[1] == source_sign[0])) {
                if (buffer[1] == source_sign[0]) {
                    char first[2], second[2];
                    first[1] = '\r';
                    first[0] = '\000';
                    second[1] = '\n';
                    second[0] = '\000';
                    fwrite(first, SIZE_OF_CHAR_UTF16, 1, *new_file);
                    fwrite(second, SIZE_OF_CHAR_UTF16, 1, *new_file);
                } else {
                    fwrite("\r", SIZE_OF_CHAR_UTF16, 1, *new_file);
                    fwrite("\n", SIZE_OF_CHAR_UTF16, 1, *new_file);
                }
            } else {
                // if not the end of a line '/n/r'
                fwrite(buffer, SIZE_OF_CHAR_UTF16, 1, *new_file);
            }
        } else { //swap case
            char first[1], second[1];
            if ((buffer[0] == source_sign[0]) || (buffer[1] == source_sign[0])) {
                char one[2], two[2];
                value_index = get_significant_place(buffer,source_sign);
                other_index = 1 - value_index;
                one[other_index] = '\r';
                one[value_index] = '\000';
                two[other_index] = '\n';
                two[value_index] = '\000';
                fwrite(one, SIZE_OF_CHAR_UTF16, 1, *new_file);
                fwrite(two, SIZE_OF_CHAR_UTF16, 1, *new_file);
            } else {
                //replace place
                first[0] = buffer[0];
                second[0] = buffer[1];
                fwrite(second, 1, 1, *new_file);
                fwrite(first, 1, 1, *new_file);
            }
        }
    }
}

/**
 * The function compares the flags and send to the matching function.
 * @param sourceFile - the source file input to read.
 * @param newFile - the output file to write.
 * @param src_flag - the type to text
 * @param new_file_flag - the type to text to change to
 */
void option2(FILE **src_file, FILE **new_file, char *src_flag, char *new_file_flag, int swap_flag) {
    // check if we need to copy to a file with the same type
    if(strcmp(src_flag,new_file_flag) == 0){
        option1(src_file, new_file, swap_flag);
        // check if we need to copy from unix to mac or from mac to unix
    } else if(((strcmp(src_flag,"-unix") == 0) && (strcmp(new_file_flag,"-mac") == 0)) ||
              ((strcmp(new_file_flag,"-unix") == 0) && (strcmp(src_flag,"-mac")==0))){
        unix_mac(src_file, new_file, src_flag, swap_flag);
        // check if we need to copy from win to mac or from mac to unix
    } else if(((strcmp(src_flag,"-win") == 0) && (strcmp(new_file_flag,"-mac") == 0)) ||
              ((strcmp(src_flag,"-win") == 0) && (strcmp(new_file_flag,"-unix") == 0))){
        win_to_mac_or_unix(src_file, new_file, new_file_flag, swap_flag);
        // check if we need to copy from unix to mac to win
    } else if(((strcmp(src_flag,"-unix") == 0) && (strcmp(new_file_flag,"-win") == 0)) ||
              ((strcmp(src_flag,"-mac") == 0) && (strcmp(new_file_flag,"-win") == 0))){
        unix_or_mac_to_win(src_file,new_file,src_flag, swap_flag);
    }
}

/**
 * The function check the "byteOrderFlag" and send with the matching flag to the matching function.
 * @param src_file - the name of the input source to read
 * @param new_file - the name of the output file to write
 * @param src_flag - the type to text
 * @param new_file_flag - the type to text to change to
 * @param byte_order_flag - the flag "keep" or "swap"
 */
void option3(FILE **src_file, FILE **new_file, char *src_flag, char *new_file_flag, char *byte_order_flag) {
    // "keep"- do the same without swapping
    if (strcmp(byte_order_flag, "-keep") == 0) {
        option2(src_file, new_file, src_flag, new_file_flag, 0);
        //"swap"- swap the byte
    } else if (strcmp(byte_order_flag, "-swap") == 0) {
        option2(src_file, new_file, src_flag, new_file_flag, 1);
    }
}

/*
 * The function get arguments and according to it do different things as required.
 * In addition, the function check edge cases.
 */
int main(int arg, char *argv[]) {
    if (arg > 2) {
        // if the user forgets to send source/new file
        if (!(strstr(argv[1], ".") && strstr(argv[2], "."))) {
            // problem with the input
        } else {
            // if there are more than 4 arguments
            // and the fifth one is not "-keep" or "-swap"
            if ((arg > 5) && (strcmp(argv[5], "-keep") != 0 && strcmp(argv[5], "-swap") != 0)) {
                // problem with the input
            } else {
                FILE *source = fopen(argv[1], "r");
                // check if the source file was opened
                if (source == NULL) {
                    return 0;
                }
                FILE *new_file = fopen(argv[2], "w");
                // check if the new file was opened
                if (new_file == NULL) {
                    fclose(source);
                    return 0;
                }
                if (arg == 3)
                    option1(&source, &new_file, 0);
                else if (arg == 5) {
                    if ((strcmp(argv[3], "-unix") == 0 || strcmp(argv[3], "-mac") == 0 || strcmp(argv[3], "-win") == 0) &&
                        (strcmp(argv[4], "-unix") == 0 || strcmp(argv[4], "-mac") == 0  || strcmp(argv[4], "-win") == 0 ))
                        // if the user don't send a flag to swap
                        option2(&source, &new_file, argv[3], argv[4], 0);
                    // if the user don't send a flag to swap
                } else if (arg == 6) {
                    option3(&source, &new_file, argv[3], argv[4], argv[5]);
                }
                // close the files
                fclose(new_file);
                fclose(source);
                return 1;
            }
        }
    }
    // if there was an error that wasn't caught before
    return 0;
}