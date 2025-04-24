#include <objbase.h>
#include <windows.h>
#include <shlobj_core.h>
#include <stdio.h>

char* WindowsExplorerLoad(char* result){
    OPENFILENAMEA sfn;
    
    ZeroMemory( &sfn , sizeof( sfn));
    sfn.lStructSize = sizeof ( sfn );
    sfn.hwndOwner = NULL;
    char syFile[MAX_PATH];
    sfn.lpstrFile = syFile;
    sfn.lpstrFile[0] = '\0';
    sfn.nMaxFile = sizeof(syFile);
    
    sfn.lpstrFilter = TEXT("PNG (*.png)\0*.PNG\0JPG (*.jpg;*.jpeg)\0*.JPG;*.JPEG\0Bitmap (*.bmp)\0*.BMP\0\0");
    
    sfn.nFilterIndex = 1;
    sfn.lpstrFileTitle = NULL ;
    sfn.nMaxFileTitle = 0 ;
    sfn.lpstrInitialDir=NULL;
    
    sfn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_NOVALIDATE|OFN_HIDEREADONLY  ;
    if(GetOpenFileNameA( &sfn ) == TRUE){
        
        DWORD  retval=0;
        
        TCHAR  buffer[1024]=TEXT(""); 
        TCHAR  buf[1024]=TEXT(""); 
        TCHAR** lppPart={NULL};
        
        retval = GetFullPathNameA(sfn.lpstrFile, sfn.nMaxFile, result, lppPart);
        
        return result;
    }
    return result;
}


char* WindowsExplorerSave(char* result){
    OPENFILENAMEA sfn;
    
    ZeroMemory( &sfn , sizeof( sfn));
    sfn.lStructSize = sizeof ( sfn );
    sfn.hwndOwner = NULL;
    char syFile[MAX_PATH];
    sfn.lpstrFile = syFile;
    sfn.lpstrFile[0] = '\0';
    sfn.nMaxFile = sizeof(syFile);
    
    sfn.lpstrFilter = TEXT("PNG (*.png)\0*.PNG\0JPG (*.jpg;*.jpeg)\0*.JPG;*.JPEG\0Bitmap (*.bmp)\0*.BMP\0\0");
    
    sfn.nFilterIndex =1;
    sfn.lpstrFileTitle = NULL ;
    sfn.nMaxFileTitle = 0 ;
    sfn.lpstrInitialDir=NULL;
    
    sfn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_NOVALIDATE|OFN_HIDEREADONLY  ;
    if(GetSaveFileNameA( &sfn ) == TRUE){
        
        DWORD  retval=0;
        
        TCHAR  buffer[1024]=TEXT(""); 
        TCHAR  buf[1024]=TEXT(""); 
        TCHAR** lppPart={NULL};
        
        retval = GetFullPathNameA(sfn.lpstrFile, sfn.nMaxFile, result, lppPart);
        
        printf("%d\n\n", sfn.nFileExtension);
        
        return result;
    }
    return result;
}
