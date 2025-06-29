#include <gtest/gtest.h>
#include "menu_printer.hpp"  // Include your header where necessary

class DecoderTest : public ::testing::Test {
protected:
    Message msg;
    Grid grid;
    Decryption decoder;

    DecoderTest() : decoder(msg, grid) {}

    void SetUp() override {
        // Reset any necessary state before each test
        msg.resetDecrypted();
        msg.resetIndex();
        grid.resetGridSize(); // Assuming the default size is set correctly
    }

    void TearDown() override {
        // Any cleanup after tests can be done here
    }
};

TEST_F(DecoderTest, TestSingleDecryptionBasic) {
    // Setting up the message for decryption
    msg.setTempMessage("OLLEH"); // Set an encrypted message (expected correct encoding)
    grid.setGridSize(3); // Set an appropriate grid size for the decryption

    // Fill the grid manually as per expected input for testing
    grid.fillGridFromUserMessage(); // Placeholder for your actual grid filling method

    decoder.decryption(); // Call the decryption method

    // Check if the decrypted message matches the expected output
    EXPECT_EQ(msg.getDecryptedMessage(), "HELLO");
}

TEST_F(DecoderTest, TestDecryptionWithMultiRound) {
    msg.setTempMessage("ENCRYPTEDMESSAGE"); // Example encrypted message
    grid.setGridSize(5); // Set a valid odd grid size

    // Assume user correctly filled grid for testing
    grid.fillGridFromUserMessage(); // Placeholder for filling method

    decoder.addDecryptRound(); // Prepare for multiple rounds
    decoder.multi_decryption(); // Perform multi-round decryption

    // Ensure the resulting decrypted message matches the expected original message
    EXPECT_EQ(msg.getDecryptedMessage(), "EXPECTEDORIGINALMESSAGE"); // Adjust expected message
}

TEST_F(DecoderTest, TestDecryptionWithNonSquareMessage) {
    msg.setTempMessage("THISISNOTASQUARE"); // Non-square message for testing

    // Test case for the expected behavior: should throw an exception
    EXPECT_THROW(decoder.decryption(), CustomException);
}

TEST_F(DecoderTest, TestDecryptionStopsAtDot) {
    msg.setTempMessage("HELLO."); // Set a message that ends with a dot
    grid.setGridSize(5); // Set grid size that can accommodate

    // Manually set grid for test (mocking the expected filled state)
    grid.fillGridFromUserMessage(); // Placeholder for actual grid filling method

    decoder.decryption(); // Perform decryption with stopping at the dot
    EXPECT_EQ(msg.getDecryptedMessage(), "HELLO."); // Check that it stops correctly
}

TEST_F(DecoderTest, TestGridSizeAdjustment) {
    msg.setTempMessage("MATCHESGRID"); // Encrypted message
    grid.setGridSize(4); // Set an even grid size (which should be invalid)

    // Expect grid adjustment to take place properly
    EXPECT_NO_THROW(decoder.autoGridSize()); // Should adjust to a valid grid size

    EXPECT_EQ(grid.getGridSize(), 5); // Confirm it adjusted to the next odd size
}

TEST_F(DecoderTest, TestInvalidCharacterHandling) {
    msg.setTempMessage("1234@"); // Invalid characters to simulate an error

    EXPECT_THROW(decoder.addMessage(), CustomException); // Expect exception on handling invalid input
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
