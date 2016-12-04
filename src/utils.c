#include <string.h>
#include "utils.h"

/**
 * reverse_endian() - reverse the endianess of a buffer
 * @buffer: pointer to the buffer
 * @len: length of the buffer
 *
 * Turns little endian buffer into a big endian buffer.
 *
 * Return:
 */
void reverse_endianess(BYTE *buffer, int len) {
	BYTE b;
	int i, j;

	for (i = 0, j = len - 1; i < j; i++, j--)
	{
		b = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = b;
	}
}

/**
 * bytes_to_word() - return word represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the word represented by a little endian buffer of two bytes.
 *
 * Return: a WORD
 */
WORD bytes_to_word(BYTE *buffer) {
	BYTE tmp[] = {0, 0};
	memcpy(tmp, buffer, 2);
	reverse_endianess(tmp, 2);

	return (WORD) ((tmp[0]<<8) + tmp[1]);
}

/**
 * bytes_to_dword() - return dword represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the dword represented by a little endian buffer of two bytes.
 *
 * Return: a DWORD
 */
DWORD bytes_to_dword(BYTE *buffer) {
	BYTE tmp[] = {0, 0, 0, 0};
	memcpy(tmp, buffer, 4);
	reverse_endianess(tmp, 4);

	return (DWORD) ((tmp[0]<<24) + (tmp[1]<<16) + (tmp[2]<<8) + tmp[0]);
}

/**
 * bytes_to_int() - return int represented by bytes array
 * @buffer: pointer to the buffer containing the two bytes
 *
 * Return the int represented by a little endian buffer of two bytes.
 *
 * Return: an integer
 */
int bytes_to_int(BYTE *buffer) {
	BYTE tmp[] = {0, 0, 0, 0};
	memcpy(tmp, buffer, 4);
	reverse_endianess(tmp, 4);

	return (int) ((tmp[0]<<24) + (tmp[1]<<16) + (tmp[2]<<8) + tmp[0]);
}
