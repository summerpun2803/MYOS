#include "fat.h"
#include "stdio.h"
#include "utils.h"
#include "string.h"
#include"memory.h"

typedef struct
{
    union
    {
        BootSector BootSector;
        uint8_t BootSectorBytes[SECTOR_SIZE];
    } BS;
    
    FAT_FileData RootDirectory;
    
    FAT_FileData OpenedFiles[10];

} FAT_Data;


FAT_Data far* g_Data;
uint8_t far* g_Fat = NULL;
uint8_t far* g_Root = NULL;
uint32_t g_RootDirectoryEnd;


bool readBootSector(DISK *disk)
{   
    return Disk_Read_Sector(disk, 0, 1, g_Data->BS.BootSectorBytes);
}

bool readFat(DISK *disk)
{
    return Disk_Read_Sector(disk, g_Data->BS.BootSector.ReservedSectors, g_Data->BS.BootSector.SectorsPerFat, g_Fat);
}

bool readRootDirectory(DISK* disk)
{
    uint32_t lba = g_Data->BS.BootSector.ReservedSectors + g_Data->BS.BootSector.FatCount * g_Data->BS.BootSector.SectorsPerFat;
    uint32_t size = g_Data->BS.BootSector.DirEntryCount * sizeof(DirectoryEntry);
    uint32_t sectors = (size + g_Data->BS.BootSector.BytesPerSector - 1) / g_Data->BS.BootSector.BytesPerSector;
        
    return Disk_Read_Sector(disk, lba, sectors , g_Root);
}

void FAT_printAll()
{
    DirectoryEntry *RootEntry = (DirectoryEntry *) g_Root;
    for(uint16_t i=0; i<g_Data->BS.BootSector.DirEntryCount; i++){
        printf("%s", RootEntry[i].Name);
    }
    printf("\r\n");
}

