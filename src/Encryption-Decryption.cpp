#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <thread> // for this_thread::sleep_for
#include <chrono> // for chrono::seconds
#include <algorithm>
#include <cmath>
#include "menu_printer.hpp"

using namespace std;

class CustomException : public exception
{
public:
    // identify kinds or error
    enum class Type
    {
        Generic,
        InvalidGridSize,
        InvalidInput
    };

private:
    string error_message;
    Type type_;

public:
    // Constructor for generic message
    explicit CustomException(const string &message)
        : error_message(message), type_(Type::Generic) {}

    // Constructor for InvalidGridSizeException
    explicit CustomException(int gridSize)
        : type_(Type::InvalidGridSize)
    {
        error_message = "Invalid grid size: " + to_string(gridSize);
    }

    // Constructor for InvalidInputException
    explicit CustomException(const string &message, bool isInputError)
        : type_(Type::InvalidInput)
    {
        error_message = "Invalid input: " + message;
    }

    virtual const char *what() const noexcept override
    {
        return error_message.c_str();
    }

    Type getType() const
    {
        return type_;
    }
};

// Struct to manage menu context and user input
struct MenuContext
{
    string inputBuffer;                // string buffer
    int userChoice = 0;                // stores what the user chooses to do
    void (*printMenuFunc)() = nullptr; // pointer function to print menu
    int maxOption = 0;                 // maximum valid option for the menu

    // Default constructor
    MenuContext() = default;

    // Constructor using initializer list
    MenuContext(void (*menuFunc)(), int optionCount)
        : printMenuFunc(menuFunc), maxOption(optionCount) {}

    // Method for error handling and getting user options
    void errorHandling()
    {
        cout << "\nEnter an option (number): ";
        getline(cin, inputBuffer);

        // Trim leading/trailing spaces
        inputBuffer.erase(0, inputBuffer.find_first_not_of(" \t\n\r"));
        inputBuffer.erase(inputBuffer.find_last_not_of(" \t\n\r") + 1);

        // Check for empty input (throw exceptionsi if true)
        if (inputBuffer.empty())
        {
            throw CustomException("Input cannot be empty", true);
        }

        stringstream ss(inputBuffer);
        int choice;

        // Validate integer input
        if (!(ss >> choice) || !ss.eof())
        {
            throw CustomException("Input is not a valid number", true);
        }

        // Range check for options
        if (choice < 1 || choice > maxOption)
        {
            throw CustomException("Option out of valid range", true);
        }

        userChoice = choice;
    }
};

void number_error(int *buffer)
{
    string line;
    getline(cin, line);

    if (line.empty())
    {
        throw CustomException("Input cannot be empty", true);
    }

    istringstream iss(line);
    int value;

    if (!(iss >> value) || !(iss.eof()))
    {
        throw CustomException("Input is not a valid integer", true);
    }

    if (value <= 0)
    {
        throw CustomException("Number must be greater than 0", true);
    }

    *buffer = value;
}

class Message
{
private:
    string message;
    string encrypted_message;
    string decrypted_message;
    string temp_message; // for decryption
    int message_index = 0;

public:
    Message() = default;

    // Add message with validation and uppercase conversion
    void addMessage()
    {
        string messageBuffer;
        while (true)
        {
            try
            {
                cout << "Enter a message to encrypt: ";
                getline(cin, messageBuffer);

                // Remove spaces
                messageBuffer.erase(remove(messageBuffer.begin(), messageBuffer.end(), ' '), messageBuffer.end());

                if (!isValidMessage(messageBuffer))
                    throw CustomException("Input must be non-empty and contain only letters (A-Z or a-z).", true);

                // Convert to uppercase
                transform(messageBuffer.begin(), messageBuffer.end(), messageBuffer.begin(),
                          [](unsigned char c)
                          { return toupper(c); });

                message = messageBuffer;
                break;
            }
            catch (const CustomException &e)
            {
                cout << "Error: " << e.what() << endl;
                cout << "Please try again." << endl;
            }
        }
    }

    bool isValidMessage(const string &input) const
    {
        if (input.empty())
            return false;
        for (char c : input)
            if (!(isalpha(static_cast<unsigned char>(c)) || c == '.'))
                return false;
        return true;
    }

    // Getters and setters
    int getMessageLength() const { return static_cast<int>(message.length()); }
    const string &getMessage() const { return message; }
    const string &getEncryptedMessage() const { return encrypted_message; }
    const string &getDecryptedMessage() const { return decrypted_message; }
    const string &getTempMessage() const { return temp_message; }
    int getMessageIndex() const { return message_index; }

