#ifndef MENU_PRINTER_HPP
#define MENU_PRINTER_HPP

#include <iostream>
#include <iomanip>

using namespace std;

void printMenu1()
{
    cout << setw(25) << setfill('*') << "\n";
    cout << "* " << "Menu - Level 1       *" << endl;
    cout << "* " << "Select an option:    *" << endl;
    cout << "* " << "1. Encrypt a message *" << endl;
    cout << "* " << "2. Decrypt a message *" << endl;
    cout << "* " << "3. Quit              *" << endl;
    cout << setw(25) << setfill('*') << "\n";
}

void printMenu2_encrypt()
{
    cout << setw(40) << setfill('*') << "\n";
    cout << "* " << "Menu - Level 2: Encryption          *" << endl;
    cout << "* " << "Select an option:                   *" << endl;
    cout << "* " << "1. Enter a message                  *" << endl;
    cout << "* " << "2. One-round encryption             *" << endl;
    cout << "* " << "3. Automatic multi-round encryption *" << endl;
    cout << "* " << "4. Back                             *" << endl;
    cout << setw(40) << setfill('*') << "\n";
}

void printMenu3_encrypt_oneRound()
{
    cout << setw(46) << setfill('*') << "\n";
    cout << "* " << "Menu - Level 3: Encryption                *" << endl;
    cout << "* " << "Select an option:                         *" << endl;
    cout << "* " << "1. Enter a grid size                      *" << endl;
    cout << "* " << "2. Automatic grid size                    *" << endl;
    cout << "* " << "3. Print the grid and the encoded message *" << endl;
    cout << "* " << "4. Back                                   *" << endl;
    cout << setw(46) << setfill('*') << "\n";
}

void printMenu3_encrypt_multi()
{
    cout << setw(46) << setfill('*') << "\n";
    cout << "* " << "Menu - Level 3: Encryption                *" << endl;
    cout << "* " << "Select an option:                         *" << endl;
    cout << "* " << "1. Enter the round number                 *" << endl;
    cout << "* " << "2. For each round, print the grid and     *\n*    the corresponding encoded message      *" << endl;
    cout << "* " << "3. Back                                   *" << endl;
    cout << setw(46) << setfill('*') << "\n";
}

void printMenu2_decrypt()
{
    cout << setw(43) << setfill('*') << "\n";
    cout << "* " << "Menu - Level 2: Decryption             *" << endl;
    cout << "* " << "Select an option:                      *" << endl;
    cout << "* " << "1. Enter a message                     *" << endl;
    cout << "* " << "2. Enter the round number              *" << endl;
    cout << "* " << "3. For each round, print the grid      *\n*  and the corresponding decoded message *" << endl;
    cout << "* " << "4. Back                                *" << endl;
    cout << setw(43) << setfill('*') << "\n";
}

#endif // MENU_PRINTER_HPP