#ifndef __FILE_INTERFACE_H
#define __FILE_INTERFACE_H

#include <cstdint>
#include <exception>
#include <string>
#include <vector>

namespace CF {
  using unsignedLong_t = uint32_t;
  using string_t = std::string;
  using octet_t = uint8_t;
  using octetSequence_t = std::vector<octet_t>;

  enum ErrorNumberType {
    CF_NOTSET,
    CF_E2BIG,
    CF_EACCES,
    CF_EAGAIN,
    CF_EBADF,
    CF_EBADMSG,
    CF_EBUSY,
    CF_ECANCELED,
    CF_ECHILD,
    CF_EDEADLK,
    CF_EDOM,
    CF_EEXIST,
    CF_EFAULT,
    CF_EFBIG,
    CF_EINPROGRESS,
    CF_EINTR,
    CF_EINVAL,
    CF_EIO,
    CF_EISDIR,
    CF_EMFILE,
    CF_EMLINK,
    CF_EMSGSIZE,
    CF_ENAMETOOLONG,
    CF_ENFILE,
    CF_ENODEV,
    CF_ENOENT,
    CF_ENOEXEC,
    CF_ENOLCK,
    CF_ENOMEM,
    CF_ENOSPC,
    CF_ENOSYS,
    CF_ENOTDIR,
    CF_ENOTEMPTY,
    CF_ENOTSUP,
    CF_ENOTTY,
    CF_ENXIO,
    CF_EPERM,
    CF_EPIPE,
    CF_ERANGE,
    CF_EROFS,
    CF_ESPIPE,
    CF_ESRCH,
    CF_ETIMEDOUT,
    CF_EXDEV
  };

  class FileException : public std::exception {
  public:
    FileException(const ErrorNumberType errCode);
    const ErrorNumberType errorNumberType;
  };

  class File {
  public:
    virtual const string_t getFileName() noexcept = 0;
    virtual const unsignedLong_t getFilePointer() noexcept = 0;
    virtual void read(octetSequence_t& data, unsignedLong_t length) = 0;
    virtual void write(const octetSequence_t& data) = 0;
    virtual unsignedLong_t const sizeOf() = 0;
    virtual void close() = 0;
    virtual void setFilePointer(unsignedLong_t filePointer) = 0;

    class IOException : public std::exception {
    public:
      IOException(const ErrorNumberType errCode);
      const ErrorNumberType errorNumberType;
    };

    class InvalidFilePointer : public std::exception {};

  protected:
    File() = default;
  };
};

#endif