    void appendEncryptedMessage(const string &temp) { encrypted_message += temp; }
    void appendEncryptedMessage(char &temp) { encrypted_message += temp; }
    void appendDecryptedMessage(char &temp) { decrypted_message += temp; }
    void setTempMessage(const string &temp) { temp_message = temp; }
    void setUserMessage(const string &temp) { temp_message = temp; }

    void incrementIndex() { ++message_index; }
    void resetIndex() { message_index = 0; }
    void resetEncrypted() { encrypted_message.clear(); }
    void resetDecrypted() { decrypted_message.clear(); }
    void resetMessageDecrypted() { return temp_message.clear(); }
};

class Grid
{
private:
    int grid_size;
    vector<vector<char>> grid;
    Message &message; // Reference to associated Message

public:
    explicit Grid(Message &msg) : message(msg), grid_size(3), grid(3, vector<char>(3, ' ')) {}

    void addGrid()
    {
        int grid_size_buffer;
        int length = message.getMessageLength();

        if (!message.getEncryptedMessage().empty())
            length = static_cast<int>(message.getEncryptedMessage().length());

        while (true)
        {
            try
            {
                cout << "Enter a grid size: ";
                number_error(&grid_size_buffer);

                if ((grid_size_buffer * grid_size_buffer / 2 + 1) < length)
                    throw CustomException(grid_size_buffer);

                if (grid_size_buffer % 2 == 0)
                    throw CustomException(grid_size_buffer);

                grid_size = grid_size_buffer;
                grid.assign(grid_size, vector<char>(grid_size, ' '));
                break;
            }
            catch (const CustomException &e)
            {
                cout << "Error: " << e.what() << endl;
                cout << "Please enter a valid odd grid size large enough for the message." << endl;
            }
        }
    }

    void autoGridSize()
    {
        int length = message.getMessageLength();
        if (!message.getEncryptedMessage().empty())
            length = static_cast<int>(message.getEncryptedMessage().length());

        grid_size = 3;
        const int max_grid_size = 99;

        while ((grid_size * grid_size / 2 + 1) < length)
        {
            grid_size += 2;
            if (grid_size > max_grid_size)
                throw CustomException("Message too long for maximum grid size", true);
        }

        grid.assign(grid_size, vector<char>(grid_size, ' '));
    }

    // Print the grid (column-major order)
    virtual void printGrid()
    {
        if (message.getMessage().empty())
        {
            cout << "Please enter a message first" << endl;
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }

        if (message.getMessage().length() > grid_size * grid_size / 2 + 1)
        {
            cout << "Error: grid cannot contain message" << endl;
            this_thread::sleep_for(chrono::seconds(1));
            return;
        }

        srand(static_cast<unsigned int>(time(nullptr)));
        for (size_t i = 0; i < grid_size; i++)
        {
            for (size_t j = 0; j < grid_size; j++)
            {
                if (grid[j][i] == ' ')
                {
                    grid[j][i] = 'A' + rand() % 26;
                }
                cout << grid[j][i] << ' ';
            }
            cout << endl;
        }
    }
    // Getters and setters
    int getGridSize() const { return grid_size; }
    vector<vector<char>> &getGrid() { return grid; }
    int getDiamondSize() const { return grid_size * grid_size / 2 + 1; }

    void resetGridSize() { grid_size = 3; }
    void setGridSize(int size) { grid_size = size; }
    void incrementGridSize() { grid_size += 2; }
};

class Processor
{
protected:
    Message &message;
    Grid &grid;

public:
    Processor(Message &msg, Grid &grd) : message(msg), grid(grd) {}

    void resetIndex() { message.resetIndex(); }
    void incrementIndex() { message.incrementIndex(); }

    Message &getMessage() { return message; }
    Grid &getClassGrid() { return grid; }
};

class Encryption : public Processor
{
private:
    int encrypt_round;

public:
    Encryption(Message &msg, Grid &grd) : Processor(msg, grd) {}

    void addEncryptRound()
    {
        int buffer;
        cout << "Enter the round number: ";
        number_error(&buffer);
        encrypt_round = buffer;
    }

    // final encrypted message
    void secret_message(vector<vector<char>> &Grid)
    {
        string temp;
        for (int col = 0; col < grid.getGridSize(); col++)
        {
            for (int row = 0; row < grid.getGridSize(); row++)
            {
                temp += Grid[col][row];
            }
        }
        message.appendEncryptedMessage(temp);
    }

