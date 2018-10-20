//Rajesh Narayan hw1

#include <stdio.h>

//2.60
unsigned replace_byte( unsigned x, int i, unsigned char b) {
    //resubmission is because I used multiplication on the first submission, replaced with left shift
    return (x & ~(0xff << (i * 8)) | (b << (i >> 3));
}


//2.71
//A - The given solution does not account for negative numbers, and it keeps the numbers as unsigned

typedef unsigned packed_t;

int xbyte(packed_t word, int bytenum){
    //converting the word to a signed integer and shifting it to the right
    int s_word = word << ((3-bytenum)<<3);
    // sign extending it to a 32bit number
    return (s_word >> 24);
}

int main(){
    printf("%x\n", replace_byte(0x12345678, 2, 0xAB));
    //printf("%x\n", replace_byte(0x12345678, 0, 0xAB));
    //printf("%x\n", xbyte(0x87654321, 0));
}