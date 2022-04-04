#include "FS.h"
#include <cassert>
#include <cstring>
#include <filesystem>
//#include <littlefs/lfs.h>
#include <lvgl/lvgl.h>

using namespace Pinetime::Controllers;

//FS::FS(Pinetime::Drivers::SpiNorFlash& driver)
//  : flashDriver {driver},
//    lfsConfig {
//      .context = this,
//      .read = SectorRead,
//      .prog = SectorProg,
//      .erase = SectorErase,
//      .sync = SectorSync,
//
//      .read_size = 16,
//      .prog_size = 8,
//      .block_size = blockSize,
//      .block_count = size / blockSize,
//      .block_cycles = 1000u,
//
//      .cache_size = 16,
//      .lookahead_size = 16,
//
//      .name_max = 50,
//      .attr_max = 50,
//    } {
//}


void FS::Init() {

//  // try mount
//  int err = lfs_mount(&lfs, &lfsConfig);
//
//  // reformat if we can't mount the filesystem
//  // this should only happen on the first boot
//  if (err != LFS_ERR_OK) {
//    lfs_format(&lfs, &lfsConfig);
//    err = lfs_mount(&lfs, &lfsConfig);
//    if (err != LFS_ERR_OK) {
//      return;
//    }
//  }
//
//#ifndef PINETIME_IS_RECOVERY
//  VerifyResource();
//  LVGLFileSystemInit();
//#endif

}

void FS::VerifyResource() {
  // validate the resource metadata
  resourcesValid = true;
}

int  FS::FileOpen(lfs_file_t* file_p, const char* fileName, const int flags) {
  // create the file in the current directory
  const char *local_filename = fileName[0]=='/' ? &fileName[1] : fileName;
  const char *mode;
  bool flag_read   = flags & LFS_O_RDONLY;
  bool flag_write  = flags & LFS_O_WRONLY;
  bool flag_create = flags & LFS_O_CREAT;
  if (flag_create) {
    if (std::filesystem::exists(local_filename)) {
      if (flag_read && flag_write) {
        mode = "rb+";
      } else if (flag_read) {
        mode = "rb";
      } else if (flag_write) {
        mode = "wb";
      } else {
        assert(false); // not implemented
      }
    } else {
      if (flag_read && flag_write) {
        mode = "wb+";
      } else if (flag_read) {
        assert(false); // read only file not existing
        mode = "rb";
      } else if (flag_write) {
        mode = "wb";
      } else {
        assert(false); // not implemented
      }
    }
  } else {
    if (std::filesystem::exists(local_filename)) {
      if (flag_read && flag_write) {
        mode = "rb+";
      } else if (flag_read) {
        mode = "rb";
      } else if (flag_write) {
        mode = "wb";
      } else {
        assert(false); // not implemented
      }
    } else {
      return LFS_ERR_IO;
    }
  }
  FILE *fptr = fopen(local_filename, mode);
  if (fptr == nullptr) {
    return LFS_ERR_BADF;
  } else {
    *file_p = fptr;
    return LFS_ERR_OK;
  }
  //return lfs_file_open(&lfs, file_p, fileName, flags);
}

int FS::FileClose(lfs_file_t* file_p) {
  return fclose(*file_p);
  //return lfs_file_close(&lfs, file_p);
}

int FS::FileRead(lfs_file_t* file_p, uint8_t* buff, uint32_t size) {
  return fread(buff, sizeof(uint8_t), size, *file_p);
  //return lfs_file_read(&lfs, file_p, buff, size);
}

int FS::FileWrite(lfs_file_t* file_p, const uint8_t* buff, uint32_t size) {
  return fwrite((void*)buff, sizeof(uint8_t), size, *file_p);
  //return lfs_file_write(&lfs, file_p, buff, size);
}

int FS::FileSeek(lfs_file_t* file_p, uint32_t pos) {
  return fseek(*file_p, pos, SEEK_SET);
  //return lfs_file_seek(&lfs, file_p, pos, whence);
}

int FS::FileDelete(const char* fileName) {
  return std::filesystem::remove(fileName);
  //return lfs_remove(&lfs, fileName);
}


int FS::DirCreate(const char* path) {
  return std::filesystem::create_directory(path);
  //return lfs_mkdir(&lfs, path);
}