    void encryption()
    {
        int size = grid.getGridSize();
        int tip = size / 2;
        int length = message.getEncryptedMessage().length() > 0 ? message.getEncryptedMessage().length() : message.getMessageLength();
        const string &current_message = message.getEncryptedMessage().length() > 0 ? message.getEncryptedMessage() : message.getMessage();

        int max_chars_half = 1 + tip * 2;
        int upper_offset = 0, lower_offset = tip - 1;
        int counter = 0;
        int msg_index = message.getMessageIndex(); // current int dex of the message

        while (msg_index < length)
        {
            // Fill upper half
            for (int i = counter; i < max_chars_half && msg_index < length; i++, msg_index++)
            {
                int row = i;
                int col = (i <= tip) ? (tip - upper_offset++) : (tip - lower_offset--);
                grid.getGrid()[row][col] = current_message[msg_index];
            }

            lower_offset = 1;
            // Fill lower half
            for (int j = size - 2 - counter; j > counter && msg_index < length; j--, msg_index++)
            {
                int col = tip + lower_offset;
                grid.getGrid()[j][col] = current_message[msg_index];
                lower_offset = (j > tip) ? lower_offset + 1 : lower_offset - 1;
            }

            max_chars_half--;
            upper_offset = 0;
            counter++;
            lower_offset = tip - counter - 1;
        }
        resetIndex(); // resetting the index position for the new round
    }

    void multi_encryption()
    {
        for (int i = 0; i < encrypt_round; i++)
        {
            grid.autoGridSize();
            encryption();
            grid.printGrid();
            message.resetEncrypted();
            secret_message(grid.getGrid());
            cout << "Corresponding encrypted message:\n"
                 << message.getEncryptedMessage() << endl;
            this_thread::sleep_for(chrono::seconds(5));
        }
        message.resetEncrypted();
    }

    void encryptAndDisplay()
    {
        if (getClassGrid().getGridSize() <= 0)
            throw CustomException("Grid size must be greater than 0. Please set the grid size first.", true);

        encryption();
        getClassGrid().printGrid();
        secret_message(getClassGrid().getGrid());
        cout << "Encrypted message (5 second display): " << getMessage().getEncryptedMessage() << endl;

        getMessage().resetEncrypted();

        this_thread::sleep_for(chrono::seconds(5));
    }

    // getter function
    int getEncryptRound() const { return encrypt_round; }
};

class Decryption : public Processor
{
private:
    int decrypt_round;

public:
    Decryption(Message &msg, Grid &grd) : Processor(msg, grd) {}

    void autoGridSize()
    {
        int root = static_cast<int>(sqrt(message.getTempMessage().length()));
        if (root % 2 == 0)
            grid.setGridSize(root - 1);
        else
            grid.setGridSize(root);

        grid.getGrid().assign(grid.getGridSize(), vector<char>(grid.getGridSize(), ' '));
    }

    void addDecryptRound()
    {
        int buffer;
        cout << "Enter the round number: ";
        number_error(&buffer);
        decrypt_round = buffer;
    }

    void addMessage()
    {
        message.resetDecrypted();
        message.resetMessageDecrypted();

        string messageBuffer;

        while (true)
        {
            try
            {
                cout << "Enter a message to decrypt: ";
                getline(cin, messageBuffer);

                // Remove spaces
                messageBuffer.erase(remove(messageBuffer.begin(), messageBuffer.end(), ' '), messageBuffer.end());

                // Validate message content
                if (!message.isValidMessage(messageBuffer))
                {
                    throw CustomException("Input must be non-empty and contain only letters (A-Z or a-z).", true);
                }

                // Check if message length is a perfect square
                int root = static_cast<int>(sqrt(messageBuffer.length()));
                if (root * root < static_cast<int>(messageBuffer.length()))
                {
                    throw CustomException("The message must be a perfect square grid", true);
                }

                // Convert to uppercase
                transform(messageBuffer.begin(), messageBuffer.end(), messageBuffer.begin(),
                          [](unsigned char c)
                          { return toupper(c); });

                break; // valid input, exit loop
            }
            catch (const CustomException &e)
            {
                cout << "Error: " << e.what() << endl;
                cout << "Please try again." << endl;
            }
        }

        message.setTempMessage(messageBuffer);
        cout << message.getTempMessage() << endl;
    }

    void printGrid()
    {
        if (message.getTempMessage().empty())
        {
            throw CustomException("Please enter a message first", true);
        }

        int gridSize = grid.getGridSize();
        int messageLength = static_cast<int>(message.getTempMessage().length());

        for (int row = 0; row < gridSize; ++row)
        {
            for (int col = 0; col < gridSize; ++col)
            {
                cout << grid.getGrid()[row][col] << ' ';
            }
            cout << endl;
        }
    }

