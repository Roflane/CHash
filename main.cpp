#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)

static std::vector<uint8_t> obf = { 0x93, 0x39, 0x73, 0x83, 0x12 };

int writeFile(const char *fileName, int position, unsigned char data) {
    FILE *file = fopen(fileName, "r+b");
    if (!file) return -1;

    if (fseek(file, position, SEEK_SET) != 0) return -1;
    if (fwrite(&data, sizeof(data), 1, file) != 1) return -1;
    fclose(file);
    return 0;
}

unsigned char RandomByte(unsigned char min, unsigned char max) {
    return min + (rand() % (max - min + 1));
}

int encBlock(const char *fileName) {
    srand(time(0));

    std::vector<uint8_t> rnd;
    rnd.resize(5);
    for (int i = 0; i < 5; i++) rnd[i] = RandomByte(0, 255);


    for (int pos = 20, i = 0; i < 5; i++, pos += 4) {
        if (writeFile(fileName, pos, obf[i] ^ rnd[i]) == 0) {
            LOG("Data wrote at pos %d with value %X\n", pos, obf[i] ^ rnd[i]);
        }
        else return -1;
    }
    return 0;
}

int main() {
    std::string fileName;
    LOG("Enter file name: ");
    std::cin >> fileName;

    if (encBlock(fileName.c_str()) == 0) {
        LOG("\nFile hash has been changed\n");
    }
    else LOG("Unexpected error\n");
    std::cin.ignore();
    std::cin.get();
}