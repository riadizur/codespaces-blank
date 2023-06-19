#include <iostream>
#include <windows.h>
#include <winscard.h>

int main() {
    SCARDCONTEXT context;
    SCARDHANDLE card;
    DWORD activeProtocol;
    BYTE atr[32];
    DWORD atrLen = sizeof(atr);

    // Inisialisasi context
    if (SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &context) != SCARD_S_SUCCESS) {
        std::cout << "Gagal menginisialisasi context" << std::endl;
        return 1;
    }

    // Cari pembaca yang tersedia
    char readerName[MAX_READERNAME];
    DWORD readerLen = sizeof(readerName);
    DWORD ret = SCardListReaders(context, NULL, readerName, &readerLen);
    if (ret != SCARD_S_SUCCESS) {
        std::cout << "Gagal mencari pembaca: " << ret << std::endl;
        SCardReleaseContext(context);
        return 1;
    }

    // Hubungkan ke pembaca pertama yang ditemukan
    ret = SCardConnect(context, readerName, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &card, &activeProtocol);
    if (ret != SCARD_S_SUCCESS) {
        std::cout << "Gagal terhubung ke SIM card: " << ret << std::endl;
        SCardReleaseContext(context);
        return 1;
    }

    // Dapatkan ATR (Answer To Reset) dari SIM card
    ret = SCardGetAttrib(card, SCARD_ATTR_ATR_STRING, atr, &atrLen);
    if (ret != SCARD_S_SUCCESS) {
        std::cout << "Gagal mendapatkan ATR dari SIM card: " << ret << std::endl;
        SCardDisconnect(card, SCARD_LEAVE_CARD);
        SCardReleaseContext(context);
        return 1;
    }

    std::cout << "Berhasil terhubung ke SIM card!" << std::endl;
    std::cout << "ATR: ";
    for (DWORD i = 0; i < atrLen; ++i) {
        printf("%02X ", atr[i]);
    }
    std::cout << std::endl;

    // Mengirim APDU ke SIM card (contoh: SELECT FILE)
    BYTE apduSelectFile[] = { 0x00, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00 };
    BYTE response[256];
    DWORD responseLen = sizeof(response);

    ret = SCardTransmit(card, SCARD_PCI_T1, apduSelectFile, sizeof(apduSelectFile), NULL, response, &responseLen);
    if (ret != SCARD_S_SUCCESS) {
        std::cout << "Gagal mengirim APDU ke SIM card: " << ret << std::endl;
    } else {
        std::cout << "Berhasil mengirim APDU!" << std::endl;
        std::cout << "Response: ";
        for (DWORD i = 0; i < responseLen; ++i) {
            printf("%02X ", response[i]);
        }
        std::cout << std::endl;
    }

    // Putuskan koneksi dan lepaskan sumber daya
    SCardDisconnect(card, SCARD_LEAVE_CARD);
    SCardReleaseContext(context);

    return 0;
}