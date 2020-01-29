// --------------------------------------------------------------------------------
//
//                      EpicBasic FileSystem Library
//
// --------------------------------------------------------------------------------

struct DirectoryStructure {
   
   int Position = 0 ;
   int EntryCount = 0 ;
   struct dirent ** Entry = {0} ;
   
} ;


static thread_local std::string CurrentPattern ;

int _ebI_PatternFilter(const struct dirent * Entry) {
   
   return !fnmatch(CurrentPattern.c_str(), Entry->d_name, 0) ;
   
}


char * _ebP_ExamineDirectory(const std::string &Path, std::string Pattern) {
   DirectoryStructure * NewDirectory = new DirectoryStructure ;
   
   CurrentPattern = Pattern ;
   
   NewDirectory->EntryCount = scandir(Path.c_str(), &NewDirectory->Entry, _ebI_PatternFilter, alphasort);
   
   if ( NewDirectory->EntryCount < 0 ) {
      
      delete NewDirectory ;
      return 0 ;
      
   }
   
   return reinterpret_cast<char*>(NewDirectory) ;
   
}


char * _ebP_OpenDirectory(const std::string &Path) {
   DirectoryStructure * NewDirectory = new DirectoryStructure ;
   
   NewDirectory->EntryCount = scandir(Path.c_str(), &NewDirectory->Entry, 0, alphasort);
   
   if ( NewDirectory->EntryCount < 0 ) {
      
      delete NewDirectory ;
      return 0 ;
      
   }
   
   return reinterpret_cast<char*>(NewDirectory) ;
   
}


char _ebP_NextDirectoryEntry(char * Directory) {
   
   while (true) {
      
      if (reinterpret_cast <DirectoryStructure *> (Directory)->Position >= reinterpret_cast <DirectoryStructure *> (Directory)->EntryCount ) {
         
         return false ;
         
      } else { 
         
         reinterpret_cast <DirectoryStructure *> (Directory)->Position++ ;
         
         return true ;
         
      }
   
   }
   
}

std::string _ebP_DirectoryEntryName(char * Directory) {
   
   return std::string(reinterpret_cast <DirectoryStructure *> (Directory)->Entry[reinterpret_cast <DirectoryStructure *> (Directory)->Position-1]->d_name) ;
   
}

char _ebP_DirectoryEntryType(char * Directory) {
   
   return reinterpret_cast <DirectoryStructure *> (Directory)->Entry[reinterpret_cast <DirectoryStructure *> (Directory)->Position-1]->d_type ;
   
}

void _ebP_CloseDirectory(char * Directory) {
   
   for(int i = 0; i < reinterpret_cast <DirectoryStructure *> (Directory)->EntryCount; i++)  {
      free(reinterpret_cast <DirectoryStructure *> (Directory)->Entry[i]);
   }
   
   free(reinterpret_cast <DirectoryStructure *> (Directory)->Entry) ;
   
   delete reinterpret_cast <DirectoryStructure *> (Directory) ;
   
}

std::string _ebP_GetCurrentDirectory() {
   long Size;
   char *Buffer;
   std::string Result ;
   
   Size = pathconf(".", _PC_PATH_MAX);
   
   if ((Buffer = (char *)malloc((size_t)Size)) != NULL) {
      
      Buffer = getcwd(Buffer, (size_t)Size);
      
      Result = std::string( Buffer ) ;
      
      free(Buffer) ;
      
      return Result ;
       
   }

   return std::string( "" ) ;

}


int _ebP_SetCurrentDirectory(const std::string &Path) {
   
   return chdir(Path.c_str()) ;
   
}


int _ebP_PathSize(const std::string &Path) {
   struct stat Stat ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return -1 ;
      
   }
   
   return Stat.st_size ;
   
}


int _ebP_PathAccessTime(const std::string &Path) {
   struct stat Stat ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return -1 ;
      
   }
   
   return Stat.st_atime ;
   
}


int _ebP_PathModifiedTime(const std::string &Path) {
   struct stat Stat ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return -1 ;
      
   }
   
   return Stat.st_mtime ;
   
}


std::string _ebP_PathOwner(const std::string &Path) {
   struct stat Stat ;
   struct passwd *PWD ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return "" ;
      
   }
   
   PWD = getpwuid(Stat.st_uid);
   return std::string((PWD == NULL) ? NULL : PWD->pw_name) ;
   
}

int _ebP_PathOwnerID(const std::string &Path) {
   struct stat Stat ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return -1 ;
      
   }
   
   return Stat.st_uid ;
   
}

std::string _ebP_PathGroup(const std::string &Path) {
   struct stat Stat ;
   struct group *GRP ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return "" ;
      
   }
   
   GRP = getgrgid(Stat.st_gid);
   return std::string((GRP == NULL) ? NULL : GRP->gr_name) ;
   
}

int _ebP_PathGroupID(const std::string &Path) {
   struct stat Stat ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return -1 ;
      
   }
   
   return Stat.st_gid ;
   
}

std::string _ebP_PathMode(const std::string &Path) {
   struct stat Stat ;
   char * Mode ;
   std::string Result ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return "" ;
      
   }
   
    Result += (S_ISDIR(Stat.st_mode) ? "d" : "-");
    Result += (Stat.st_mode & S_IRUSR ? "r" : "-");
    Result += (Stat.st_mode & S_IWUSR ? "w" : "-");
    Result += (Stat.st_mode & S_IXUSR ? "x" : "-");
    Result += (Stat.st_mode & S_IRGRP ? "r" : "-");
    Result += (Stat.st_mode & S_IWGRP ? "w" : "-");
    Result += (Stat.st_mode & S_IXGRP ? "x" : "-");
    Result += (Stat.st_mode & S_IROTH ? "r" : "-");
    Result += (Stat.st_mode & S_IWOTH ? "w" : "-");
    Result += (Stat.st_mode & S_IXOTH ? "x" : "-");
   return Result ;
   
}


int _ebP_PathModeID(const std::string &Path) {
   struct stat Stat ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return -1 ;
      
   }
   
   return Stat.st_mode ;
   
}


int _ebP_IsFile(const std::string &Path) {
   
   struct stat Stat ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return -1 ;
      
   }
   
   return S_ISREG(Stat.st_mode) ;
   
}


int _ebP_IsDirectory(const std::string &Path) {
   
   struct stat Stat ;
   
   if(lstat(Path.c_str(), &Stat) == -1) {
   
      return -1 ;
      
   }
   
   return S_ISDIR(Stat.st_mode) ;
   
}


int _ebP_DeleteFile(const std::string &Path) {
   int Status ;
   
   Status = remove(Path.c_str()) ;
 
   if( Status == 0 ) {
      
      return true ;
      
   } else {
      
      return false ;
      
   }
   
}

std::string _ebP_GetPathPart(const std::string &String) {
   
   return String.substr(0, String.find_last_of( '/' ) +1 ) ;
   
}

std::string _ebP_GetFilePart(const std::string &String) {
   
   return String.substr(String.find_last_of( '/' ) +1 ) ;
   
}

std::string _ebP_AbsolutePath(const std::string &String) {
   std::string str;
   // modern way - use dynamic memory
   char* Result = realpath(String.c_str(), NULL);

   if (!Result)
   {
#if !defined (__gnu_hurd__)
      // old way - static buffer
      char buf [PATH_MAX];
      realpath (String.c_str(), buf);
      str = buf;
#endif // __gnu_hurd__
   }
   else
   {
      str = Result;
      free (Result);
   }

   return str;
   
}
