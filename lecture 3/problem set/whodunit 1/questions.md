# Questions

## What's `stdint.h`?

header file that provides typedefs for specific width integers, e.g. `uint8_t`

## What's the point of using `uint8_t`, `uint32_t`, `int32_t`, and `uint16_t` in a program?

Defining a specific bit length helps to restrict range and save memory. Using unsigned allows you to optimize the bit length for positive values and hold a larger range of positive values with the same bit size.

## How many bytes is a `BYTE`, a `DWORD`, a `LONG`, and a `WORD`, respectively?

`BYTE` = 1 byte
`DWORD` = 4 bytes
`LONG` = 4 bytes
`WORD` = 2 bytes

## What (in ASCII, decimal, or hexadecimal) must the first two bytes of any BMP file be? Leading bytes used to identify file formats (with high probability) are generally called "magic numbers."

ASCII - BM
DECIMAL - 66 77
HEXADECIMAL - 42 4D

## What's the difference between `bfSize` and `biSize`?

`bfSize` - byte size of the entire file, including headers
`biSize` - byte size of the image with padding, no headers

## What does it mean if `biHeight` is negative?

bitmap is organized top-down

## What field in `BITMAPINFOHEADER` specifies the BMP's color depth (i.e., bits per pixel)?

`biBitCount`

## Why might `fopen` return `NULL` in `copy.c`?

if the file is not found or there is an error opening it

## Why is the third argument to `fread` always `1` in our code?

because it is reading one character at a top to copy one character at a time

## What value does `copy.c` assign to `padding` if `bi.biWidth` is `3`?

padding is set to 3

sizeof(RGBTRIPLE) = 24 bits = 3 bytes
9 % 4 = 1
4 - 1 = 3
3 % 4 = 4

## What does `fseek` do?

it moves to the specificed byte in the file stream

## What is `SEEK_CUR`?

the current position of the file pointer
