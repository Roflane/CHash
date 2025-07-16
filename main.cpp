#include <iostream>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#define log(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define log_red(fmt, ...) printf("\033[31m" fmt "\033[0m", ##__VA_ARGS__)
#define log_green(fmt, ...) printf("\033[32m" fmt "\033[0m", ##__VA_ARGS__)
#define log_blue(fmt, ...) printf("\033[34m" fmt "\033[0m", ##__VA_ARGS__)

typedef unsigned char byte;
static std::vector<uint8_t> obf = { 0x93, 0x39, 0x73, 0x83, 0x12 };

typedef struct {
    std::string name;
    uint32_t pos;
} FileMetadata;

// https://github.com/Roflane/libbase
byte RandomByte(uint8_t min, uint8_t max)  {
    if (max < min) return 0;

    uint16_t num;
    __asm__ __volatile__(
        "rdrand %%bx\n\t"
        "mov %%bx, %[uint16]"
        : [uint16] "=r" (num)
        :
        : "bx"
    );
    return num % (max - min + 1) + min;
}

void dumpFileInfo(FileMetadata &fileMetadata) {
    FILE *file = fopen(fileMetadata.name.c_str(), "rb");
    if (!file) {
        log("File not found/couldn't open file\n");
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    log("File size: %ld\n", size);
    fileMetadata.pos = size;
}

byte writeFile(const char *fileName, uint32_t pos, byte data) {
    FILE *file = fopen(fileName, "r+b");
    if (!file) {
        log("File not found/couldn't open file\n");
        return 1;
    }
    if (fseek(file, pos, SEEK_SET) != 0) {
        log("Couldn't seek to pos %d\n", pos);
        return 1;
    }
    if (fwrite(&data, sizeof(data), 1, file) != 1) {
        log("Couldn't write data\n");
        return 1;
    }
    fclose(file);
    return 0;
}

int encBlock(const char *fileName, uint32_t pos) {
    std::vector<byte> rnd;
    rnd.resize(5);
    for (int i = 0; i < 5; i++) rnd[i] = RandomByte(0, 255);

    for (int i = 0; i < 5; i++, pos += 4) {
        byte factor = RandomByte(0, 255);
        auto enc = obf[i] ^ (rnd[i] | factor);
        if (writeFile(fileName, pos, enc) == 0) {
            log_green("Data wrote at pos %d with value %X\n", pos, factor);
        }
        else return -1;
    }
    return 0;
}

void printOptions() {
    log("0. Exit\n");
    log("1. Select file\n");
    log("2. Dump file info\n");
    log("3. Write file\n");
}

enum class EOption : uint32_t {
    EXIT = 0,
    SELECT = 1,
    DUMP = 2,
    WRITE = 3
};



[[noreturn]] void runCHash() {
    uint32_t option;
    auto selectOption = [&]() {
        log("Enter option: ");
        std::cin >> option;
        log("\n");
    };

    FileMetadata fileMetadata;
    auto selectFile = [&]() {
        log("Enter file name: ");
        std::cin >> fileMetadata.name;
    };

    while (true) {
        system("cls");

        printOptions();
        selectOption();

        switch (static_cast<EOption>(option)) {
            case EOption::SELECT:
                selectFile();
                break;
            case EOption::DUMP:
                dumpFileInfo(fileMetadata);
                break;
            case EOption::WRITE:
                uint32_t pos;
                log("Enter pos: ");
                std::cin >> pos;

                if (encBlock(fileMetadata.name.c_str(), pos) == 0) {
                    log("\nFile hash has been changed\n");
                }
                else log_red("Unexpected error\n");
                break;
            default: log("Invalid option\n");
        }

        log_blue("Press enter to continue\n");
        std::cin.ignore();
        std::cin.get();
    }
}

int main() { runCHash(); }