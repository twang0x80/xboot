#ifndef __XFS_PLATFORM_H__
#define __XFS_PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xfs/xfs.h>

void * __xfs_platform_get_context(void);
void * __xfs_platform_create_lock(void);
void __xfs_platform_destroy_lock(void * lock);
void __xfs_platform_lock(void * lock);
void __xfs_platform_unlock(void * lock);
const char * __xfs_platform_directory_separator(void);
char * __xfs_platform_absolute_path(const char * path);
char * __xfs_platform_cvt_to_dependent(const char * prepend, const char * dirname, const char * append);

void __xfs_platform_sort(void * entries, size_t max, int (*cmp)(void *, size_t, size_t), void (*swap)(void *, size_t, size_t));
void * __xfs_platform_open_read(const char * filename);
void * __xfs_platform_open_write(const char * filename);
void * __xfs_platform_open_append(const char * filename);
s64_t __xfs_platform_read(void * handle, void * buf, u32_t size, u32_t count);
s64_t __xfs_platform_write(void * handle, const void * buf, u32_t size, u32_t count);
s64_t __xfs_platform_get_last_modtime(const char * filename);
s64_t __xfs_platform_tell(void * handle);
s64_t __xfs_platform_filelength(void * handle);
bool_t __xfs_platform_seek(void * handle, u64_t pos);
bool_t __xfs_platform_eof(void * handle);
bool_t __xfs_platform_flush(void * handle);
bool_t __xfs_platform_close(void * handle);
bool_t __xfs_platform_exists(const char * filename);
bool_t __xfs_platform_is_link(const char * filename);
bool_t __xfs_platform_is_directory(const char * filename);
bool_t __xfs_platform_mkdir(const char * path);
bool_t __xfs_platform_delete(const char * path);
void __xfs_platform_enumerate(const char * path, xfs_enumfiles_callback_t cb, const char * odir, void * cbdata);

#ifdef __cplusplus
}
#endif

#endif /* __XFS_PLATFORM_H__ */
