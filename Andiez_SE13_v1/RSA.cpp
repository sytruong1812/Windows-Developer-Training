#include <windows.h>
#include <wincrypt.h>

#pragma comment(lib, "advapi32.lib")

// Hàm tạo cặp khóa RSA
BOOL CreateRSAKeyPair(HCRYPTPROV hCryptProv, HCRYPTKEY& hPublicKey, HCRYPTKEY& hPrivateKey) {
    BOOL bResult = FALSE;

    // Tạo cặp khóa RSA
    bResult = CryptGenKey(hCryptProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hPublicKey);
    if (!bResult) {
        return FALSE;
    }

    // Tạo khóa bí mật tương ứng với khóa công khai
    bResult = CryptGetUserKey(hCryptProv, AT_KEYEXCHANGE, &hPrivateKey);
    if (!bResult) {
        CryptDestroyKey(hPublicKey);
        return FALSE;
    }

    return TRUE;
}

// Hàm mã hóa thông điệp RSA
BOOL RSAEncrypt(HCRYPTKEY hPublicKey, const BYTE* pData, DWORD dwDataLen, BYTE*& pEncryptedData, DWORD& dwEncryptedDataLen) {
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

    bResult = CryptEncrypt(hPublicKey, 0, TRUE, 0, pEncryptedData, &dwDataLen, dwBufferLen);
    if (!bResult) {
        delete[] pEncryptedData;
        return FALSE;
    }

    dwEncryptedDataLen = dwBufferLen;
    return TRUE;
}

// Hàm giải mã thông điệp RSA
BOOL RSADecrypt(HCRYPTKEY hPrivateKey, const BYTE* pEncryptedData, DWORD dwEncryptedDataLen, BYTE*& pDecryptedData, DWORD& dwDecryptedDataLen) {
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

    bResult = CryptDecrypt(hPrivateKey, 0, TRUE, 0, pDecryptedData, &dwDecryptedDataLen);
    if (!bResult) {
        delete[] pDecryptedData;
        return FALSE;
    }

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

    // Mã hóa dữ liệu
    BYTE* pEncryptedData = nullptr;
    DWORD dwEncryptedDataLen = 0;
    if (!RSAEncrypt(hPublicKey, originalData, dwDataLen, pEncryptedData, dwEncryptedDataLen)) {
        CryptDestroyKey(hPublicKey);
        CryptDestroyKey(hPrivateKey);
        CryptReleaseContext(hCryptProv, 0);
        return 1;
    }

    // Giải mã dữ liệu
    BYTE* pDecryptedData = nullptr;
    DWORD dwDecryptedDataLen = 0;
    if (!RSADecrypt(hPrivateKey, pEncryptedData, dwEncryptedDataLen, pDecryptedData, dwDecryptedDataLen)) {
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
