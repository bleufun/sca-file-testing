#include "File.h"
#include "FileImpl.h"
#include <iostream>

FileImpl::FileImpl(const CF::string_t _fileName, const bool _isReadOnly)
  : fileName(_fileName), isReadOnly(_isReadOnly)
{
  const std::ios::openmode fileOpenMode = isReadOnly ?
    std::ios::binary | std::ios::in :
    std::ios::binary | std::ios::in | std::ios::out;

  fileHandle = std::basic_fstream<char>();
  fileHandle.open(fileName, fileOpenMode);
  fileHandle.exceptions(std::fstream::badbit | std::fstream::failbit);
  if(!fileHandle.is_open())
    throw CF::FileException(CF::CF_EIO);
}

const CF::string_t FileImpl::getFileName() noexcept {
  return fileName;
}

const CF::unsignedLong_t FileImpl::getFilePointer() noexcept {
  return fileHandle.is_open() ? (CF::unsignedLong_t)fileHandle.tellg() : 0;
}

void FileImpl::read(CF::octetSequence_t &data, CF::unsignedLong_t length) {
  CF::unsignedLong_t oldPointer = getFilePointer();
  data.resize(length);
  try {
    fileHandle.read((char *)data.data(), length);
  }
  catch(std::ios::failure) {
    if(!fileHandle.eof()) {
      // fileHandle.clear();
      setFilePointer(oldPointer);
      throw CF::File::IOException(CF::ErrorNumberType::CF_EIO);
    }
    else if(!fileHandle.is_open()) {
      throw CF::File::IOException(CF::ErrorNumberType::CF_EIO);
    } else {
      fileHandle.clear();
    }
  }
  data.resize(fileHandle.gcount());

  if(fileHandle.eof())
    fileHandle.clear();
}

void FileImpl::write(const CF::octetSequence_t &data) {
  if(isReadOnly)
    throw CF::File::IOException(CF::ErrorNumberType::CF_EACCES);

  CF::unsignedLong_t oldPos = getFilePointer();
  try {
    fileHandle.write((char*)data.data(), data.size());
    fileHandle.flush(); // sync with the underlying file
  }
  catch(std::ios::failure) {
    if (fileHandle.is_open()) {
      fileHandle.clear();
      fileHandle.seekp(oldPos);
    }
    throw CF::File::IOException(CF::ErrorNumberType::CF_EIO);
  }
}

CF::unsignedLong_t const FileImpl::sizeOf() {
  if(!fileHandle.is_open())
    throw CF::FileException(CF::ErrorNumberType::CF_EIO);

  CF::unsignedLong_t oldPointer = fileHandle.tellg();
  fileHandle.seekg(0, std::ios::end);
  CF::unsignedLong_t fileSize = fileHandle.tellg();
  fileHandle.seekg(oldPointer);
  return fileSize;
}

void FileImpl::close() {
  try {
    fileHandle.close();
  }
  catch(std::ios::failure) {
    throw CF::FileException(CF::ErrorNumberType::CF_EIO);
  }
}

void FileImpl::setFilePointer(CF::unsignedLong_t filePointer) {
  if(sizeOf() < filePointer) {
    throw CF::File::InvalidFilePointer();
  }
  if(!fileHandle.good()) {
    throw CF::FileException(CF::ErrorNumberType::CF_EIO);
  }
  fileHandle.seekg(filePointer);
}

CF::File::IOException::IOException(CF::ErrorNumberType errCode)
  : errorNumberType(errCode) {}

CF::FileException::FileException(CF::ErrorNumberType errCode)
  : errorNumberType(errCode) {}
