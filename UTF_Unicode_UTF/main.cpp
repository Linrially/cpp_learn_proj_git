#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <bitset>

using namespace std;

#define FILE_READ "c:\\example\\testUTF-8.txt"
#define FILE_WRITE "c:\\example\\outUTF-8.txt"


bool receiver(ifstream &in_stream, list <unsigned int> &lst_unicode)
{

    unsigned int result;                //  Unicode
    unsigned char bytes_left=0;         // to count bytes in sequence of bytes
    unsigned char mask;                 // to delete bit-mask (flag bits)
    char in_char;                       // to read a file

    while(in_stream.good())
    {

        in_stream.read(&in_char, 1);
       // bitset<8> bset(in_char);

        //debugging output
        //cout << "Incoming byte:\t0x" << hex << (in_char & 0xFF) << "\t0b" << bset << endl;
        // to out number, (in_char & 0b11111111) good is also.

        unsigned char positive_char = in_char;

        if (bytes_left==0) // start to read next sequence
        {
            if ((positive_char >> 7) == 0)
            {
                mask = 0b01111111;
                bytes_left=0;
                //debugging output
                //cout << "Single byte" << endl;
            }
            else if ((positive_char >> 5) == 6)
            {
                mask = 0b00011111;
                bytes_left=1;
                //debugging output
                //cout << "Two bytes sequence" << endl;
            }
            else if ((positive_char >> 4) == 14)
            {
                mask = 0b00001111;
                bytes_left=2;
                //debugging output
                //cout << "Three bytes sequence" << endl;
            }
            else if ((positive_char >> 3) == 30)
            {
                mask = 0b00000111;
                bytes_left=3;
                //debugging output
                //cout << "Four bytes sequence" << endl;
            }
            else
            {
                cout << "This is non-utf-8 sequence!!!" << endl;
                return false;
            }

            //debugging output
            unsigned char masked_ch = mask & positive_char;
            //bitset<8> mask_bset(masked_ch);
            //cout << "Masked byte:\t0x" << hex << (masked_ch & 0xFF) << "\t0b" << mask_bset << endl;

            result = masked_ch << bytes_left*6;



        }
        else
        {
            if ((positive_char >> 6) !=2)
            {
                cout << "!!! Illegal continuation byte !!!" << endl;
                return false;
            }

            bytes_left--;
            unsigned int temp_res = (positive_char & 0b00111111) << bytes_left*6;
            result += temp_res;


        }

        if (bytes_left == 0)
        {
            //cout << "Unicode\t0x" << hex << result << endl << endl;
            lst_unicode.push_back(result);
            result=0;


        }

    }
    return true;



}

bool to_upper_case_unicode(const list <unsigned int> &lst_unicode, list <unsigned int> &lst_upper)
{
    if (lst_unicode.size() > 0)
    {
        for (auto element : lst_unicode)
        {
            if (element > 0x42F && element < 0x450) // lowercase letters of Russian (cyrillic)
                element -= 0x20;
            else if (element == 0x451)
                element = 0x401;

            lst_upper.push_back(element);
        }
        return true;
    }
    else
        return false;

}


bool to_lower_case_unicode(const list <unsigned int> &lst_unicode, list <unsigned int> &lst_lower)
{
    if (lst_unicode.size() > 0)
    {
        for (auto element : lst_unicode)
        {
            if (element > 0x40F && element < 0x430) // uppercase letters of Russian (cyrillic)
                element += 0x20;
            else if (element == 0x401)
                element = 0x451;

            lst_lower.push_back(element);
        }
        return true;
    }
    else
        return false;

}

bool UnicodeToUTF(const list <unsigned int> &lst_unicode, list <unsigned char> &lst_utf8)
{

        unsigned int bytes_left=0;
        unsigned char mask(0), masked_ch(0), elder_byte(0);


    for(auto un_number : lst_unicode)
    {

        if (bytes_left==0)
        {
            if(un_number < 0x80)
            {
                mask=0;
                bytes_left=0;

            }
            else if(un_number < 0x800)
            {
                mask=0b11000000;
                bytes_left=1;

            }
            else if(un_number < 0x10000)
            {
                mask=0b11100000;
                bytes_left=2;
            }
            else if(un_number < 0x110000)
            {
                mask=0b11110000;
                bytes_left=3;
            }
            else
            {
                cout << "Illegal sequence!!!" << endl;
                return false;
            }

            elder_byte = (un_number >> bytes_left*6);
            masked_ch = elder_byte | mask;
            lst_utf8.push_back(masked_ch);

        }

            mask=0b10000000;

            while(bytes_left!=0)
            {
                bytes_left--;
                unsigned char temp_res = (un_number >> bytes_left*6 | mask);
                lst_utf8.push_back(temp_res);
            }

    }
    return true;
}



int main()
{
    ifstream in_read;
    in_read.open(FILE_READ, ifstream::binary);
    if (!in_read.is_open())
    {
        cout << "Cannot open the file to read: " << FILE_READ << endl;
        return EXIT_FAILURE;
    }

    ofstream in_write;
    in_write.open(FILE_WRITE, ofstream::binary);
    if (!in_write.is_open())
    {
        cout << "Cannot open the file to write: " << FILE_WRITE << endl;
        in_read.close();
        return EXIT_FAILURE;
    }


    list <unsigned int> lst_unicode;
    receiver(in_read, lst_unicode);

    //for (auto s: lst_unicode)
     //   cout << s << "\t";

    cout << endl;

    list <unsigned char> lst_utf8;
    list <unsigned int> lst_upper_unicode;
    list <unsigned int> lst_lower_unicode;

    //to_upper_case_unicode(lst_unicode, lst_upper_unicode);
    //to_lower_case_unicode(lst_unicode, lst_lower_unicode);

    UnicodeToUTF(lst_unicode, lst_utf8);


    // сначала без изменений посмотрим текст в файле после операций из UTF - Unicode - UTF.
    for (auto utf_el: lst_utf8)
        in_write << utf_el;


    in_read.close();
    in_write.close();
    return 0;
}
