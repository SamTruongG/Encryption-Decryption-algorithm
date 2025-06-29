#include <gtest/gtest.h>
#include "menu_printer.hpp"  // Include your header where necessary

class EncoderTest : public ::testing::Test {
protected:
    Message msg;
    Grid grid;
    Encryption encoder;

    EncoderTest() : encoder(msg, grid) {}

    void SetUp() override {
        // Reset any necessary state before each test
        msg.resetEncrypted();
        msg.resetDecrypted();
        msg.resetIndex();
        grid.resetGridSize(); // Assuming the default size is set correctly
    }

    void TearDown() override {
        // Any cleanup after tests can be done here
    }
};

TEST_F(EncoderTest, TestSingleEncryptionBasic) {
    msg.setTempMessage("HELLO"); // Set a valid message
    grid.setGridSize(3); // Set a suitable grid size

    encoder.encryption(); // Call the encryption method

    // Check that the encrypted message is not empty
    EXPECT_FALSE(msg.getEncryptedMessage().empty());
}

TEST_F(EncoderTest, TestEncryptionWithOddGridSize) {
    msg.setTempMessage("HELLO"); // Set a valid message
    grid.setGridSize(5); // Set an odd grid size

    encoder.encryption(); // Encrypt the message

    auto encryptedMsg = msg.getEncryptedMessage();
    // Verify the result based on your encryption logic (example: length should be greater than 0)
    EXPECT_EQ(encryptedMsg.length(), 5); // Assumed expected length based on the message
}

TEST_F(EncoderTest, TestEncryptionWithMultiRound) {
    msg.setTempMessage("HELLOWORLD"); // Set a longer valid message
    encoder.addEncryptRound(); // Set the number of rounds, assume user inputs this suitably

    encoder.multi_encryption(); // Perform multi-round encryption

    // Check that the encrypted message is produced after multiple rounds
    EXPECT_FALSE(msg.getEncryptedMessage().empty());
}

TEST_F(EncoderTest, TestEncryptionGridOverflow) {
    msg.setTempMessage("THISMESSAGEISTOOLONG"); // Set an intentionally long message
    grid.setGridSize(5); // Set an odd grid size that's too small

    // This should throw an exception due to grid size constraints
    EXPECT_THROW(encoder.encryption(), CustomException);
}

TEST_F(EncoderTest, TestInvalidMessageHandling) {
    msg.setTempMessage("12345"); // Set an invalid message (non-alpha characters)

    // This should throw an exception when attempting to encrypt
    EXPECT_THROW(encoder.encryption(), CustomException);
}

TEST_F(EncoderTest, TestSettingRounds) {
    int rounds = 3; // Number of encryption rounds
    encoder.addEncryptRound(); // Assuming this method sets the rounds
    EXPECT_EQ(encoder.getEncryptRound(), rounds); // Check if the rounds are set properly
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