// Delete directory and all files inside
int FS::DirDelete(const char* path) {
  return std::filesystem::remove_all(path);

  //lfs_dir_t lfs_dir;
  //lfs_info entryInfo;

  //int err;
  //err = lfs_dir_open(&lfs, &lfs_dir, path);
  //if (err) {
  //  return err;
  //}
  //while (lfs_dir_read(&lfs, &lfs_dir, &entryInfo)) {
  //  lfs_remove(&lfs, entryInfo.name);
  //}
  //lfs_dir_close(&lfs, &lfs_dir);
  //return LFS_ERR_OK;
}

// check if file exists, if so write file-size into info object
int FS::Stat(const char* path, lfs_info* info) {
  if (!std::filesystem::exists(path))
  {
    return LFS_ERR_NOENT; // No directory entry
  }
  info->size = std::filesystem::file_size(path);
  return LFS_ERR_OK;
}

/*

    ----------- Interface between littlefs and SpiNorFlash -----------

*/
//int FS::SectorSync(const struct lfs_config* c) {
//  return 0;
//}
//
//int FS::SectorErase(const struct lfs_config* c, lfs_block_t block) {
//  Pinetime::Controllers::FS& lfs = *(static_cast<Pinetime::Controllers::FS*>(c->context));
//  const size_t address = startAddress + (block * blockSize);
//  lfs.flashDriver.SectorErase(address);
//  return lfs.flashDriver.EraseFailed() ? -1 : 0;
//}
//
//int FS::SectorProg(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size) {
//  Pinetime::Controllers::FS& lfs = *(static_cast<Pinetime::Controllers::FS*>(c->context));
//  const size_t address = startAddress + (block * blockSize) + off;
//  lfs.flashDriver.Write(address, (uint8_t*) buffer, size);
//  return lfs.flashDriver.ProgramFailed() ? -1 : 0;
//}
//
//int FS::SectorRead(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size) {
//  Pinetime::Controllers::FS& lfs = *(static_cast<Pinetime::Controllers::FS*>(c->context));
//  const size_t address = startAddress + (block * blockSize) + off;
//  lfs.flashDriver.Read(address, static_cast<uint8_t*>(buffer), size);
//  return 0;
//}

/*

    ----------- LVGL filesystem integration -----------

*/

namespace {
  lv_fs_res_t lvglOpen(lv_fs_drv_t* drv, void* file_p, const char* path, lv_fs_mode_t mode) {

    lfs_file_t* file = static_cast<lfs_file_t*>(file_p);
    FS* filesys = static_cast<FS*>(drv->user_data);
    int ret = filesys->FileOpen(file, path, LFS_O_RDONLY);
    if (ret != LFS_ERR_OK) {
      return LV_FS_RES_FS_ERR;
    }
    return LV_FS_RES_OK;
  }

  lv_fs_res_t lvglClose(lv_fs_drv_t* drv, void* file_p) {
    FS* filesys = static_cast<FS*>(drv->user_data);
    lfs_file_t* file = static_cast<lfs_file_t*>(file_p);
    filesys->FileClose(file);

    return LV_FS_RES_OK;
  }

  lv_fs_res_t lvglRead(lv_fs_drv_t* drv, void* file_p, void* buf, uint32_t btr, uint32_t* br) {
    FS* filesys = static_cast<FS*>(drv->user_data);
    lfs_file_t* file = static_cast<lfs_file_t*>(file_p);
    filesys->FileRead(file, static_cast<uint8_t*>(buf), btr);
    *br = btr;
    return LV_FS_RES_OK;
  }
  lv_fs_res_t lvglSeek(lv_fs_drv_t* drv, void* file_p, uint32_t pos) {
    FS* filesys = static_cast<FS*>(drv->user_data);
    lfs_file_t* file = static_cast<lfs_file_t*>(file_p);
    filesys->FileSeek(file, pos);
    return LV_FS_RES_OK;
  }
}

void FS::LVGLFileSystemInit() {
  lv_fs_drv_init(&fs_drv);

  fs_drv.file_size = sizeof(lfs_file_t);
  fs_drv.letter = 'F';
  fs_drv.open_cb = lvglOpen;
  fs_drv.close_cb = lvglClose;
  fs_drv.read_cb = lvglRead;
  fs_drv.seek_cb = lvglSeek;

  fs_drv.user_data = this;

  lv_fs_drv_register(&fs_drv);
}
