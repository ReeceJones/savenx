// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>
#include <savedata.h>

void dumpSaveFiles()
{
    Result result;
    FsSaveDataInfoReader saveDataInfoReader;
    FsSaveDataInfo buf;
    s64 entries;
    FILE* outFile;
    NsApplicationControlData* applicationControlData;
    u64 realApplicationControlDataSize = 0;
    NacpLanguageEntry* applicationLanguageEntry;
    FsFileSystem saveDataFileSystem;
    FsDir saveDataRootDir;
    FsDirectoryEntry saveDataRootEntry;
    s64 saveDataRootDirEntries;

    outFile = fopen("sdmc:/savedump.txt", "w");

    result = fsOpenSaveDataInfoReader(&saveDataInfoReader, 1);
    if (R_FAILED(result))
    {
        fprintf(outFile, "Failed to open save data info reader. Aborting!");
        fflush(outFile);
        fclose(outFile);
        return;
    }

    while (R_SUCCEEDED(result = fsSaveDataInfoReaderRead(&saveDataInfoReader, &buf, 1, &entries)) && entries > 0)
    {
        applicationControlData = (NsApplicationControlData*)malloc(sizeof(NsApplicationControlData));
        memset(applicationControlData, 0, sizeof(NsApplicationControlData));
        memset(&saveDataFileSystem, 0, sizeof(FsFileSystem));
        result = nsGetApplicationControlData(NsApplicationControlSource_Storage, buf.application_id, applicationControlData, 
                                                sizeof(NsApplicationControlData), &realApplicationControlDataSize);
        if (R_FAILED(result))
        {
            printf("Failed to read application control data for application id %lX->%lu\n", buf.application_id, (unsigned long)R_DESCRIPTION(result));
            continue;
        }
        

        result = nacpGetLanguageEntry(&applicationControlData->nacp, &applicationLanguageEntry);
        if (R_FAILED(result))
        {
            printf("Failed to read language entry for application id %lu\n", buf.application_id);
            continue;
        }
        printf("%s\n", applicationLanguageEntry->name);
        fprintf(outFile, "Name: %s\nApplication Id: %lu\nSave Image Size: %lu\nSave Data Id: %lu\nSave Data Type: %d\nSave Data Index: %d\nSave Data Space Id: %d\nSystem Save Data Id: %lu\n",
                applicationLanguageEntry->name, buf.application_id, buf.size, buf.save_data_id, buf.save_data_type, buf.save_data_index, buf.save_data_space_id, buf.system_save_data_id);

        free(applicationControlData);
        // output all files
        result = fsOpen_SaveData(&saveDataFileSystem, buf.application_id, buf.uid);
        if (R_FAILED(result))
        {
            fprintf(outFile, "Failed to open save data file system for application id %lX->%u\n\n", buf.application_id, R_DESCRIPTION(result));
            continue;
        }
        result = fsFsOpenDirectory(&saveDataFileSystem, "/",  FsDirOpenMode_ReadFiles, &saveDataRootDir);
        if (R_FAILED(result))
        {
            fprintf(outFile, "Failed to open root directory of save data file system for applicaiton id %lX\n\n", buf.application_id);
            continue;
        }
        fprintf(outFile, "Root save directory listing: { ");
        while (R_SUCCEEDED(result = fsDirRead(&saveDataRootDir, &saveDataRootDirEntries, 1, &saveDataRootEntry)) && saveDataRootDirEntries > 0)
        {
            printf("File: %s\n", saveDataRootEntry.name);
            fprintf(outFile, "%s, ", saveDataRootEntry.name);
        }
        
        fprintf(outFile, " }\n");

        fprintf(outFile, "\n");
    }

    fflush(outFile);
    fclose(outFile);
}

// Main program entrypoint
int main(int argc, char* argv[])
{
    Result result;

    // This example uses a text console, as a simple way to output text to the screen.
    // If you want to write a software-rendered graphics application,
    //   take a look at the graphics/simplegfx example, which uses the libnx Framebuffer API instead.
    // If on the other hand you want to write an OpenGL based application,
    //   take a look at the graphics/opengl set of examples, which uses EGL instead.
    consoleInit(NULL);

    result = nsInitialize();
    if (R_FAILED(result))
    {
        printf("failed to initialize ns services\n");
    }
    else
    {
        // Other initialization goes here. As a demonstration, we print hello world.
        printf("starting\n");
        dumpSaveFiles();
        printf("done\n");
    }

    printf("Press minus button to return to menu\n");

    // Main loop
    while (appletMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been
        // just pressed in this frame compared to the previous one
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_MINUS)
            break; // break in order to return to hbmenu

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
