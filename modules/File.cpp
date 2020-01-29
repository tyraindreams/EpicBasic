// --------------------------------------------------------------------------------
//
//                      EpicBasic File Library
//
// --------------------------------------------------------------------------------

int _ebP_OpenFile(const std::string &Path) {
   
   return open(Path.c_str(), O_RDWR ) ;
   
}

int _ebP_ReadFile(const std::string &Path) {
   
   return open(Path.c_str(), O_RDONLY ) ;
   
}

int _ebP_CreateFile(const std::string &Path) {
   
   return open(Path.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) ;
   
}

const auto& _ebP_CloseFile = close ;

const auto& _ebP_Read = read;
const auto& _ebP_Write = write;

unsigned char _ebP_ReadCharacter(const int &File) {
   unsigned char Character ;
   
   return (read(File, &Character, 1) == 1) ? (unsigned char) Character : EOF;
   
}

std::string _ebP_ReadString(const int &File) {
   std::string Result = "" ;
   char Character ;
   
   while (read(File, &Character, 1) > 0) {
      
      if (Character == 10 || Character == 0) {
         
         break ;
         
      } else if (Character != 13) {
         
         Result += Character ;
         
      }
      
   }
   
   return Result ;
   
}

int _ebP_WriteCharacter(const int &File, unsigned char Character) {
   
   return write(File, &Character, 1) ;
   
}

int _ebP_WriteString(const int &File,const std::string &Line) {
   
   return write(File, Line.c_str(), Line.size()) ;
   
}

int _ebP_WriteStringN(const int &File, std::string Line) {
   
   Line += "\r\n" ;
   
   return write(File, Line.c_str(), Line.size()) ;
   
}

int _ebP_Eof(const int &File) {
   unsigned char Character ;
   off_t CurrentPosition ;
   
   CurrentPosition = lseek(File, 0, SEEK_CUR);
   
   if (read(File, &Character, 1) == 0) {
      
      lseek(File,CurrentPosition,SEEK_SET) ;
      
      return true ;
      
   } else {
      
      lseek(File,CurrentPosition,SEEK_SET) ;
      
      return false ;
      
   }
   
}

int _ebP_Loc(const int &File) {
   
   return lseek(File, 0, SEEK_CUR);
   
}

int _ebP_Lof(const int &File) {
   int CurrentPosition ;
   int EndPosition ;
   
   CurrentPosition = lseek(File, 0, SEEK_CUR) ;
   EndPosition = lseek(File, 0, SEEK_END);
   lseek(File, CurrentPosition, SEEK_SET);
   
   return EndPosition ;
   
}

void _ebP_FileSeek(const int &File, const long &Position) {
   
   lseek(File,Position,SEEK_SET) ;
   
}

char _ebP_WaitFileEvent(int File, long Timeout) {
   
   fd_set fds;
   FD_ZERO(&fds);
   FD_SET(File, &fds);
   
   timeval tv; tv.tv_sec = 0; tv.tv_usec = Timeout*1000;
   
   select (File+1, &fds /* read*/, NULL /* write*/, NULL /* exception */, &tv);
   
   if (FD_ISSET(File, &fds)) {
      
       return true ;
      
   } else {
      
      return false ;
      
   }
   
}