//
// Created by longduping on 2019/11/6.
//

#include <unistd.h>
#include <sys/mman.h>

/* mmap 内存映射
 *
 * void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offsize);
 *
 * @breif
 *          mmap()用来将某个文件内容映射到内存中，对该内存区域的存取即是直接对该文件内容的读写。
 * @param start  对应到内存起始地址（=NULL 则系统自动对应）
 * @param length 对应到内存的长度
 * @param prot
                PROT_EXEC  映射区域可被执行；
                PROT_READ  映射区域可被读取；
                PROT_WRITE 映射区域可被写入；
                PROT_NONE  映射区域不能存取。
 * @param flags
                MAP_FIXED       如果参数 start 所指的地址无法成功建立映射时，则放弃映射，不对地址做修正。通常不鼓励用此旗标。
                MAP_SHARED      对应射区域的写入数据会复制回文件内，而且允许其他映射该文件的进程共享。
                MAP_PRIVATE     对应射区域的写入操作会产生一个映射文件的复制，即私人的"写入时复制" (copy on write)对此区域作的任何修改都不会写回原来的文件内容。
                MAP_ANONYMOUS   建立匿名映射，此时会忽略参数fd，不涉及文件，而且映射区域无法和其他进程共享。
                MAP_DENYWRITE   只允许对应射区域的写入操作，其他对文件直接写入的操作将会被拒绝。
                MAP_LOCKED      将映射区域锁定住，这表示该区域不会被置换(swap)。
                Ps: 在调用mmap()时必须要指定MAP_SHARED 或MAP_PRIVATE。
 * @param fd 映射的文件描述符
 * @param offsize 文件映射的偏移量，通常设置为0，代表从文件最前方开始对应，offset必须是分页大小的整数倍。
 *
 * @return 起始地址指针， 失败返回 MAP_FAILED, 具体错误信息存于 errno
 *              部分错误信息：
                EBADF  参数fd 不是有效的文件描述词。
                EACCES  存取权限有误。如果是MAP_PRIVATE 情况下文件必须可读，使用MAP_SHARED 则要有PROT_WRITE 以及该文件要能写入。
                EINVAL  参数start、length 或offset 有一个不合法。
                EAGAIN  文件被锁住，或是有太多内存被锁住。
                ENOMEM  内存不足。
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main()
{
    int fd;
    void *start;
    struct stat sb;
    /*打开/etc/passwd */
    fd = open("/etc/passwd", O_RDONLY);
    /* 取得文件信息 */
    fstat(fd, &sb);
    /* 映射 */
    start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    /* 判断是否映射成功 */
    if (start == MAP_FAILED) {
        printf("[ERROR] %s\n", strerror(errno));
        return -1;
    }
    /* 打印该映射区域的内容 */
    printf("%s", start);

    /* 解除映射 */
    munmap(start, sb.st_size);

    /* 关闭文件 */
    close(fd);
    return 0;
}