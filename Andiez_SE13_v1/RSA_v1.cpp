#include <windows.h>
#include <wincrypt.h>

#pragma comment(lib, "advapi32.lib")

// Hàm mã hóa dữ liệu trong buffer sử dụng RSA
BOOL RSAEncryptBuffer(HCRYPTKEY hPublicKey, const BYTE* pBuffer, DWORD dwBufferSize, BYTE*& pEncryptedData, DWORD& dwEncryptedDataLen) {
    BOOL bResult = FALSE;

    // Lấy kích thước buffer cần thiết cho dữ liệu đã mã hóa
    DWORD dwBufferLen = 0;
    bResult = CryptEncrypt(hPublicKey, 0, TRUE, 0, NULL, &dwBufferLen, 0);
    if (!bResult) {
        return FALSE;
    }

    // Mã hóa dữ liệu
    pEncryptedData = new BYTE[dwBufferLen];
    ZeroMemory(pEncryptedData, dwBufferLen);

    memcpy(pEncryptedData, pBuffer, dwBufferSize);

    bResult = CryptEncrypt(hPublicKey, 0, TRUE, 0, pEncryptedData, &dwBufferSize, dwBufferLen);
    if (!bResult) {
        delete[] pEncryptedData;
        return FALSE;
    }

    dwEncryptedDataLen = dwBufferSize;
    return TRUE;
}

// Hàm giải mã dữ liệu trong buffer sử dụng RSA
BOOL RSADecryptBuffer(HCRYPTKEY hPrivateKey, const BYTE* pEncryptedData, DWORD dwEncryptedDataLen, BYTE*& pDecryptedData, DWORD& dwDecryptedDataLen) {
    BOOL bResult = FALSE;

    // Lấy kích thước buffer cần thiết cho dữ liệu giải mã
    DWORD dwBufferLen = 0;
    bResult = CryptDecrypt(hPrivateKey, 0, TRUE, 0, NULL, &dwBufferLen);
    if (!bResult) {
        return FALSE;
    }

    // Giải mã dữ liệu
    pDecryptedData = new BYTE[dwBufferLen];
    ZeroMemory(pDecryptedData, dwBufferLen);

    memcpy(pDecryptedData, pEncryptedData, dwEncryptedDataLen);

    bResult = CryptDecrypt(hPrivateKey, 0, TRUE, 0, pDecryptedData, &dwEncryptedDataLen);
    if (!bResult) {
        delete[] pDecryptedData;
        return FALSE;
    }

    dwDecryptedDataLen = dwEncryptedDataLen;
    return TRUE;
}

int main() {
    // Khởi tạo ngữ cảnh CSP (Cryptographic Service Provider)
    HCRYPTPROV hCryptProv = 0;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return 1;
    }

    HCRYPTKEY hPublicKey = 0;
    HCRYPTKEY hPrivateKey = 0;

    // Tạo cặp khóa RSA
    if (!CreateRSAKeyPair(hCryptProv, hPublicKey, hPrivateKey)) {
        CryptReleaseContext(hCryptProv, 0);
        return 1;
    }

    // Dữ liệu cần mã hóa
    const BYTE originalData[] = "Hello, RSA!";
    DWORD dwDataLen = sizeof(originalData);

    // Mã hóa dữ liệu trong buffer
    BYTE* pEncryptedData = nullptr;
    DWORD dwEncryptedDataLen = 0;
    if (!RSAEncryptBuffer(hPublicKey, originalData, dwDataLen, pEncryptedData, dwEncryptedDataLen)) {
        CryptDestroyKey(hPublicKey);
        CryptDestroyKey(hPrivateKey);
        CryptReleaseContext(hCryptProv, 0);
        return 1;
    }

    // In kết quả mã hóa
    wprintf(L"Encrypted Data: ");
    for (DWORD i = 0; i < dwEncryptedDataLen; ++i) {
        wprintf(L"%02X ", pEncryptedData[i]);
    }
    wprintf(L"\n");

    // Giải phóng bộ nhớ và tài nguyên
    CryptDestroyKey(hPublicKey);
    CryptDestroyKey(hPrivateKey);
    CryptReleaseContext(hCryptProv, 0);
    delete[] pEncryptedData;

    return 0;
}

int main() {
    // Khởi tạo ngữ cảnh CSP (Cryptographic Service Provider)
    HCRYPTPROV hCryptProv = 0;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return 1;
    }

    HCRYPTKEY hPublicKey = 0;
    HCRYPTKEY hPrivateKey = 0;

    // Tạo cặp khóa RSA
    if (!CreateRSAKeyPair(hCryptProv, hPublicKey, hPrivateKey)) {
        CryptReleaseContext(hCryptProv, 0);
        return 1;
    }

    // Dữ liệu cần mã hóa
    const BYTE originalData[] = "Hello, RSA!";
    DWORD dwDataLen = sizeof(originalData);

    // Mã hóa dữ liệu trong buffer
    BYTE* pEncryptedData = nullptr;
    DWORD dwEncryptedDataLen = 0;
    if (!RSAEncryptBuffer(hPublicKey, originalData, dwDataLen, pEncryptedData, dwEncryptedDataLen)) {
        CryptDestroyKey(hPublicKey);
        CryptDestroyKey(hPrivateKey);
        CryptReleaseContext(hCryptProv, 0);
        return 1;
    }

    // Giải mã dữ liệu trong buffer
    BYTE* pDecryptedData = nullptr;
    DWORD dwDecryptedDataLen = 0;
    if (!RSADecryptBuffer(hPrivateKey, pEncryptedData, dwEncryptedDataLen, pDecryptedData, dwDecryptedDataLen)) {
        CryptDestroyKey(hPublicKey);
        CryptDestroyKey(hPrivateKey);
        CryptReleaseContext(hCryptProv, 0);
        delete[] pEncryptedData;
        return 1;
    }

    // In kết quả giải mã
    wprintf(L"Decrypted Data: %.*s\n", dwDecryptedDataLen, pDecryptedData);

    // Giải phóng bộ nhớ và tài nguyên
    CryptDestroyKey(hPublicKey);
    CryptDestroyKey(hPrivateKey);
    CryptReleaseContext(hCryptProv, 0);
    delete[] pEncryptedData;
    delete[] pDecryptedData;

    return 0;
}