bool FAT_Initialize(DISK *disk)
{
    g_Data = (FAT_Data far*)MEMORY_FAT_ADDR;


    if (!readBootSector(disk)) {
        puts("FAT INIT: Boot sector read failed\r\n");
        return false;
    }

    g_Fat = (uint8_t far*)g_Data + sizeof(FAT_Data);
    uint32_t g_FatSize = g_Data->BS.BootSector.SectorsPerFat * g_Data->BS.BootSector.BytesPerSector;

    if(!readFat(disk)) {
        printf("FAT INIT: FAT Read Error \r\n");
        return false;
    }

    g_Root = (uint8_t *)g_Fat + g_FatSize;
    uint32_t g_RootSize = g_Data->BS.BootSector.DirEntryCount * sizeof(DirectoryEntry);
    g_RootSize = align(g_RootSize, g_Data->BS.BootSector.BytesPerSector);

    uint32_t rootDirLba = g_Data->BS.BootSector.ReservedSectors + g_Data->BS.BootSector.SectorsPerFat * g_Data->BS.BootSector.FatCount;
    uint32_t rootDirSize = sizeof(DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;

    g_Data->RootDirectory.Public.Handle = ROOT_DIRECTORY_HANDLE;
    g_Data->RootDirectory.Public.IsDirectory = true;
    g_Data->RootDirectory.Public.Position = 0;
    g_Data->RootDirectory.Public.Size = sizeof(DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;
    g_Data->RootDirectory.Opened = true;
    g_Data->RootDirectory.FirstCluster = rootDirLba;
    g_Data->RootDirectory.CurrentCluster = rootDirLba;
    g_Data->RootDirectory.CurrentSectorInCluster = 0;

    if (!Disk_Read_Sector(disk, rootDirLba, 1, g_Data->RootDirectory.Buffer))
    {
        printf("FAT: read root directory failed\r\n");
        return false;
    }
    uint32_t sectors = (rootDirSize + g_Data->BS.BootSector.BytesPerSector - 1) / g_Data->BS.BootSector.BytesPerSector;
    g_RootDirectoryEnd = rootDirLba + sectors;
    
    for(int i=0; i<10; i++){
        g_Data->OpenedFiles[i].Opened = false;
    }

    return true;
}

uint32_t FAT_ClusterToLba(uint16_t currentCluster)
{
    uint32_t lba = g_RootDirectoryEnd + (currentCluster - 2) * g_Data->BS.BootSector.SectorsPerCluster;
    return lba;
}

uint16_t NextCluster(uint32_t clusterChain){
    uint32_t index = (clusterChain * 3)/2;

    uint16_t table_value =  (*(uint16_t far*)(g_Fat + index));
    table_value = (clusterChain & 1) ? table_value >> 4 : table_value & 0xfff;
    return table_value;
}

bool ReadFileEntry(DISK *disk, FAT_File far* file, DirectoryEntry *entry)
{
    return FAT_Read(disk, file, sizeof(DirectoryEntry), entry) == sizeof(DirectoryEntry);
}

bool FAT_FindFile(DISK *disk, FAT_File far* file, const char* name, DirectoryEntry *BufferOut)
{
    char fatName[11];
    DirectoryEntry entry;

    memset(fatName, ' ', sizeof(fatName));
    fatName[11] = '\0';

    const char* ext = strchr(name, '.');
    if (ext == NULL)
        ext = name + 11;

    for(int i=0; i<8 && name[i] && name[i] !='.'; i++){
        fatName[i] = toUpper(name[i]);
    }
    if(ext != NULL){
        for(int i=0; i<3 && ext[i+1]; i++){
            fatName[i + 8] = toUpper(ext[i + 1]);
        }
    }
    printf("%s \r\n", fatName);
    while(ReadFileEntry(disk, file, &entry)){
        printf("%s \r\n", entry.Name);
        if(memcmp(fatName, entry.Name, 4) == 0){
            *BufferOut = entry;
            return true;
        }
    }

    return false;
}

void FAT_Close(FAT_File far* file)
{
    if (file->Handle == ROOT_DIRECTORY_HANDLE)
    {
        file->Position = 0;
        g_Data->RootDirectory.CurrentCluster = g_Data->RootDirectory.FirstCluster;
    }
    else
    {
        g_Data->OpenedFiles[file->Handle].Opened = false;
    }
}
FAT_File far* FAT_Open(DISK* disk, const char* path)
{
    char name[MAX_PATH_SIZE];

    if(path[0] == '/') path++;
    
    FAT_File far* current = &g_Data->RootDirectory.Public;

    while(*path){

        bool isLast = false;
        const char* delim = strchr(path, '/');

        if(delim != NULL){
            int len = delim - path;
            memcpy(name, path, len);
            name[len + 1] = '\0';
            path = delim + 1;
        }else{

            unsigned len = strlen(path);
            memcpy(name, path, len);
            name[len + 1] = '\0';
            path += len;
            isLast = true;
        }

        DirectoryEntry entry;
        if(FAT_FindFile(disk, current, name, &entry)){
            FAT_Close(current);
            if(!isLast && entry.Attributes & FAT_ATTRIBUTE_DIRECTORY == 0){
                printf("FAT: This %s is not a directory \r\n", name);
                return NULL;
            }
            printf("FAT: found %s \r\n", entry.Name);
            current = FAT_OpenEntry(disk, &entry);

        }else{
            FAT_Close(current);
            printf("FAT: %s not found\r\n", name);
            return NULL;
        }
    }

    return current;
}

uint32_t FAT_Read(DISK* disk, FAT_File far* file, uint32_t byteCount, void* dataOut)
{
    FAT_FileData far* fd = (file->Handle == ROOT_DIRECTORY_HANDLE)? &g_Data->RootDirectory : &g_Data->OpenedFiles[file->Handle];
    uint8_t *U8dataOut = (uint8_t *) dataOut;
    
    if(!fd->Public.IsDirectory){
        byteCount = min(byteCount, fd->Public.Size - fd->Public.Position);
    }

    while(byteCount > 0){
        
        uint32_t leftBuffer = SECTOR_SIZE - (fd->Public.Position % SECTOR_SIZE);
        uint32_t take = min(leftBuffer, byteCount);
        // printf("%d \r\n", fd->Public.Position);
        memcpy(U8dataOut, fd->Buffer + fd->Public.Position % SECTOR_SIZE, take);
        U8dataOut += take;
        byteCount -= take;
        fd->Public.Position += take;

        if(leftBuffer == take){

            if(fd->Public.Handle == ROOT_DIRECTORY_HANDLE){
                fd->CurrentCluster++;
                // printf("%d", fd->CurrentCluster);
                if(!Disk_Read_Sector(disk, fd->CurrentCluster, 1, fd->Buffer)){
                    printf("FAT: Error reading the Root FAT_Read\r\n");
                    return 0;
                }
            }
            else{
                if(++fd->CurrentSectorInCluster >= g_Data->BS.BootSector.SectorsPerCluster){
                    fd->CurrentSectorInCluster = 0;
                    fd->CurrentCluster = NextCluster(fd->CurrentCluster);
                }
                if (fd->CurrentCluster >= 0xFF8){
                    // Mark end of file
                    fd->Public.Size = fd->Public.Position;
                    printf("FAT: EOF reached \r\n");
                    break;
                }
                if(!Disk_Read_Sector(disk, FAT_ClusterToLba(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer)){
                    printf("FAT: Error reading the file FAT_Read\r\n");
                    return 0;
                }
            }
        }
        
        
    }
    return U8dataOut - (uint8_t*)dataOut;

}

FAT_File far* FAT_OpenEntry(DISK *disk, DirectoryEntry *entry)
{
    int handle = -1;

    for(int i=0; i<10 && handle < 0; i++){
        if(!g_Data->OpenedFiles[i].Opened){
            handle = i; 
        }
    }

    if(handle < 0) {
        printf("FAT: Out of Handles");
        return false;
    }

    FAT_FileData far* fd = &g_Data->OpenedFiles[handle];
    fd->Public.Handle = handle;
    fd->Public.IsDirectory = (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
    fd->Public.Position = 0;
    fd->Public.Size = entry->Size;
    fd->FirstCluster = entry->FirstClusterLow + ((uint32_t)entry->FirstClusterHigh << 16);
    fd->CurrentCluster = fd->FirstCluster;
    fd->CurrentSectorInCluster = 0;

    if(!Disk_Read_Sector(disk, FAT_ClusterToLba(fd->CurrentCluster), 1, fd->Buffer)){
        printf("FAT: Error reading in Open Entry");
    }

    fd->Opened = true;
    return &fd->Public;
}
// /ab/cd/ef