    void fillGridFromUserMessage()
    {
        int gridSize = grid.getGridSize();
        int messageLength = static_cast<int>(message.getTempMessage().length());
        int index = 0;

        grid.getGrid().assign(gridSize, vector<char>(gridSize, '.'));

        for (int row = 0; row < gridSize; ++row)
        {
            for (int col = 0; col < gridSize; ++col)
            {
                if (index < messageLength)
                {
                    grid.getGrid()[col][row] = message.getTempMessage()[index++];
                }
            }
        }
    }

    void decryption(bool stopAtDot = true)
    {
        int tip = grid.getGridSize() / 2;
        int upper_offset = 0, lower_offset = tip - 1;
        int max_chars_half = 1 + tip * 2;
        int counter = 0;
        int chars_added = 0;
        bool foundDot = false;
        char c;

        while (!foundDot && chars_added < grid.getDiamondSize())
        {
            for (int i = counter; i < max_chars_half; i++, chars_added++)
            {
                c = grid.getGrid()[tip - (i <= tip ? upper_offset++ : lower_offset--)][i];
                if (stopAtDot && c == '.')
                {
                    message.appendDecryptedMessage(c);
                    foundDot = true;
                    break;
                }
                message.appendDecryptedMessage(c);
            }
            if (foundDot)
                break;

            lower_offset = 1;
            for (int j = grid.getGridSize() - 2 - counter; j > counter; j--, chars_added++)
            {
                c = grid.getGrid()[tip + lower_offset][j]; // Fixed here
                lower_offset += (j > tip) ? 1 : -1;
                if (stopAtDot && c == '.')
                {
                    foundDot = true;
                    break;
                }
                message.appendDecryptedMessage(c);
            }
            max_chars_half--;
            upper_offset = 0;
            counter++;
            lower_offset = tip - counter - 1;
        }
    }

    void multi_decryption()
    {
        for (int i = 0; i < decrypt_round; i++)
        {
            autoGridSize();
            fillGridFromUserMessage();
            printGrid();

            bool stopAtDot = (i == decrypt_round - 1);
            decryption(stopAtDot);

            cout << "Corresponding decoded message: " << message.getDecryptedMessage() << endl;
            truncate_decrypt_message();
            message.resetDecrypted();

            this_thread::sleep_for(chrono::seconds(5));
        }
        message.resetMessageDecrypted();
    }

    void truncate_decrypt_message()
    {
        int root = static_cast<int>(sqrt(message.getDecryptedMessage().length()));

        if (root * root < static_cast<int>(message.getDecryptedMessage().length()))
        {
            string temp = message.getDecryptedMessage().substr(0, root * root);
            message.setUserMessage(temp);
        }
        else
        {
            message.setUserMessage(message.getDecryptedMessage());
        }
    }

    string getMessageDecrypt() const { return message.getTempMessage(); }
};

class AppContext
{
private:
    Message message;
    Grid grid;
    Encryption encryptor;
    Decryption decryptor;
    string input;
    int userChoice = 0;

public:
    AppContext() : grid(message), encryptor(message, grid), decryptor(message, grid) {}

    // getter functions
    const string &getInput() const { return input; }
    int getUserChoice() const { return userChoice; }
    Message &getMessage() { return message; }
    Grid &getGrid() { return grid; }
    Encryption &getEncryptor() { return encryptor; }
    Decryption &getDecryptor() { return decryptor; }

    // setter functions
    void setInput(string &temp) { input = temp; }
    void setUserChoice(int temp) { userChoice = temp; }
};

// Updated menu functions take AppContext& instead of separate encryptor/decryptor
void menu1(AppContext &ctx);
void menu2_encrypt(AppContext &ctx);
void menu3_encrypt_oneRound(AppContext &ctx);
void menu3_encrypt_multi(AppContext &ctx);
void menu2_decrypt(AppContext &ctx);

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    AppContext ctx; // struct that holds are functionalities
    menu1(ctx);     // starting off with menu 1

    cout << "Thank you for using the encryption program!" << endl;
    return 0;
}

void no_message(AppContext &ctx, int option)
{
    if (ctx.getEncryptor().getMessage().getMessageLength() == 0 && option == 1)
    {
        cout << "Enter a message first" << endl;
        this_thread::sleep_for(chrono::seconds(2));
        system("clear");
        menu1(ctx);
        return;
    }
    if (ctx.getDecryptor().getMessageDecrypt().length() == 0 && option == 2)
    {
        cout << "Enter a message first" << endl;
        this_thread::sleep_for(chrono::seconds(2));
        system("clear");
        menu1(ctx);
        return;
    }
}

