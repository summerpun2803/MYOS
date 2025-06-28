#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef uint8_t bool;
#define true 1
#define false 0

typedef struct 
{
    uint8_t BootJumpInstruction[3];
    uint8_t OemIdentifier[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FatCount;   
    uint16_t DirEntryCount;
    uint16_t TotalSectors;
    uint8_t MediaDescriptorType;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t LargeSectorCount;

    // extended boot record
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;          // serial number, value doesn't matter
    uint8_t VolumeLabel[11];    // 11 bytes, padded with spaces
    uint8_t SystemId[8];

    // ... we don't care about code ...

} __attribute__((packed)) BootSector;

typedef struct 
{
    uint8_t Name[11];
    uint8_t Attributes;
    uint8_t _Reserved;
    uint8_t CreatedTimeTenths;
    uint16_t CreatedTime;
    uint16_t CreatedDate;
    uint16_t AccessedDate;
    uint16_t FirstClusterHigh;
    uint16_t ModifiedTime;
    uint16_t ModifiedDate;
    uint16_t FirstClusterLow;
    uint32_t Size;
} __attribute__((packed)) DirectoryEntry;


BootSector g_BootSector;
uint8_t* g_Fat = NULL;
DirectoryEntry *g_RootDirectory;
uint32_t g_RootDirectoryEnd;


bool readBootSector(FILE *disk){
    return fread(&g_BootSector, sizeof(g_BootSector), 1, disk) > 0;
}

bool readSector(FILE *disk, uint16_t count, uint16_t lba, void *bufferOut){

    bool ok = true;
    ok = ok && (fseek(disk, lba*g_BootSector.BytesPerSector, SEEK_SET)==0);
    ok = ok && (fread(bufferOut, g_BootSector.BytesPerSector, count, disk) == count);

    return ok;
}

bool readFat(FILE *disk){
    g_Fat = (uint8_t *) malloc(g_BootSector.SectorsPerFat * g_BootSector.BytesPerSector);
    return readSector(disk, g_BootSector.SectorsPerFat, g_BootSector.ReservedSectors ,g_Fat);
}

bool readRootDirectory(FILE* disk){
    uint16_t lba = g_BootSector.ReservedSectors + g_BootSector.FatCount * g_BootSector.SectorsPerFat;
    uint32_t size = sizeof(DirectoryEntry) * g_BootSector.DirEntryCount;
    uint32_t sectors = (size / g_BootSector.BytesPerSector);
    printf("size class: %lu , total size: %d" , sizeof(DirectoryEntry), size);
    if (size % g_BootSector.BytesPerSector > 0)
        printf("plus plus");
        sectors++;
    g_RootDirectoryEnd = lba + sectors;
    g_RootDirectory =  (DirectoryEntry *) malloc(sectors * g_BootSector.BytesPerSector);
    printf("%d",sectors * g_BootSector.BytesPerSector);
    return readSector(disk, sectors, lba, g_RootDirectory);

}

DirectoryEntry* findFile(char* fileName){
    for(uint16_t i=0; i<g_BootSector.DirEntryCount; i++){
        printf("%s \n",g_RootDirectory[i].Name);
        if(memcmp(fileName, g_RootDirectory[i].Name, 4) == 0) return &g_RootDirectory[i];
    }

    return NULL;
}

uint16_t next_cluster(uint32_t clusterChain){
    uint32_t index = (clusterChain * 3)/2;

    uint16_t table_value =  (*(uint16_t *)(g_Fat + index));
    table_value = (clusterChain & 1) ? table_value >> 4 : table_value & 0xfff;
    return table_value;
}

bool readFile(DirectoryEntry* fileEntry, FILE* disk, uint8_t* buffer){

    bool ok = true;
    uint16_t currentCluster = fileEntry->FirstClusterLow;

    while(ok && currentCluster < 0x0FF8){
        uint32_t lba = g_RootDirectoryEnd + (currentCluster - 2) * g_BootSector.SectorsPerCluster;
        ok = ok && readSector(disk, g_BootSector.SectorsPerCluster, lba, buffer);
        buffer += g_BootSector.SectorsPerCluster * g_BootSector.BytesPerSector;
        currentCluster = next_cluster(currentCluster);
    }

    return ok;
}


int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Syntax: %s <disk image> <file name>\n", argv[0]);
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb");
    if (!disk) {
        fprintf(stderr, "Cannot open disk image %s!\n", argv[1]);
        return -1;
    }

    if (!readBootSector(disk)) {
        fprintf(stderr, "Could not read boot sector!\n");
        return -2;
    }

    if (!readFat(disk)) {
        fprintf(stderr, "Could not read FAT!\n");
        free(g_Fat);
        return -3;
    }
    

    if (!readRootDirectory(disk)) {
        fprintf(stderr, "Could not read FAT!\n");
        free(g_Fat);
        free(g_RootDirectory);
        return -4;
    }

    DirectoryEntry* fileEntry = findFile(argv[2]);
    if (!fileEntry) {
        fprintf(stderr, "Could not find file %s!\n", argv[2]);
        free(g_Fat);
        free(g_RootDirectory);
        return -5;
    }

    uint8_t* buffer = (uint8_t*) malloc(fileEntry->Size + g_BootSector.BytesPerSector);
    if (!readFile(fileEntry, disk, buffer)) {
        fprintf(stderr, "Could not read file %s!\n", argv[2]);
        free(g_Fat);
        free(g_RootDirectory);
        free(buffer);
        return -5;
    }

    for (size_t i = 0; i < fileEntry->Size; i++)
    {
        if (isprint(buffer[i])) fputc(buffer[i], stdout);
        else printf("<%02x>", buffer[i]);
    }

    free(buffer);
    free(g_Fat);
    free(g_RootDirectory);
    return 0;
}