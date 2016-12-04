#ifndef UTILS_H
#define UTILS_H

#include "t2fs.h"

/**
 * umax() - return maximum between two numbers
 * @a: a number
 * @b: a number
 *
 * Return: the maximum of a and b. They must be unsigned integers.
 */
unsigned int umax(unsigned int a, unsigned int b);

/**
 * umin() - return minimum between two numbers
 * @a: a number
 * @b: a number
 *
 * Return: the minimum of a and b. They must be unsigned integers.
 */
unsigned int umin(unsigned int a, unsigned int b);

/**
 * alloc_buffer() - allocate buffer
 * @size: size of the buffer in sectors
 *
 * Return a pointer to a buffer of size `size`*SECTOR_SIZE.
 *
 * Return: the pointer to the allocated area.
 */
BYTE *alloc_buffer(int size);

/**
 * reverse_endian() - reverse the endianess of a buffer
 * @buffer: pointer to the buffer
 * @len: length of the buffer
 *
 * Turns little endian buffer into a big endian buffer.
 *
 * Return:
 */
void reverse_endianess(BYTE *buffer, int len);

/**
 * bytes_to_word() - return word represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the word represented by a little endian buffer of two bytes.
 *
 * Return: a WORD
 */
WORD bytes_to_word(BYTE *buffer);

/**
 * bytes_to_dword() - return dword represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the dword represented by a little endian buffer of two bytes.
 *
 * Return: a DWORD
 */
DWORD bytes_to_dword(BYTE *buffer);

/**
 * bytes_to_int() - return int represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the int represented by a little endian buffer of two bytes.
 *
 * Return: an integer
 */
int bytes_to_int(BYTE *buffer);

#endif /* ifndef UTILS_H */