// Example menu1 implementation
void menu1(AppContext &ctx)
{
    MenuContext context(printMenu1, 3);

    while (true)
    {
        system("clear");
        printMenu1();
        try
        {
            context.errorHandling(); // Throws on invalid inpu∫t
            break;                   // Valid input, exit loop
        }
        catch (const CustomException &e)
        {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again." << endl;
            this_thread::sleep_for(chrono::seconds(1)); // Optional pause for readability
        }
    }

    ctx.setInput(context.inputBuffer);
    ctx.setUserChoice(context.userChoice);

    switch (ctx.getUserChoice())
    {
    case 1:
        menu2_encrypt(ctx);
        break;
    case 2:
        menu2_decrypt(ctx);
        break;
    case 3:
        system("clear");
        cout << "Quitting..." << endl;
        exit(0);
    }
}

void menu2_encrypt(AppContext &ctx)
{
    MenuContext context(printMenu2_encrypt, 4);

    while (true)
    {
        system("clear");
        context.printMenuFunc();
        context.inputBuffer = ctx.getInput();
        try
        {
            context.errorHandling(); // Throws on invalid input

            switch (context.userChoice)
            {
            case 1:
                ctx.getEncryptor().getMessage().addMessage();
                break;
            case 2:
                menu3_encrypt_oneRound(ctx);
                return;
            case 3:
                menu3_encrypt_multi(ctx);
                return;
            case 4:
                menu1(ctx);
                return;
            }
        }
        catch (const CustomException &e)
        {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again." << endl;
            this_thread::sleep_for(chrono::seconds(1)); // Optional pause for readability
        }
    }
}

void menu3_encrypt_oneRound(AppContext &ctx)
{
    MenuContext context(printMenu3_encrypt_oneRound, 4);
    bool stayInMenu = true;

    no_message(ctx, 1);

    while (stayInMenu)
    {
        system("clear");
        context.printMenuFunc();
        try
        {
            context.errorHandling(); // Throws on invalid input
            ctx.setInput(context.inputBuffer);

            switch (context.userChoice)
            {
            case 1:
                ctx.getEncryptor().getClassGrid().addGrid();
                break;
            case 2:
                ctx.getEncryptor().getClassGrid().autoGridSize();
                break;
            case 3:
                ctx.getEncryptor().encryptAndDisplay();
                this_thread::sleep_for(chrono::seconds(5));
                break;

                break;
            case 4:
                menu2_encrypt(ctx);
                return;
            }
        }
        catch (const CustomException &e)
        {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again." << endl;
            this_thread::sleep_for(chrono::seconds(1)); // Optional pause for readability
        }
    }
}

void menu3_encrypt_multi(AppContext &ctx)
{
    MenuContext context(printMenu3_encrypt_multi, 3);
    bool stayInMenu = true;

    no_message(ctx, 1);

    while (stayInMenu)
    {
        system("clear");
        context.printMenuFunc();
        ctx.setInput(context.inputBuffer);

        try
        {
            context.errorHandling(); // Throws on invalid input

            switch (context.userChoice)
            {
            case 1:
                ctx.getEncryptor().addEncryptRound();
                break;
            case 2:
                ctx.getEncryptor().multi_encryption();
                break;
            case 3:
                menu2_encrypt(ctx);
                return;
            }
        }
        catch (const CustomException &e)
        {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again." << endl;
            this_thread::sleep_for(chrono::seconds(1)); // Optional pause for readability
        }
    }
}

void menu2_decrypt(AppContext &ctx)
{
    MenuContext context(printMenu2_decrypt, 4);
    bool stayInMenu = true;

    while (stayInMenu)
    {
        system("clear");
        context.printMenuFunc();
        ctx.setInput(context.inputBuffer);
        try
        {
            context.errorHandling(); // Throws on invalid input

            switch (context.userChoice)
            {
            case 1:
                ctx.getDecryptor().addMessage();
                break;
            case 2:
                ctx.getDecryptor().addDecryptRound();
                break;
            case 3:
                no_message(ctx, 2); // check if the message decrypt is in there
                ctx.getDecryptor().multi_decryption();
                break;
            case 4:
                menu1(ctx);
                return;
            }
        }
        catch (const CustomException &e)
        {
            cout << "Error: " << e.what() << endl;
            cout << "Please try again." << endl;
            this_thread::sleep_for(chrono::seconds(1)); // Optional pause for readability
        }
    }
}
