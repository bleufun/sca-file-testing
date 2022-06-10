#include <gtest/gtest.h>
#include <string>
#include <cstdio>
#include "File.h"
#include "FileImpl.h"

const std::string testFileName = "file.txt";

class fileImplTestBase : public ::testing::Test {
protected:
  void SetUp(bool readOnly, std::string dummyFileContent = "") {
    const testing::TestInfo* const testInfo = testing::UnitTest::GetInstance()->current_test_info();
    fullFileName = testInfo->test_suite_name() + std::string("_") + testInfo->name() + std::string("_") + testFileName;

    // create test file
    FILE *tempFile = fopen(fullFileName.c_str(), "w+");
    fwrite(dummyFileContent.c_str(), sizeof(char), dummyFileContent.size(),
           tempFile);
    fclose(tempFile);

    fi = new FileImpl(fullFileName, readOnly);
  }

  void TearDown() override {
    try {
      fi->close();
    }
    catch(std::exception &e) {
      // ignore exceptions
    }
    delete fi;
    remove(fullFileName.c_str());
  }

  bool compareFileContent(const CF::octetSequence_t content) {
  std::ifstream inputFile(fullFileName, std::ifstream::binary);

  int i = 0;
  for (CF::octet_t c; inputFile >> c;) {
    if (c != content[i++]) {
      inputFile.close();
      return false;
    }
    }
    inputFile.close();
    if(content.size() != i)
      return false;
    return true;
  }

  CF::string_t fullFileName;
  FileImpl* fi;
};

using fileImplTest = fileImplTestBase;
using fileImplReadTest = fileImplTestBase;
using fileImplWriteTest = fileImplTestBase;
using fileImplPointerTest = fileImplTestBase;
using fileImplPropertiesTest = fileImplTestBase;
using fileImplReadFlagTest = fileImplTestBase;

TEST(fileImplBasics, constructorTest) {
  std::string filename ="fileImplBasics_constructorTest_file.txt";
  FILE *tempFile = fopen(filename.c_str(), "w+");
  fclose(tempFile);

  EXPECT_NO_THROW({
    FileImpl fi(filename, false);
    EXPECT_TRUE(fi.fileHandle.is_open());
    fi.close();
    FileImpl fi2(filename, true);
    EXPECT_TRUE(fi2.fileHandle.is_open());
    fi2.close();
  });

  remove(filename.c_str());
}

/*
 * The read operation shall read, from the referenced file, the number of octets
 * specified by the input length parameter (..).
 */
TEST_F(fileImplReadTest, readFromFile) {
  std::string dummy = "abcd";
  //CF::octetSequence_t dummySequence(dummy.begin(), dummy.end());
  CF::octetSequence_t dummySequence = {'a','b','c','d'};
  SetUp(true, dummy);

  CF::octetSequence_t tempSeq;
  EXPECT_NO_THROW(fi->read(tempSeq, dummy.size()));
  EXPECT_EQ(tempSeq, dummySequence);
}

/*
 * The read operation shall raise the IOException when a read error occurs (..).
 */
TEST_F(fileImplReadTest, readIOException) {
  SetUp(true);
  ASSERT_NO_THROW({fi->close();});

  CF::octetSequence_t seq2;
  EXPECT_THROW({ fi->read(seq2, 4); }, CF::File::IOException);
}

/*
 * The read operation shall read less than the number of octets specified in the
 * input length parameter, when an end of file is encountered.
 */
TEST_F(fileImplReadTest, readAfterEOF) {
  std::string dummy = "abcd";
  CF::octetSequence_t dummySequence = {'a', 'b', 'c', 'd'};
  SetUp(true, dummy);
  ASSERT_GE(dummy.size(), 2);
  CF::octetSequence_t tempSeq;

  ASSERT_NO_THROW(fi->read(tempSeq, 1)); // remove first octet
  ASSERT_NO_THROW({fi->read(tempSeq, dummy.size() + 1 - 1);}); // try to read everything to the eof plus one octet
  EXPECT_EQ(tempSeq.size(), dummySequence.size() - 1);
  EXPECT_EQ(tempSeq, CF::octetSequence_t({'b','c','d'}));
  EXPECT_EQ(tempSeq, CF::octetSequence_t(dummySequence.begin() + 1, dummySequence.end()));

  ASSERT_NO_THROW({fi->read(tempSeq, 1);});
  EXPECT_EQ(tempSeq.size(), 0);
}

/*
 * The write operation shall write data to the file referenced
 */
TEST_F(fileImplWriteTest, writeToFile) {
  std::string dummy = "abcd";
  CF::octetSequence_t dummySequence = {'a','b','c','d'};
  CF::octetSequence_t tempSeq;
  SetUp(false, dummy);

  EXPECT_NO_THROW({
    fi->write(tempSeq);
    // fi->close();
  });
  EXPECT_TRUE(compareFileContent(dummySequence));
}

TEST_F(fileImplWriteTest, writeIOException) {
  std::string dummy = "abcd";
  CF::octetSequence_t dummySequence = {'a','b','c','d'};
  SetUp(false, dummy);
  ASSERT_NO_THROW(fi->close());

  EXPECT_THROW(fi->write(dummySequence), CF::File::IOException);
}

