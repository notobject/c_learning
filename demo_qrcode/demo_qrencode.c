/*
 * @Author: Long Duping 
 * @Date: 2019-12-06 14:10:57 
 * @Last Modified by: Long Duping
 * @Last Modified time: 2019-12-06 15:41:36
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <qrencode.h>
#include <errno.h>
#include <png.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

static int casesensitive = 1;
static int eightbit = 0;
static int version = 0;
static int margin = 2;
static int micro = 0;
static QRecLevel level = QR_ECLEVEL_L;
static QRencodeMode hint = QR_MODE_8;

static void writeUTF8_margin(FILE *fp, int realwidth, const char *white,
                             const char *reset, const char *full)
{
    int x, y;

    for (y = 0; y < margin / 2; y++)
    {
        fputs(white, fp);
        for (x = 0; x < realwidth; x++)
            fputs(full, fp);
        fputs(reset, fp);
        fputc('\n', fp);
    }
}

static FILE *openFile(const char *outfile)
{
    FILE *fp;

    if (outfile == NULL || (outfile[0] == '-' && outfile[1] == '\0'))
    {
        fp = stdout;
    }
    else
    {
        fp = fopen(outfile, "wb");
        if (fp == NULL)
        {
            fprintf(stderr, "Failed to create file: %s\n", outfile);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
    }
    return fp;
}

static int writeUTF8(const QRcode *qrcode, FILE *fp, int use_ansi, int invert)
{
    int x, y;
    int realwidth;
    const char *white, *reset;
    const char *empty, *lowhalf, *uphalf, *full;

    empty = " ";
    lowhalf = "\342\226\204";
    uphalf = "\342\226\200";
    full = "\342\226\210";

    if (invert)
    {
        const char *tmp;

        tmp = empty;
        empty = full;
        full = tmp;

        tmp = lowhalf;
        lowhalf = uphalf;
        uphalf = tmp;
    }

    if (use_ansi)
    {
        if (use_ansi == 2)
        {
            white = "\033[38;5;231m\033[48;5;16m";
        }
        else
        {
            white = "\033[40;37;1m";
        }
        reset = "\033[0m";
    }
    else
    {
        white = "";
        reset = "";
    }

    realwidth = (qrcode->width + margin * 2);

    /* top margin */
    writeUTF8_margin(fp, realwidth, white, reset, full);

    /* data */
    for (y = 0; y < qrcode->width; y += 2)
    {
        unsigned char *row1, *row2;
        row1 = qrcode->data + y * qrcode->width;
        row2 = row1 + qrcode->width;

        fputs(white, fp);

        for (x = 0; x < margin; x++)
        {
            fputs(full, fp);
        }

        for (x = 0; x < qrcode->width; x++)
        {
            if (row1[x] & 1)
            {
                if (y < qrcode->width - 1 && row2[x] & 1)
                {
                    fputs(empty, fp);
                }
                else
                {
                    fputs(lowhalf, fp);
                }
            }
            else if (y < qrcode->width - 1 && row2[x] & 1)
            {
                fputs(uphalf, fp);
            }
            else
            {
                fputs(full, fp);
            }
        }

        for (x = 0; x < margin; x++)
            fputs(full, fp);

        fputs(reset, fp);
        fputc('\n', fp);
    }

    /* bottom margin */
    writeUTF8_margin(fp, realwidth, white, reset, full);

    fclose(fp);

    return 0;
}

static QRcode *encode(const unsigned char *intext, int length)
{
    QRcode *code;

    if (micro)
    {
        if (eightbit)
        {
            code = QRcode_encodeDataMQR(length, intext, version, level);
        }
        else
        {
            code = QRcode_encodeStringMQR((char *)intext, version, level, hint, casesensitive);
        }
    }
    else if (eightbit)
    {
        code = QRcode_encodeData(length, intext, version, level);
    }
    else
    {
        code = QRcode_encodeString((char *)intext, version, level, hint, casesensitive);
    }

    return code;
}

int printQRcode(FILE *fp, unsigned char *intext, int length)
{
    QRcode *qrcode = NULL;
    qrcode = encode(intext, length);
    if (qrcode == NULL)
    {
        if (errno == ERANGE)
        {
            fprintf(stderr, "Failed to encode the input data: Input data too large\n");
        }
        else
        {
            perror("Failed to encode the input data");
        }
        return -1;
    }
    return writeUTF8(qrcode, fp, 0, 0);
}

int main(int argc, char const *argv[])
{
    /* Demo */
    unsigned char *intext = "http://www.baidu.com";
    FILE *fp = openFile("/tmp/qrcode.tmp");
    printQRcode(fp, intext, strlen(intext));

    int fd = open("/tmp/qrcode.tmp", O_RDONLY);
    struct stat sb;
    fstat(fd, &sb);
    void *start = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    char message[10240];
    snprintf(message, 10240, "%s", (unsigned char *)start);
    munmap(start, sb.st_size);
    close(fd);
    printf("%s", message);
    return 0;
}
