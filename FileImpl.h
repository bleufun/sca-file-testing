#ifndef __FILE_INTERFACE_IMPLEMENTATION_H
#define __FILE_INTERFACE_IMPLEMENTATION_H

#include <fstream>
#include "File.h"

#ifdef CPP_TEST_FLAG
#include <gtest/gtest.h>
#endif

class FileImpl : private CF::File {
public:
  FileImpl(CF::string_t _fileName, bool isReadOnly);

  const CF::string_t getFileName() noexcept;
  const CF::unsignedLong_t getFilePointer() noexcept;

  void read(CF::octetSequence_t &data, CF::unsignedLong_t length);
  void write(const CF::octetSequence_t &data);
  CF::unsignedLong_t const sizeOf();
  void close();
  void setFilePointer(CF::unsignedLong_t filePointer);
private:
  std::basic_fstream<char> fileHandle;
  CF::string_t fileName;
  bool isReadOnly;

  #ifdef CPP_TEST_FLAG
  FRIEND_TEST(fileImplBasics, constructorTest);
  FRIEND_TEST(fileImplPropertiesTest, onClose);
  #endif
};

#endif
