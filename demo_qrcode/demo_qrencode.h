/*
 * @Author: Long Duping 
 * @Date: 2019-12-06 14:50:05 
 * @Last Modified by: Long Duping
 * @Last Modified time: 2019-12-06 14:56:08
 */

#ifndef __DEMO_QRENCODE_H
#define __DEMO_QRENCODE_H

/** print this ↓
 *  █████████████████████████████
    ██ ▄▄▄▄▄ ██▀▄███▄ ██ ▄▄▄▄▄ ██
    ██ █   █ █▄▀█▄▀█▀▄██ █   █ ██
    ██ █▄▄▄█ ██▄▀▀ ▄   █ █▄▄▄█ ██
    ██▄▄▄▄▄▄▄█ █▄▀▄▀ ▀ █▄▄▄▄▄▄▄██
    ██▄ ▄▀▄▄▄▄ ▀█▀ ▄██▀▄ ███▄█▀██
    ██▄▀ ██ ▄██▀ ▄ ▀▀▀ ▄ ▄██▄ ▄██
    ██▄  ▀ ▄▄ ▀█  ██▄█ ▀ ▄█ █▄ ██
    ██▄▀█   ▄▄ ▀█▀█▄█▀█ ▀▄█▀▄ ▄██
    ██▄██▄▄█▄▄▀ █▀ ▀█  ▄▄▄  ▀▀▀██
    ██ ▄▄▄▄▄ █ ▄▀▄ ▀ ▄ █▄█ ▀█ ▄██
    ██ █   █ █▄ █ ██ █▄▄▄▄ █▀▄▄██
    ██ █▄▄▄█ █ ▀█▀█▄█▀ ▀█▄  █▀▄██
    ██▄▄▄▄▄▄▄█▄███▄██▄▄██▄███▄▄██
    █████████████████████████████
 * 
 * @filename: NULL or "-" -> stdout
 * @intext: text or link 
 * @length: length of intext
 */
int printQRcode(char *filename, unsigned char *intext, int length);

#endif