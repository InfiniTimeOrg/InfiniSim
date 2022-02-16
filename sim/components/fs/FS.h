#pragma once

#include <cstdio>
#include <cstdint>
//#include "drivers/SpiNorFlash.h"
//#include <littlefs/lfs.h>
#include <lvgl/lvgl.h>

using lfs_file_t = FILE*;

// copied from src/libs/littlefs/lfs.h
// Possible error codes, these are negative to allow
// valid positive return values
enum lfs_error {
    LFS_ERR_OK          = 0,    // No error
    LFS_ERR_IO          = -5,   // Error during device operation
    LFS_ERR_CORRUPT     = -84,  // Corrupted
    LFS_ERR_NOENT       = -2,   // No directory entry
    LFS_ERR_EXIST       = -17,  // Entry already exists
    LFS_ERR_NOTDIR      = -20,  // Entry is not a dir
    LFS_ERR_ISDIR       = -21,  // Entry is a dir
    LFS_ERR_NOTEMPTY    = -39,  // Dir is not empty
    LFS_ERR_BADF        = -9,   // Bad file number
    LFS_ERR_FBIG        = -27,  // File too large
    LFS_ERR_INVAL       = -22,  // Invalid parameter
    LFS_ERR_NOSPC       = -28,  // No space left on device
    LFS_ERR_NOMEM       = -12,  // No more memory available
    LFS_ERR_NOATTR      = -61,  // No data/attr available
    LFS_ERR_NAMETOOLONG = -36,  // File name too long
};

enum lfs_open_flags {
    // open flags
    LFS_O_RDONLY = 1,         // Open a file as read only
#ifndef LFS_READONLY
    LFS_O_WRONLY = 2,         // Open a file as write only
    LFS_O_RDWR   = 3,         // Open a file as read and write
    LFS_O_CREAT  = 0x0100,    // Create a file if it does not exist
    LFS_O_EXCL   = 0x0200,    // Fail if a file already exists
    LFS_O_TRUNC  = 0x0400,    // Truncate the existing file to zero size
    LFS_O_APPEND = 0x0800,    // Move to end of file on every write
#endif
};

// File seek flags
enum lfs_whence_flags {
    LFS_SEEK_SET = 0,   // Seek relative to an absolute position
    LFS_SEEK_CUR = 1,   // Seek relative to the current file position
    LFS_SEEK_END = 2,   // Seek relative to the end of the file
};

typedef int32_t  lfs_ssize_t;

namespace Pinetime {
  namespace Controllers {
    class FS {
    public:
      //FS(Pinetime::Drivers::SpiNorFlash&);

      void Init();
      void LVGLFileSystemInit();

      int FileOpen(lfs_file_t* file_p, const char* fileName, const int flags);
      int FileClose(lfs_file_t* file_p);
      int FileRead(lfs_file_t* file_p, uint8_t* buff, uint32_t size);
      int FileWrite(lfs_file_t* file_p, const uint8_t* buff, uint32_t size);
      int FileSeek(lfs_file_t* file_p, uint32_t pos);

      int FileDelete(const char* fileName);

      //int DirOpen(const char* path, lfs_dir_t* lfs_dir);
      //int DirClose(lfs_dir_t* lfs_dir);
      //int DirRead(lfs_dir_t* dir, lfs_info* info);
      //int DirRewind(lfs_dir_t* dir);
      int DirCreate(const char* path);
      int DirDelete(const char* path);

      lfs_ssize_t GetFSSize();
      int Rename(const char* oldPath, const char* newPath);
      //int Stat(const char* path, lfs_info* info);
      void VerifyResource();

      static size_t getSize() {
        return size;
      }
      static size_t getBlockSize() {
        return blockSize;
      }

    private:

      //Pinetime::Drivers::SpiNorFlash& flashDriver;

      /*
      * External Flash MAP (4 MBytes)
      *
      * 0x000000 +---------------------------------------+
      *          |  Bootloader Assets                    |
      *          |  256 KBytes                           |
      *          |                                       |
      * 0x040000 +---------------------------------------+
      *          |  OTA                                  |
      *          |  464 KBytes                           |
      *          |                                       |
      *          |                                       |
      *          |                                       |
      * 0x0B4000 +---------------------------------------+
      *          |  File System                          |
      *          |                                       |
      *          |                                       |
      *          |                                       |
      *          |                                       |
      * 0x400000 +---------------------------------------+
      *
      */
      static constexpr size_t startAddress = 0x0B4000;
      static constexpr size_t size = 0x34C000;
      static constexpr size_t blockSize = 4096;
  
      lv_fs_drv_t fs_drv;
      
      bool resourcesValid = false;
      //const struct lfs_config lfsConfig;

      //lfs_t lfs;

      //static int SectorSync(const struct lfs_config* c);
      //static int SectorErase(const struct lfs_config* c, lfs_block_t block);
      //static int SectorProg(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
      //static int SectorRead(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);

    };
  }
}
