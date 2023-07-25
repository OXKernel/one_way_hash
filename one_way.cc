//
//  Copyright (C) 2023. Roger Doss.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//
// @file:
//  one_way.cc
//
// @description:
//  One way hash function based on
//  string hash function from Java with
//  improvements.
//  NOTE: The str_to_hex function has
//  the unfortunate side effect of
//  loosing information in the input text
//  in that it always produced the same
//  signature for same strings regardless
//  of case. We changed the logic
//  to work with the raw input instead.
//
// @author:
//  Roger Doss
//
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>

using std::string;
using std::ifstream;
using std::cout;
using std::endl;
using std::cerr;

#include "crc.h"

static int CHAR_POSITION=0;
static int SUM=0;
static int NULL_COUNT=0;

char dec_to_hex(char ch)
{
    if(ch <= 0) {
        return (((++NULL_COUNT) % 10) + '0');
    } else if(ch >= 1 && ch <= 9) {
        return (ch + '0');
    } else if(ch >= 10 && ch <= 15) {
        return ((ch-10) + 'A');
    } else {
        cerr << "convert:: char out of range: [" << ch << "]" << endl;
        return 'x';
    }
}

string
str_to_hex(char *data, int dlen)
{
    string hex="";
    for(int i = 0; i < dlen; ++i) {
        hex += dec_to_hex(data[i] & 0xF);
        hex += dec_to_hex((data[i] & 0xF0) >> 8);
   }
   return hex;
}

bool
is_blank(char *data, int dlen)
{
    if(!data) {
        return true;
    }
    if(!dlen) {
        return true;
    }
    for(int i = 0; i < dlen; ++i) {
        if(!isspace(data[i])) {
            return false;
        }
    }
    return true;
}

unsigned sum(const char *data, unsigned len)
{
    unsigned long rtvl = 0;
    if(!data || !len) { return 0; }
    for(unsigned i = 0; i < len; ++i) {
        rtvl += data[i];
        rtvl &= 0xFF;
    }
    return (rtvl & 0xFF);
}

unsigned hash(const char *str, unsigned len)
{
    unsigned rtvl = 0;
    unsigned multiplier = 127;
    if(!str) {
        return 0;
    }
    for(int i = 0; i < len; ++i) {
        rtvl = (multiplier * rtvl) + str[i];
    }
    return rtvl;
}

unsigned left_rotate(unsigned x, unsigned c)
{
    return (x << c) | (x >> 32 - c);
}

void accumulate(unsigned *acc, unsigned block, unsigned rotate, char *hex, unsigned start, unsigned end)
{
    acc[block] ^= hash(&hex[start],(end-start));
    acc[block] = left_rotate(acc[block],rotate);
}

int
main(int argc, char **argv)
{
    unsigned signature[4]={
        198,
        87,
        210,
        81
    };
    char buf[1024]={0};
    unsigned len = 0;
    unsigned accum[12] = {
        2088827387,
        498545115,
        1525472798,
        806030149,
        166054252,
        680404740,
        1010620879,
        1246987489,
        0,
        0,
        0,
        0
    };

    if(argc != 2) {
        cerr << "syntax:: one_way filename" << endl;
        return 1;
    }

    ifstream in(argv[1]);
    if(!in.good()) {
        cerr << "one_way:: error opening file [" << argv[1] << "]" << endl;
        return 2;
    }

    F_CRC_InicializaTabla();

    while(!in.eof()) {
        in.read(buf,1024);
        len = in.gcount();
        CHAR_POSITION += len;
        if(is_blank(buf,len)) {
            break;
        }
        if(len <= 1024) {
            // Modify one input bit.
            buf[0]++;
            unsigned min = (len < 4) ? len:4;
            for(int i = 0; i < min; ++i) {
                signature[i] ^= buf[i];
            }
        }
        SUM += sum(buf,len);
        if(len < 1024) {
            accumulate(accum, 0, 1, buf, 0, len);
        } else {
            accumulate(accum, 0, 1, buf, 0,    128);
            accumulate(accum, 1, 2, buf, 128,  256);
            accumulate(accum, 2, 3, buf, 256,  384);
            accumulate(accum, 3, 4, buf, 384,  512);
            accumulate(accum, 4, 5, buf, 512,  640);
            accumulate(accum, 5, 6, buf, 640,  768);
            accumulate(accum, 6, 7, buf, 768,  896);
            accumulate(accum, 7, 8, buf, 896, 1024);
        }
        accum[8] ^= F_CRC_CalculaCheckSum((unsigned char *)buf,len);
    }
    unsigned tmp[4]={
        181,
        81,
        209,
        66
    };
    unsigned min = (len < 4) ? len:4;
    for(unsigned int i = 0; i < min; ++i) {
        tmp[i] ^= buf[i];
    }
    for(unsigned int i = 0; i < min; ++i) {
        signature[i] ^= tmp[i];
    }
    accum[9] = CHAR_POSITION;
    accum[10] = SUM;
    memcpy(&accum[11],signature, 4);
    cout << str_to_hex((char *)accum, 36) << " " << argv[1] << endl;
    return 0;
}
