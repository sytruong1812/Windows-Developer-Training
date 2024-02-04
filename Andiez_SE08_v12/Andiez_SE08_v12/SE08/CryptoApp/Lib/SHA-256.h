#pragma once
#include <windows.h>
#include <iostream>
#include <cstring>

typedef struct
{
	BYTE data[64];
	size_t datalen;
	size_t bitlen[2];
	size_t state[8];
} SHA256_CTX;

#define SHA256_DIGEST_LENGTH 32
#define DBL_INT_ADD(a, b, c) if (a > 0xffffffff - (c)) ++b; a += c;
#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (SHA256_DIGEST_LENGTH - (b))))
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (SHA256_DIGEST_LENGTH - (b))))

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x, 22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x, 25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

int SHA256One(BYTE* data, size_t dataLen, BYTE* digest);
int SHA256Print(BYTE* digest);

void SHA256Init(SHA256_CTX * ctx);
void SHA256Update(SHA256_CTX * ctx, BYTE* data, size_t len);
void SHA256Final(SHA256_CTX * ctx, BYTE digest[32]);