/*
 * If the file was opened (..) with an input read_Only parameter value of TRUE,
 * writes to the file are considered to be in error (..).
 */
TEST_F(fileImplReadFlagTest, writeOnReadOnly) {
  std::string dummy = "abcd";
  CF::octetSequence_t dummySeq = {'a','b','c','d'};
  SetUp(true, dummy);
  ASSERT_NO_THROW({

    EXPECT_THROW({ fi->write(dummySeq); }, CF::File::IOException);
    EXPECT_EQ(fi->getFilePointer(), 0);
  });
}

/*
 * (..) filePointer points to the beginning of the file upon construction of
 * the file object (..).
 */
TEST_F(fileImplPointerTest, pointerOnObjectConstruction) {
  SetUp(false, "abcd");

  EXPECT_EQ(fi->getFilePointer(), 0);
}

/*
 * The setFilePointer operation shall raise the InvalidFilePointer exception when
 * the value of the filePointer parameter exceeds the file size (..).
 */
TEST_F(fileImplPointerTest, pointerBeyondFileException) {
  std::string dummy = "abcd";
  SetUp(false, dummy);
  ASSERT_NO_THROW(fi->setFilePointer(dummy.size() - 1));

  EXPECT_THROW({ fi->setFilePointer(dummy.size() + 1024); }, CF::File::InvalidFilePointer);
  EXPECT_EQ(fi->getFilePointer(), dummy.size() - 1);
}

/*
 * The read operation shall (..) advance the value of the filePointer attribute
by the number of octets actually read (..).
 */
TEST_F(fileImplPointerTest, filePointerAfterRead) {
  std::string dummy = "abcd";
  SetUp(false, dummy);
  ASSERT_EQ(fi->getFilePointer(), 0);

  CF::octetSequence_t seq;
  ASSERT_NO_THROW(fi->read(seq, dummy.size()));

  EXPECT_EQ(fi->getFilePointer(), dummy.size());
}

/*
 * The write operation shall increment the filePointer attribute to reflect
 * the number of octets written (..).
 */
TEST_F(fileImplPointerTest, filePointerAfterWrite) {
  SetUp(false);
  ASSERT_EQ(fi->getFilePointer(), 0);
  ASSERT_NO_THROW(fi->write({'x','y'}));

  EXPECT_EQ(fi->getFilePointer(), 2);
}

/*
 * The setFilePointer operation shall raise the CF FileException when the file
 * pointer for the referenced file cannot be set to the value of the input
 * filePointer parameter (..).
 */
TEST_F(fileImplPointerTest, filePointerAfterCloseFileException) {
  std::string dummy = "abcd";
  SetUp(false, dummy);
  ASSERT_NO_THROW(fi->close());

  EXPECT_THROW(fi->setFilePointer(dummy.size() - 1), CF::FileException);
}

/*
 * The sizeOf operation shall return the number of octets stored in the file (..).
 */
TEST_F(fileImplPropertiesTest, onSizeOf) {
  std::string dummy = "abcd";
  SetUp(false, dummy);
  CF::octetSequence_t finalFileContent = {'a','b','c','d','e','f'};

  EXPECT_EQ(fi->sizeOf(), dummy.size());
  EXPECT_NO_THROW(fi->setFilePointer(dummy.size()));
  ASSERT_NO_THROW(fi->write({'e','f'}));

  EXPECT_EQ(fi->sizeOf(), dummy.size() + 2);
  EXPECT_TRUE(compareFileContent(finalFileContent));
}

/*
 * The sizeOf operation shall raise the CF FileException when a file-related
 * error occurs (..).
 */
TEST_F(fileImplPropertiesTest, sizeOfFileExceptionAfterClose) {
  SetUp(false);

  fi->close();
  EXPECT_THROW(fi->sizeOf(), CF::FileException);
}

/*
 * The close operation shall release any OE file resources associated with the
 * component (..). The close operation shall raise the CF FileException when it cannot
 * successfully close the file (..).
 */
TEST_F(fileImplPropertiesTest, onClose) {
  SetUp(false);
  ASSERT_TRUE(fi->fileHandle.is_open());
  EXPECT_NO_THROW(fi->close());
  ASSERT_FALSE(fi->fileHandle.is_open());

  EXPECT_THROW(fi->close(), CF::FileException);
  ASSERT_FALSE(fi->fileHandle.is_open());
}

TEST_F(fileImplTest, readAndWrite) {
  CF::octetSequence_t seq;
  const CF::octetSequence_t result = {'c', 'd'};
  SetUp(false);

  EXPECT_NO_THROW({
      fi->write({'a','b','c','d'});
      EXPECT_EQ(fi->getFilePointer(), 4);
      fi->setFilePointer(2);
      fi->read(seq, 10);
      EXPECT_EQ(fi->getFilePointer(), 4);
    });
  EXPECT_TRUE(compareFileContent({'a','b','c','d'}));
  EXPECT_EQ(seq, result);
}
