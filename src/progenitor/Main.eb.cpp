#include <string>
#include <cstdlib>
#include <stddef.h>
#include <iostream>
#include <csignal>
#include <vector>
#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <sys/param.h>
#include <sys/stat.h>
#include <pcre.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <sys/wait.h>

// --------------------------------------------------------------------------------
//
//                      EpicBasic Embedded Debugger
//
// --------------------------------------------------------------------------------

#define _ebD_Debugger 1

enum {
   _ebD_Debugger_Flag_LineTrace,
   _ebD_Debugger_Flag_CallTrace,
   _ebD_Debugger_Flag_CallPop,
   _ebD_Debugger_Flag_Interupt,
   _ebD_Debugger_Flag_Exception,
   _ebD_Debugger_Flag_Error,
   _ebD_Debugger_Flag_Warning,
   _ebD_Debugger_Flag_Call,
   _ebD_Debugger_Flag_Initialize,
   _ebD_Debugger_Flag_Deinitialize,
} ;

void _ebD_CallDebugger(const char Flag, const char * Message) ;
void _ebD_InteruptDebugger(int SignalNumber) ;
void _ebD_TerminateDebugger() ;

void _ebD_InteruptDebugger(int SignalNumber) {
   
   if (SignalNumber == SIGSEGV) {
      
      _ebD_CallDebugger(_ebD_Debugger_Flag_Error, "Segmentation Fault Occured.") ;
      
   } else if (SignalNumber == SIGFPE) {
      
      _ebD_CallDebugger(_ebD_Debugger_Flag_Error, "Arithmetic Error.") ;
      
   } else if (SignalNumber == SIGILL) {
      
      _ebD_CallDebugger(_ebD_Debugger_Flag_Error, "Illegal Instruction.") ;
      
   } else if (SignalNumber == SIGINT) {
      
      _ebD_CallDebugger(_ebD_Debugger_Flag_Interupt, "") ;
      
   }
   
}

void _ebD_TerminateDebugger(int SignalNumber) {
   
   exit(SignalNumber) ;
   
}

void _ebD_CallDebugger(const char Flag, const char * Message) {
   static const char * LineTrace = "" ;
   static const char * CallTrace = "" ;
   static const char * LineCaller = "" ;
   static std::vector<const char *> CallStack ;
   static char Step = false ;
   
   if (Flag == _ebD_Debugger_Flag_Initialize) {
      
      signal(SIGINT, _ebD_InteruptDebugger) ;
      signal(SIGSEGV, _ebD_InteruptDebugger) ;
      signal(SIGFPE, _ebD_InteruptDebugger) ;
      signal(SIGILL, _ebD_InteruptDebugger) ;
      
      return ;
      
   }
   
   if (Flag == _ebD_Debugger_Flag_LineTrace) {
      
      LineTrace = Message ;
      
   } else if (Flag == _ebD_Debugger_Flag_CallTrace) {
      
      CallStack.push_back(Message) ;
      CallTrace = Message ;
      LineCaller = LineTrace ;
      
   } else if (Flag == _ebD_Debugger_Flag_CallPop) {
      
      CallStack.pop_back() ;
      
   }
   
   if (Flag == _ebD_Debugger_Flag_Error || Flag == _ebD_Debugger_Flag_Interupt || Flag == _ebD_Debugger_Flag_Call || (Step == true && Flag == _ebD_Debugger_Flag_LineTrace) || Flag == _ebD_Debugger_Flag_Exception) {
      
      if (Flag == _ebD_Debugger_Flag_Call) {
         
         std::cout << "Debugger was called by the program: " << std::string(LineTrace) << std::endl ;
         std::cout.flush() ;
         
      } else if (Flag == _ebD_Debugger_Flag_Interupt) {
         
         std::cout << "Debugger was called by interupt signal: " << std::string(LineTrace) << std::endl ;
         std::cout.flush() ;
         
      } else if (Flag == _ebD_Debugger_Flag_Error) {
         
         std::cout << "Debugger was called due to an error: " << std::string(LineTrace) << ": " << std::string(Message) << std::endl ;
         std::cout.flush() ;
         
      } else if (Flag == _ebD_Debugger_Flag_Warning) {
         
         std::cout << "Debugger was called due to an Warning: " << std::string(LineTrace) << ": " << std::string(Message) << std::endl ;
         std::cout.flush() ;

      } else if (Flag == _ebD_Debugger_Flag_Exception) {
         
         std::cout << "Debugger was called due to an unhandled Exception: " << std::string(LineTrace) << std::endl ;
         std::cout.flush() ;

      }
      
      if (Step == true) {
         std::cout << "Debugger was called by step: " << std::string(LineTrace) << std::endl ;
         std::cout.flush() ;
      }
      
      std::string Command ;
      
      std::cout << std::endl ;
      std::cout << "-- EpicBasic Debugger  --" << std::endl << std::endl ;
      std::cout << "type 'help' to display commands" << std::endl << std::endl ;
      std::cout.flush() ;
      
      while (true) {
         
         std::cout << "? " ;
         std::cin >> Command ;
         
         if (Command == "help" || Command == "h") {
            
            std::cout << std::endl ;
            std::cout << " --- EpicBasic Debugger Help ---" << std::endl ;
            std::cout << " Command    Short    Description" << std::endl ;
            std::cout << " -------------------------------" << std::endl ;
            std::cout << " help       h        Display this help message" << std::endl ;
            std::cout << " callstack  cs       Display the full the callstack" << std::endl ;
            std::cout << " calltrace  ct       Display the last procedure call and what line called it and the current position on the callstack" << std::endl ;
            std::cout << " line       l        Display the last line executed" << std::endl ;
            std::cout << " step       s        Step the program 1 line" << std::endl ;
            std::cout << " run        r        Resume running the program" << std::endl ;
            std::cout << " exit       x        Exit the program" << std::endl << std::endl ;
            std::cout.flush() ;
            
         } else if (Command == "calltrace" || Command == "ct") {
            
            std::cout << "CallStack     " << std::string(CallStack.back()) << std::endl ;
            std::cout << "CallTrace     " << std::string(CallTrace) << std::endl ;
            std::cout << "Line          " << std::string(LineCaller) << std::endl << std::endl ;
            std::cout.flush() ;
            
         } else if (Command == "callstack" || Command == "cs") {
            
            std::cout << std::endl ;
            
            for (auto& a : CallStack) { 
               
               std::cout << std::string(a) << std::endl ;
               
            } 
            
            std::cout << std::endl ;
            std::cout.flush() ;
            
         } else if (Command == "line" || Command == "l") {
            
            std::cout << "Line          " << std::string(LineTrace) << std::endl << std::endl ;
            std::cout.flush() ;
            
         } else if (Command == "step" || Command == "s") {
            
            Step = true ;
            break ;
            
         } else if (Command == "run" || Command == "r") {
            
            Step = false ;
            break ;
            
         } else if (Command == "exit" || Command == "x") {
            
            _ebD_TerminateDebugger(0) ;
            
         }
         
      }
      
   }
   
   return ;
   
}

// --------------------------------------------------------------------------------
//
//                      EpicBasic Console Library
//
// --------------------------------------------------------------------------------

signed char _ebI__TTYStatus = false ;

void _ebI_InitConsole() {
   
   if (isatty(fileno(stdout)) == 1) {
      
      _ebI__TTYStatus = true ;
      
   } else {
      
      _ebI__TTYStatus = false ;
      
   }
   
}

void _ebP_SetTTYStatus(signed char Status) {
   
   _ebI__TTYStatus = Status ;
   
}

signed char _ebP_GetTTYStatus() {
   
   return _ebI__TTYStatus ;
   
}

void _ebP_Print(const std::string &Text) {
   
   std::cout << Text ;
   
}

void _ebP_PrintN(const std::string &Text) {
   
   std::cout << Text << std::endl ;
   
}

inline std::string _ebP_Input(void) {
   std::string Input ;
   
   std::cin >> Input ;
   
   return Input ;
   
}

void _ebP_ClearConsole() {
   
   if (_ebI__TTYStatus) {
      
      std::cout << "\033[2J" ;
      
   }
   
}

void _ebP_ResetConsole() {
   
   if (_ebI__TTYStatus) {
      
      std::cout << "\033[0m" ;
      
   }
   
}

void _ebP_ConsoleTextBold() {
   
   if (_ebI__TTYStatus) {
      
      std::cout << "\033[1m" ;
      
   }
   
}

void _ebP_ConsoleTextColor(int Color) {
   
   if (_ebI__TTYStatus) {
      
      Color = Color & 7 ;
      std::cout << "\033["+(std::to_string(Color + 30))+"m" ;
      
   }
   
}

void _ebP_ConsoleBackgroundColor(int Color) {
   
   if (_ebI__TTYStatus) {
      
      Color = Color & 7 ;
      std::cout << "\033["+(std::to_string(Color + 40))+"m" ;
      
   }
   
}

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
   
   return write(File, Line.c_str(), Line.length()) ;
   
}

int _ebP_WriteStringN(const int &File, std::string Line) {
   
   Line = Line + "\r\n" ;
   
   return write(File, Line.c_str(), Line.length()) ;
   
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

// --------------------------------------------------------------------------------
//
//                      EpicBasic FileSystem Library
//
// --------------------------------------------------------------------------------

struct DirectoryStructure {
   
   DIR * Directory ;
   dirent * Entry ;
   
} ;

char * _ebP_OpenDirectory(std::string Path) {
   DirectoryStructure * NewDirectory = new DirectoryStructure ;
   
   NewDirectory->Directory = opendir(Path.c_str()) ;
   
   return reinterpret_cast <char *> (NewDirectory) ;
   
}

char _ebP_NextDirectoryEntry(char * Directory) {
   
   reinterpret_cast <DirectoryStructure *> (Directory)->Entry = readdir(reinterpret_cast <DirectoryStructure *> (Directory)->Directory) ;
   
   if (reinterpret_cast <DirectoryStructure *> (Directory)->Entry == NULL ) {
      
      return false ;
      
   } else { 
      
      return true ;
      
   }
   
}

std::string _ebP_DirectoryEntryName(char * Directory) {
   
   return std::string(reinterpret_cast <DirectoryStructure *> (Directory)->Entry->d_name) ;
   
}

char _ebP_DirectoryEntryType(char * Directory) {
   
   return reinterpret_cast <DirectoryStructure *> (Directory)->Entry->d_type ;
   
}

int _ebP_CloseDirectory(char * Directory) {
   int Result ;
   
   Result = closedir(reinterpret_cast <DirectoryStructure *> (Directory)->Directory) ;
   
   delete reinterpret_cast <DirectoryStructure *> (Directory) ;
   
   return Result ;
   
}

std::string _ebP_GetCurrentDirectory() {
   long Size;
   char *Buffer;

   Size = pathconf(".", _PC_PATH_MAX);


   if ((Buffer = (char *)malloc((size_t)Size)) != NULL) {
       
      Buffer = getcwd(Buffer, (size_t)Size);
       
       return std::string( Buffer ) ;
       
   }

   return std::string( "" ) ;

}


int _ebP_SetCurrentDirectory(const std::string &Path) {
   
   return chdir(Path.c_str()) ;
   
}

int _ebP_FileSize(const std::string &Path) {
   struct stat Stat ;
   
   if(stat(Path.c_str(), &Stat) == -1) {
   
      return -1 ;
      
   }
   
   return Stat.st_size ;
   
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

// --------------------------------------------------------------------------------
//
//                      EpicBasic RegularExpression Library
//
// --------------------------------------------------------------------------------

const char * _pcreErrorString ;
int _pcreErrorOffset;

struct _RegularExpression {
   pcre * CompiledExpression ;
   pcre_extra * Optimizations ;
   int ExecResult = 0 ;
   int CurrentGroup = 0 ;
   int Offset = 0 ;
   int SubStringVector[30000] ;
   std::string CurrentQuery ;
   std::string CurrentMatch ;
} ;

char * _ebP_CompileRegularExpression(const std::string &RegularExpressionString) {
   _RegularExpression * RegularExpression = new _RegularExpression ;
   
   RegularExpression->CompiledExpression = pcre_compile(RegularExpressionString.c_str(), 0, &_pcreErrorString, &_pcreErrorOffset, NULL) ;
   
   if(RegularExpression->CompiledExpression == NULL) {
   
      return NULL ;
      
   }

   RegularExpression->Optimizations = pcre_study(RegularExpression->CompiledExpression, PCRE_STUDY_JIT_COMPILE, &_pcreErrorString);
   
   if(_pcreErrorString != NULL) {
   
      return NULL ;
      
   }
   
   return reinterpret_cast <char *> (RegularExpression) ;
   
}

int _ebP_ExamineRegularExpression(const char * RegularExpression, const std::string &Query) {
   _RegularExpression * RegularExpressionObject = (_RegularExpression*)RegularExpression ;
   
   RegularExpressionObject->CurrentQuery = Query ;
   RegularExpressionObject->Offset = 0 ;
   RegularExpressionObject->CurrentGroup = 0 ;
   RegularExpressionObject->ExecResult = pcre_exec(RegularExpressionObject->CompiledExpression, RegularExpressionObject->Optimizations, RegularExpressionObject->CurrentQuery.c_str(), RegularExpressionObject->CurrentQuery.length(), RegularExpressionObject->Offset, 0, RegularExpressionObject->SubStringVector, 30000) ;
   
   if(RegularExpressionObject->ExecResult < 0) {
      
      return false ;
      
   }
   
   return true ;
   
}

int _ebP_NextRegularExpressionMatch(const char * RegularExpression) {
   _RegularExpression * RegularExpressionObject = (_RegularExpression*)RegularExpression ;
   const char *psubStrMatchStr ;
   
   if (RegularExpressionObject->Offset < RegularExpressionObject->CurrentQuery.length() ) {
   
      RegularExpressionObject->ExecResult = pcre_exec(RegularExpressionObject->CompiledExpression, RegularExpressionObject->Optimizations, RegularExpressionObject->CurrentQuery.c_str(), RegularExpressionObject->CurrentQuery.length(), RegularExpressionObject->Offset, 0, RegularExpressionObject->SubStringVector, 30000) ;
   
      if(RegularExpressionObject->ExecResult < 0) {
      
         return false ;
         
      }
      
      pcre_get_substring(RegularExpressionObject->CurrentQuery.c_str(), RegularExpressionObject->SubStringVector, RegularExpressionObject->ExecResult, RegularExpressionObject->CurrentGroup, &(psubStrMatchStr));
      
      RegularExpressionObject->CurrentMatch = psubStrMatchStr ;
      pcre_free_substring(psubStrMatchStr) ;      
      RegularExpressionObject->Offset = RegularExpressionObject->SubStringVector[(RegularExpressionObject->CurrentGroup*2)+1] ;

      return true ;
      
   }
   
   return false ;
   
}

int _ebP_RegularExpressionMatchGroup(const char * RegularExpression) {
   _RegularExpression * RegularExpressionObject = (_RegularExpression*)RegularExpression ;
   
   if (RegularExpressionObject->CurrentGroup < RegularExpressionObject->ExecResult ) {
      
      return RegularExpressionObject->ExecResult-1 ;
      
   } else {
      
      return false ;
      
   }
   
}

int _ebP_RegularExpressionMatchPosition(const char * RegularExpression) {
   _RegularExpression * RegularExpressionObject = (_RegularExpression*)RegularExpression ;
   
   if (RegularExpressionObject->CurrentGroup < RegularExpressionObject->ExecResult ) {
      
      return RegularExpressionObject->SubStringVector[(RegularExpressionObject->CurrentGroup*2)+1]-RegularExpressionObject->CurrentMatch.length()+1 ; // fix this
      
   } else {
      
      return false ;
      
   }
   
}

std::string _ebP_RegularExpressionMatchString(const char * RegularExpression) {
   _RegularExpression * RegularExpressionObject = (_RegularExpression*)RegularExpression ;
   
   if (RegularExpressionObject->CurrentGroup < RegularExpressionObject->ExecResult ) {
      
      return RegularExpressionObject->CurrentMatch ;
      
   } else {
      
      return "" ;
      
   }
   
}

void _ebP_FreeRegularExpression(const char * RegularExpression) {
   _RegularExpression * RegularExpressionObject = (_RegularExpression*)RegularExpression ;
   
   pcre_free(RegularExpressionObject->CompiledExpression) ;
   
   if(RegularExpressionObject->Optimizations != NULL) {
      
      pcre_free(RegularExpressionObject->Optimizations) ;
      
   }
   
   delete RegularExpressionObject ;
}   


// --------------------------------------------------------------------------------
//
//                      EpicBasic Memory Library
//
// --------------------------------------------------------------------------------


#define _ebP_AllocateMemory(Size) reinterpret_cast <char *> (malloc(Size))
   
const auto& _ebP_ZeroAllocateMemory = calloc ;

#define _ebP_ReAllocateMemory(Pointer, Size) reinterpret_cast <char *> (realloc(Pointer, Size))

const auto& _ebP_FreeMemory = free ;

#define _ebP_PokeC(Address, Pointer) (*(unsigned char *) (Address) = (Pointer))

#define _ebP_PeekC(Address) (*(unsigned char *)(Address))

#define _ebP_PokeB(Address, Pointer) (*(signed char *) (Address) = (Pointer))

#define _ebP_PeekB(Address) (*(signed char *)(Address))

#define _ebP_PokeI(Address, Pointer) (*(int *) (Address) = (Pointer))

#define _ebP_PeekI(Address) (*(int *)(Address))

#define _ebP_PokeL(Address, Pointer) (*(long *) (Address) = (Pointer))

#define _ebP_PeekL(Address) (*(long *)(Address))

#define _ebP_PokeP(Address, Pointer) (*(char **) (Address) = (Pointer))

#define _ebP_PeekP(Address) (*(char **)(Address))

#define _ebP_PokeS(Address, String) strcpy((char *) Address, String.c_str())

std::string _ebP_PeekS(const char * Address) {
   
   return (char *) Address ;
   
}

char * _ebP_DuplicateString(const std::string String) {
   
   return strdup(String.c_str()) ;
   
}

// --------------------------------------------------------------------------------
//
//                      EpicBasic System Library
//
// --------------------------------------------------------------------------------

// Courtesy of thothonegan
// --- x86 ---
#if defined(__i386__) || defined(_M_IX86)
   #undef _ebI_ARCHITECTURE_X86
   #undef _ebI_ARCHITECTURE_X86_32
   #undef _ebI_ARCHITECTURE_ENDIAN_LITTLE
   #undef _ebI_ARCHITECTURE_ADDRESSSPACE_BITS

   #define _ebI_ARCHITECTURE_X86               1
   #define _ebI_ARCHITECTURE_X86_32            1
   #define _ebI_ARCHITECTURE_ENDIAN_LITTLE     1
   #define _ebI_ARCHITECTURE_ADDRESSSPACE_BITS 32
   #define _ebI_ARCHITECTURE_STRING "x86_32"

#elif defined(__amd64__) || defined(_M_X64)
   #undef _ebI_ARCHITECTURE_X86
   #undef _ebI_ARCHITECTURE_X86_64
   #undef _ebI_ARCHITECTURE_X86_64
   #undef _ebI_ARCHITECTURE_ENDIAN_LITTLE
   #undef _ebI_ARCHITECTURE_ADDRESSSPACE_BITS

   #define _ebI_ARCHITECTURE_X86               1
   #define _ebI_ARCHITECTURE_X86_64            1
   #define _ebI_ARCHITECTURE_ENDIAN_LITTLE     1
   #define _ebI_ARCHITECTURE_ADDRESSSPACE_BITS 64
   #define _ebI_ARCHITECTURE_STRING "x86_64"

// --- PPC ---
#elif defined(__ppc__) || defined(M_PPC)
   #undef _ebI_ARCHITECTURE_PPC
   #undef _ebI_ARCHITECTURE_PPC_32
   #undef _ebI_ARCHITECTURE_ENDIAN_BIG
   #undef _ebI_ARCHITECTURE_ADDRESSSPACE_BITS

   #define _ebI_ARCHITECTURE_PPC 1
   #define _ebI_ARCHITECTURE_PPC_32 1
   #define _ebI_ARCHITECTURE_ENDIAN_BIG 1
   #define _ebI_ARCHITECTURE_ADDRESSSPACE_BITS 32
   #define _ebI_ARCHITECTURE_STRING "ppc32"

#elif defined(__ppc64__) || defined(__powerpc64__)
   #undef _ebI_ARCHITECTURE_PPC
   #undef _ebI_ARCHITECTURE_PPC_64
   #undef _ebI_ARCHITECTURE_ENDIAN_BIG
   #undef _ebI_ARCHITECTURE_ADDRESSSPACE_BITS

   #define _ebI_ARCHITECTURE_PPC 1
   #define _ebI_ARCHITECTURE_PPC_64 1
   #define _ebI_ARCHITECTURE_ENDIAN_BIG 1
   #define _ebI_ARCHITECTURE_ADDRESSSPACE_BITS 64

   #if defined(__PPU__)
            #undef _ebI_ARCHITECTURE_PPC_64_CELL
            #define _ebI_ARCHITECTURE_PPC_64_CELL
            #define _ebI_ARCHITECTURE_STRING "ppc64_cell_ppu"
   #else
            #define _ebI_ARCHITECTURE_STRING "ppc64"
   #endif

// --- CELLSPU ---
#elif defined(__SPU__)
   #undef _ebI_ARCHITECTURE_CELLSPU
   #undef _ebI_ARCHITECTURE_ENDIAN_BIG

   #define _ebI_ARCHITECTURE_CELLSPU 1
   #define _ebI_ARCHITECTURE_ENDIAN_BIG 1
   #define _ebI_ARCHITECTURE_STRING "cellspu"

// --- MIPS ---
#elif defined(__mips__)
   #undef _ebI_ARCHITECTURE_MIPS
   #undef _ebI_ARCHITECTURE_ENDIAN_LITTLE

   #define _ebI_ARCHITECTURE_MIPS 1
            
   // Note that while the arch can be either, all devices we've worked with are LE. Fix later if this isnt always 1.
   #define _ebI_ARCHITECTURE_ENDIAN_LITTLE 1
   #define _ebI_ARCHITECTURE_STRING "mips"
            
// --- ARM ---
#elif defined (__arm__)
   #undef _ebI_ARCHITECTURE_ARM
   #undef _ebI_ARCHITECTURE_ARM_AARCH32
   #undef _ebI_ARCHITECTURE_ENDIAN_LITTLE
   #undef _ebI_ARCHITECTURE_ADDRESSSPACE_BITS

   #define _ebI_ARCHITECTURE_ARM 1
   #define _ebI_ARCHITECTURE_ARM_AARCH32 1
   #define _ebI_ARCHITECTURE_ENDIAN_LITTLE 1
   #define _ebI_ARCHITECTURE_ADDRESSSPACE_BITS 32

   // Based off macros in <arch/arm.h> [osx]
   #if defined(__ARM_ARCH_7A__)
            #undef  _ebI_ARCHITECTURE_ARM_AARCH32_V7
            #define _ebI_ARCHITECTURE_ARM_AARCH32_V7 1
   #endif

   #if defined(_ebI_ARCHITECTURE_ARM_AARCH32_V7) \
            || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6ZK__) \
            || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__) \
            || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6__)
            
            #undef  _ebI_ARCHITECTURE_ARM_AARCH32_V6
            #define _ebI_ARCHITECTURE_ARM_AARCH32_V6 1
   #endif

   #if defined(_ebI_ARCHITECTURE_ARM_AARCH32_V6) \
            || defined(__ARM_ARCH_5E__) || defined(__ARM_ARCH_5TE__) \
            || defined(__ARM_ARCH_5TEJ__) || defined(__ARM_ARCH_5__) \
            || defined(__ARM_ARCH_5T__)
            
            #undef  _ebI_ARCHITECTURE_ARM_AARCH32_V5
            #define _ebI_ARCHITECTURE_ARM_AARCH32_V5 1
   #endif

   #if defined (_ebI_ARCHITECTURE_ARM_AARCH32_V5) \
            || defined (__ARM_ARCH_4T__) || defined (__ARM_ARCH_4__)
            
            #undef  _ebI_ARCHITECTURE_ARM_AARCH32_V4
            #define _ebI_ARCHITECTURE_ARM_AARCH32_V4 1
   #endif

   #if defined (_ebI_ARCHITECTURE_ARM_AARCH32_V7)
            #define _ebI_ARCHITECTURE_STRING "arm32_v7"
   #elif defined (_ebI_ARCHITECTURE_ARM_AARCH32_V6)
            #define _ebI_ARCHITECTURE_STRING "arm32_v6"
   #elif defined (_ebI_ARCHITECTURE_ARM_AARCH32_V5)
            #define _ebI_ARCHITECTURE_STRING "arm32_v5"
   #elif defined (_ebI_ARCHITECTURE_ARM_AARCH32_V4)
            #define _ebI_ARCHITECTURE_STRING "arm32_v4"
   #else // unknown level
            #define _ebI_ARCHITECTURE_STRING "arm32"
   #endif

// --- PNaCl---
#elif defined (__pnacl__)
   #undef _ebI_ARCHITECTURE_PNACL
   #undef _ebI_ARCHITECTURE_ENDIAN_LITTLE
   #undef _ebI_ARCHITECTURE_ADDRESSSPACE_BITS

   #define _ebI_ARCHITECTURE_PNACL 1
   #define _ebI_ARCHITECTURE_ENDIAN_LITTLE 1
   #define _ebI_ARCHITECTURE_ADDRESSSPACE_BITS 32
   #define _ebI_ARCHITECTURE_STRING "pnacl"

// --- Enscripten ---
#elif defined (EMSCRIPTEN)
   #undef _ebI_ARCHITECTURE_EMSCRIPTEN
   #undef _ebI_ARCHITECTURE_ENDIAN_LITTLE
   #undef _ebI_ARCHITECTURE_ADDRESSSPACE_BITS

   #define _ebI_ARCHITECTURE_EMSCRIPTEN 1
   #define _ebI_ARCHITECTURE_ENDIAN_LITTLE 1
   #define _ebI_ARCHITECTURE_ADDRESSSPACE_BITS 32
   #define _ebI_ARCHITECTURE_STRING "emscripten"

#endif // arch checks

// Platform checks

#if defined (WIN32) || defined(WIN64)
   #define _ebI_PLATFORM_STRING "Windows"
   #define _ebI_PLATFORM_UNIXLIKE 0
   #define _ebI_PLATFORM_WINDOWS 1
   #define _ebI_PLATFORM_UNIX 0
   #define _ebI_PLATFORM_BSD 0
#elif defined (__CYGWIN__)
   #define _ebI_PLATFORM_STRING "Windows-Cygwin"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_WINDOWS 1
   #define _ebI_PLATFORM_UNIX 0
   #define _ebI_PLATFORM_BSD 0
#elif defined (__FreeBSD__)
   #define _ebI_PLATFORM_STRING "FreeBSD"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_FREEBSD 1
   #define _ebI_PLATFORM_UNIX 1
   #define _ebI_PLATFORM_BSD 1
#elif defined (__NetBSD__)
   #define _ebI_PLATFORM_STRING "NetBSD"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_NETBSD 1
   #define _ebI_PLATFORM_UNIX 1
   #define _ebI_PLATFORM_BSD 1
#elif defined (__OpenBSD__)
   #define _ebI_PLATFORM_STRING "OpenBSD"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_OPENBSD 1
   #define _ebI_PLATFORM_UNIX 1
   #define _ebI_PLATFORM_BSD 1
#elif defined (__DragonFly__)
   #define _ebI_PLATFORM_STRING "DragonFlyBSD"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_DRAGONFLYBSD 1
   #define _ebI_PLATFORM_UNIX 1
   #define _ebI_PLATFORM_BSD 1
#elif defined (__hpux)
   #define _ebI_PLATFORM_STRING "HP-UX"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_HPUX 1
   #define _ebI_PLATFORM_UNIX 1
   #define _ebI_PLATFORM_BSD 0
#elif defined (_AIX)
   #define _ebI_PLATFORM_STRING "AIX"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_AIX 1
   #define _ebI_PLATFORM_UNIX 1
   #define _ebI_PLATFORM_BSD 0
#elif defined (__sun)
   #define _ebI_PLATFORM_STRING "Solaris"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_SOLARIS 1
   #define _ebI_PLATFORM_UNIX 1
   #define _ebI_PLATFORM_BSD 0
#elif defined (__APPLE__) && defined(__MACH__)
   #include <TargetConditionals.h>
   #if TARGET_IPHONE_SIMULATOR == 1
      #define _ebI_PLATFORM_STRING "iPhone"
   #elif TARGET_OS_IPHONE == 1
      #define _ebI_PLATFORM_STRING "iPhone" // How did you do this?
   #elif TARGET_OS_MAC == 1
      #define _ebI_PLATFORM_STRING "OSX"
      #define _ebI_PLATFORM_OSX 1
   #endif
   #define _ebI_PLATFORM_OSX 1
   #define _ebI_PLATFORM_UNIX 1
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_BSD 1
#elif defined (__linux) || defined(__linux__) || defined(__gnu_linux)
   #define _ebI_PLATFORM_STRING "Linux"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_LINUX 1
   #define _ebI_PLATFORM_UNIX 0
   #define _ebI_PLATFORM_BSD 0
#elif defined (ANDROID)
   #define _ebI_PLATFORM_STRING "Android"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_LINUX 1
   #define _ebI_PLATFORM_UNIX 0
   #define _ebI_PLATFORM_BSD 0
#elif defined (unix) || defined(__unix) || defined(__unix__)
   #define _ebI_PLATFORM_STRING "Unix"
   #define _ebI_PLATFORM_UNIXLIKE 1
   #define _ebI_PLATFORM_UNIX 1 // apparently...
   #define _ebI_PLATFORM_BSD 0
#elif defined (__HAIKU__)
   #define _ebI_PLATFORM_STRING "Haiku"
   #define _ebI_PLATFORM_UNIXLIKE 0
   #define _ebI_PLATFORM_HAIKU 1
   #define _ebI_PLATFORM_UNIX 0
   #define _ebI_PLATFORM_BSD 0
#endif // arch checks


std::string _ebP_UserName() {
   
   return std::string ( getlogin() ) ;
   
}

void _ebP_MoreCowbell() {
   
   // I've got a fever and the only prescription is more cowbell.
   static long long Cowbell ;
   Cowbell = Cowbell + 1 ;
   
}

void _ebP_Delay(const int Time) {
   
   usleep(1000 * Time) ;
   
}

double _ebP_LoadAverage(const signed char Index) {
   double Load[3] ;

   getloadavg(Load, 3) ;
   
   return Load[Index] ;
   
}

const auto& _ebP_Time = time ;

char * _ebP_GMTTime() {
   time_t Time = time(0) ;
   
   return reinterpret_cast <char*>(gmtime(&Time)) ;
   
}

char * _ebP_LocalTime() {
   time_t Time = time(0) ;
   
   return reinterpret_cast <char*>(localtime(&Time)) ;
   
}

// --------------------------------------------------------------------------------
//
//                      EpicBasic String Library
//
// --------------------------------------------------------------------------------

std::string _ebP_ReplaceString(std::string str, const std::string& oldStr, const std::string& newStr){
  size_t pos = 0;
  
  while((pos = str.find(oldStr, pos)) != std::string::npos) {
     
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
     
  }
  
  return str ;
  
}

std::string _ebP_RepeatString(const std::string &String, const long Times) {
   long int t ;
   std::string Result ;
   
   for (t = 0; t < Times; t++ ) {
      
      Result = Result + String ;
      
   }
   
   return Result ;
   
}

std::string _ebP_Space(const long Number) {
   
   return std::string(Number, ' ');
   
}

std::string _ebP_Right(const std::string &String, const long Trim) {
   
   try {
      
      return String.substr(String.length()-Trim, String.length()) ;
      
   } catch (...) {
      
      return "" ;
      
   }
   
}

std::string _ebP_Left(const std::string &String, const long Trim) {
   
   try {
      
      return String.substr(0, Trim) ;
      
   } catch (...) {
      
      return "" ;
      
   }
   
}

std::string _ebP_Mid(const std::string &String, const long Start, const long Finish) {
      
   try {
      
      return String.substr(Start, Finish) ;
      
   } catch (...) {
      
      return "" ;
      
   }
   
}

std::string _ebP_LCase(std::string String) {
   
   std::transform(String.begin(), String.end(), String.begin(), ::tolower) ;
   
   return String ;
   
}

std::string _ebP_UCase(std::string String) {
   
   std::transform(String.begin(), String.end(), String.begin(), ::toupper) ;
   
   return String ;
}

#define _ebP_Len(String) String.length()

#define _ebP_Size(String) String.size()

std::string _ebP_Chr(const char Number) {;
   
   return std::string(1, Number) ;
   
}

template< typename T >
std::string _ebP_Str(const T Number) {
   
   std::string String = std::to_string(Number) ;
   
   return String ;
   
}

std::string _ebP_StrF(const float Number) {
   
   std::string String = std::to_string(Number) ;
   
   return String ;
   
}

int _ebP_Val(const std::string &String) {
   
   try {
   
      int Number = std::stoi(String,nullptr,0) ;
      
      return Number ;
      
   } catch (...) {
      
      return false ;
      
   }
   
}

float _ebP_ValF(const std::string &String) {
   
   try {
   
      float Number = std::stoi (String,nullptr,0) ;
      
      return Number ;
      
   } catch (...) {
      
      return false ;
      
   }
   
}

template< typename T >
inline std::string _ebP_Hex(const T Number ) {
  std::stringstream Stream;
  
  Stream << "0x" << std::setfill ('0') << std::setw(sizeof(T)*2) << std::hex << Number ;
  
  return Stream.str();
  
}

std::string _ebP_StringField(const std::string &String, const long Field, const std::string &Delimiter) {
   long CurrentField = 0 ;
   std::istringstream Stream(String);
   std::string Token;
   
   while (std::getline(Stream, Token, Delimiter.c_str()[0])) {
         
         CurrentField = CurrentField + 1 ;
         
         if (CurrentField == Field) {
            
            break ;
            
         }
         
   }
   
   return Token ;
   
}

long _ebP_CountString(const std::string &String, const std::string &Delimiter) {
    if (Delimiter.length() == 0) return 0;
    int Count = 0;
    for (size_t offset = String.find(Delimiter); offset != std::string::npos; offset = String.find(Delimiter, offset + Delimiter.length())) {
        Count++;
    }
    
    return Count;
    
}

std::string _ebP_StrTime(const std::string &Format, char * Time) {
   char StrTime[31] ;
   struct tm * TimeObject ;
   TimeObject = reinterpret_cast <tm *> (Time) ;
   
   strftime(StrTime, 30, Format.c_str(), TimeObject) ;
   
   return std::string(StrTime) ;
   
}

signed char _ebP_IsANumber(const std::string &String){
   const char * CString = String.c_str() ;
   
   while (*CString) {
      
      if (isdigit(*CString++) == 0) return false ;
      
   }
   
    return true ;
    
}

// --------------------------------------------------------------------------------
//
//                      EpicBasic Process Library
//
// --------------------------------------------------------------------------------

std::string _ebP_GetEnvironmentVariable(const std::string &Variable) {
   char * Result ;
   
   Result = getenv(Variable.c_str()) ;
   
   if (Result == NULL) {
      
      return std::string("") ;
      
   } else {
      
      return std::string(Result) ;
      
   }
   
}

int _ebP_SetEnvironmentVariable(const std::string &Variable, const std::string &Value) {
   
   return setenv(Variable.c_str(), Value.c_str(), 1) ;
   
}


int _ebP_RunProgram(const std::string &Program) {
   int Status ;
   pid_t PID ;

   PID = fork() ;
   
   if (PID == 0) {
      
      execl("/bin/sh", "/bin/sh", "-c", Program.c_str(), NULL) ;
      _exit(EXIT_FAILURE);
      
   } else if (PID < 0) {
      
      Status = -1;
      
   } else if (waitpid(PID, &Status, 0) != PID) {
      
      Status = -1 ;
      
   }
   if (Status != -1) {
      
      if (WIFEXITED(Status)) {
         
         return WEXITSTATUS(Status) ;
         
      } else {
         
         return -1 ;
         
      }

   } else {
      
      return Status ;
      
   }
   
}

struct _ebI_Program {
   int STDOUT = 0 ;
   int STDIN = 0 ;
   pid_t PID ;
   int Status ;
   int Exited = false ;
} ;

#define _ebP_ProgramOutput(Program) reinterpret_cast<_ebI_Program*>(Program)->STDOUT
#define _ebP_ProgramInput(Program) reinterpret_cast<_ebI_Program*>(Program)->STDIN

char * _ebP_OpenProgram(const std::string &Program) {
   int STDOUT[2];
   int STDIN[2];
   pid_t PID ;
   
   _ebI_Program * ProgramObject = new _ebI_Program ;
   
   if (pipe(STDIN) == -1) {
      
      return 0 ;
      
   }
   
   if (pipe(STDOUT) == -1) {
      
      return 0 ;
      
   }
   
   PID = fork();
   
   if (PID == -1) {
      
      return 0 ;
      
   } else if (PID == 0) {
      
      while ((dup2(STDOUT[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
      while ((dup2(STDIN[1], STDIN_FILENO) == -1) && (errno == EINTR)) {}
      
      close(STDIN[1]);
      close(STDIN[0]);
      close(STDOUT[1]);
      close(STDOUT[0]);
      execl("/bin/sh", "/bin/sh", "-c", Program.c_str(), NULL) ;
      _exit(1);
      
   }
   
   close(STDIN[1]);
   close(STDOUT[1]);
   
   ProgramObject->STDIN = STDIN[0] ;
   ProgramObject->STDOUT = STDOUT[0] ;
   ProgramObject->PID = PID ;
   ProgramObject->Exited = false ;
   
   return reinterpret_cast<char*>(ProgramObject) ;
   
}

char * _ebP_ReadProgram(const std::string &Program) {
   int STDOUT[2];
   pid_t PID ;
   
   _ebI_Program * ProgramObject = new _ebI_Program ;
   
   if (pipe(STDOUT) == -1) {
      
      return 0 ;
      
   }

   PID = fork();
   
   if (PID == -1) {
      
      return 0 ;
      
   } else if (PID == 0) {
      
      while ((dup2(STDOUT[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
      
      close(STDOUT[1]);
      close(STDOUT[0]);
      execl("/bin/sh", "/bin/sh", "-c", Program.c_str(), NULL) ;
      _exit(1);
      
   }
   
   close(STDOUT[1]);
   
   ProgramObject->STDOUT = STDOUT[0] ;
   ProgramObject->PID = PID ;
   
   return reinterpret_cast<char*>(ProgramObject) ;
   
}


int _ebP_CloseProgram(char * Program) {
   int Status ;
   
   close(reinterpret_cast<_ebI_Program*>(Program)->STDIN) ;
   close(reinterpret_cast<_ebI_Program*>(Program)->STDOUT) ;
   
   if (reinterpret_cast<_ebI_Program*>(Program)->Exited == true) {
      
      Status = reinterpret_cast<_ebI_Program*>(Program)->Status ;
      
   } else {
      
      waitpid(reinterpret_cast<_ebI_Program*>(Program)->PID, &Status, WUNTRACED);
      
   }
   
   delete reinterpret_cast<_ebI_Program*>(Program) ;
   
   return WEXITSTATUS(Status) ;
   
}


int _ebP_ProgramRunning(char * Program) {
   int Status ;
   
   if (reinterpret_cast<_ebI_Program*>(Program)->Exited == true) {
      
      return false ;
      
   }
   
   waitpid(reinterpret_cast<_ebI_Program*>(Program)->PID, &Status, WNOHANG | WNOWAIT);
   
   if (WIFEXITED(Status)) {
      
      reinterpret_cast<_ebI_Program*>(Program)->Status = Status ;
      reinterpret_cast<_ebI_Program*>(Program)->Exited = true ;
      
      return false ;
      
   }
   
   return true ;
   
}


int _ebI_argc ;
char ** _ebI_argv ;

int _ebP_CountProgramParameters() {
   
   return _ebI_argc ;
   
}

std::string _ebP_ProgramParameter(const int &Index) {
   std::string Parameter ;
   
   if (Index < _ebI_argc) {
      
      Parameter = _ebI_argv[Index] ;
      
      return  Parameter ;
      
   } else {
      
      return "" ;
      
   }
   
}


const auto& _ebP_Fork = fork ;


#define _ebD_LineReference1 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:_EBMain():1"
#define _ebD_LineReference2 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:InitializeGlobals():8"
#define _ebD_LineReference3 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:52"
#define _ebD_LineReference4 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:67"
#define _ebD_LineReference5 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:68"
#define _ebD_LineReference6 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:69"
#define _ebD_LineReference7 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:70"
#define _ebD_LineReference8 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:71"
#define _ebD_LineReference9 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:72"
#define _ebD_LineReference10 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:74"
#define _ebD_LineReference11 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:82"
#define _ebD_LineReference12 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:83"
#define _ebD_LineReference13 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:85"
#define _ebD_LineReference14 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:87"
#define _ebD_LineReference15 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:89"
#define _ebD_LineReference16 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:91"
#define _ebD_LineReference17 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:93"
#define _ebD_LineReference18 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:97"
#define _ebD_LineReference19 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:99"
#define _ebD_LineReference20 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:101"
#define _ebD_LineReference21 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:103"
#define _ebD_LineReference22 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:105"
#define _ebD_LineReference23 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:106"
#define _ebD_LineReference24 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:108"
#define _ebD_LineReference25 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:110"
#define _ebD_LineReference26 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:112"
#define _ebD_LineReference27 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:114"
#define _ebD_LineReference28 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:116"
#define _ebD_LineReference29 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:120"
#define _ebD_LineReference30 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:122"
#define _ebD_LineReference31 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:124"
#define _ebD_LineReference32 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:126"
#define _ebD_LineReference33 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:128"
#define _ebD_LineReference34 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:130"
#define _ebD_LineReference35 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:132"
#define _ebD_LineReference36 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:134"
#define _ebD_LineReference37 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:144"
#define _ebD_LineReference38 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:154"
#define _ebD_LineReference39 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:155"
#define _ebD_LineReference40 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:157"
#define _ebD_LineReference41 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:158"
#define _ebD_LineReference42 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:159"
#define _ebD_LineReference43 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:161"
#define _ebD_LineReference44 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:162"
#define _ebD_LineReference45 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:167"
#define _ebD_LineReference46 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:182"
#define _ebD_LineReference47 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:210"
#define _ebD_LineReference48 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:219"
#define _ebD_LineReference49 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Global.eb:230"
#define _ebD_LineReference50 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EBTokens.eb:264"
#define _ebD_LineReference51 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EBTokens.eb:266"
#define _ebD_LineReference52 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EBTokens.eb:268"
#define _ebD_LineReference53 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EBTokens.eb:269"
#define _ebD_LineReference54 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EBTokens.eb:271"
#define _ebD_LineReference55 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EBTokens.eb:273"
#define _ebD_LineReference56 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:CheckStructure():8"
#define _ebD_LineReference57 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:10"
#define _ebD_LineReference58 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:12"
#define _ebD_LineReference59 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:14"
#define _ebD_LineReference60 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:15"
#define _ebD_LineReference61 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:17"
#define _ebD_LineReference62 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:19"
#define _ebD_LineReference63 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:21"
#define _ebD_LineReference64 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:23"
#define _ebD_LineReference65 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:25"
#define _ebD_LineReference66 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:27"
#define _ebD_LineReference67 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:28"
#define _ebD_LineReference68 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:30"
#define _ebD_LineReference69 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:31"
#define _ebD_LineReference70 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:33"
#define _ebD_LineReference71 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:35"
#define _ebD_LineReference72 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:37"
#define _ebD_LineReference73 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:39"
#define _ebD_LineReference74 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:41"
#define _ebD_LineReference75 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:43"
#define _ebD_LineReference76 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:44"
#define _ebD_LineReference77 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:46"
#define _ebD_LineReference78 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:48"
#define _ebD_LineReference79 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:50"
#define _ebD_LineReference80 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:52"
#define _ebD_LineReference81 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:54"
#define _ebD_LineReference82 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:56"
#define _ebD_LineReference83 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:57"
#define _ebD_LineReference84 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:59"
#define _ebD_LineReference85 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:61"
#define _ebD_LineReference86 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:63"
#define _ebD_LineReference87 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:65"
#define _ebD_LineReference88 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:67"
#define _ebD_LineReference89 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:69"
#define _ebD_LineReference90 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:70"
#define _ebD_LineReference91 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:72"
#define _ebD_LineReference92 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:74"
#define _ebD_LineReference93 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:76"
#define _ebD_LineReference94 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:78"
#define _ebD_LineReference95 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:80"
#define _ebD_LineReference96 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:82"
#define _ebD_LineReference97 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:84"
#define _ebD_LineReference98 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:86"
#define _ebD_LineReference99 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:88"
#define _ebD_LineReference100 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Structures.eb:ResolveStructure():93"
#define _ebD_LineReference101 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:AddGlobalVariable():8"
#define _ebD_LineReference102 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:10"
#define _ebD_LineReference103 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:12"
#define _ebD_LineReference104 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:14"
#define _ebD_LineReference105 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:15"
#define _ebD_LineReference106 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:17"
#define _ebD_LineReference107 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:18"
#define _ebD_LineReference108 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:20"
#define _ebD_LineReference109 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:22"
#define _ebD_LineReference110 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:24"
#define _ebD_LineReference111 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:26"
#define _ebD_LineReference112 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:27"
#define _ebD_LineReference113 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:28"
#define _ebD_LineReference114 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:29"
#define _ebD_LineReference115 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:30"
#define _ebD_LineReference116 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:CreateConstant():35"
#define _ebD_LineReference117 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:37"
#define _ebD_LineReference118 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:39"
#define _ebD_LineReference119 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:40"
#define _ebD_LineReference120 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:42"
#define _ebD_LineReference121 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:44"
#define _ebD_LineReference122 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:45"
#define _ebD_LineReference123 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:47"
#define _ebD_LineReference124 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:49"
#define _ebD_LineReference125 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:51"
#define _ebD_LineReference126 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:53"
#define _ebD_LineReference127 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:55"
#define _ebD_LineReference128 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:56"
#define _ebD_LineReference129 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:57"
#define _ebD_LineReference130 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:58"
#define _ebD_LineReference131 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:59"
#define _ebD_LineReference132 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GlobalData.eb:60"
#define _ebD_LineReference133 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:QueueProcedure():8"
#define _ebD_LineReference134 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:10"
#define _ebD_LineReference135 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:ProcedureDeclared():15"
#define _ebD_LineReference136 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:17"
#define _ebD_LineReference137 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:19"
#define _ebD_LineReference138 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:21"
#define _ebD_LineReference139 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:23"
#define _ebD_LineReference140 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:25"
#define _ebD_LineReference141 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:26"
#define _ebD_LineReference142 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:27"
#define _ebD_LineReference143 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:29"
#define _ebD_LineReference144 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:ProcedureReturnType():34"
#define _ebD_LineReference145 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:36"
#define _ebD_LineReference146 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:38"
#define _ebD_LineReference147 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:39"
#define _ebD_LineReference148 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:41"
#define _ebD_LineReference149 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:43"
#define _ebD_LineReference150 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:45"
#define _ebD_LineReference151 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:47"
#define _ebD_LineReference152 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:48"
#define _ebD_LineReference153 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:49"
#define _ebD_LineReference154 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:51"
#define _ebD_LineReference155 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:ProcedureParameterCount():56"
#define _ebD_LineReference156 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:58"
#define _ebD_LineReference157 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:60"
#define _ebD_LineReference158 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:61"
#define _ebD_LineReference159 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:62"
#define _ebD_LineReference160 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:63"
#define _ebD_LineReference161 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:65"
#define _ebD_LineReference162 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:ProcedureParameterType():70"
#define _ebD_LineReference163 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:72"
#define _ebD_LineReference164 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:74"
#define _ebD_LineReference165 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:75"
#define _ebD_LineReference166 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:76"
#define _ebD_LineReference167 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:77"
#define _ebD_LineReference168 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:79"
#define _ebD_LineReference169 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:81"
#define _ebD_LineReference170 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:CreateProcedure():86"
#define _ebD_LineReference171 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:88"
#define _ebD_LineReference172 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:91"
#define _ebD_LineReference173 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:92"
#define _ebD_LineReference174 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:94"
#define _ebD_LineReference175 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:96"
#define _ebD_LineReference176 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:97"
#define _ebD_LineReference177 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:99"
#define _ebD_LineReference178 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:101"
#define _ebD_LineReference179 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:103"
#define _ebD_LineReference180 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:105"
#define _ebD_LineReference181 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:106"
#define _ebD_LineReference182 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:108"
#define _ebD_LineReference183 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:109"
#define _ebD_LineReference184 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:110"
#define _ebD_LineReference185 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:111"
#define _ebD_LineReference186 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:112"
#define _ebD_LineReference187 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:113"
#define _ebD_LineReference188 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:114"
#define _ebD_LineReference189 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:115"
#define _ebD_LineReference190 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:116"
#define _ebD_LineReference191 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:AddProcedureParameter():121"
#define _ebD_LineReference192 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:123"
#define _ebD_LineReference193 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:125"
#define _ebD_LineReference194 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:127"
#define _ebD_LineReference195 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:128"
#define _ebD_LineReference196 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:130"
#define _ebD_LineReference197 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:132"
#define _ebD_LineReference198 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:134"
#define _ebD_LineReference199 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:135"
#define _ebD_LineReference200 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:AddProcedureVariable():140"
#define _ebD_LineReference201 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:142"
#define _ebD_LineReference202 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:144"
#define _ebD_LineReference203 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:146"
#define _ebD_LineReference204 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:148"
#define _ebD_LineReference205 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:150"
#define _ebD_LineReference206 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:151"
#define _ebD_LineReference207 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:153"
#define _ebD_LineReference208 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:154"
#define _ebD_LineReference209 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:156"
#define _ebD_LineReference210 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:158"
#define _ebD_LineReference211 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:160"
#define _ebD_LineReference212 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:162"
#define _ebD_LineReference213 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:164"
#define _ebD_LineReference214 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:165"
#define _ebD_LineReference215 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:167"
#define _ebD_LineReference216 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:168"
#define _ebD_LineReference217 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:170"
#define _ebD_LineReference218 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:172"
#define _ebD_LineReference219 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:174"
#define _ebD_LineReference220 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:176"
#define _ebD_LineReference221 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:178"
#define _ebD_LineReference222 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:180"
#define _ebD_LineReference223 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:182"
#define _ebD_LineReference224 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:184"
#define _ebD_LineReference225 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:185"
#define _ebD_LineReference226 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:187"
#define _ebD_LineReference227 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:188"
#define _ebD_LineReference228 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:190"
#define _ebD_LineReference229 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:192"
#define _ebD_LineReference230 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:194"
#define _ebD_LineReference231 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:196"
#define _ebD_LineReference232 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:198"
#define _ebD_LineReference233 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:199"
#define _ebD_LineReference234 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:201"
#define _ebD_LineReference235 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:202"
#define _ebD_LineReference236 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:204"
#define _ebD_LineReference237 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:206"
#define _ebD_LineReference238 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:208"
#define _ebD_LineReference239 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:210"
#define _ebD_LineReference240 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:212"
#define _ebD_LineReference241 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:214"
#define _ebD_LineReference242 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:216"
#define _ebD_LineReference243 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:218"
#define _ebD_LineReference244 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:220"
#define _ebD_LineReference245 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:221"
#define _ebD_LineReference246 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:223"
#define _ebD_LineReference247 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:224"
#define _ebD_LineReference248 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:226"
#define _ebD_LineReference249 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:228"
#define _ebD_LineReference250 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:230"
#define _ebD_LineReference251 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:232"
#define _ebD_LineReference252 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:234"
#define _ebD_LineReference253 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:235"
#define _ebD_LineReference254 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:237"
#define _ebD_LineReference255 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:238"
#define _ebD_LineReference256 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:240"
#define _ebD_LineReference257 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:242"
#define _ebD_LineReference258 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:244"
#define _ebD_LineReference259 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:246"
#define _ebD_LineReference260 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:248"
#define _ebD_LineReference261 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:249"
#define _ebD_LineReference262 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:250"
#define _ebD_LineReference263 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:251"
#define _ebD_LineReference264 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:252"
#define _ebD_LineReference265 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:VariableType():257"
#define _ebD_LineReference266 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:259"
#define _ebD_LineReference267 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:261"
#define _ebD_LineReference268 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:263"
#define _ebD_LineReference269 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:265"
#define _ebD_LineReference270 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:267"
#define _ebD_LineReference271 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:269"
#define _ebD_LineReference272 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:271"
#define _ebD_LineReference273 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:273"
#define _ebD_LineReference274 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:275"
#define _ebD_LineReference275 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:277"
#define _ebD_LineReference276 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:279"
#define _ebD_LineReference277 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:281"
#define _ebD_LineReference278 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:283"
#define _ebD_LineReference279 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:285"
#define _ebD_LineReference280 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:287"
#define _ebD_LineReference281 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:AddProcedureEmit():292"
#define _ebD_LineReference282 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:294"
#define _ebD_LineReference283 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:296"
#define _ebD_LineReference284 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:298"
#define _ebD_LineReference285 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:300"
#define _ebD_LineReference286 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:301"
#define _ebD_LineReference287 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:302"
#define _ebD_LineReference288 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:AddDeBlockedProcedureEmit():307"
#define _ebD_LineReference289 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:309"
#define _ebD_LineReference290 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:310"
#define _ebD_LineReference291 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:311"
#define _ebD_LineReference292 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:IncreaseBlockLevel():316"
#define _ebD_LineReference293 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:318"
#define _ebD_LineReference294 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:320"
#define _ebD_LineReference295 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:322"
#define _ebD_LineReference296 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:323"
#define _ebD_LineReference297 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:DecreaseBlockLevel():328"
#define _ebD_LineReference298 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:330"
#define _ebD_LineReference299 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:332"
#define _ebD_LineReference300 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:BlockLevelType():337"
#define _ebD_LineReference301 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:339"
#define _ebD_LineReference302 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:341"
#define _ebD_LineReference303 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:BlockLevelSpecialFlag():346"
#define _ebD_LineReference304 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:348"
#define _ebD_LineReference305 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:350"
#define _ebD_LineReference306 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:SetBlockLevelSpecialFlag():355"
#define _ebD_LineReference307 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:357"
#define _ebD_LineReference308 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:359"
#define _ebD_LineReference309 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:SetBlockLevelExpressionType():364"
#define _ebD_LineReference310 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:366"
#define _ebD_LineReference311 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:368"
#define _ebD_LineReference312 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:BlockLevelExpressionType():372"
#define _ebD_LineReference313 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:374"
#define _ebD_LineReference314 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:376"
#define _ebD_LineReference315 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:SetBlockLevelExpression():380"
#define _ebD_LineReference316 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:382"
#define _ebD_LineReference317 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:384"
#define _ebD_LineReference318 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:BlockLevelExpression():389"
#define _ebD_LineReference319 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:391"
#define _ebD_LineReference320 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:393"
#define _ebD_LineReference321 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:CheckBlockLevel():398"
#define _ebD_LineReference322 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:400"
#define _ebD_LineReference323 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:402"
#define _ebD_LineReference324 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:404"
#define _ebD_LineReference325 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:406"
#define _ebD_LineReference326 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:407"
#define _ebD_LineReference327 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:409"
#define _ebD_LineReference328 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:411"
#define _ebD_LineReference329 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:413"
#define _ebD_LineReference330 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:415"
#define _ebD_LineReference331 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:416"
#define _ebD_LineReference332 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:418"
#define _ebD_LineReference333 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:420"
#define _ebD_LineReference334 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:422"
#define _ebD_LineReference335 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:424"
#define _ebD_LineReference336 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:425"
#define _ebD_LineReference337 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:427"
#define _ebD_LineReference338 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:429"
#define _ebD_LineReference339 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:431"
#define _ebD_LineReference340 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:433"
#define _ebD_LineReference341 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:434"
#define _ebD_LineReference342 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:436"
#define _ebD_LineReference343 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:438"
#define _ebD_LineReference344 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProcedureData.eb:440"
#define _ebD_LineReference345 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:CheckIf():8"
#define _ebD_LineReference346 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:10"
#define _ebD_LineReference347 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:12"
#define _ebD_LineReference348 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:14"
#define _ebD_LineReference349 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:16"
#define _ebD_LineReference350 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:18"
#define _ebD_LineReference351 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:20"
#define _ebD_LineReference352 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:22"
#define _ebD_LineReference353 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:23"
#define _ebD_LineReference354 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:25"
#define _ebD_LineReference355 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:27"
#define _ebD_LineReference356 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:29"
#define _ebD_LineReference357 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:31"
#define _ebD_LineReference358 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:32"
#define _ebD_LineReference359 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:34"
#define _ebD_LineReference360 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:36"
#define _ebD_LineReference361 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:38"
#define _ebD_LineReference362 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:42"
#define _ebD_LineReference363 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:44"
#define _ebD_LineReference364 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:46"
#define _ebD_LineReference365 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:48"
#define _ebD_LineReference366 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:50"
#define _ebD_LineReference367 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:52"
#define _ebD_LineReference368 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:54"
#define _ebD_LineReference369 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:56"
#define _ebD_LineReference370 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:60"
#define _ebD_LineReference371 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:62"
#define _ebD_LineReference372 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:64"
#define _ebD_LineReference373 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:66"
#define _ebD_LineReference374 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:68"
#define _ebD_LineReference375 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:70"
#define _ebD_LineReference376 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:74"
#define _ebD_LineReference377 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:76"
#define _ebD_LineReference378 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:80"
#define _ebD_LineReference379 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:82"
#define _ebD_LineReference380 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:84"
#define _ebD_LineReference381 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:88"
#define _ebD_LineReference382 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:90"
#define _ebD_LineReference383 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:92"
#define _ebD_LineReference384 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:96"
#define _ebD_LineReference385 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:98"
#define _ebD_LineReference386 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:100"
#define _ebD_LineReference387 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:104"
#define _ebD_LineReference388 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:106"
#define _ebD_LineReference389 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:110"
#define _ebD_LineReference390 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:112"
#define _ebD_LineReference391 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:116"
#define _ebD_LineReference392 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:117"
#define _ebD_LineReference393 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:119"
#define _ebD_LineReference394 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:121"
#define _ebD_LineReference395 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:123"
#define _ebD_LineReference396 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:125"
#define _ebD_LineReference397 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:127"
#define _ebD_LineReference398 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:131"
#define _ebD_LineReference399 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:133"
#define _ebD_LineReference400 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:135"
#define _ebD_LineReference401 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:137"
#define _ebD_LineReference402 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:139"
#define _ebD_LineReference403 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:141"
#define _ebD_LineReference404 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:143"
#define _ebD_LineReference405 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:145"
#define _ebD_LineReference406 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:147"
#define _ebD_LineReference407 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:149"
#define _ebD_LineReference408 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:151"
#define _ebD_LineReference409 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:153"
#define _ebD_LineReference410 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:155"
#define _ebD_LineReference411 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:CheckElse():160"
#define _ebD_LineReference412 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:162"
#define _ebD_LineReference413 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:164"
#define _ebD_LineReference414 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:165"
#define _ebD_LineReference415 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:167"
#define _ebD_LineReference416 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:169"
#define _ebD_LineReference417 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:171"
#define _ebD_LineReference418 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:173"
#define _ebD_LineReference419 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:175"
#define _ebD_LineReference420 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:176"
#define _ebD_LineReference421 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:178"
#define _ebD_LineReference422 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:180"
#define _ebD_LineReference423 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:182"
#define _ebD_LineReference424 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:184"
#define _ebD_LineReference425 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:186"
#define _ebD_LineReference426 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:188"
#define _ebD_LineReference427 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:190"
#define _ebD_LineReference428 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:CheckEndIf():195"
#define _ebD_LineReference429 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:197"
#define _ebD_LineReference430 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:199"
#define _ebD_LineReference431 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:200"
#define _ebD_LineReference432 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:202"
#define _ebD_LineReference433 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:204"
#define _ebD_LineReference434 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:206"
#define _ebD_LineReference435 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:208"
#define _ebD_LineReference436 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:210"
#define _ebD_LineReference437 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:212"
#define _ebD_LineReference438 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:214"
#define _ebD_LineReference439 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:216"
#define _ebD_LineReference440 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:218"
#define _ebD_LineReference441 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:220"
#define _ebD_LineReference442 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:222"
#define _ebD_LineReference443 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:224"
#define _ebD_LineReference444 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:CheckSelect():229"
#define _ebD_LineReference445 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:230"
#define _ebD_LineReference446 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:231"
#define _ebD_LineReference447 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:233"
#define _ebD_LineReference448 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:235"
#define _ebD_LineReference449 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:237"
#define _ebD_LineReference450 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:239"
#define _ebD_LineReference451 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:243"
#define _ebD_LineReference452 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:245"
#define _ebD_LineReference453 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:249"
#define _ebD_LineReference454 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:251"
#define _ebD_LineReference455 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:253"
#define _ebD_LineReference456 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:255"
#define _ebD_LineReference457 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:257"
#define _ebD_LineReference458 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:259"
#define _ebD_LineReference459 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:263"
#define _ebD_LineReference460 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:265"
#define _ebD_LineReference461 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:269"
#define _ebD_LineReference462 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:271"
#define _ebD_LineReference463 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:273"
#define _ebD_LineReference464 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:277"
#define _ebD_LineReference465 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:279"
#define _ebD_LineReference466 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:281"
#define _ebD_LineReference467 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:285"
#define _ebD_LineReference468 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:287"
#define _ebD_LineReference469 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:289"
#define _ebD_LineReference470 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:293"
#define _ebD_LineReference471 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:295"
#define _ebD_LineReference472 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:297"
#define _ebD_LineReference473 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:301"
#define _ebD_LineReference474 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:303"
#define _ebD_LineReference475 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:305"
#define _ebD_LineReference476 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:309"
#define _ebD_LineReference477 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:310"
#define _ebD_LineReference478 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:312"
#define _ebD_LineReference479 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:314"
#define _ebD_LineReference480 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:316"
#define _ebD_LineReference481 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:318"
#define _ebD_LineReference482 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:320"
#define _ebD_LineReference483 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:322"
#define _ebD_LineReference484 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:324"
#define _ebD_LineReference485 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:328"
#define _ebD_LineReference486 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:330"
#define _ebD_LineReference487 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:332"
#define _ebD_LineReference488 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:334"
#define _ebD_LineReference489 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:336"
#define _ebD_LineReference490 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:338"
#define _ebD_LineReference491 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:340"
#define _ebD_LineReference492 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:342"
#define _ebD_LineReference493 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:343"
#define _ebD_LineReference494 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:345"
#define _ebD_LineReference495 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:347"
#define _ebD_LineReference496 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:349"
#define _ebD_LineReference497 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:351"
#define _ebD_LineReference498 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:353"
#define _ebD_LineReference499 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:355"
#define _ebD_LineReference500 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:359"
#define _ebD_LineReference501 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:361"
#define _ebD_LineReference502 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:363"
#define _ebD_LineReference503 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:365"
#define _ebD_LineReference504 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:367"
#define _ebD_LineReference505 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:369"
#define _ebD_LineReference506 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:371"
#define _ebD_LineReference507 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:373"
#define _ebD_LineReference508 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:375"
#define _ebD_LineReference509 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:377"
#define _ebD_LineReference510 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:379"
#define _ebD_LineReference511 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:381"
#define _ebD_LineReference512 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:CheckCase():386"
#define _ebD_LineReference513 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:388"
#define _ebD_LineReference514 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:390"
#define _ebD_LineReference515 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:391"
#define _ebD_LineReference516 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:393"
#define _ebD_LineReference517 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:395"
#define _ebD_LineReference518 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:397"
#define _ebD_LineReference519 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:399"
#define _ebD_LineReference520 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:400"
#define _ebD_LineReference521 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:402"
#define _ebD_LineReference522 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:404"
#define _ebD_LineReference523 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:406"
#define _ebD_LineReference524 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:408"
#define _ebD_LineReference525 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:410"
#define _ebD_LineReference526 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:412"
#define _ebD_LineReference527 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:414"
#define _ebD_LineReference528 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:418"
#define _ebD_LineReference529 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:420"
#define _ebD_LineReference530 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:422"
#define _ebD_LineReference531 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:424"
#define _ebD_LineReference532 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:426"
#define _ebD_LineReference533 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:430"
#define _ebD_LineReference534 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:432"
#define _ebD_LineReference535 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:434"
#define _ebD_LineReference536 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:436"
#define _ebD_LineReference537 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:438"
#define _ebD_LineReference538 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:440"
#define _ebD_LineReference539 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:442"
#define _ebD_LineReference540 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:444"
#define _ebD_LineReference541 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:446"
#define _ebD_LineReference542 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:448"
#define _ebD_LineReference543 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:CheckDefault():453"
#define _ebD_LineReference544 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:455"
#define _ebD_LineReference545 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:457"
#define _ebD_LineReference546 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:458"
#define _ebD_LineReference547 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:460"
#define _ebD_LineReference548 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:462"
#define _ebD_LineReference549 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:464"
#define _ebD_LineReference550 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:466"
#define _ebD_LineReference551 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:468"
#define _ebD_LineReference552 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:469"
#define _ebD_LineReference553 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:471"
#define _ebD_LineReference554 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:473"
#define _ebD_LineReference555 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:475"
#define _ebD_LineReference556 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:477"
#define _ebD_LineReference557 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:479"
#define _ebD_LineReference558 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:481"
#define _ebD_LineReference559 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:483"
#define _ebD_LineReference560 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:CheckEndSelect():487"
#define _ebD_LineReference561 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:489"
#define _ebD_LineReference562 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:491"
#define _ebD_LineReference563 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:492"
#define _ebD_LineReference564 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:494"
#define _ebD_LineReference565 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:496"
#define _ebD_LineReference566 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:498"
#define _ebD_LineReference567 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:500"
#define _ebD_LineReference568 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:502"
#define _ebD_LineReference569 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:504"
#define _ebD_LineReference570 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:506"
#define _ebD_LineReference571 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:508"
#define _ebD_LineReference572 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:CheckXInclude():8"
#define _ebD_LineReference573 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:10"
#define _ebD_LineReference574 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:12"
#define _ebD_LineReference575 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:14"
#define _ebD_LineReference576 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:15"
#define _ebD_LineReference577 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:17"
#define _ebD_LineReference578 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:19"
#define _ebD_LineReference579 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:20"
#define _ebD_LineReference580 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:21"
#define _ebD_LineReference581 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:22"
#define _ebD_LineReference582 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:23"
#define _ebD_LineReference583 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:25"
#define _ebD_LineReference584 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:27"
#define _ebD_LineReference585 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:29"
#define _ebD_LineReference586 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:31"
#define _ebD_LineReference587 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:33"
#define _ebD_LineReference588 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:35"
#define _ebD_LineReference589 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:36"
#define _ebD_LineReference590 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:38"
#define _ebD_LineReference591 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:40"
#define _ebD_LineReference592 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:41"
#define _ebD_LineReference593 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:45"
#define _ebD_LineReference594 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:46"
#define _ebD_LineReference595 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:48"
#define _ebD_LineReference596 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:50"
#define _ebD_LineReference597 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:51"
#define _ebD_LineReference598 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:52"
#define _ebD_LineReference599 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:53"
#define _ebD_LineReference600 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:54"
#define _ebD_LineReference601 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:56"
#define _ebD_LineReference602 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:58"
#define _ebD_LineReference603 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:60"
#define _ebD_LineReference604 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:62"
#define _ebD_LineReference605 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:63"
#define _ebD_LineReference606 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:64"
#define _ebD_LineReference607 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:65"
#define _ebD_LineReference608 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:66"
#define _ebD_LineReference609 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:68"
#define _ebD_LineReference610 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:70"
#define _ebD_LineReference611 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:74"
#define _ebD_LineReference612 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:75"
#define _ebD_LineReference613 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:77"
#define _ebD_LineReference614 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:79"
#define _ebD_LineReference615 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:CheckInclude():83"
#define _ebD_LineReference616 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:85"
#define _ebD_LineReference617 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:87"
#define _ebD_LineReference618 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:89"
#define _ebD_LineReference619 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:90"
#define _ebD_LineReference620 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:92"
#define _ebD_LineReference621 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:94"
#define _ebD_LineReference622 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:95"
#define _ebD_LineReference623 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:96"
#define _ebD_LineReference624 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:97"
#define _ebD_LineReference625 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:98"
#define _ebD_LineReference626 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:100"
#define _ebD_LineReference627 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:102"
#define _ebD_LineReference628 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:104"
#define _ebD_LineReference629 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:106"
#define _ebD_LineReference630 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:108"
#define _ebD_LineReference631 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:109"
#define _ebD_LineReference632 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:111"
#define _ebD_LineReference633 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:113"
#define _ebD_LineReference634 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:114"
#define _ebD_LineReference635 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:116"
#define _ebD_LineReference636 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:118"
#define _ebD_LineReference637 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:119"
#define _ebD_LineReference638 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:120"
#define _ebD_LineReference639 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:121"
#define _ebD_LineReference640 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:122"
#define _ebD_LineReference641 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:124"
#define _ebD_LineReference642 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:126"
#define _ebD_LineReference643 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:130"
#define _ebD_LineReference644 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:131"
#define _ebD_LineReference645 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:133"
#define _ebD_LineReference646 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Include.eb:135"
#define _ebD_LineReference647 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:CheckTInclude():8"
#define _ebD_LineReference648 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:10"
#define _ebD_LineReference649 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:12"
#define _ebD_LineReference650 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:14"
#define _ebD_LineReference651 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:15"
#define _ebD_LineReference652 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:17"
#define _ebD_LineReference653 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:19"
#define _ebD_LineReference654 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:21"
#define _ebD_LineReference655 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:23"
#define _ebD_LineReference656 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:25"
#define _ebD_LineReference657 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:CheckTLink():29"
#define _ebD_LineReference658 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:31"
#define _ebD_LineReference659 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:33"
#define _ebD_LineReference660 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:35"
#define _ebD_LineReference661 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:36"
#define _ebD_LineReference662 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:38"
#define _ebD_LineReference663 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:40"
#define _ebD_LineReference664 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:42"
#define _ebD_LineReference665 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:44"
#define _ebD_LineReference666 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:46"
#define _ebD_LineReference667 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:CheckTInitialize():50"
#define _ebD_LineReference668 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:52"
#define _ebD_LineReference669 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:54"
#define _ebD_LineReference670 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:56"
#define _ebD_LineReference671 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:57"
#define _ebD_LineReference672 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:59"
#define _ebD_LineReference673 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:61"
#define _ebD_LineReference674 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:63"
#define _ebD_LineReference675 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:65"
#define _ebD_LineReference676 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:67"
#define _ebD_LineReference677 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:CheckTConstant():71"
#define _ebD_LineReference678 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:73"
#define _ebD_LineReference679 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:75"
#define _ebD_LineReference680 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:77"
#define _ebD_LineReference681 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:78"
#define _ebD_LineReference682 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:80"
#define _ebD_LineReference683 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:82"
#define _ebD_LineReference684 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:84"
#define _ebD_LineReference685 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:86"
#define _ebD_LineReference686 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:88"
#define _ebD_LineReference687 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:90"
#define _ebD_LineReference688 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:91"
#define _ebD_LineReference689 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:93"
#define _ebD_LineReference690 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:95"
#define _ebD_LineReference691 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:97"
#define _ebD_LineReference692 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:99"
#define _ebD_LineReference693 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:101"
#define _ebD_LineReference694 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:103"
#define _ebD_LineReference695 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:104"
#define _ebD_LineReference696 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:106"
#define _ebD_LineReference697 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:108"
#define _ebD_LineReference698 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:110"
#define _ebD_LineReference699 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:112"
#define _ebD_LineReference700 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:114"
#define _ebD_LineReference701 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:115"
#define _ebD_LineReference702 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:117"
#define _ebD_LineReference703 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:119"
#define _ebD_LineReference704 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:121"
#define _ebD_LineReference705 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:123"
#define _ebD_LineReference706 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:CheckExternalProcedure():127"
#define _ebD_LineReference707 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:129"
#define _ebD_LineReference708 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:131"
#define _ebD_LineReference709 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:133"
#define _ebD_LineReference710 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:135"
#define _ebD_LineReference711 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:136"
#define _ebD_LineReference712 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:140"
#define _ebD_LineReference713 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:142"
#define _ebD_LineReference714 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:144"
#define _ebD_LineReference715 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:146"
#define _ebD_LineReference716 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:147"
#define _ebD_LineReference717 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:149"
#define _ebD_LineReference718 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:151"
#define _ebD_LineReference719 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:153"
#define _ebD_LineReference720 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:155"
#define _ebD_LineReference721 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:157"
#define _ebD_LineReference722 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:158"
#define _ebD_LineReference723 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:159"
#define _ebD_LineReference724 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:160"
#define _ebD_LineReference725 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:161"
#define _ebD_LineReference726 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:162"
#define _ebD_LineReference727 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:164"
#define _ebD_LineReference728 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:166"
#define _ebD_LineReference729 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:167"
#define _ebD_LineReference730 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:169"
#define _ebD_LineReference731 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:171"
#define _ebD_LineReference732 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:173"
#define _ebD_LineReference733 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:175"
#define _ebD_LineReference734 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:177"
#define _ebD_LineReference735 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:179"
#define _ebD_LineReference736 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:181"
#define _ebD_LineReference737 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:183"
#define _ebD_LineReference738 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:185"
#define _ebD_LineReference739 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:186"
#define _ebD_LineReference740 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:188"
#define _ebD_LineReference741 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:190"
#define _ebD_LineReference742 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:192"
#define _ebD_LineReference743 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:194"
#define _ebD_LineReference744 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:196"
#define _ebD_LineReference745 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:198"
#define _ebD_LineReference746 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:199"
#define _ebD_LineReference747 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:201"
#define _ebD_LineReference748 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:203"
#define _ebD_LineReference749 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:205"
#define _ebD_LineReference750 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:207"
#define _ebD_LineReference751 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:209"
#define _ebD_LineReference752 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:211"
#define _ebD_LineReference753 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:213"
#define _ebD_LineReference754 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:214"
#define _ebD_LineReference755 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:216"
#define _ebD_LineReference756 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:218"
#define _ebD_LineReference757 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:220"
#define _ebD_LineReference758 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:222"
#define _ebD_LineReference759 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:224"
#define _ebD_LineReference760 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:226"
#define _ebD_LineReference761 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Translation.eb:227"
#define _ebD_LineReference762 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:CheckFor():8"
#define _ebD_LineReference763 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:10"
#define _ebD_LineReference764 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:12"
#define _ebD_LineReference765 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:14"
#define _ebD_LineReference766 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:16"
#define _ebD_LineReference767 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:18"
#define _ebD_LineReference768 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:20"
#define _ebD_LineReference769 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:22"
#define _ebD_LineReference770 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:24"
#define _ebD_LineReference771 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:26"
#define _ebD_LineReference772 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:27"
#define _ebD_LineReference773 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:31"
#define _ebD_LineReference774 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:33"
#define _ebD_LineReference775 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:35"
#define _ebD_LineReference776 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:36"
#define _ebD_LineReference777 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:38"
#define _ebD_LineReference778 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:40"
#define _ebD_LineReference779 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:44"
#define _ebD_LineReference780 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:46"
#define _ebD_LineReference781 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:48"
#define _ebD_LineReference782 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:50"
#define _ebD_LineReference783 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:52"
#define _ebD_LineReference784 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:54"
#define _ebD_LineReference785 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:55"
#define _ebD_LineReference786 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:57"
#define _ebD_LineReference787 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:59"
#define _ebD_LineReference788 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:61"
#define _ebD_LineReference789 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:63"
#define _ebD_LineReference790 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:64"
#define _ebD_LineReference791 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:66"
#define _ebD_LineReference792 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:70"
#define _ebD_LineReference793 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:72"
#define _ebD_LineReference794 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:74"
#define _ebD_LineReference795 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:76"
#define _ebD_LineReference796 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:80"
#define _ebD_LineReference797 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:81"
#define _ebD_LineReference798 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:83"
#define _ebD_LineReference799 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:85"
#define _ebD_LineReference800 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:87"
#define _ebD_LineReference801 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:89"
#define _ebD_LineReference802 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:90"
#define _ebD_LineReference803 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:92"
#define _ebD_LineReference804 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:94"
#define _ebD_LineReference805 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:96"
#define _ebD_LineReference806 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:98"
#define _ebD_LineReference807 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:100"
#define _ebD_LineReference808 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:102"
#define _ebD_LineReference809 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:104"
#define _ebD_LineReference810 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:106"
#define _ebD_LineReference811 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:108"
#define _ebD_LineReference812 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:110"
#define _ebD_LineReference813 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:114"
#define _ebD_LineReference814 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:116"
#define _ebD_LineReference815 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:117"
#define _ebD_LineReference816 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:119"
#define _ebD_LineReference817 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:121"
#define _ebD_LineReference818 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:123"
#define _ebD_LineReference819 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:125"
#define _ebD_LineReference820 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:127"
#define _ebD_LineReference821 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:129"
#define _ebD_LineReference822 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:131"
#define _ebD_LineReference823 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:133"
#define _ebD_LineReference824 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:135"
#define _ebD_LineReference825 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:137"
#define _ebD_LineReference826 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:139"
#define _ebD_LineReference827 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:CheckNext():143"
#define _ebD_LineReference828 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:145"
#define _ebD_LineReference829 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:147"
#define _ebD_LineReference830 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:148"
#define _ebD_LineReference831 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:150"
#define _ebD_LineReference832 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:152"
#define _ebD_LineReference833 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:154"
#define _ebD_LineReference834 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:156"
#define _ebD_LineReference835 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:158"
#define _ebD_LineReference836 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:160"
#define _ebD_LineReference837 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:162"
#define _ebD_LineReference838 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:164"
#define _ebD_LineReference839 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:166"
#define _ebD_LineReference840 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:168"
#define _ebD_LineReference841 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:170"
#define _ebD_LineReference842 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:172"
#define _ebD_LineReference843 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:CheckWhile():176"
#define _ebD_LineReference844 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:178"
#define _ebD_LineReference845 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:180"
#define _ebD_LineReference846 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:182"
#define _ebD_LineReference847 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:184"
#define _ebD_LineReference848 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:186"
#define _ebD_LineReference849 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:188"
#define _ebD_LineReference850 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:190"
#define _ebD_LineReference851 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:192"
#define _ebD_LineReference852 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:196"
#define _ebD_LineReference853 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:198"
#define _ebD_LineReference854 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:200"
#define _ebD_LineReference855 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:202"
#define _ebD_LineReference856 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:204"
#define _ebD_LineReference857 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:206"
#define _ebD_LineReference858 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:210"
#define _ebD_LineReference859 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:212"
#define _ebD_LineReference860 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:216"
#define _ebD_LineReference861 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:218"
#define _ebD_LineReference862 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:220"
#define _ebD_LineReference863 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:224"
#define _ebD_LineReference864 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:226"
#define _ebD_LineReference865 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:228"
#define _ebD_LineReference866 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:232"
#define _ebD_LineReference867 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:234"
#define _ebD_LineReference868 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:236"
#define _ebD_LineReference869 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:240"
#define _ebD_LineReference870 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:242"
#define _ebD_LineReference871 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:246"
#define _ebD_LineReference872 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:248"
#define _ebD_LineReference873 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:252"
#define _ebD_LineReference874 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:253"
#define _ebD_LineReference875 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:255"
#define _ebD_LineReference876 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:257"
#define _ebD_LineReference877 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:259"
#define _ebD_LineReference878 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:261"
#define _ebD_LineReference879 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:263"
#define _ebD_LineReference880 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:267"
#define _ebD_LineReference881 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:269"
#define _ebD_LineReference882 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:271"
#define _ebD_LineReference883 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:273"
#define _ebD_LineReference884 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:275"
#define _ebD_LineReference885 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:277"
#define _ebD_LineReference886 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:279"
#define _ebD_LineReference887 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:281"
#define _ebD_LineReference888 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:283"
#define _ebD_LineReference889 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:285"
#define _ebD_LineReference890 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:287"
#define _ebD_LineReference891 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:289"
#define _ebD_LineReference892 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:291"
#define _ebD_LineReference893 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:CheckWend():295"
#define _ebD_LineReference894 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:297"
#define _ebD_LineReference895 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:299"
#define _ebD_LineReference896 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:300"
#define _ebD_LineReference897 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:302"
#define _ebD_LineReference898 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:304"
#define _ebD_LineReference899 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:306"
#define _ebD_LineReference900 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:308"
#define _ebD_LineReference901 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:310"
#define _ebD_LineReference902 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:312"
#define _ebD_LineReference903 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:314"
#define _ebD_LineReference904 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:316"
#define _ebD_LineReference905 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:318"
#define _ebD_LineReference906 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:320"
#define _ebD_LineReference907 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:322"
#define _ebD_LineReference908 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Loops.eb:324"
#define _ebD_LineReference909 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:CheckImport():8"
#define _ebD_LineReference910 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:10"
#define _ebD_LineReference911 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:12"
#define _ebD_LineReference912 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:14"
#define _ebD_LineReference913 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:15"
#define _ebD_LineReference914 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:17"
#define _ebD_LineReference915 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:19"
#define _ebD_LineReference916 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:20"
#define _ebD_LineReference917 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:21"
#define _ebD_LineReference918 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:22"
#define _ebD_LineReference919 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:23"
#define _ebD_LineReference920 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:25"
#define _ebD_LineReference921 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:27"
#define _ebD_LineReference922 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:29"
#define _ebD_LineReference923 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:31"
#define _ebD_LineReference924 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:33"
#define _ebD_LineReference925 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:35"
#define _ebD_LineReference926 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:36"
#define _ebD_LineReference927 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:38"
#define _ebD_LineReference928 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:40"
#define _ebD_LineReference929 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:42"
#define _ebD_LineReference930 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:44"
#define _ebD_LineReference931 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:45"
#define _ebD_LineReference932 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:47"
#define _ebD_LineReference933 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:49"
#define _ebD_LineReference934 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:51"
#define _ebD_LineReference935 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:53"
#define _ebD_LineReference936 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:55"
#define _ebD_LineReference937 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:57"
#define _ebD_LineReference938 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:59"
#define _ebD_LineReference939 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:61"
#define _ebD_LineReference940 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:62"
#define _ebD_LineReference941 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:64"
#define _ebD_LineReference942 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:66"
#define _ebD_LineReference943 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:67"
#define _ebD_LineReference944 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:69"
#define _ebD_LineReference945 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:71"
#define _ebD_LineReference946 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:75"
#define _ebD_LineReference947 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:76"
#define _ebD_LineReference948 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:78"
#define _ebD_LineReference949 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Import.eb:80"
#define _ebD_LineReference950 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:List():15"
#define _ebD_LineReference951 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:17"
#define _ebD_LineReference952 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:18"
#define _ebD_LineReference953 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:19"
#define _ebD_LineReference954 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:21"
#define _ebD_LineReference955 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:23"
#define _ebD_LineReference956 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:25"
#define _ebD_LineReference957 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:27"
#define _ebD_LineReference958 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:31"
#define _ebD_LineReference959 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:33"
#define _ebD_LineReference960 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:37"
#define _ebD_LineReference961 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:39"
#define _ebD_LineReference962 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:43"
#define _ebD_LineReference963 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:45"
#define _ebD_LineReference964 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:49"
#define _ebD_LineReference965 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:51"
#define _ebD_LineReference966 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:55"
#define _ebD_LineReference967 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:57"
#define _ebD_LineReference968 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:61"
#define _ebD_LineReference969 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:63"
#define _ebD_LineReference970 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:65"
#define _ebD_LineReference971 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:67"
#define _ebD_LineReference972 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:69"
#define _ebD_LineReference973 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:71"
#define _ebD_LineReference974 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:NewList():76"
#define _ebD_LineReference975 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:79"
#define _ebD_LineReference976 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:82"
#define _ebD_LineReference977 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:83"
#define _ebD_LineReference978 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:84"
#define _ebD_LineReference979 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:85"
#define _ebD_LineReference980 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:87"
#define _ebD_LineReference981 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:CurrentListElement():93"
#define _ebD_LineReference982 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:95"
#define _ebD_LineReference983 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:96"
#define _ebD_LineReference984 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:98"
#define _ebD_LineReference985 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:AddListElement():104"
#define _ebD_LineReference986 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:106"
#define _ebD_LineReference987 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:108"
#define _ebD_LineReference988 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:110"
#define _ebD_LineReference989 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:111"
#define _ebD_LineReference990 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:112"
#define _ebD_LineReference991 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:113"
#define _ebD_LineReference992 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:114"
#define _ebD_LineReference993 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:118"
#define _ebD_LineReference994 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:119"
#define _ebD_LineReference995 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:120"
#define _ebD_LineReference996 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:121"
#define _ebD_LineReference997 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:122"
#define _ebD_LineReference998 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:123"
#define _ebD_LineReference999 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:125"
#define _ebD_LineReference1000 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:TruncateListElement():130"
#define _ebD_LineReference1001 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:132"
#define _ebD_LineReference1002 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:134"
#define _ebD_LineReference1003 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:135"
#define _ebD_LineReference1004 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:138"
#define _ebD_LineReference1005 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:139"
#define _ebD_LineReference1006 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:140"
#define _ebD_LineReference1007 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:144"
#define _ebD_LineReference1008 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:145"
#define _ebD_LineReference1009 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:146"
#define _ebD_LineReference1010 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:147"
#define _ebD_LineReference1011 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:148"
#define _ebD_LineReference1012 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:149"
#define _ebD_LineReference1013 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:151"
#define _ebD_LineReference1014 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:153"
#define _ebD_LineReference1015 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:NewMap():158"
#define _ebD_LineReference1016 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:161"
#define _ebD_LineReference1017 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:164"
#define _ebD_LineReference1018 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:165"
#define _ebD_LineReference1019 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:166"
#define _ebD_LineReference1020 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:167"
#define _ebD_LineReference1021 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:169"
#define _ebD_LineReference1022 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:SetMapElement():175"
#define _ebD_LineReference1023 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:177"
#define _ebD_LineReference1024 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:179"
#define _ebD_LineReference1025 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:181"
#define _ebD_LineReference1026 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:182"
#define _ebD_LineReference1027 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:183"
#define _ebD_LineReference1028 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:184"
#define _ebD_LineReference1029 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:185"
#define _ebD_LineReference1030 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:186"
#define _ebD_LineReference1031 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:190"
#define _ebD_LineReference1032 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:191"
#define _ebD_LineReference1033 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:192"
#define _ebD_LineReference1034 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:194"
#define _ebD_LineReference1035 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:196"
#define _ebD_LineReference1036 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:198"
#define _ebD_LineReference1037 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:200"
#define _ebD_LineReference1038 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:201"
#define _ebD_LineReference1039 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:203"
#define _ebD_LineReference1040 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:205"
#define _ebD_LineReference1041 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:207"
#define _ebD_LineReference1042 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:209"
#define _ebD_LineReference1043 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:210"
#define _ebD_LineReference1044 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:211"
#define _ebD_LineReference1045 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:212"
#define _ebD_LineReference1046 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:213"
#define _ebD_LineReference1047 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:217"
#define _ebD_LineReference1048 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:219"
#define _ebD_LineReference1049 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:221"
#define _ebD_LineReference1050 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:FindMapKey():227"
#define _ebD_LineReference1051 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:229"
#define _ebD_LineReference1052 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:231"
#define _ebD_LineReference1053 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:233"
#define _ebD_LineReference1054 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:237"
#define _ebD_LineReference1055 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:238"
#define _ebD_LineReference1056 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:239"
#define _ebD_LineReference1057 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:241"
#define _ebD_LineReference1058 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:243"
#define _ebD_LineReference1059 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:245"
#define _ebD_LineReference1060 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:247"
#define _ebD_LineReference1061 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:248"
#define _ebD_LineReference1062 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:250"
#define _ebD_LineReference1063 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:252"
#define _ebD_LineReference1064 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:254"
#define _ebD_LineReference1065 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:256"
#define _ebD_LineReference1066 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:CurrentMapElement():262"
#define _ebD_LineReference1067 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:264"
#define _ebD_LineReference1068 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:265"
#define _ebD_LineReference1069 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:267"
#define _ebD_LineReference1070 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:CurrentMapKey():273"
#define _ebD_LineReference1071 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:275"
#define _ebD_LineReference1072 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:276"
#define _ebD_LineReference1073 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:278"
#define _ebD_LineReference1074 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:NextElement():284"
#define _ebD_LineReference1075 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:286"
#define _ebD_LineReference1076 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:287"
#define _ebD_LineReference1077 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:289"
#define _ebD_LineReference1078 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:291"
#define _ebD_LineReference1079 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:292"
#define _ebD_LineReference1080 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:296"
#define _ebD_LineReference1081 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:298"
#define _ebD_LineReference1082 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:PreviousElement():304"
#define _ebD_LineReference1083 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:306"
#define _ebD_LineReference1084 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:307"
#define _ebD_LineReference1085 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:309"
#define _ebD_LineReference1086 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:311"
#define _ebD_LineReference1087 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:312"
#define _ebD_LineReference1088 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:316"
#define _ebD_LineReference1089 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:318"
#define _ebD_LineReference1090 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:SetElementIndex():324"
#define _ebD_LineReference1091 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:326"
#define _ebD_LineReference1092 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:328"
#define _ebD_LineReference1093 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:330"
#define _ebD_LineReference1094 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:331"
#define _ebD_LineReference1095 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:334"
#define _ebD_LineReference1096 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:336"
#define _ebD_LineReference1097 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:ResetElement():341"
#define _ebD_LineReference1098 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:343"
#define _ebD_LineReference1099 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:ElementCount():347"
#define _ebD_LineReference1100 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:348"
#define _ebD_LineReference1101 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:ElementIndex():351"
#define _ebD_LineReference1102 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/MapsLists.eb:352"
#define _ebD_LineReference1103 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:Error():8"
#define _ebD_LineReference1104 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:10"
#define _ebD_LineReference1105 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:11"
#define _ebD_LineReference1106 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:12"
#define _ebD_LineReference1107 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:13"
#define _ebD_LineReference1108 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:14"
#define _ebD_LineReference1109 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:15"
#define _ebD_LineReference1110 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:16"
#define _ebD_LineReference1111 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:17"
#define _ebD_LineReference1112 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:18"
#define _ebD_LineReference1113 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:19"
#define _ebD_LineReference1114 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:21"
#define _ebD_LineReference1115 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:23"
#define _ebD_LineReference1116 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:Warning():28"
#define _ebD_LineReference1117 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:30"
#define _ebD_LineReference1118 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:32"
#define _ebD_LineReference1119 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:33"
#define _ebD_LineReference1120 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:34"
#define _ebD_LineReference1121 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:35"
#define _ebD_LineReference1122 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:36"
#define _ebD_LineReference1123 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:37"
#define _ebD_LineReference1124 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:38"
#define _ebD_LineReference1125 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:39"
#define _ebD_LineReference1126 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:40"
#define _ebD_LineReference1127 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:41"
#define _ebD_LineReference1128 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:43"
#define _ebD_LineReference1129 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:45"
#define _ebD_LineReference1130 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:47"
#define _ebD_LineReference1131 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:49"
#define _ebD_LineReference1132 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:53"
#define _ebD_LineReference1133 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:55"
#define _ebD_LineReference1134 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:57"
#define _ebD_LineReference1135 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:ErrorNote():62"
#define _ebD_LineReference1136 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:64"
#define _ebD_LineReference1137 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:65"
#define _ebD_LineReference1138 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:66"
#define _ebD_LineReference1139 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:67"
#define _ebD_LineReference1140 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:68"
#define _ebD_LineReference1141 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:69"
#define _ebD_LineReference1142 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:70"
#define _ebD_LineReference1143 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:71"
#define _ebD_LineReference1144 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:72"
#define _ebD_LineReference1145 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:73"
#define _ebD_LineReference1146 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:75"
#define _ebD_LineReference1147 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:Note():80"
#define _ebD_LineReference1148 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:82"
#define _ebD_LineReference1149 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:83"
#define _ebD_LineReference1150 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:84"
#define _ebD_LineReference1151 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:85"
#define _ebD_LineReference1152 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:86"
#define _ebD_LineReference1153 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:87"
#define _ebD_LineReference1154 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:88"
#define _ebD_LineReference1155 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:89"
#define _ebD_LineReference1156 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:90"
#define _ebD_LineReference1157 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:91"
#define _ebD_LineReference1158 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:92"
#define _ebD_LineReference1159 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:93"
#define _ebD_LineReference1160 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:95"
#define _ebD_LineReference1161 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:ErrorSuggest():100"
#define _ebD_LineReference1162 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:102"
#define _ebD_LineReference1163 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:103"
#define _ebD_LineReference1164 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:104"
#define _ebD_LineReference1165 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:105"
#define _ebD_LineReference1166 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:106"
#define _ebD_LineReference1167 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:107"
#define _ebD_LineReference1168 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:108"
#define _ebD_LineReference1169 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:109"
#define _ebD_LineReference1170 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:110"
#define _ebD_LineReference1171 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:111"
#define _ebD_LineReference1172 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:112"
#define _ebD_LineReference1173 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:113"
#define _ebD_LineReference1174 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:115"
#define _ebD_LineReference1175 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:WarningSuggest():120"
#define _ebD_LineReference1176 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:122"
#define _ebD_LineReference1177 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:124"
#define _ebD_LineReference1178 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:125"
#define _ebD_LineReference1179 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:126"
#define _ebD_LineReference1180 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:127"
#define _ebD_LineReference1181 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:128"
#define _ebD_LineReference1182 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:129"
#define _ebD_LineReference1183 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:130"
#define _ebD_LineReference1184 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:131"
#define _ebD_LineReference1185 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:132"
#define _ebD_LineReference1186 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:133"
#define _ebD_LineReference1187 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:134"
#define _ebD_LineReference1188 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:135"
#define _ebD_LineReference1189 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:137"
#define _ebD_LineReference1190 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:139"
#define _ebD_LineReference1191 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:141"
#define _ebD_LineReference1192 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:143"
#define _ebD_LineReference1193 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:147"
#define _ebD_LineReference1194 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:149"
#define _ebD_LineReference1195 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:151"
#define _ebD_LineReference1196 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:TokenizeSourceFile():8"
#define _ebD_LineReference1197 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:10"
#define _ebD_LineReference1198 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:12"
#define _ebD_LineReference1199 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:14"
#define _ebD_LineReference1200 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:16"
#define _ebD_LineReference1201 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:18"
#define _ebD_LineReference1202 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:20"
#define _ebD_LineReference1203 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:22"
#define _ebD_LineReference1204 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:24"
#define _ebD_LineReference1205 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:25"
#define _ebD_LineReference1206 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:27"
#define _ebD_LineReference1207 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:29"
#define _ebD_LineReference1208 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:31"
#define _ebD_LineReference1209 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:32"
#define _ebD_LineReference1210 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:34"
#define _ebD_LineReference1211 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:35"
#define _ebD_LineReference1212 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:39"
#define _ebD_LineReference1213 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:41"
#define _ebD_LineReference1214 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:43"
#define _ebD_LineReference1215 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:45"
#define _ebD_LineReference1216 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:47"
#define _ebD_LineReference1217 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:49"
#define _ebD_LineReference1218 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:51"
#define _ebD_LineReference1219 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:53"
#define _ebD_LineReference1220 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:55"
#define _ebD_LineReference1221 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:57"
#define _ebD_LineReference1222 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:59"
#define _ebD_LineReference1223 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:60"
#define _ebD_LineReference1224 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:64"
#define _ebD_LineReference1225 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:68"
#define _ebD_LineReference1226 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:70"
#define _ebD_LineReference1227 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:72"
#define _ebD_LineReference1228 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:73"
#define _ebD_LineReference1229 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:74"
#define _ebD_LineReference1230 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:75"
#define _ebD_LineReference1231 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:77"
#define _ebD_LineReference1232 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:79"
#define _ebD_LineReference1233 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:81"
#define _ebD_LineReference1234 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:83"
#define _ebD_LineReference1235 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:85"
#define _ebD_LineReference1236 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:87"
#define _ebD_LineReference1237 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:89"
#define _ebD_LineReference1238 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:90"
#define _ebD_LineReference1239 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:91"
#define _ebD_LineReference1240 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:92"
#define _ebD_LineReference1241 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:94"
#define _ebD_LineReference1242 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:96"
#define _ebD_LineReference1243 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:97"
#define _ebD_LineReference1244 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:99"
#define _ebD_LineReference1245 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Tokenize.eb:101"
#define _ebD_LineReference1246 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:SetFile():8"
#define _ebD_LineReference1247 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:10"
#define _ebD_LineReference1248 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:11"
#define _ebD_LineReference1249 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:12"
#define _ebD_LineReference1250 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:13"
#define _ebD_LineReference1251 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:Token():17"
#define _ebD_LineReference1252 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:19"
#define _ebD_LineReference1253 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:TokenType():23"
#define _ebD_LineReference1254 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:25"
#define _ebD_LineReference1255 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:TokenLine():29"
#define _ebD_LineReference1256 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:31"
#define _ebD_LineReference1257 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:TokenPosition():35"
#define _ebD_LineReference1258 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:37"
#define _ebD_LineReference1259 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:SeekToken():42"
#define _ebD_LineReference1260 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:44"
#define _ebD_LineReference1261 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:46"
#define _ebD_LineReference1262 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:48"
#define _ebD_LineReference1263 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:50"
#define _ebD_LineReference1264 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:52"
#define _ebD_LineReference1265 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:54"
#define _ebD_LineReference1266 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:56"
#define _ebD_LineReference1267 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:58"
#define _ebD_LineReference1268 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:60"
#define _ebD_LineReference1269 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:61"
#define _ebD_LineReference1270 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:63"
#define _ebD_LineReference1271 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:NextToken():67"
#define _ebD_LineReference1272 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:69"
#define _ebD_LineReference1273 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:71"
#define _ebD_LineReference1274 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:72"
#define _ebD_LineReference1275 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:74"
#define _ebD_LineReference1276 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TokenTable.eb:76"
#define _ebD_LineReference1277 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:CheckType():8"
#define _ebD_LineReference1278 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:10"
#define _ebD_LineReference1279 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:12"
#define _ebD_LineReference1280 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:14"
#define _ebD_LineReference1281 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:18"
#define _ebD_LineReference1282 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:22"
#define _ebD_LineReference1283 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:26"
#define _ebD_LineReference1284 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:30"
#define _ebD_LineReference1285 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:34"
#define _ebD_LineReference1286 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:38"
#define _ebD_LineReference1287 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:42"
#define _ebD_LineReference1288 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:46"
#define _ebD_LineReference1289 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:50"
#define _ebD_LineReference1290 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:54"
#define _ebD_LineReference1291 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CheckType.eb:56"
#define _ebD_LineReference1292 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:GarbageCheck():9"
#define _ebD_LineReference1293 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:11"
#define _ebD_LineReference1294 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:13"
#define _ebD_LineReference1295 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:15"
#define _ebD_LineReference1296 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:17"
#define _ebD_LineReference1297 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:19"
#define _ebD_LineReference1298 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:21"
#define _ebD_LineReference1299 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:23"
#define _ebD_LineReference1300 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:25"
#define _ebD_LineReference1301 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:29"
#define _ebD_LineReference1302 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:31"
#define _ebD_LineReference1303 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:33"
#define _ebD_LineReference1304 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:35"
#define _ebD_LineReference1305 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:39"
#define _ebD_LineReference1306 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:40"
#define _ebD_LineReference1307 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:42"
#define _ebD_LineReference1308 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:44"
#define _ebD_LineReference1309 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/GarbageCheck.eb:46"
#define _ebD_LineReference1310 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:BlockConstant():8"
#define _ebD_LineReference1311 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:10"
#define _ebD_LineReference1312 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:12"
#define _ebD_LineReference1313 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:14"
#define _ebD_LineReference1314 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:15"
#define _ebD_LineReference1315 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:17"
#define _ebD_LineReference1316 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:19"
#define _ebD_LineReference1317 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:21"
#define _ebD_LineReference1318 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:23"
#define _ebD_LineReference1319 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:25"
#define _ebD_LineReference1320 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:26"
#define _ebD_LineReference1321 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:28"
#define _ebD_LineReference1322 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:30"
#define _ebD_LineReference1323 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:32"
#define _ebD_LineReference1324 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:34"
#define _ebD_LineReference1325 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:36"
#define _ebD_LineReference1326 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:38"
#define _ebD_LineReference1327 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:42"
#define _ebD_LineReference1328 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:46"
#define _ebD_LineReference1329 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:50"
#define _ebD_LineReference1330 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:51"
#define _ebD_LineReference1331 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:53"
#define _ebD_LineReference1332 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:55"
#define _ebD_LineReference1333 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:57"
#define _ebD_LineReference1334 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:59"
#define _ebD_LineReference1335 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:61"
#define _ebD_LineReference1336 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:63"
#define _ebD_LineReference1337 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:64"
#define _ebD_LineReference1338 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:BlockEnumeration():69"
#define _ebD_LineReference1339 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:71"
#define _ebD_LineReference1340 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:73"
#define _ebD_LineReference1341 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:75"
#define _ebD_LineReference1342 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:77"
#define _ebD_LineReference1343 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:79"
#define _ebD_LineReference1344 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:81"
#define _ebD_LineReference1345 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:83"
#define _ebD_LineReference1346 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:85"
#define _ebD_LineReference1347 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:89"
#define _ebD_LineReference1348 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:90"
#define _ebD_LineReference1349 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:92"
#define _ebD_LineReference1350 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:94"
#define _ebD_LineReference1351 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:96"
#define _ebD_LineReference1352 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:98"
#define _ebD_LineReference1353 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:100"
#define _ebD_LineReference1354 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:102"
#define _ebD_LineReference1355 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:ConfigureConstant():107"
#define _ebD_LineReference1356 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:109"
#define _ebD_LineReference1357 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:111"
#define _ebD_LineReference1358 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:113"
#define _ebD_LineReference1359 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:114"
#define _ebD_LineReference1360 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:116"
#define _ebD_LineReference1361 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:118"
#define _ebD_LineReference1362 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:120"
#define _ebD_LineReference1363 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:122"
#define _ebD_LineReference1364 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:124"
#define _ebD_LineReference1365 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:126"
#define _ebD_LineReference1366 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:127"
#define _ebD_LineReference1367 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:129"
#define _ebD_LineReference1368 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:131"
#define _ebD_LineReference1369 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:133"
#define _ebD_LineReference1370 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:135"
#define _ebD_LineReference1371 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:137"
#define _ebD_LineReference1372 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:139"
#define _ebD_LineReference1373 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:140"
#define _ebD_LineReference1374 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:142"
#define _ebD_LineReference1375 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:144"
#define _ebD_LineReference1376 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:146"
#define _ebD_LineReference1377 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:148"
#define _ebD_LineReference1378 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:150"
#define _ebD_LineReference1379 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:151"
#define _ebD_LineReference1380 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:153"
#define _ebD_LineReference1381 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:155"
#define _ebD_LineReference1382 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:157"
#define _ebD_LineReference1383 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:159"
#define _ebD_LineReference1384 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:160"
#define _ebD_LineReference1385 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:162"
#define _ebD_LineReference1386 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:163"
#define _ebD_LineReference1387 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:164"
#define _ebD_LineReference1388 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:166"
#define _ebD_LineReference1389 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:168"
#define _ebD_LineReference1390 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:172"
#define _ebD_LineReference1391 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:174"
#define _ebD_LineReference1392 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:178"
#define _ebD_LineReference1393 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:180"
#define _ebD_LineReference1394 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:184"
#define _ebD_LineReference1395 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:186"
#define _ebD_LineReference1396 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:188"
#define _ebD_LineReference1397 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:190"
#define _ebD_LineReference1398 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:ConstantType():195"
#define _ebD_LineReference1399 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:197"
#define _ebD_LineReference1400 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:199"
#define _ebD_LineReference1401 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:200"
#define _ebD_LineReference1402 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:202"
#define _ebD_LineReference1403 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:204"
#define _ebD_LineReference1404 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:206"
#define _ebD_LineReference1405 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:208"
#define _ebD_LineReference1406 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:210"
#define _ebD_LineReference1407 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:ConstantMacro():215"
#define _ebD_LineReference1408 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:217"
#define _ebD_LineReference1409 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:219"
#define _ebD_LineReference1410 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:220"
#define _ebD_LineReference1411 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:222"
#define _ebD_LineReference1412 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:224"
#define _ebD_LineReference1413 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:226"
#define _ebD_LineReference1414 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:228"
#define _ebD_LineReference1415 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Constants.eb:230"
#define _ebD_LineReference1416 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:BlockComment():8"
#define _ebD_LineReference1417 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:10"
#define _ebD_LineReference1418 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:12"
#define _ebD_LineReference1419 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:14"
#define _ebD_LineReference1420 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:16"
#define _ebD_LineReference1421 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:19"
#define _ebD_LineReference1422 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:21"
#define _ebD_LineReference1423 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:22"
#define _ebD_LineReference1424 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:26"
#define _ebD_LineReference1425 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:27"
#define _ebD_LineReference1426 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:28"
#define _ebD_LineReference1427 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:30"
#define _ebD_LineReference1428 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:32"
#define _ebD_LineReference1429 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Comments.eb:34"
#define _ebD_LineReference1430 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:CallProcedure():8"
#define _ebD_LineReference1431 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:10"
#define _ebD_LineReference1432 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:11"
#define _ebD_LineReference1433 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:12"
#define _ebD_LineReference1434 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:14"
#define _ebD_LineReference1435 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:16"
#define _ebD_LineReference1436 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:18"
#define _ebD_LineReference1437 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:20"
#define _ebD_LineReference1438 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:22"
#define _ebD_LineReference1439 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:24"
#define _ebD_LineReference1440 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:26"
#define _ebD_LineReference1441 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:30"
#define _ebD_LineReference1442 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:31"
#define _ebD_LineReference1443 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:33"
#define _ebD_LineReference1444 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:35"
#define _ebD_LineReference1445 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:37"
#define _ebD_LineReference1446 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:39"
#define _ebD_LineReference1447 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:41"
#define _ebD_LineReference1448 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:43"
#define _ebD_LineReference1449 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:45"
#define _ebD_LineReference1450 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:47"
#define _ebD_LineReference1451 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:51"
#define _ebD_LineReference1452 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:53"
#define _ebD_LineReference1453 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:55"
#define _ebD_LineReference1454 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:57"
#define _ebD_LineReference1455 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:58"
#define _ebD_LineReference1456 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:60"
#define _ebD_LineReference1457 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:64"
#define _ebD_LineReference1458 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:68"
#define _ebD_LineReference1459 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:69"
#define _ebD_LineReference1460 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:71"
#define _ebD_LineReference1461 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:75"
#define _ebD_LineReference1462 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:77"
#define _ebD_LineReference1463 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:81"
#define _ebD_LineReference1464 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:82"
#define _ebD_LineReference1465 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:84"
#define _ebD_LineReference1466 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:88"
#define _ebD_LineReference1467 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:89"
#define _ebD_LineReference1468 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:91"
#define _ebD_LineReference1469 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:93"
#define _ebD_LineReference1470 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:95"
#define _ebD_LineReference1471 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:CheckProcedure():100"
#define _ebD_LineReference1472 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:102"
#define _ebD_LineReference1473 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:104"
#define _ebD_LineReference1474 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:106"
#define _ebD_LineReference1475 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:107"
#define _ebD_LineReference1476 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:111"
#define _ebD_LineReference1477 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:113"
#define _ebD_LineReference1478 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:115"
#define _ebD_LineReference1479 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:117"
#define _ebD_LineReference1480 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:118"
#define _ebD_LineReference1481 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:120"
#define _ebD_LineReference1482 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:122"
#define _ebD_LineReference1483 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:124"
#define _ebD_LineReference1484 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:126"
#define _ebD_LineReference1485 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:128"
#define _ebD_LineReference1486 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:129"
#define _ebD_LineReference1487 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:130"
#define _ebD_LineReference1488 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:131"
#define _ebD_LineReference1489 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:132"
#define _ebD_LineReference1490 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:133"
#define _ebD_LineReference1491 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:135"
#define _ebD_LineReference1492 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:137"
#define _ebD_LineReference1493 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:138"
#define _ebD_LineReference1494 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:140"
#define _ebD_LineReference1495 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:142"
#define _ebD_LineReference1496 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:144"
#define _ebD_LineReference1497 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:145"
#define _ebD_LineReference1498 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:147"
#define _ebD_LineReference1499 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:149"
#define _ebD_LineReference1500 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:151"
#define _ebD_LineReference1501 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:152"
#define _ebD_LineReference1502 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:154"
#define _ebD_LineReference1503 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:156"
#define _ebD_LineReference1504 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:158"
#define _ebD_LineReference1505 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:160"
#define _ebD_LineReference1506 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:165"
#define _ebD_LineReference1507 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:166"
#define _ebD_LineReference1508 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:168"
#define _ebD_LineReference1509 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:170"
#define _ebD_LineReference1510 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:174"
#define _ebD_LineReference1511 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:176"
#define _ebD_LineReference1512 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:178"
#define _ebD_LineReference1513 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:179"
#define _ebD_LineReference1514 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:181"
#define _ebD_LineReference1515 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:183"
#define _ebD_LineReference1516 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:184"
#define _ebD_LineReference1517 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:186"
#define _ebD_LineReference1518 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:188"
#define _ebD_LineReference1519 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:190"
#define _ebD_LineReference1520 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:192"
#define _ebD_LineReference1521 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:194"
#define _ebD_LineReference1522 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:196"
#define _ebD_LineReference1523 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:197"
#define _ebD_LineReference1524 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:199"
#define _ebD_LineReference1525 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:206"
#define _ebD_LineReference1526 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:207"
#define _ebD_LineReference1527 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:209"
#define _ebD_LineReference1528 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:213"
#define _ebD_LineReference1529 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:217"
#define _ebD_LineReference1530 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:218"
#define _ebD_LineReference1531 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:220"
#define _ebD_LineReference1532 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:224"
#define _ebD_LineReference1533 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:225"
#define _ebD_LineReference1534 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:227"
#define _ebD_LineReference1535 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:229"
#define _ebD_LineReference1536 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:231"
#define _ebD_LineReference1537 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:233"
#define _ebD_LineReference1538 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:235"
#define _ebD_LineReference1539 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:236"
#define _ebD_LineReference1540 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:238"
#define _ebD_LineReference1541 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:242"
#define _ebD_LineReference1542 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:244"
#define _ebD_LineReference1543 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:246"
#define _ebD_LineReference1544 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:248"
#define _ebD_LineReference1545 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:250"
#define _ebD_LineReference1546 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:252"
#define _ebD_LineReference1547 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:254"
#define _ebD_LineReference1548 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:256"
#define _ebD_LineReference1549 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:258"
#define _ebD_LineReference1550 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:259"
#define _ebD_LineReference1551 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:261"
#define _ebD_LineReference1552 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:263"
#define _ebD_LineReference1553 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:265"
#define _ebD_LineReference1554 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:267"
#define _ebD_LineReference1555 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:269"
#define _ebD_LineReference1556 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:271"
#define _ebD_LineReference1557 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:272"
#define _ebD_LineReference1558 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:274"
#define _ebD_LineReference1559 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:276"
#define _ebD_LineReference1560 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:278"
#define _ebD_LineReference1561 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:280"
#define _ebD_LineReference1562 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:282"
#define _ebD_LineReference1563 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:284"
#define _ebD_LineReference1564 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:286"
#define _ebD_LineReference1565 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:287"
#define _ebD_LineReference1566 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:289"
#define _ebD_LineReference1567 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:291"
#define _ebD_LineReference1568 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:293"
#define _ebD_LineReference1569 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:295"
#define _ebD_LineReference1570 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:297"
#define _ebD_LineReference1571 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:299"
#define _ebD_LineReference1572 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:301"
#define _ebD_LineReference1573 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:303"
#define _ebD_LineReference1574 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:305"
#define _ebD_LineReference1575 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:307"
#define _ebD_LineReference1576 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:309"
#define _ebD_LineReference1577 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:311"
#define _ebD_LineReference1578 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:313"
#define _ebD_LineReference1579 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:315"
#define _ebD_LineReference1580 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:317"
#define _ebD_LineReference1581 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:318"
#define _ebD_LineReference1582 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:320"
#define _ebD_LineReference1583 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:322"
#define _ebD_LineReference1584 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:324"
#define _ebD_LineReference1585 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:326"
#define _ebD_LineReference1586 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:328"
#define _ebD_LineReference1587 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:330"
#define _ebD_LineReference1588 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:332"
#define _ebD_LineReference1589 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:333"
#define _ebD_LineReference1590 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:CheckDeclare():338"
#define _ebD_LineReference1591 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:340"
#define _ebD_LineReference1592 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:342"
#define _ebD_LineReference1593 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:344"
#define _ebD_LineReference1594 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:346"
#define _ebD_LineReference1595 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:347"
#define _ebD_LineReference1596 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:352"
#define _ebD_LineReference1597 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:354"
#define _ebD_LineReference1598 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:356"
#define _ebD_LineReference1599 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:358"
#define _ebD_LineReference1600 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:359"
#define _ebD_LineReference1601 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:361"
#define _ebD_LineReference1602 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:363"
#define _ebD_LineReference1603 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:365"
#define _ebD_LineReference1604 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:367"
#define _ebD_LineReference1605 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:369"
#define _ebD_LineReference1606 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:370"
#define _ebD_LineReference1607 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:371"
#define _ebD_LineReference1608 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:372"
#define _ebD_LineReference1609 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:373"
#define _ebD_LineReference1610 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:374"
#define _ebD_LineReference1611 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:376"
#define _ebD_LineReference1612 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:378"
#define _ebD_LineReference1613 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:379"
#define _ebD_LineReference1614 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:381"
#define _ebD_LineReference1615 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:383"
#define _ebD_LineReference1616 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:385"
#define _ebD_LineReference1617 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:387"
#define _ebD_LineReference1618 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:389"
#define _ebD_LineReference1619 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:391"
#define _ebD_LineReference1620 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:393"
#define _ebD_LineReference1621 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:395"
#define _ebD_LineReference1622 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:397"
#define _ebD_LineReference1623 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:398"
#define _ebD_LineReference1624 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:400"
#define _ebD_LineReference1625 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:402"
#define _ebD_LineReference1626 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:404"
#define _ebD_LineReference1627 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:406"
#define _ebD_LineReference1628 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:408"
#define _ebD_LineReference1629 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:410"
#define _ebD_LineReference1630 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:411"
#define _ebD_LineReference1631 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:413"
#define _ebD_LineReference1632 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:415"
#define _ebD_LineReference1633 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:417"
#define _ebD_LineReference1634 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:419"
#define _ebD_LineReference1635 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:421"
#define _ebD_LineReference1636 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:423"
#define _ebD_LineReference1637 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:425"
#define _ebD_LineReference1638 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:426"
#define _ebD_LineReference1639 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:428"
#define _ebD_LineReference1640 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:430"
#define _ebD_LineReference1641 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:432"
#define _ebD_LineReference1642 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:434"
#define _ebD_LineReference1643 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:436"
#define _ebD_LineReference1644 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:438"
#define _ebD_LineReference1645 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:440"
#define _ebD_LineReference1646 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:441"
#define _ebD_LineReference1647 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:CheckProcedureReturn():446"
#define _ebD_LineReference1648 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:448"
#define _ebD_LineReference1649 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:450"
#define _ebD_LineReference1650 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:452"
#define _ebD_LineReference1651 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:454"
#define _ebD_LineReference1652 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:456"
#define _ebD_LineReference1653 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:458"
#define _ebD_LineReference1654 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:462"
#define _ebD_LineReference1655 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:464"
#define _ebD_LineReference1656 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:466"
#define _ebD_LineReference1657 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:468"
#define _ebD_LineReference1658 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:470"
#define _ebD_LineReference1659 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:472"
#define _ebD_LineReference1660 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:474"
#define _ebD_LineReference1661 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:478"
#define _ebD_LineReference1662 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:480"
#define _ebD_LineReference1663 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:482"
#define _ebD_LineReference1664 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:486"
#define _ebD_LineReference1665 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:487"
#define _ebD_LineReference1666 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:489"
#define _ebD_LineReference1667 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:491"
#define _ebD_LineReference1668 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:495"
#define _ebD_LineReference1669 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:497"
#define _ebD_LineReference1670 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:499"
#define _ebD_LineReference1671 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:501"
#define _ebD_LineReference1672 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:503"
#define _ebD_LineReference1673 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:505"
#define _ebD_LineReference1674 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:507"
#define _ebD_LineReference1675 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:511"
#define _ebD_LineReference1676 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:513"
#define _ebD_LineReference1677 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:515"
#define _ebD_LineReference1678 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:519"
#define _ebD_LineReference1679 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:520"
#define _ebD_LineReference1680 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:522"
#define _ebD_LineReference1681 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:524"
#define _ebD_LineReference1682 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:526"
#define _ebD_LineReference1683 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:CheckEnd():531"
#define _ebD_LineReference1684 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:533"
#define _ebD_LineReference1685 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:535"
#define _ebD_LineReference1686 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:537"
#define _ebD_LineReference1687 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:539"
#define _ebD_LineReference1688 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:541"
#define _ebD_LineReference1689 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:543"
#define _ebD_LineReference1690 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:545"
#define _ebD_LineReference1691 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:547"
#define _ebD_LineReference1692 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:551"
#define _ebD_LineReference1693 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:553"
#define _ebD_LineReference1694 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:555"
#define _ebD_LineReference1695 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:559"
#define _ebD_LineReference1696 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:561"
#define _ebD_LineReference1697 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:563"
#define _ebD_LineReference1698 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:565"
#define _ebD_LineReference1699 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Procedures.eb:567"
#define _ebD_LineReference1700 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:CheckExpression():8"
#define _ebD_LineReference1701 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:10"
#define _ebD_LineReference1702 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:12"
#define _ebD_LineReference1703 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:14"
#define _ebD_LineReference1704 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:16"
#define _ebD_LineReference1705 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:18"
#define _ebD_LineReference1706 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:20"
#define _ebD_LineReference1707 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:22"
#define _ebD_LineReference1708 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:24"
#define _ebD_LineReference1709 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:26"
#define _ebD_LineReference1710 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:28"
#define _ebD_LineReference1711 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:32"
#define _ebD_LineReference1712 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:36"
#define _ebD_LineReference1713 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:38"
#define _ebD_LineReference1714 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:42"
#define _ebD_LineReference1715 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:44"
#define _ebD_LineReference1716 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:45"
#define _ebD_LineReference1717 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:47"
#define _ebD_LineReference1718 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:49"
#define _ebD_LineReference1719 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:51"
#define _ebD_LineReference1720 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:55"
#define _ebD_LineReference1721 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:57"
#define _ebD_LineReference1722 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:58"
#define _ebD_LineReference1723 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:60"
#define _ebD_LineReference1724 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:62"
#define _ebD_LineReference1725 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:64"
#define _ebD_LineReference1726 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:68"
#define _ebD_LineReference1727 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:70"
#define _ebD_LineReference1728 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:72"
#define _ebD_LineReference1729 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:73"
#define _ebD_LineReference1730 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:75"
#define _ebD_LineReference1731 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:77"
#define _ebD_LineReference1732 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:79"
#define _ebD_LineReference1733 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:80"
#define _ebD_LineReference1734 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:82"
#define _ebD_LineReference1735 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:84"
#define _ebD_LineReference1736 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:85"
#define _ebD_LineReference1737 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:87"
#define _ebD_LineReference1738 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:89"
#define _ebD_LineReference1739 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:91"
#define _ebD_LineReference1740 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:95"
#define _ebD_LineReference1741 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:97"
#define _ebD_LineReference1742 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:99"
#define _ebD_LineReference1743 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:103"
#define _ebD_LineReference1744 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:105"
#define _ebD_LineReference1745 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:107"
#define _ebD_LineReference1746 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:109"
#define _ebD_LineReference1747 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:111"
#define _ebD_LineReference1748 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:113"
#define _ebD_LineReference1749 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:115"
#define _ebD_LineReference1750 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:119"
#define _ebD_LineReference1751 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:121"
#define _ebD_LineReference1752 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:122"
#define _ebD_LineReference1753 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:124"
#define _ebD_LineReference1754 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:128"
#define _ebD_LineReference1755 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:130"
#define _ebD_LineReference1756 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:132"
#define _ebD_LineReference1757 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:134"
#define _ebD_LineReference1758 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:136"
#define _ebD_LineReference1759 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:137"
#define _ebD_LineReference1760 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:139"
#define _ebD_LineReference1761 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:141"
#define _ebD_LineReference1762 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:143"
#define _ebD_LineReference1763 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:145"
#define _ebD_LineReference1764 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:149"
#define _ebD_LineReference1765 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:151"
#define _ebD_LineReference1766 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:155"
#define _ebD_LineReference1767 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:156"
#define _ebD_LineReference1768 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:158"
#define _ebD_LineReference1769 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:160"
#define _ebD_LineReference1770 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:162"
#define _ebD_LineReference1771 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:164"
#define _ebD_LineReference1772 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:166"
#define _ebD_LineReference1773 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:168"
#define _ebD_LineReference1774 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:170"
#define _ebD_LineReference1775 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:172"
#define _ebD_LineReference1776 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:174"
#define _ebD_LineReference1777 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:176"
#define _ebD_LineReference1778 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:178"
#define _ebD_LineReference1779 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:180"
#define _ebD_LineReference1780 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:182"
#define _ebD_LineReference1781 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:184"
#define _ebD_LineReference1782 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:186"
#define _ebD_LineReference1783 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:188"
#define _ebD_LineReference1784 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:190"
#define _ebD_LineReference1785 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:194"
#define _ebD_LineReference1786 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:198"
#define _ebD_LineReference1787 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:202"
#define _ebD_LineReference1788 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:206"
#define _ebD_LineReference1789 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:210"
#define _ebD_LineReference1790 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:211"
#define _ebD_LineReference1791 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:213"
#define _ebD_LineReference1792 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:217"
#define _ebD_LineReference1793 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:218"
#define _ebD_LineReference1794 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:220"
#define _ebD_LineReference1795 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:222"
#define _ebD_LineReference1796 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:224"
#define _ebD_LineReference1797 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:226"
#define _ebD_LineReference1798 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:CompareExpression():230"
#define _ebD_LineReference1799 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:232"
#define _ebD_LineReference1800 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:233"
#define _ebD_LineReference1801 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:235"
#define _ebD_LineReference1802 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:237"
#define _ebD_LineReference1803 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:239"
#define _ebD_LineReference1804 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:241"
#define _ebD_LineReference1805 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:243"
#define _ebD_LineReference1806 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:245"
#define _ebD_LineReference1807 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:247"
#define _ebD_LineReference1808 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:249"
#define _ebD_LineReference1809 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:251"
#define _ebD_LineReference1810 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:255"
#define _ebD_LineReference1811 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:259"
#define _ebD_LineReference1812 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:261"
#define _ebD_LineReference1813 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:265"
#define _ebD_LineReference1814 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:267"
#define _ebD_LineReference1815 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:268"
#define _ebD_LineReference1816 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:270"
#define _ebD_LineReference1817 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:272"
#define _ebD_LineReference1818 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:274"
#define _ebD_LineReference1819 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:278"
#define _ebD_LineReference1820 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:280"
#define _ebD_LineReference1821 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:281"
#define _ebD_LineReference1822 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:283"
#define _ebD_LineReference1823 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:285"
#define _ebD_LineReference1824 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:287"
#define _ebD_LineReference1825 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:291"
#define _ebD_LineReference1826 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:293"
#define _ebD_LineReference1827 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:295"
#define _ebD_LineReference1828 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:297"
#define _ebD_LineReference1829 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:299"
#define _ebD_LineReference1830 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:303"
#define _ebD_LineReference1831 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:305"
#define _ebD_LineReference1832 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:307"
#define _ebD_LineReference1833 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:308"
#define _ebD_LineReference1834 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:310"
#define _ebD_LineReference1835 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:313"
#define _ebD_LineReference1836 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:315"
#define _ebD_LineReference1837 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:317"
#define _ebD_LineReference1838 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:319"
#define _ebD_LineReference1839 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:321"
#define _ebD_LineReference1840 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:322"
#define _ebD_LineReference1841 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:324"
#define _ebD_LineReference1842 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:326"
#define _ebD_LineReference1843 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:328"
#define _ebD_LineReference1844 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:332"
#define _ebD_LineReference1845 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:333"
#define _ebD_LineReference1846 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:335"
#define _ebD_LineReference1847 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:337"
#define _ebD_LineReference1848 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:339"
#define _ebD_LineReference1849 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:341"
#define _ebD_LineReference1850 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:343"
#define _ebD_LineReference1851 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:345"
#define _ebD_LineReference1852 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:347"
#define _ebD_LineReference1853 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:348"
#define _ebD_LineReference1854 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:350"
#define _ebD_LineReference1855 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:352"
#define _ebD_LineReference1856 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:354"
#define _ebD_LineReference1857 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:356"
#define _ebD_LineReference1858 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:358"
#define _ebD_LineReference1859 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:360"
#define _ebD_LineReference1860 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:362"
#define _ebD_LineReference1861 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:364"
#define _ebD_LineReference1862 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:366"
#define _ebD_LineReference1863 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:368"
#define _ebD_LineReference1864 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:370"
#define _ebD_LineReference1865 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:374"
#define _ebD_LineReference1866 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:378"
#define _ebD_LineReference1867 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:382"
#define _ebD_LineReference1868 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:386"
#define _ebD_LineReference1869 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:390"
#define _ebD_LineReference1870 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:394"
#define _ebD_LineReference1871 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:396"
#define _ebD_LineReference1872 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:397"
#define _ebD_LineReference1873 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:399"
#define _ebD_LineReference1874 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:401"
#define _ebD_LineReference1875 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:403"
#define _ebD_LineReference1876 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:405"
#define _ebD_LineReference1877 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:407"
#define _ebD_LineReference1878 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:411"
#define _ebD_LineReference1879 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:415"
#define _ebD_LineReference1880 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:419"
#define _ebD_LineReference1881 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:423"
#define _ebD_LineReference1882 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:427"
#define _ebD_LineReference1883 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:429"
#define _ebD_LineReference1884 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:433"
#define _ebD_LineReference1885 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:434"
#define _ebD_LineReference1886 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:435"
#define _ebD_LineReference1887 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:437"
#define _ebD_LineReference1888 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:439"
#define _ebD_LineReference1889 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:441"
#define _ebD_LineReference1890 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Expressions.eb:443"
#define _ebD_LineReference1891 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:CheckConcatenate():8"
#define _ebD_LineReference1892 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:10"
#define _ebD_LineReference1893 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:11"
#define _ebD_LineReference1894 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:13"
#define _ebD_LineReference1895 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:15"
#define _ebD_LineReference1896 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:17"
#define _ebD_LineReference1897 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:19"
#define _ebD_LineReference1898 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:21"
#define _ebD_LineReference1899 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:25"
#define _ebD_LineReference1900 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:27"
#define _ebD_LineReference1901 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:29"
#define _ebD_LineReference1902 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:31"
#define _ebD_LineReference1903 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:33"
#define _ebD_LineReference1904 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:35"
#define _ebD_LineReference1905 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:37"
#define _ebD_LineReference1906 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:38"
#define _ebD_LineReference1907 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:40"
#define _ebD_LineReference1908 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:42"
#define _ebD_LineReference1909 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:44"
#define _ebD_LineReference1910 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:48"
#define _ebD_LineReference1911 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:50"
#define _ebD_LineReference1912 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:52"
#define _ebD_LineReference1913 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:54"
#define _ebD_LineReference1914 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:56"
#define _ebD_LineReference1915 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:58"
#define _ebD_LineReference1916 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:60"
#define _ebD_LineReference1917 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:61"
#define _ebD_LineReference1918 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:63"
#define _ebD_LineReference1919 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:65"
#define _ebD_LineReference1920 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:67"
#define _ebD_LineReference1921 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:71"
#define _ebD_LineReference1922 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:73"
#define _ebD_LineReference1923 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:75"
#define _ebD_LineReference1924 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:77"
#define _ebD_LineReference1925 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:79"
#define _ebD_LineReference1926 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:81"
#define _ebD_LineReference1927 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:83"
#define _ebD_LineReference1928 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:85"
#define _ebD_LineReference1929 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:87"
#define _ebD_LineReference1930 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:89"
#define _ebD_LineReference1931 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:93"
#define _ebD_LineReference1932 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:95"
#define _ebD_LineReference1933 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:97"
#define _ebD_LineReference1934 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:98"
#define _ebD_LineReference1935 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:100"
#define _ebD_LineReference1936 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:102"
#define _ebD_LineReference1937 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:106"
#define _ebD_LineReference1938 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:108"
#define _ebD_LineReference1939 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:110"
#define _ebD_LineReference1940 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:112"
#define _ebD_LineReference1941 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:114"
#define _ebD_LineReference1942 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:115"
#define _ebD_LineReference1943 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:117"
#define _ebD_LineReference1944 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:119"
#define _ebD_LineReference1945 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:121"
#define _ebD_LineReference1946 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:125"
#define _ebD_LineReference1947 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:126"
#define _ebD_LineReference1948 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:128"
#define _ebD_LineReference1949 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:132"
#define _ebD_LineReference1950 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:133"
#define _ebD_LineReference1951 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:135"
#define _ebD_LineReference1952 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:137"
#define _ebD_LineReference1953 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:139"
#define _ebD_LineReference1954 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:141"
#define _ebD_LineReference1955 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:143"
#define _ebD_LineReference1956 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:145"
#define _ebD_LineReference1957 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:147"
#define _ebD_LineReference1958 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:149"
#define _ebD_LineReference1959 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:151"
#define _ebD_LineReference1960 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:153"
#define _ebD_LineReference1961 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:155"
#define _ebD_LineReference1962 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:157"
#define _ebD_LineReference1963 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:159"
#define _ebD_LineReference1964 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:161"
#define _ebD_LineReference1965 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:162"
#define _ebD_LineReference1966 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:164"
#define _ebD_LineReference1967 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:166"
#define _ebD_LineReference1968 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:168"
#define _ebD_LineReference1969 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:170"
#define _ebD_LineReference1970 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:172"
#define _ebD_LineReference1971 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:174"
#define _ebD_LineReference1972 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:176"
#define _ebD_LineReference1973 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:CompareConcatenate():180"
#define _ebD_LineReference1974 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:182"
#define _ebD_LineReference1975 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:184"
#define _ebD_LineReference1976 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:186"
#define _ebD_LineReference1977 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:188"
#define _ebD_LineReference1978 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:190"
#define _ebD_LineReference1979 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:194"
#define _ebD_LineReference1980 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:196"
#define _ebD_LineReference1981 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:197"
#define _ebD_LineReference1982 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:199"
#define _ebD_LineReference1983 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:201"
#define _ebD_LineReference1984 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:203"
#define _ebD_LineReference1985 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:207"
#define _ebD_LineReference1986 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:209"
#define _ebD_LineReference1987 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:210"
#define _ebD_LineReference1988 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:212"
#define _ebD_LineReference1989 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:214"
#define _ebD_LineReference1990 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:216"
#define _ebD_LineReference1991 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:220"
#define _ebD_LineReference1992 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:222"
#define _ebD_LineReference1993 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:224"
#define _ebD_LineReference1994 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:226"
#define _ebD_LineReference1995 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:228"
#define _ebD_LineReference1996 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:232"
#define _ebD_LineReference1997 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:234"
#define _ebD_LineReference1998 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:236"
#define _ebD_LineReference1999 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:237"
#define _ebD_LineReference2000 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:239"
#define _ebD_LineReference2001 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:242"
#define _ebD_LineReference2002 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:244"
#define _ebD_LineReference2003 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:246"
#define _ebD_LineReference2004 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:248"
#define _ebD_LineReference2005 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:250"
#define _ebD_LineReference2006 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:251"
#define _ebD_LineReference2007 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:253"
#define _ebD_LineReference2008 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:255"
#define _ebD_LineReference2009 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:257"
#define _ebD_LineReference2010 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:261"
#define _ebD_LineReference2011 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:262"
#define _ebD_LineReference2012 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:264"
#define _ebD_LineReference2013 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:268"
#define _ebD_LineReference2014 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:269"
#define _ebD_LineReference2015 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:271"
#define _ebD_LineReference2016 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:273"
#define _ebD_LineReference2017 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:275"
#define _ebD_LineReference2018 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:277"
#define _ebD_LineReference2019 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:279"
#define _ebD_LineReference2020 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:281"
#define _ebD_LineReference2021 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:283"
#define _ebD_LineReference2022 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:285"
#define _ebD_LineReference2023 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:287"
#define _ebD_LineReference2024 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:291"
#define _ebD_LineReference2025 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:293"
#define _ebD_LineReference2026 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:294"
#define _ebD_LineReference2027 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:296"
#define _ebD_LineReference2028 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:298"
#define _ebD_LineReference2029 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:300"
#define _ebD_LineReference2030 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:302"
#define _ebD_LineReference2031 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:304"
#define _ebD_LineReference2032 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:308"
#define _ebD_LineReference2033 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:312"
#define _ebD_LineReference2034 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:313"
#define _ebD_LineReference2035 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:315"
#define _ebD_LineReference2036 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:317"
#define _ebD_LineReference2037 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:319"
#define _ebD_LineReference2038 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:321"
#define _ebD_LineReference2039 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Concatenate.eb:323"
#define _ebD_LineReference2040 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:CompileMainPath():8"
#define _ebD_LineReference2041 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:10"
#define _ebD_LineReference2042 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:12"
#define _ebD_LineReference2043 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:14"
#define _ebD_LineReference2044 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:16"
#define _ebD_LineReference2045 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:18"
#define _ebD_LineReference2046 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:20"
#define _ebD_LineReference2047 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:22"
#define _ebD_LineReference2048 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:24"
#define _ebD_LineReference2049 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:26"
#define _ebD_LineReference2050 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:27"
#define _ebD_LineReference2051 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:28"
#define _ebD_LineReference2052 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:29"
#define _ebD_LineReference2053 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:30"
#define _ebD_LineReference2054 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:32"
#define _ebD_LineReference2055 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:34"
#define _ebD_LineReference2056 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:36"
#define _ebD_LineReference2057 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:40"
#define _ebD_LineReference2058 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:42"
#define _ebD_LineReference2059 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:44"
#define _ebD_LineReference2060 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:46"
#define _ebD_LineReference2061 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:48"
#define _ebD_LineReference2062 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:50"
#define _ebD_LineReference2063 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:51"
#define _ebD_LineReference2064 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:52"
#define _ebD_LineReference2065 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:53"
#define _ebD_LineReference2066 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:54"
#define _ebD_LineReference2067 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:56"
#define _ebD_LineReference2068 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:58"
#define _ebD_LineReference2069 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:NotImplemented():63"
#define _ebD_LineReference2070 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:65"
#define _ebD_LineReference2071 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:66"
#define _ebD_LineReference2072 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:68"
#define _ebD_LineReference2073 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:70"
#define _ebD_LineReference2074 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:72"
#define _ebD_LineReference2075 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:74"
#define _ebD_LineReference2076 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:76"
#define _ebD_LineReference2077 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:CompileCPP11():81"
#define _ebD_LineReference2078 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:83"
#define _ebD_LineReference2079 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:85"
#define _ebD_LineReference2080 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:87"
#define _ebD_LineReference2081 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:89"
#define _ebD_LineReference2082 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:91"
#define _ebD_LineReference2083 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:93"
#define _ebD_LineReference2084 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:95"
#define _ebD_LineReference2085 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:97"
#define _ebD_LineReference2086 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:99"
#define _ebD_LineReference2087 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:103"
#define _ebD_LineReference2088 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:105"
#define _ebD_LineReference2089 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:107"
#define _ebD_LineReference2090 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:108"
#define _ebD_LineReference2091 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:109"
#define _ebD_LineReference2092 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:113"
#define _ebD_LineReference2093 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:115"
#define _ebD_LineReference2094 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:119"
#define _ebD_LineReference2095 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:121"
#define _ebD_LineReference2096 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:123"
#define _ebD_LineReference2097 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:124"
#define _ebD_LineReference2098 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:126"
#define _ebD_LineReference2099 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:128"
#define _ebD_LineReference2100 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:130"
#define _ebD_LineReference2101 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:134"
#define _ebD_LineReference2102 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:138"
#define _ebD_LineReference2103 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:142"
#define _ebD_LineReference2104 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:146"
#define _ebD_LineReference2105 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:150"
#define _ebD_LineReference2106 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:154"
#define _ebD_LineReference2107 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:158"
#define _ebD_LineReference2108 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:162"
#define _ebD_LineReference2109 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:166"
#define _ebD_LineReference2110 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:170"
#define _ebD_LineReference2111 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:174"
#define _ebD_LineReference2112 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:178"
#define _ebD_LineReference2113 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:182"
#define _ebD_LineReference2114 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:186"
#define _ebD_LineReference2115 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:190"
#define _ebD_LineReference2116 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:194"
#define _ebD_LineReference2117 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:198"
#define _ebD_LineReference2118 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:202"
#define _ebD_LineReference2119 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:204"
#define _ebD_LineReference2120 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:208"
#define _ebD_LineReference2121 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:210"
#define _ebD_LineReference2122 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:212"
#define _ebD_LineReference2123 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:213"
#define _ebD_LineReference2124 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:215"
#define _ebD_LineReference2125 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:217"
#define _ebD_LineReference2126 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:219"
#define _ebD_LineReference2127 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:221"
#define _ebD_LineReference2128 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:223"
#define _ebD_LineReference2129 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:225"
#define _ebD_LineReference2130 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:229"
#define _ebD_LineReference2131 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:231"
#define _ebD_LineReference2132 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:232"
#define _ebD_LineReference2133 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:234"
#define _ebD_LineReference2134 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:236"
#define _ebD_LineReference2135 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:238"
#define _ebD_LineReference2136 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:242"
#define _ebD_LineReference2137 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:246"
#define _ebD_LineReference2138 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:250"
#define _ebD_LineReference2139 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:254"
#define _ebD_LineReference2140 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:258"
#define _ebD_LineReference2141 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:262"
#define _ebD_LineReference2142 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:266"
#define _ebD_LineReference2143 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:270"
#define _ebD_LineReference2144 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:274"
#define _ebD_LineReference2145 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:278"
#define _ebD_LineReference2146 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:282"
#define _ebD_LineReference2147 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:286"
#define _ebD_LineReference2148 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:290"
#define _ebD_LineReference2149 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:291"
#define _ebD_LineReference2150 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:293"
#define _ebD_LineReference2151 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:297"
#define _ebD_LineReference2152 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:301"
#define _ebD_LineReference2153 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:303"
#define _ebD_LineReference2154 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:305"
#define _ebD_LineReference2155 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:307"
#define _ebD_LineReference2156 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:308"
#define _ebD_LineReference2157 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:312"
#define _ebD_LineReference2158 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:314"
#define _ebD_LineReference2159 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:316"
#define _ebD_LineReference2160 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:317"
#define _ebD_LineReference2161 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:319"
#define _ebD_LineReference2162 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:321"
#define _ebD_LineReference2163 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:325"
#define _ebD_LineReference2164 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:327"
#define _ebD_LineReference2165 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:329"
#define _ebD_LineReference2166 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:331"
#define _ebD_LineReference2167 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:333"
#define _ebD_LineReference2168 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:335"
#define _ebD_LineReference2169 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:337"
#define _ebD_LineReference2170 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:339"
#define _ebD_LineReference2171 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:340"
#define _ebD_LineReference2172 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:342"
#define _ebD_LineReference2173 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:344"
#define _ebD_LineReference2174 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:346"
#define _ebD_LineReference2175 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:348"
#define _ebD_LineReference2176 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:350"
#define _ebD_LineReference2177 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:354"
#define _ebD_LineReference2178 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:356"
#define _ebD_LineReference2179 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:360"
#define _ebD_LineReference2180 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:364"
#define _ebD_LineReference2181 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:365"
#define _ebD_LineReference2182 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:367"
#define _ebD_LineReference2183 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:369"
#define _ebD_LineReference2184 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:371"
#define _ebD_LineReference2185 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:373"
#define _ebD_LineReference2186 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:375"
#define _ebD_LineReference2187 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:379"
#define _ebD_LineReference2188 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:381"
#define _ebD_LineReference2189 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:383"
#define _ebD_LineReference2190 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:385"
#define _ebD_LineReference2191 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:387"
#define _ebD_LineReference2192 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:389"
#define _ebD_LineReference2193 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:391"
#define _ebD_LineReference2194 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:393"
#define _ebD_LineReference2195 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:397"
#define _ebD_LineReference2196 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:399"
#define _ebD_LineReference2197 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:401"
#define _ebD_LineReference2198 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:405"
#define _ebD_LineReference2199 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:406"
#define _ebD_LineReference2200 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:408"
#define _ebD_LineReference2201 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:410"
#define _ebD_LineReference2202 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:414"
#define _ebD_LineReference2203 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:416"
#define _ebD_LineReference2204 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:420"
#define _ebD_LineReference2205 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:422"
#define _ebD_LineReference2206 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:423"
#define _ebD_LineReference2207 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:425"
#define _ebD_LineReference2208 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:427"
#define _ebD_LineReference2209 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:429"
#define _ebD_LineReference2210 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:433"
#define _ebD_LineReference2211 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:435"
#define _ebD_LineReference2212 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:437"
#define _ebD_LineReference2213 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:443"
#define _ebD_LineReference2214 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:444"
#define _ebD_LineReference2215 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:446"
#define _ebD_LineReference2216 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:448"
#define _ebD_LineReference2217 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:450"
#define _ebD_LineReference2218 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:452"
#define _ebD_LineReference2219 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:454"
#define _ebD_LineReference2220 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:456"
#define _ebD_LineReference2221 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:458"
#define _ebD_LineReference2222 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:460"
#define _ebD_LineReference2223 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:462"
#define _ebD_LineReference2224 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:464"
#define _ebD_LineReference2225 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/TargetCPP11.eb:466"
#define _ebD_LineReference2226 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:CPP11Type():8"
#define _ebD_LineReference2227 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:10"
#define _ebD_LineReference2228 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:12"
#define _ebD_LineReference2229 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:16"
#define _ebD_LineReference2230 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:20"
#define _ebD_LineReference2231 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:24"
#define _ebD_LineReference2232 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:28"
#define _ebD_LineReference2233 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:32"
#define _ebD_LineReference2234 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:36"
#define _ebD_LineReference2235 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:40"
#define _ebD_LineReference2236 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:44"
#define _ebD_LineReference2237 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:48"
#define _ebD_LineReference2238 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:52"
#define _ebD_LineReference2239 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:54"
#define _ebD_LineReference2240 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/CPP11.eb:56"
#define _ebD_LineReference2241 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:EmitCPP11():8"
#define _ebD_LineReference2242 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:10"
#define _ebD_LineReference2243 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:12"
#define _ebD_LineReference2244 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:13"
#define _ebD_LineReference2245 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:14"
#define _ebD_LineReference2246 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:15"
#define _ebD_LineReference2247 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:16"
#define _ebD_LineReference2248 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:18"
#define _ebD_LineReference2249 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:20"
#define _ebD_LineReference2250 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:21"
#define _ebD_LineReference2251 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:22"
#define _ebD_LineReference2252 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:23"
#define _ebD_LineReference2253 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:24"
#define _ebD_LineReference2254 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:26"
#define _ebD_LineReference2255 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:28"
#define _ebD_LineReference2256 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:30"
#define _ebD_LineReference2257 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:32"
#define _ebD_LineReference2258 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:34"
#define _ebD_LineReference2259 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:36"
#define _ebD_LineReference2260 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:38"
#define _ebD_LineReference2261 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:40"
#define _ebD_LineReference2262 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:41"
#define _ebD_LineReference2263 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:42"
#define _ebD_LineReference2264 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:43"
#define _ebD_LineReference2265 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:44"
#define _ebD_LineReference2266 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:46"
#define _ebD_LineReference2267 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:48"
#define _ebD_LineReference2268 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:50"
#define _ebD_LineReference2269 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:52"
#define _ebD_LineReference2270 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:54"
#define _ebD_LineReference2271 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:56"
#define _ebD_LineReference2272 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:58"
#define _ebD_LineReference2273 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:60"
#define _ebD_LineReference2274 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:61"
#define _ebD_LineReference2275 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:63"
#define _ebD_LineReference2276 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:65"
#define _ebD_LineReference2277 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:67"
#define _ebD_LineReference2278 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:69"
#define _ebD_LineReference2279 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:70"
#define _ebD_LineReference2280 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:71"
#define _ebD_LineReference2281 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:73"
#define _ebD_LineReference2282 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:75"
#define _ebD_LineReference2283 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:77"
#define _ebD_LineReference2284 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:79"
#define _ebD_LineReference2285 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:80"
#define _ebD_LineReference2286 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:81"
#define _ebD_LineReference2287 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:82"
#define _ebD_LineReference2288 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:83"
#define _ebD_LineReference2289 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:85"
#define _ebD_LineReference2290 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:87"
#define _ebD_LineReference2291 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:89"
#define _ebD_LineReference2292 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:91"
#define _ebD_LineReference2293 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:93"
#define _ebD_LineReference2294 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:97"
#define _ebD_LineReference2295 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:99"
#define _ebD_LineReference2296 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:101"
#define _ebD_LineReference2297 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:103"
#define _ebD_LineReference2298 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:105"
#define _ebD_LineReference2299 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:107"
#define _ebD_LineReference2300 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:109"
#define _ebD_LineReference2301 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:111"
#define _ebD_LineReference2302 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:113"
#define _ebD_LineReference2303 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:115"
#define _ebD_LineReference2304 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:117"
#define _ebD_LineReference2305 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:119"
#define _ebD_LineReference2306 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:121"
#define _ebD_LineReference2307 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:123"
#define _ebD_LineReference2308 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:124"
#define _ebD_LineReference2309 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:125"
#define _ebD_LineReference2310 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:126"
#define _ebD_LineReference2311 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:127"
#define _ebD_LineReference2312 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:129"
#define _ebD_LineReference2313 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:131"
#define _ebD_LineReference2314 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:133"
#define _ebD_LineReference2315 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:135"
#define _ebD_LineReference2316 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:137"
#define _ebD_LineReference2317 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:138"
#define _ebD_LineReference2318 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:139"
#define _ebD_LineReference2319 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:140"
#define _ebD_LineReference2320 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:141"
#define _ebD_LineReference2321 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:142"
#define _ebD_LineReference2322 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:144"
#define _ebD_LineReference2323 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:146"
#define _ebD_LineReference2324 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:148"
#define _ebD_LineReference2325 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:149"
#define _ebD_LineReference2326 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:151"
#define _ebD_LineReference2327 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:153"
#define _ebD_LineReference2328 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:155"
#define _ebD_LineReference2329 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:157"
#define _ebD_LineReference2330 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:159"
#define _ebD_LineReference2331 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:161"
#define _ebD_LineReference2332 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:163"
#define _ebD_LineReference2333 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:165"
#define _ebD_LineReference2334 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:169"
#define _ebD_LineReference2335 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:171"
#define _ebD_LineReference2336 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:173"
#define _ebD_LineReference2337 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:175"
#define _ebD_LineReference2338 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:176"
#define _ebD_LineReference2339 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:178"
#define _ebD_LineReference2340 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:180"
#define _ebD_LineReference2341 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:182"
#define _ebD_LineReference2342 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:184"
#define _ebD_LineReference2343 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:186"
#define _ebD_LineReference2344 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:187"
#define _ebD_LineReference2345 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:188"
#define _ebD_LineReference2346 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:189"
#define _ebD_LineReference2347 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:190"
#define _ebD_LineReference2348 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:192"
#define _ebD_LineReference2349 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:194"
#define _ebD_LineReference2350 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:196"
#define _ebD_LineReference2351 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:197"
#define _ebD_LineReference2352 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:199"
#define _ebD_LineReference2353 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:200"
#define _ebD_LineReference2354 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:202"
#define _ebD_LineReference2355 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:204"
#define _ebD_LineReference2356 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:205"
#define _ebD_LineReference2357 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:207"
#define _ebD_LineReference2358 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:209"
#define _ebD_LineReference2359 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:213"
#define _ebD_LineReference2360 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:215"
#define _ebD_LineReference2361 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:217"
#define _ebD_LineReference2362 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:218"
#define _ebD_LineReference2363 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:219"
#define _ebD_LineReference2364 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:221"
#define _ebD_LineReference2365 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:223"
#define _ebD_LineReference2366 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:225"
#define _ebD_LineReference2367 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:227"
#define _ebD_LineReference2368 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:229"
#define _ebD_LineReference2369 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:231"
#define _ebD_LineReference2370 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:232"
#define _ebD_LineReference2371 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:234"
#define _ebD_LineReference2372 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:236"
#define _ebD_LineReference2373 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:238"
#define _ebD_LineReference2374 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:240"
#define _ebD_LineReference2375 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:242"
#define _ebD_LineReference2376 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:244"
#define _ebD_LineReference2377 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:246"
#define _ebD_LineReference2378 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:247"
#define _ebD_LineReference2379 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:248"
#define _ebD_LineReference2380 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:250"
#define _ebD_LineReference2381 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:252"
#define _ebD_LineReference2382 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:254"
#define _ebD_LineReference2383 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:255"
#define _ebD_LineReference2384 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:257"
#define _ebD_LineReference2385 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:259"
#define _ebD_LineReference2386 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:261"
#define _ebD_LineReference2387 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:262"
#define _ebD_LineReference2388 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:263"
#define _ebD_LineReference2389 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:264"
#define _ebD_LineReference2390 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:265"
#define _ebD_LineReference2391 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:267"
#define _ebD_LineReference2392 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:269"
#define _ebD_LineReference2393 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:270"
#define _ebD_LineReference2394 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:271"
#define _ebD_LineReference2395 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:273"
#define _ebD_LineReference2396 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:275"
#define _ebD_LineReference2397 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:277"
#define _ebD_LineReference2398 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:279"
#define _ebD_LineReference2399 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:280"
#define _ebD_LineReference2400 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:281"
#define _ebD_LineReference2401 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:282"
#define _ebD_LineReference2402 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:283"
#define _ebD_LineReference2403 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:284"
#define _ebD_LineReference2404 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:285"
#define _ebD_LineReference2405 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:286"
#define _ebD_LineReference2406 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:287"
#define _ebD_LineReference2407 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:288"
#define _ebD_LineReference2408 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:289"
#define _ebD_LineReference2409 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:290"
#define _ebD_LineReference2410 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:291"
#define _ebD_LineReference2411 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:293"
#define _ebD_LineReference2412 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:295"
#define _ebD_LineReference2413 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:297"
#define _ebD_LineReference2414 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:299"
#define _ebD_LineReference2415 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:300"
#define _ebD_LineReference2416 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:301"
#define _ebD_LineReference2417 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:302"
#define _ebD_LineReference2418 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:303"
#define _ebD_LineReference2419 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:305"
#define _ebD_LineReference2420 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:307"
#define _ebD_LineReference2421 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:309"
#define _ebD_LineReference2422 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:311"
#define _ebD_LineReference2423 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:312"
#define _ebD_LineReference2424 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:314"
#define _ebD_LineReference2425 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:316"
#define _ebD_LineReference2426 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:318"
#define _ebD_LineReference2427 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:322"
#define _ebD_LineReference2428 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:324"
#define _ebD_LineReference2429 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:326"
#define _ebD_LineReference2430 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:328"
#define _ebD_LineReference2431 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:330"
#define _ebD_LineReference2432 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:333"
#define _ebD_LineReference2433 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:334"
#define _ebD_LineReference2434 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:336"
#define _ebD_LineReference2435 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:338"
#define _ebD_LineReference2436 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:339"
#define _ebD_LineReference2437 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:341"
#define _ebD_LineReference2438 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:342"
#define _ebD_LineReference2439 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:344"
#define _ebD_LineReference2440 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:346"
#define _ebD_LineReference2441 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:348"
#define _ebD_LineReference2442 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:350"
#define _ebD_LineReference2443 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:354"
#define _ebD_LineReference2444 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:356"
#define _ebD_LineReference2445 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:358"
#define _ebD_LineReference2446 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:359"
#define _ebD_LineReference2447 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:360"
#define _ebD_LineReference2448 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:362"
#define _ebD_LineReference2449 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:364"
#define _ebD_LineReference2450 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:366"
#define _ebD_LineReference2451 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:368"
#define _ebD_LineReference2452 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:370"
#define _ebD_LineReference2453 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:372"
#define _ebD_LineReference2454 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:373"
#define _ebD_LineReference2455 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:375"
#define _ebD_LineReference2456 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:377"
#define _ebD_LineReference2457 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:379"
#define _ebD_LineReference2458 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:381"
#define _ebD_LineReference2459 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:383"
#define _ebD_LineReference2460 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:385"
#define _ebD_LineReference2461 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:387"
#define _ebD_LineReference2462 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:388"
#define _ebD_LineReference2463 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:389"
#define _ebD_LineReference2464 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:391"
#define _ebD_LineReference2465 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:393"
#define _ebD_LineReference2466 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:395"
#define _ebD_LineReference2467 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:397"
#define _ebD_LineReference2468 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:398"
#define _ebD_LineReference2469 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:400"
#define _ebD_LineReference2470 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:402"
#define _ebD_LineReference2471 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:404"
#define _ebD_LineReference2472 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:406"
#define _ebD_LineReference2473 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:408"
#define _ebD_LineReference2474 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:410"
#define _ebD_LineReference2475 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:412"
#define _ebD_LineReference2476 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:414"
#define _ebD_LineReference2477 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:416"
#define _ebD_LineReference2478 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:420"
#define _ebD_LineReference2479 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:422"
#define _ebD_LineReference2480 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:424"
#define _ebD_LineReference2481 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:426"
#define _ebD_LineReference2482 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:428"
#define _ebD_LineReference2483 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:430"
#define _ebD_LineReference2484 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:431"
#define _ebD_LineReference2485 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:433"
#define _ebD_LineReference2486 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:435"
#define _ebD_LineReference2487 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:437"
#define _ebD_LineReference2488 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:439"
#define _ebD_LineReference2489 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:440"
#define _ebD_LineReference2490 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:441"
#define _ebD_LineReference2491 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:443"
#define _ebD_LineReference2492 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:445"
#define _ebD_LineReference2493 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:446"
#define _ebD_LineReference2494 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:448"
#define _ebD_LineReference2495 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:450"
#define _ebD_LineReference2496 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/EmitCPP11.eb:452"
#define _ebD_LineReference2497 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:CheckProgramParameters():8"
#define _ebD_LineReference2498 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:10"
#define _ebD_LineReference2499 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:11"
#define _ebD_LineReference2500 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:13"
#define _ebD_LineReference2501 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:15"
#define _ebD_LineReference2502 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:17"
#define _ebD_LineReference2503 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:19"
#define _ebD_LineReference2504 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:23"
#define _ebD_LineReference2505 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:27"
#define _ebD_LineReference2506 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:31"
#define _ebD_LineReference2507 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:35"
#define _ebD_LineReference2508 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:39"
#define _ebD_LineReference2509 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:43"
#define _ebD_LineReference2510 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:47"
#define _ebD_LineReference2511 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:51"
#define _ebD_LineReference2512 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:55"
#define _ebD_LineReference2513 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:59"
#define _ebD_LineReference2514 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:60"
#define _ebD_LineReference2515 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:61"
#define _ebD_LineReference2516 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:62"
#define _ebD_LineReference2517 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:64"
#define _ebD_LineReference2518 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:66"
#define _ebD_LineReference2519 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:70"
#define _ebD_LineReference2520 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:71"
#define _ebD_LineReference2521 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:72"
#define _ebD_LineReference2522 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:73"
#define _ebD_LineReference2523 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:75"
#define _ebD_LineReference2524 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:77"
#define _ebD_LineReference2525 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:81"
#define _ebD_LineReference2526 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:82"
#define _ebD_LineReference2527 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:83"
#define _ebD_LineReference2528 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:84"
#define _ebD_LineReference2529 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:85"
#define _ebD_LineReference2530 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:86"
#define _ebD_LineReference2531 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:87"
#define _ebD_LineReference2532 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:98"
#define _ebD_LineReference2533 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:99"
#define _ebD_LineReference2534 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:100"
#define _ebD_LineReference2535 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:102"
#define _ebD_LineReference2536 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:104"
#define _ebD_LineReference2537 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:108"
#define _ebD_LineReference2538 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:109"
#define _ebD_LineReference2539 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:111"
#define _ebD_LineReference2540 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:115"
#define _ebD_LineReference2541 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:117"
#define _ebD_LineReference2542 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:118"
#define _ebD_LineReference2543 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:120"
#define _ebD_LineReference2544 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:122"
#define _ebD_LineReference2545 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:124"
#define _ebD_LineReference2546 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:126"
#define _ebD_LineReference2547 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:129"
#define _ebD_LineReference2548 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:131"
#define _ebD_LineReference2549 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:132"
#define _ebD_LineReference2550 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:133"
#define _ebD_LineReference2551 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:135"
#define _ebD_LineReference2552 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:137"
#define _ebD_LineReference2553 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:139"
#define _ebD_LineReference2554 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:141"
#define _ebD_LineReference2555 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:143"
#define _ebD_LineReference2556 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:144"
#define _ebD_LineReference2557 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:145"
#define _ebD_LineReference2558 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:146"
#define _ebD_LineReference2559 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:147"
#define _ebD_LineReference2560 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:149"
#define _ebD_LineReference2561 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:151"
#define _ebD_LineReference2562 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/ProgramParameters.eb:153"
#define _ebD_LineReference2563 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:46"
#define _ebD_LineReference2564 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:47"
#define _ebD_LineReference2565 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:49"
#define _ebD_LineReference2566 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:50"
#define _ebD_LineReference2567 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:51"
#define _ebD_LineReference2568 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:52"
#define _ebD_LineReference2569 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:54"
#define _ebD_LineReference2570 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:56"
#define _ebD_LineReference2571 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:58"
#define _ebD_LineReference2572 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:60"
#define _ebD_LineReference2573 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:62"
#define _ebD_LineReference2574 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:64"
#define _ebD_LineReference2575 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:66"
#define _ebD_LineReference2576 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:68"
#define _ebD_LineReference2577 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:72"
#define _ebD_LineReference2578 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:74"
#define _ebD_LineReference2579 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:78"
#define _ebD_LineReference2580 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:80"
#define _ebD_LineReference2581 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:82"
#define _ebD_LineReference2582 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:84"
#define _ebD_LineReference2583 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:86"
#define _ebD_LineReference2584 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:88"
#define _ebD_LineReference2585 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:89"
#define _ebD_LineReference2586 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:91"
#define _ebD_LineReference2587 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:95"
#define _ebD_LineReference2588 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:97"
#define _ebD_LineReference2589 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:99"
#define _ebD_LineReference2590 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:101"
#define _ebD_LineReference2591 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:102"
#define _ebD_LineReference2592 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:103"
#define _ebD_LineReference2593 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:105"
#define _ebD_LineReference2594 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:106"
#define _ebD_LineReference2595 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:107"
#define _ebD_LineReference2596 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:109"
#define _ebD_LineReference2597 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:111"
#define _ebD_LineReference2598 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:112"
#define _ebD_LineReference2599 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:113"
#define _ebD_LineReference2600 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:115"
#define _ebD_LineReference2601 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:117"
#define _ebD_LineReference2602 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:119"
#define _ebD_LineReference2603 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:121"
#define _ebD_LineReference2604 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:123"
#define _ebD_LineReference2605 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:125"
#define _ebD_LineReference2606 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:127"
#define _ebD_LineReference2607 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:129"
#define _ebD_LineReference2608 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:131"
#define _ebD_LineReference2609 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:133"
#define _ebD_LineReference2610 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:135"
#define _ebD_LineReference2611 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:137"
#define _ebD_LineReference2612 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:139"
#define _ebD_LineReference2613 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:140"
#define _ebD_LineReference2614 "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Main.eb:143"

struct _ebS_Files {
   char * _ebF_Tokens ;
   char * _ebF_Lines ;
} ;

struct _ebS_Procedures {
   signed char _ebF_ProcedureType = 0 ;
   signed char _ebF_ReturnType = 0 ;
   char * _ebF_Parameters ;
   char * _ebF_Variables ;
   char * _ebF_Emit ;
   char * _ebF_BlockLevel ;
   char * _ebF_DeclaredPath ;
   long int _ebF_DeclaredLine = 0 ;
   signed char _ebF_Declared = 0 ;
} ;

struct _ebS_Variables {
   signed char _ebF_Type = 0 ;
   char * _ebF_MetaType ;
   char * _ebF_DeclaredPath ;
   long int _ebF_DeclaredLine = 0 ;
   long int _ebF_Position = 0 ;
   signed char _ebF_Reference = 0 ;
} ;

struct _ebS_BlockLevel {
   signed char _ebF_Type = 0 ;
   signed char _ebF_SpecialFlag = 0 ;
   char * _ebF_Expression ;
   signed char _ebF_ExpressionType = 0 ;
} ;

struct _ebS_Globals {
   signed char _ebF_GlobalType = 0 ;
   char * _ebF_DeclaredPath ;
   long int _ebF_DeclaredLine = 0 ;
   long int _ebF_Position = 0 ;
} ;

struct _ebS_Structures {
   char * _ebF_Fields ;
   char * _ebF_DeclaredIn ;
   long int _ebF_DeclaredLine = 0 ;
} ;

struct _ebS_Constants {
   signed char _ebF_ConstantType = 0 ;
   char * _ebF_Macro ;
   signed char _ebF_VariableType = 0 ;
   char * _ebF_DeclaredPath ;
   long int _ebF_DeclaredLine = 0 ;
} ;

struct _ebS_ListRoot {
   long int _ebF_CurrentPosition = 0 ;
   char * _ebF_ElementRoot ;
   long int _ebF_ListSize = 0 ;
   long int _ebF_ElementSize = 0 ;
} ;

void _ebP_InitializeGlobals() ;

void _ebP_CheckStructure() ;

void _ebP_ResolveStructure() ;

void _ebP_AddGlobalVariable(std::string _ebV_Name, signed char _ebV_Type) ;

void _ebP_CreateConstant(std::string _ebV_ConstantName, signed char _ebV_Type, std::string _ebV_Macro, std::string _ebV_Path, long int _ebV_Line, signed char _ebV_ConstantType) ;

void _ebP_QueueProcedure(std::string _ebV_ProcedureName) ;

signed char _ebP_ProcedureDeclared(std::string _ebV_ProcedureName) ;

signed char _ebP_ProcedureReturnType(std::string _ebV_ProcedureName) ;

long int _ebP_ProcedureParameterCount(std::string _ebV_ProcedureName) ;

signed char _ebP_ProcedureParameterType(std::string _ebV_ProcedureName, long int _ebV_Index) ;

void _ebP_CreateProcedure(std::string _ebV_ProcedureName, signed char _ebV_ReturnType, std::string _ebV_Path, long int _ebV_Line, signed char _ebV_ProcedureType) ;

void _ebP_AddProcedureParameter(std::string _ebV_Name, signed char _ebV_Type) ;

void _ebP_AddProcedureVariable(std::string _ebV_Name, signed char _ebV_Type) ;

signed char _ebP_VariableType(std::string _ebV_Name) ;

void _ebP_AddProcedureEmit(std::string _ebV_Line) ;

void _ebP_AddDeBlockedProcedureEmit(std::string _ebV_Line) ;

void _ebP_IncreaseBlockLevel(signed char _ebV_BlockType) ;

void _ebP_DecreaseBlockLevel() ;

signed char _ebP_BlockLevelType() ;

signed char _ebP_BlockLevelSpecialFlag() ;

void _ebP_SetBlockLevelSpecialFlag() ;

void _ebP_SetBlockLevelExpressionType(signed char _ebV_Type) ;

signed char _ebP_BlockLevelExpressionType() ;

void _ebP_SetBlockLevelExpression(std::string _ebV_Expression) ;

std::string _ebP_BlockLevelExpression() ;

void _ebP_CheckBlockLevel() ;

void _ebP_CheckIf() ;

void _ebP_CheckElse() ;

void _ebP_CheckEndIf() ;

void _ebP_CheckSelect() ;

void _ebP_CheckCase() ;

void _ebP_CheckDefault() ;

void _ebP_CheckEndSelect() ;

void _ebP_CheckXInclude() ;

void _ebP_CheckInclude() ;

void _ebP_CheckTInclude() ;

void _ebP_CheckTLink() ;

void _ebP_CheckTInitialize() ;

void _ebP_CheckTConstant() ;

void _ebP_CheckExternalProcedure() ;

void _ebP_CheckFor() ;

void _ebP_CheckNext() ;

void _ebP_CheckWhile() ;

void _ebP_CheckWend() ;

void _ebP_CheckImport() ;

signed char _ebP_List(char * _ebV_List, std::string _ebV_Commands) ;

char * _ebP_NewList(long int _ebV_Size) ;

char * _ebP_CurrentListElement(char * _ebV_List) ;

void _ebP_AddListElement(char * _ebV_List) ;

void _ebP_TruncateListElement(char * _ebV_List) ;

char * _ebP_NewMap(long int _ebV_Size) ;

void _ebP_SetMapElement(char * _ebV_Map, std::string _ebV_Key) ;

int _ebP_FindMapKey(char * _ebV_Map, std::string _ebV_Key) ;

char * _ebP_CurrentMapElement(char * _ebV_Map) ;

std::string _ebP_CurrentMapKey(char * _ebV_Map) ;

int _ebP_NextElement(char * _ebV_List) ;

int _ebP_PreviousElement(char * _ebV_List) ;

void _ebP_SetElementIndex(char * _ebV_List, long int _ebV_Index) ;

void _ebP_ResetElement(char * _ebV_List) ;

long int _ebP_ElementCount(char * _ebV_List) ;

long int _ebP_ElementIndex(char * _ebV_List) ;

void _ebP_Error(std::string _ebV_Error, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) ;

void _ebP_Warning(std::string _ebV_Warning, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) ;

void _ebP_ErrorNote(std::string _ebV_Note, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) ;

void _ebP_Note(std::string _ebV_Note, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) ;

void _ebP_ErrorSuggest(std::string _ebV_Error, std::string _ebV_Suggest, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) ;

void _ebP_WarningSuggest(std::string _ebV_Warning, std::string _ebV_Suggest, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) ;

void _ebP_TokenizeSourceFile(std::string _ebV_Path) ;

void _ebP_SetFile(std::string _ebV_Path) ;

std::string _ebP_Token() ;

signed char _ebP_TokenType() ;

long int _ebP_TokenLine() ;

long int _ebP_TokenPosition() ;

signed char _ebP_SeekToken() ;

void _ebP_NextToken() ;

signed char _ebP_CheckType() ;

void _ebP_GarbageCheck() ;

void _ebP_BlockConstant() ;

void _ebP_BlockEnumeration() ;

void _ebP_ConfigureConstant() ;

signed char _ebP_ConstantType(std::string _ebV_ConstantName) ;

std::string _ebP_ConstantMacro(std::string _ebV_ConstantName) ;

void _ebP_BlockComment() ;

void _ebP_CallProcedure() ;

void _ebP_CheckProcedure() ;

void _ebP_CheckDeclare() ;

void _ebP_CheckProcedureReturn() ;

void _ebP_CheckEnd() ;

void _ebP_CheckExpression() ;

void _ebP_CompareExpression() ;

void _ebP_CheckConcatenate() ;

void _ebP_CompareConcatenate() ;

void _ebP_CompileMainPath() ;

void _ebP_NotImplemented() ;

void _ebP_CompileCPP11() ;

std::string _ebP_CPP11Type(signed char _ebV_Type) ;

void _ebP_EmitCPP11(std::string _ebV_OutPutPath) ;

void _ebP_CheckProgramParameters() ;

void _ebP__EBMain() ;

int main(int argc, char ** argv){

   _ebD_CallDebugger(_ebD_Debugger_Flag_Initialize, "" ) ;
   _ebI_InitConsole() ;
   _ebI_argv = argv ;
   _ebI_argc = argc ;

   try {
   _ebP__EBMain() ;
   } catch (...) {
   _ebD_CallDebugger(_ebD_Debugger_Flag_Exception, "") ;
   }
   return EXIT_SUCCESS ;

}

std::string _ebV_CompilerVersion ;
long int _ebV_TotalLines = 0 ;
long int _ebV_TotalWarnings = 0 ;
long int _ebV_WarningThreshold = 0 ;
std::string _ebV_OLevel ;
signed char _ebV_SyntaxCheck = 0 ;
signed char _ebV_CommentedOnly = 0 ;
signed char _ebV_ConfigureDefaults = 0 ;
signed char _ebV_CommentPassthrough = 0 ;
signed char _ebV_InlineHeader = 0 ;
signed char _ebV_DebuggerFlag = 0 ;
char * _ebV_DebuggerReference ;
std::string _ebV_MainPath ;
std::string _ebV_ExecutablePath ;
std::string _ebV_HomePath ;
char * _ebV_ModulePath ;
char * _ebV_HeaderPath ;
std::string _ebV_WorkingEmit ;
char * _ebV_Files ;
char * _ebV_ActiveLines ;
char * _ebV_ActiveTokenTable ;
char * _ebV_TInitializeMap ;
char * _ebV_TIncludeFileMap ;
char * _ebV_ImportMap ;
char * _ebV_TLinkMap ;
char * _ebV_ProcedureList ;
char * _ebV_ProcedureMap ;
char * _ebV_ActiveProcedure ;
char * _ebV_GlobalMap ;
char * _ebV_StructureMap ;
char * _ebV_ConstantMap ;
char * _ebV_Regex ;


void _ebP_InitializeGlobals() {
   std::string _ebV_Path ;
   int _ebV_c = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference2) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference3) ;
   _ebV_CompilerVersion = std::string( "EpicBasic" ) + " " + std::string( "0.1.3" ) + " (" + std::string( _ebI_PLATFORM_STRING ) + " - " + std::string( _ebI_ARCHITECTURE_STRING ) + ")" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference4) ;
   _ebV_SyntaxCheck = false ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference5) ;
   _ebV_CommentedOnly = false ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference6) ;
   _ebV_ConfigureDefaults = false ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference7) ;
   _ebV_CommentPassthrough = false ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference8) ;
   _ebV_InlineHeader = false ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference9) ;
   _ebV_DebuggerFlag = false ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference10) ;
   _ebV_DebuggerReference = _ebP_NewMap( 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference11) ;
   _ebV_HeaderPath = _ebP_NewMap( 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference12) ;
   _ebV_Path = _ebP_GetEnvironmentVariable( "EPICBASIC_HEADER_PATH" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference13) ;
   if ( _ebV_Path == std::string( "" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference14) ;
      _ebV_Path = std::string( "/usr/share/epicbasic/modules/:/usr/local/share/epicbasic/modules/" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference15) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference16) ;
   if ( _ebP_CountString( _ebV_Path , ":" ) == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference17) ;
      _ebP_SetMapElement( _ebV_HeaderPath , _ebV_Path ) ;
   } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference18) ;
      for ( _ebV_c = 0 ; _ebV_c != _ebP_CountString( _ebV_Path , ":" ) + 2 ; _ebV_c = _ebV_c + 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference19) ;
         _ebP_SetMapElement( _ebV_HeaderPath , _ebP_StringField( _ebV_Path , _ebV_c , ":" ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference20) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference21) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference22) ;
   _ebV_ModulePath = _ebP_NewMap( 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference23) ;
   _ebV_Path = _ebP_GetEnvironmentVariable( "EPICBASIC_MODULE_PATH" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference24) ;
   if ( _ebV_Path == std::string( "" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference25) ;
      _ebV_Path = std::string( "/usr/share/epicbasic/modules/:/usr/local/share/epicbasic/modules/" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference26) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference27) ;
   if ( _ebP_CountString( _ebV_Path , ":" ) == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference28) ;
      _ebP_SetMapElement( _ebV_ModulePath , _ebV_Path ) ;
   } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference29) ;
      for ( _ebV_c = 0 ; _ebV_c != _ebP_CountString( _ebV_Path , ":" ) + 2 ; _ebV_c = _ebV_c + 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference30) ;
         _ebP_SetMapElement( _ebV_ModulePath , _ebP_StringField( _ebV_Path , _ebV_c , ":" ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference31) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference32) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference33) ;
   _ebV_HomePath = _ebP_GetEnvironmentVariable( "EPICBASIC_HOME_PATH" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference34) ;
   if ( _ebV_HomePath == std::string( "" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference35) ;
      _ebV_HomePath = std::string( "/usr/share/epicbasic/" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference36) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference37) ;
   _ebV_Files = _ebP_NewMap( sizeof( struct _ebS_Files)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference38) ;
   _ebV_TInitializeMap = _ebP_NewMap( 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference39) ;
   _ebV_TIncludeFileMap = _ebP_NewMap( 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference40) ;
   _ebP_SetMapElement( _ebV_TIncludeFileMap , "string" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference41) ;
   _ebP_SetMapElement( _ebV_TIncludeFileMap , "cstdlib" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference42) ;
   _ebP_SetMapElement( _ebV_TIncludeFileMap , "stddef.h" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference43) ;
   _ebV_ImportMap = _ebP_NewMap( 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference44) ;
   _ebV_TLinkMap = _ebP_NewMap( 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference45) ;
   _ebV_ProcedureList = _ebP_NewMap( 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference46) ;
   _ebV_ProcedureMap = _ebP_NewMap( sizeof( struct _ebS_Procedures)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference47) ;
   _ebV_GlobalMap = _ebP_NewMap( sizeof( struct _ebS_Globals)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference48) ;
   _ebV_StructureMap = _ebP_NewMap( sizeof( struct _ebS_Structures)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference49) ;
   _ebV_ConstantMap = _ebP_NewMap( sizeof( struct _ebS_Constants)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckStructure() {
   std::string _ebV_StuctureName ;
   char * _ebV_VariableMap ;
   std::string _ebV_FieldName ;
   signed char _ebV_FieldType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference56) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference57) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference58) ;
   if ( _ebP_TokenType( ) != 59 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference59) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference60) ;
      _ebP_Error( "Syntax error. Expected structure name." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference61) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference62) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference63) ;
   _ebV_StuctureName = _ebP_Token( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference64) ;
   if ( _ebP_FindMapKey( _ebV_StructureMap , _ebV_StuctureName ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference65) ;
      _ebP_SetMapElement( _ebV_StructureMap , _ebV_StuctureName ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference66) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference67) ;
      _ebP_Error( _ebV_StuctureName + " is already a declared structure." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference68) ;
      _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_StructureMap ) + offsetof( struct _ebS_Structures, _ebF_DeclaredIn)  ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference69) ;
      _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + sizeof(char*) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_StructureMap ) + offsetof( struct _ebS_Structures, _ebF_DeclaredLine)  ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference70) ;
      _ebP_ErrorNote( _ebV_StuctureName + " was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_StructureMap ) ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + sizeof(char*) ) ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_StructureMap ) + offsetof( struct _ebS_Structures, _ebF_DeclaredLine)  ) , 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference71) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference72) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference73) ;
   _ebP_CreateConstant( "#SizeOf_" + _ebV_StuctureName , 5 , "sizeof( struct _ebS_" + _ebV_StuctureName + ") " , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference74) ;
   _ebP_SetMapElement( _ebV_StructureMap , _ebV_StuctureName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference75) ;
   _ebP_PokeP( _ebP_CurrentMapElement( _ebV_StructureMap ) + offsetof( struct _ebS_Structures, _ebF_DeclaredIn)  , _ebP_DuplicateString( _ebP_CurrentMapKey( _ebV_Files ) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference76) ;
   _ebP_PokeL( _ebP_CurrentMapElement( _ebV_StructureMap ) + offsetof( struct _ebS_Structures, _ebF_DeclaredLine)  , _ebP_TokenLine( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference77) ;
   _ebV_VariableMap = _ebP_NewMap( sizeof(char) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference78) ;
   _ebP_PokeP( _ebP_CurrentMapElement( _ebV_StructureMap ) + offsetof( struct _ebS_Structures, _ebF_Fields)  , _ebV_VariableMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference79) ;
   static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference80) ;
   while ( _ebP_TokenType( ) != 8 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference81) ;
      if ( _ebP_TokenType( ) != 58 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference82) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference83) ;
         _ebP_Error( "Syntax error. Expected field variable declaration." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference84) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference85) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference86) ;
      _ebV_FieldName = _ebP_Token( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference87) ;
      _ebP_CreateConstant( "#OffsetOf_" + _ebV_StuctureName + "_" + _ebV_FieldName , 5 , "offsetof( struct _ebS_" + _ebV_StuctureName + ", _ebF_" + _ebV_FieldName + ") " , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference88) ;
      if ( _ebP_FindMapKey( _ebV_VariableMap , _ebV_FieldName ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference89) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference90) ;
         _ebP_Error( "Syntax error. FieldName must be unique." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference91) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference92) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference93) ;
      _ebP_SetMapElement( _ebV_VariableMap , _ebV_FieldName ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference94) ;
      _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference95) ;
      _ebV_FieldType = _ebP_CheckType( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference96) ;
      _ebP_PokeB( _ebP_CurrentMapElement( _ebV_VariableMap ) , _ebV_FieldType ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference97) ;
      static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference98) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference99) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_ResolveStructure() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference100) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_AddGlobalVariable(std::string _ebV_Name, signed char _ebV_Type) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference101) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference102) ;
   if ( _ebP_FindMapKey( _ebV_GlobalMap , _ebV_Name ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference103) ;
      _ebP_SetMapElement( _ebV_GlobalMap , _ebV_Name ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference104) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference105) ;
      _ebP_Error( _ebV_Name + " was already declared in the global scope." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference106) ;
      _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredPath)  ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference107) ;
      _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredLine)  ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference108) ;
      _ebP_ErrorNote( _ebV_Name + " was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredPath)  ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredLine)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_Position)  ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference109) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference110) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference111) ;
   _ebP_SetMapElement( _ebV_GlobalMap , _ebV_Name ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference112) ;
   _ebP_PokeB( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_GlobalType)  , _ebV_Type ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference113) ;
   _ebP_PokeP( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredPath)  , _ebP_DuplicateString( _ebP_CurrentMapKey( _ebV_Files ) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference114) ;
   _ebP_PokeL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredLine)  , _ebP_TokenLine( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference115) ;
   _ebP_PokeL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_Position)  , _ebP_TokenPosition( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CreateConstant(std::string _ebV_ConstantName, signed char _ebV_Type, std::string _ebV_Macro, std::string _ebV_Path, long int _ebV_Line, signed char _ebV_ConstantType) {
   char * _ebV_CurrentConstant ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference116) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference117) ;
   if ( _ebP_FindMapKey( _ebV_ConstantMap , _ebV_ConstantName ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference118) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference119) ;
      _ebP_Error( _ebV_ConstantName + " was already declared." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference120) ;
      _ebP_SetMapElement( _ebV_ConstantMap , _ebV_ConstantName ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference121) ;
      _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_CurrentMapElement( _ebV_ConstantMap ) + sizeof(char) + sizeof(char*) + sizeof(char) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference122) ;
      _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + sizeof(char*) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ConstantMap ) + sizeof(char) + sizeof(char*) + sizeof(char) + sizeof(char*) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference123) ;
      _ebP_ErrorNote( _ebV_ConstantName + " was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ConstantMap ) + sizeof(char) + sizeof(char*) + sizeof(char) ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + sizeof(char) + sizeof(char*) ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ConstantMap ) + sizeof(char) + sizeof(char*) + sizeof(char) + sizeof(char*) ) , 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference124) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference125) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference126) ;
   _ebP_SetMapElement( _ebV_ConstantMap , _ebV_ConstantName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference127) ;
   _ebV_CurrentConstant = _ebP_CurrentMapElement( _ebV_ConstantMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference128) ;
   _ebP_PokeB( _ebV_CurrentConstant , _ebV_ConstantType ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference129) ;
   _ebP_PokeP( _ebV_CurrentConstant + sizeof(char) , _ebP_DuplicateString( _ebV_Macro ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference130) ;
   _ebP_PokeB( _ebV_CurrentConstant + sizeof(char) + sizeof(char*) , _ebV_Type ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference131) ;
   _ebP_PokeP( _ebV_CurrentConstant + sizeof(char) + sizeof(char*) + sizeof(char) , _ebP_DuplicateString( _ebV_Path ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference132) ;
   _ebP_PokeL( _ebV_CurrentConstant + sizeof(char) + sizeof(char*) + sizeof(char) + sizeof(char*) , _ebV_Line ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_QueueProcedure(std::string _ebV_ProcedureName) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference133) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference134) ;
   _ebP_SetMapElement( _ebV_ProcedureList , _ebV_ProcedureName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_ProcedureDeclared(std::string _ebV_ProcedureName) {
   std::string _ebV_ResetProcedure ;
   char * _ebV_CurrentProcedure ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference135) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference136) ;
   if ( _ebP_FindMapKey( _ebV_ProcedureMap , _ebV_ProcedureName ) == false ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference137) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return false ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference138) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference139) ;
   _ebV_ResetProcedure = _ebP_CurrentMapKey( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference140) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ProcedureName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference141) ;
   _ebV_CurrentProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference142) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ResetProcedure ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference143) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebV_CurrentProcedure + offsetof( struct _ebS_Procedures, _ebF_Declared)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_ProcedureReturnType(std::string _ebV_ProcedureName) {
   std::string _ebV_ResetProcedure ;
   char * _ebV_CurrentProcedure ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference144) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference145) ;
   if ( _ebP_FindMapKey( _ebV_ProcedureMap , _ebV_ProcedureName ) == false ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference146) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference147) ;
      _ebP_Error( _ebV_ProcedureName + "() is not a declared procedure." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference148) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference149) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference150) ;
   _ebV_ResetProcedure = _ebP_CurrentMapKey( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference151) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ProcedureName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference152) ;
   _ebV_CurrentProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference153) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ResetProcedure ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference154) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebV_CurrentProcedure + offsetof( struct _ebS_Procedures, _ebF_ReturnType)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

long int _ebP_ProcedureParameterCount(std::string _ebV_ProcedureName) {
   std::string _ebV_ResetProcedure ;
   char * _ebV_CurrentProcedure ;
   char * _ebV_ActiveParameterMap ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference155) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference156) ;
   _ebV_ResetProcedure = _ebP_CurrentMapKey( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference157) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ProcedureName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference158) ;
   _ebV_CurrentProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference159) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ResetProcedure ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference160) ;
   _ebV_ActiveParameterMap = _ebP_PeekP( _ebV_CurrentProcedure + offsetof( struct _ebS_Procedures, _ebF_Parameters)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference161) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_ElementCount( _ebV_ActiveParameterMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_ProcedureParameterType(std::string _ebV_ProcedureName, long int _ebV_Index) {
   std::string _ebV_ResetProcedure ;
   char * _ebV_CurrentProcedure ;
   char * _ebV_ActiveParameterMap ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference162) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference163) ;
   _ebV_ResetProcedure = _ebP_CurrentMapKey( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference164) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ProcedureName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference165) ;
   _ebV_CurrentProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference166) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ResetProcedure ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference167) ;
   _ebV_ActiveParameterMap = _ebP_PeekP( _ebV_CurrentProcedure + offsetof( struct _ebS_Procedures, _ebF_Parameters)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference168) ;
   _ebP_SetElementIndex( _ebV_ActiveParameterMap , _ebV_Index ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference169) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebP_CurrentMapElement( _ebV_ActiveParameterMap ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CreateProcedure(std::string _ebV_ProcedureName, signed char _ebV_ReturnType, std::string _ebV_Path, long int _ebV_Line, signed char _ebV_ProcedureType) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference170) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference171) ;
   if ( _ebP_FindMapKey( _ebV_ProcedureMap , _ebV_ProcedureName ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference172) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference173) ;
      _ebP_Error( _ebV_ProcedureName + "() was already declared." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference174) ;
      _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ProcedureName ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference175) ;
      _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference176) ;
      _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) , _ebP_PeekL( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  + sizeof(char*) ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference177) ;
      _ebP_ErrorNote( _ebV_ProcedureName + "() was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  + sizeof(char*) ) , 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference178) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference179) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference180) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ProcedureName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference181) ;
   _ebV_ActiveProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference182) ;
   _ebP_PokeB( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_ProcedureType)  , _ebV_ProcedureType ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference183) ;
   _ebP_PokeB( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_ReturnType)  , _ebV_ReturnType ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference184) ;
   _ebP_PokeP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Parameters)  , _ebP_NewMap( sizeof(char) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference185) ;
   _ebP_PokeP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Variables)  , _ebP_NewMap( sizeof( struct _ebS_Variables)  ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference186) ;
   _ebP_PokeP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Emit)  , _ebP_NewList( sizeof(char*) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference187) ;
   _ebP_PokeP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  , _ebP_NewList( sizeof( struct _ebS_BlockLevel)  ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference188) ;
   _ebP_PokeP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  , _ebP_DuplicateString( _ebV_Path ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference189) ;
   _ebP_PokeL( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_DeclaredLine)  , _ebV_Line ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference190) ;
   _ebP_PokeB( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Declared)  , false ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_AddProcedureParameter(std::string _ebV_Name, signed char _ebV_Type) {
   char * _ebV_ActiveParameterMap ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference191) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference192) ;
   _ebV_ActiveParameterMap = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Parameters)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference193) ;
   if ( _ebP_FindMapKey( _ebV_ActiveParameterMap , _ebV_Name ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference194) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference195) ;
      _ebP_Error( "Parameter names must be unique." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference196) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference197) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference198) ;
   _ebP_SetMapElement( _ebV_ActiveParameterMap , _ebV_Name ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference199) ;
   _ebP_PokeB( _ebP_CurrentMapElement( _ebV_ActiveParameterMap ) , _ebV_Type ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_AddProcedureVariable(std::string _ebV_Name, signed char _ebV_Type) {
   char * _ebV_ActiveParameterMap ;
   std::string _ebV_ResetFile ;
   char * _ebV_ActiveVariableMap ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference200) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference201) ;
   _ebV_ActiveParameterMap = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Parameters)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference202) ;
   if ( _ebP_FindMapKey( _ebV_ActiveParameterMap , _ebV_Name ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference203) ;
      _ebP_SetMapElement( _ebV_ActiveParameterMap , _ebV_Name ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference204) ;
      if ( _ebP_PeekB( _ebP_CurrentMapElement( _ebV_ActiveParameterMap ) ) != _ebV_Type ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference205) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference206) ;
         _ebP_Error( _ebV_Name + " was declared as a parameter in current procedure space with a different type." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference207) ;
         _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference208) ;
         _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  + sizeof(char*) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference209) ;
         _ebP_ErrorNote( _ebV_Name + " was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  + sizeof(char*) ) , 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference210) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference211) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference212) ;
      _ebV_ResetFile = _ebP_CurrentMapKey( _ebV_Files ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference213) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference214) ;
      _ebP_Warning( _ebV_Name + " was already declared in current procedure space." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference215) ;
      _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference216) ;
      _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  + sizeof(char*) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference217) ;
      _ebP_Note( _ebV_Name + " was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  + sizeof(char*) ) , 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference218) ;
      _ebP_SetMapElement( _ebV_Files , _ebV_ResetFile ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference219) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference220) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference221) ;
   if ( _ebP_FindMapKey( _ebV_GlobalMap , _ebV_Name ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference222) ;
      _ebP_SetMapElement( _ebV_GlobalMap , _ebV_Name ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference223) ;
      if ( _ebP_PeekB( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_GlobalType)  ) != _ebV_Type ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference224) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference225) ;
         _ebP_Error( _ebV_Name + " was already declared with a different type." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference226) ;
         _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredPath)  ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference227) ;
         _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredLine)  ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference228) ;
         _ebP_ErrorNote( _ebV_Name + " was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredLine)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_Position)  ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference229) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference230) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference231) ;
      _ebV_ResetFile = _ebP_CurrentMapKey( _ebV_Files ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference232) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference233) ;
      _ebP_Warning( _ebV_Name + " was already declared." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference234) ;
      _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredPath)  ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference235) ;
      _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredLine)  ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference236) ;
      _ebP_Note( _ebV_Name + " was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredPath)  ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_DeclaredLine)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_Position)  ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference237) ;
      _ebP_SetMapElement( _ebV_Files , _ebV_ResetFile ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference238) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference239) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference240) ;
   _ebV_ActiveVariableMap = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Variables)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference241) ;
   if ( _ebP_FindMapKey( _ebV_ActiveVariableMap , _ebV_Name ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference242) ;
      _ebP_SetMapElement( _ebV_ActiveVariableMap , _ebV_Name ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference243) ;
      if ( _ebP_PeekB( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_Type)  ) != _ebV_Type ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference244) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference245) ;
         _ebP_Error( _ebV_Name + " was already declared with a different type." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference246) ;
         _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference247) ;
         _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredLine)  ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference248) ;
         _ebP_ErrorNote( _ebV_Name + " was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  + sizeof(char*) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  + sizeof(char*) + sizeof(long) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference249) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference250) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference251) ;
      _ebV_ResetFile = _ebP_CurrentMapKey( _ebV_Files ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference252) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference253) ;
      _ebP_Warning( _ebV_Name + " was already declared." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference254) ;
      _ebP_SetMapElement( _ebV_Files , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference255) ;
      _ebP_SetElementIndex( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  + sizeof(char*) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference256) ;
      _ebP_Note( _ebV_Name + " was declared here." , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  ) ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ) ) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  + sizeof(char*) ) , _ebP_PeekL( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  + sizeof(char*) + sizeof(long) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference257) ;
      _ebP_SetMapElement( _ebV_Files , _ebV_ResetFile ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference258) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference259) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference260) ;
   _ebP_SetMapElement( _ebV_ActiveVariableMap , _ebV_Name ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference261) ;
   _ebP_PokeB( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_Type)  , _ebV_Type ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference262) ;
   _ebP_PokeP( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredPath)  , _ebP_DuplicateString( _ebP_CurrentMapKey( _ebV_Files ) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference263) ;
   _ebP_PokeL( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_DeclaredLine)  , _ebP_TokenLine( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference264) ;
   _ebP_PokeL( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_Position)  , _ebP_TokenPosition( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_VariableType(std::string _ebV_Name) {
   char * _ebV_ActiveParameterMap ;
   char * _ebV_ActiveVariableMap ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference265) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference266) ;
   _ebV_ActiveParameterMap = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Parameters)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference267) ;
   if ( _ebP_FindMapKey( _ebV_ActiveParameterMap , _ebV_Name ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference268) ;
      _ebP_SetMapElement( _ebV_ActiveParameterMap , _ebV_Name ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference269) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebP_CurrentMapElement( _ebV_ActiveParameterMap ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference270) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference271) ;
   if ( _ebP_FindMapKey( _ebV_GlobalMap , _ebV_Name ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference272) ;
      _ebP_SetMapElement( _ebV_GlobalMap , _ebV_Name ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference273) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebP_CurrentMapElement( _ebV_GlobalMap ) + offsetof( struct _ebS_Globals, _ebF_GlobalType)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference274) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference275) ;
   _ebV_ActiveVariableMap = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Variables)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference276) ;
   if ( _ebP_FindMapKey( _ebV_ActiveVariableMap , _ebV_Name ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference277) ;
      _ebP_SetMapElement( _ebV_ActiveVariableMap , _ebV_Name ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference278) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebP_CurrentMapElement( _ebV_ActiveVariableMap ) + offsetof( struct _ebS_Variables, _ebF_Type)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference279) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference280) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 0 ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_AddProcedureEmit(std::string _ebV_Line) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference281) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference282) ;
   if ( _ebV_CommentPassthrough == false && _ebP_Len( _ebV_Line ) == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference283) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference284) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference285) ;
   _ebV_Line = _ebP_Space( ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) * 3 ) + 3 ) + _ebV_Line ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference286) ;
   _ebP_AddListElement( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Emit)  ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference287) ;
   _ebP_PokeP( _ebP_CurrentListElement( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Emit)  ) ) , _ebP_DuplicateString( _ebV_Line ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_AddDeBlockedProcedureEmit(std::string _ebV_Line) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference288) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference289) ;
   _ebV_Line = _ebP_Space( ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) * 3 ) ) + _ebV_Line ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference290) ;
   _ebP_AddListElement( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Emit)  ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference291) ;
   _ebP_PokeP( _ebP_CurrentListElement( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Emit)  ) ) , _ebP_DuplicateString( _ebV_Line ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_IncreaseBlockLevel(signed char _ebV_BlockType) {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference292) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference293) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference294) ;
   _ebP_AddListElement( _ebV_BlockLevel ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference295) ;
   _ebP_PokeB( _ebP_CurrentListElement( _ebV_BlockLevel ) + offsetof( struct _ebS_BlockLevel, _ebF_Type)  , _ebV_BlockType ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference296) ;
   _ebP_PokeB( _ebP_CurrentListElement( _ebV_BlockLevel ) + offsetof( struct _ebS_BlockLevel, _ebF_SpecialFlag)  , false ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_DecreaseBlockLevel() {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference297) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference298) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference299) ;
   _ebP_TruncateListElement( _ebV_BlockLevel ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_BlockLevelType() {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference300) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference301) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference302) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebP_CurrentListElement( _ebV_BlockLevel ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_BlockLevelSpecialFlag() {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference303) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference304) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference305) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebP_CurrentListElement( _ebV_BlockLevel ) + offsetof( struct _ebS_BlockLevel, _ebF_SpecialFlag)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_SetBlockLevelSpecialFlag() {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference306) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference307) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference308) ;
   _ebP_PokeB( _ebP_CurrentListElement( _ebV_BlockLevel ) + offsetof( struct _ebS_BlockLevel, _ebF_SpecialFlag)  , true ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_SetBlockLevelExpressionType(signed char _ebV_Type) {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference309) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference310) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference311) ;
   _ebP_PokeB( _ebP_CurrentListElement( _ebV_BlockLevel ) + offsetof( struct _ebS_BlockLevel, _ebF_ExpressionType)  , _ebV_Type ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_BlockLevelExpressionType() {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference312) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference313) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference314) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebP_CurrentListElement( _ebV_BlockLevel ) + offsetof( struct _ebS_BlockLevel, _ebF_ExpressionType)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_SetBlockLevelExpression(std::string _ebV_Expression) {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference315) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference316) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference317) ;
   _ebP_PokeP( _ebP_CurrentListElement( _ebV_BlockLevel ) + offsetof( struct _ebS_BlockLevel, _ebF_Expression)  , _ebP_DuplicateString( _ebV_Expression ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

std::string _ebP_BlockLevelExpression() {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference318) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference319) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference320) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_BlockLevel ) + offsetof( struct _ebS_BlockLevel, _ebF_Expression)  ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckBlockLevel() {
   char * _ebV_BlockLevel ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference321) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference322) ;
   _ebV_BlockLevel = _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference323) ;
   if ( _ebP_ElementCount( _ebV_BlockLevel ) > 0 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference324) ;
      if ( _ebP_BlockLevelType( ) == 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference325) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference326) ;
         _ebP_Error( "Syntax error. While without a Wend." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference327) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference328) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference329) ;
      if ( _ebP_BlockLevelType( ) == 0 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference330) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference331) ;
         _ebP_Error( "Syntax error. If without an EndIf." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference332) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference333) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference334) ;
      if ( _ebP_BlockLevelType( ) == 2 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference335) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference336) ;
         _ebP_Error( "Syntax error. For without a Next." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference337) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference338) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference339) ;
      if ( _ebP_BlockLevelType( ) == 3 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference340) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference341) ;
         _ebP_Error( "Syntax error. Select without an EndSelect." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference342) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference343) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference344) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckIf() {
   signed char _ebV_ConditionalElseIf = 0 ;
   signed char _ebV_TokenType = 0 ;
   std::string _ebV_VariableName ;
   signed char _ebV_CurrentType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference345) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference346) ;
   _ebV_ConditionalElseIf = false ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference347) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference348) ;
   _ebV_TokenType = _ebP_TokenType( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference349) ;
   if ( _ebV_TokenType == 26 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference350) ;
      _ebV_ConditionalElseIf = true ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference351) ;
      if ( _ebP_BlockLevelSpecialFlag( ) == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference352) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference353) ;
         _ebP_Error( "Syntax error. ElseIf cannot follow an Else." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference354) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference355) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference356) ;
      if ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) == 0 || _ebP_BlockLevelType( ) != 0 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference357) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference358) ;
         _ebP_Error( "Syntax error. ElseIf without an If." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference359) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference360) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference361) ;
      _ebV_WorkingEmit = "} else if ( " ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference362) ;
      _ebP_IncreaseBlockLevel( 0 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference363) ;
      _ebV_WorkingEmit = "if ( " ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference364) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference365) ;
   while ( _ebP_TokenType( ) != 81 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference366) ;
      _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference367) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference368) ;
      if ( _ebV_TokenType == 53 || _ebV_TokenType == 52 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference369) ;
         _ebP_CompareExpression( ) ;
      } else if ( _ebV_TokenType == 58 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference370) ;
         _ebV_VariableName = _ebP_Token( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference371) ;
         _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference372) ;
         _ebV_CurrentType = _ebP_CheckType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference373) ;
         static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference374) ;
         if ( _ebV_CurrentType == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference375) ;
            _ebP_CompareConcatenate( ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference376) ;
            _ebP_CompareExpression( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference377) ;
         }
      } else if ( _ebV_TokenType == 59 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference378) ;
         _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference379) ;
         if ( _ebP_VariableType( _ebV_VariableName ) == 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference380) ;
            _ebP_CompareExpression( ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference381) ;
            _ebV_CurrentType = _ebP_VariableType( _ebV_VariableName ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference382) ;
            if ( _ebV_CurrentType == 10 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference383) ;
               _ebP_CompareConcatenate( ) ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference384) ;
               _ebP_CompareExpression( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference385) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference386) ;
         }
      } else if ( _ebV_TokenType == 4 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference387) ;
         if ( _ebP_ProcedureReturnType( _ebP_Token( ) ) == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference388) ;
            _ebP_CompareConcatenate( ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference389) ;
            _ebP_CompareExpression( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference390) ;
         }
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference391) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference392) ;
         _ebP_Error( "Syntax error." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference393) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference394) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference395) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference396) ;
      if ( _ebV_TokenType == 24 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference397) ;
         _ebV_WorkingEmit += "&& " ;
      } else if ( _ebV_TokenType == 25 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference398) ;
         _ebV_WorkingEmit += "|| " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference399) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference400) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference401) ;
   _ebV_WorkingEmit += ") { " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference402) ;
   static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference403) ;
   if ( _ebV_DebuggerFlag == true && _ebV_ConditionalElseIf == false ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference404) ;
      _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference405) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference406) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference407) ;
   _ebP_AddDeBlockedProcedureEmit( _ebV_WorkingEmit ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference408) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference409) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference410) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckElse() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference411) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference412) ;
   if ( _ebP_BlockLevelSpecialFlag( ) == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference413) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference414) ;
      _ebP_Error( "Syntax error. Else can only be used once per conditional." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference415) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference416) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference417) ;
   _ebP_SetBlockLevelSpecialFlag( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference418) ;
   if ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) == 0 || _ebP_BlockLevelType( ) != 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference419) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference420) ;
      _ebP_Error( "Syntax error. Else without an If." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference421) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference422) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference423) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference424) ;
   _ebP_AddDeBlockedProcedureEmit( "} else { " ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference425) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference426) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference427) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckEndIf() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference428) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference429) ;
   if ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) == 0 || _ebP_BlockLevelType( ) != 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference430) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference431) ;
      _ebP_Error( "Syntax error. EndIf without an If." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference432) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference433) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference434) ;
   _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference435) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference436) ;
      _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference437) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference438) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference439) ;
   _ebP_AddDeBlockedProcedureEmit( "}" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference440) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference441) ;
   _ebP_DecreaseBlockLevel( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference442) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference443) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckSelect() {
   signed char _ebV_TokenType = 0 ;
   signed char _ebV_CurrentType = 0 ;
   std::string _ebV_VariableName ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference444) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference445) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_Call, "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Conditionals.eb:230") ;;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference446) ;
   _ebP_NextToken( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference447) ;
   _ebV_TokenType = _ebP_TokenType( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference448) ;
   if ( _ebV_TokenType == 52 || _ebV_TokenType == 53 || _ebV_TokenType == 54 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference449) ;
      _ebV_CurrentType = 4 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference450) ;
      _ebP_CheckExpression( ) ;
   } else if ( _ebV_TokenType == 48 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference451) ;
      _ebV_CurrentType = 10 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference452) ;
      _ebP_CheckConcatenate( ) ;
   } else if ( _ebV_TokenType == 58 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference453) ;
      _ebV_VariableName = _ebP_Token( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference454) ;
      _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference455) ;
      _ebV_CurrentType = _ebP_CheckType( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference456) ;
      static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference457) ;
      if ( _ebV_CurrentType == 10 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference458) ;
         _ebP_CheckConcatenate( ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference459) ;
         _ebP_CheckExpression( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference460) ;
      }
   } else if ( _ebV_TokenType == 59 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference461) ;
      _ebV_VariableName = _ebP_Token( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference462) ;
      if ( _ebP_VariableType( _ebV_VariableName ) == 0 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference463) ;
         _ebP_CheckExpression( ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference464) ;
         _ebV_CurrentType = _ebP_VariableType( _ebV_VariableName ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference465) ;
         if ( _ebV_CurrentType == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference466) ;
            _ebP_CheckConcatenate( ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference467) ;
            _ebP_CheckExpression( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference468) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference469) ;
      }
   } else if ( _ebV_TokenType == 4 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference470) ;
      if ( _ebP_ProcedureReturnType( _ebP_Token( ) ) == 10 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference471) ;
         _ebV_CurrentType = 10 ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference472) ;
         _ebP_CompareConcatenate( ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference473) ;
         _ebV_CurrentType = 4 ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference474) ;
         _ebP_CompareExpression( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference475) ;
      }
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference476) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference477) ;
      _ebP_Error( "Syntax error." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference478) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference479) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference480) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference481) ;
   _ebP_IncreaseBlockLevel( 3 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference482) ;
   _ebP_SetBlockLevelExpression( _ebV_WorkingEmit ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference483) ;
   if ( _ebV_CurrentType == 10 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference484) ;
      _ebP_SetBlockLevelExpressionType( 10 ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference485) ;
      _ebP_SetBlockLevelExpressionType( _ebV_CurrentType ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference486) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference487) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference488) ;
   static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference489) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference490) ;
   static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference491) ;
   if ( _ebP_TokenType( ) != 18 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference492) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference493) ;
      _ebP_Error( "Syntax error. Expected Case." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference494) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference495) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference496) ;
   _ebV_WorkingEmit = "if ( " + _ebP_BlockLevelExpression( ) + "== " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference497) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference498) ;
   if ( _ebP_BlockLevelExpressionType( ) == 10 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference499) ;
      _ebP_CheckConcatenate( ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference500) ;
      _ebP_CheckExpression( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference501) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference502) ;
   _ebV_WorkingEmit += ") { " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference503) ;
   static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference504) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference505) ;
      _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference506) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference507) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference508) ;
   _ebP_AddDeBlockedProcedureEmit( _ebV_WorkingEmit ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference509) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference510) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference511) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckCase() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference512) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference513) ;
   if ( _ebP_BlockLevelSpecialFlag( ) == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference514) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference515) ;
      _ebP_Error( "Syntax error. Case cannot come after Default." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference516) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference517) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference518) ;
   if ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) == 0 || _ebP_BlockLevelType( ) != 3 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference519) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference520) ;
      _ebP_Error( "Syntax error. Case without a Select." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference521) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference522) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference523) ;
   _ebV_WorkingEmit = "} else if ( " + _ebP_BlockLevelExpression( ) + "== " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference524) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference525) ;
   while ( 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference526) ;
      if ( _ebP_BlockLevelExpressionType( ) == 10 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference527) ;
         _ebP_CheckConcatenate( ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference528) ;
         _ebP_CheckExpression( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference529) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference530) ;
      if ( _ebP_TokenType( ) == 64 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference531) ;
         _ebV_WorkingEmit += "|| " + _ebP_BlockLevelExpression( ) + "== " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference532) ;
         _ebP_NextToken( ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference533) ;
         break ;;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference534) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference535) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference536) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference537) ;
   _ebV_WorkingEmit += ") { " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference538) ;
   static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference539) ;
   _ebP_AddDeBlockedProcedureEmit( _ebV_WorkingEmit ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference540) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference541) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference542) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckDefault() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference543) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference544) ;
   if ( _ebP_BlockLevelSpecialFlag( ) == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference545) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference546) ;
      _ebP_Error( "Syntax error. Default can only be used once per conditional." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference547) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference548) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference549) ;
   _ebP_SetBlockLevelSpecialFlag( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference550) ;
   if ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) == 0 || _ebP_BlockLevelType( ) != 3 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference551) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference552) ;
      _ebP_Error( "Syntax error. Default without a Select." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference553) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference554) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference555) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference556) ;
   _ebP_AddDeBlockedProcedureEmit( "} else { " ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference557) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference558) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference559) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckEndSelect() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference560) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference561) ;
   if ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) == 0 || _ebP_BlockLevelType( ) != 3 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference562) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference563) ;
      _ebP_Error( "Syntax error. EndSelect without a Select." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference564) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference565) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference566) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference567) ;
   _ebP_AddDeBlockedProcedureEmit( "}" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference568) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference569) ;
   _ebP_DecreaseBlockLevel( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference570) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference571) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckXInclude() {
   std::string _ebV_Path ;
   std::string _ebV_IncludePath ;
   long int _ebV_TokenIndex = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference572) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference573) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference574) ;
   if ( _ebP_TokenType( ) == 48 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference575) ;
      _ebV_Path = _ebP_CurrentMapKey( _ebV_Files ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference576) ;
      _ebV_IncludePath = _ebP_AbsolutePath( _ebP_GetPathPart( _ebV_Path ) + _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference577) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference578) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference579) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference580) ;
         _ebP_AddProcedureEmit( "// ; XIncludeFile " + _ebV_IncludePath ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference581) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference582) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference583) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference584) ;
      if ( _ebP_FindMapKey( _ebV_Files , _ebV_IncludePath ) == false ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference585) ;
         _ebV_TokenIndex = _ebP_ElementIndex( _ebV_ActiveTokenTable ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference586) ;
         _ebP_Print( "XIncluding " ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference586) ;
         _ebP_ConsoleTextColor( 4 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference586) ;
         _ebP_Print( "\"" + _ebV_IncludePath + "\"" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference586) ;
         _ebP_ResetConsole( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference586) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference586) ;
         _ebP_Print( "...\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference587) ;
         _ebP_TokenizeSourceFile( _ebV_IncludePath ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference588) ;
         _ebP_SetFile( _ebV_IncludePath ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference589) ;
         _ebP_ResetElement( _ebV_ActiveTokenTable ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference590) ;
         _ebP_CompileCPP11( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference591) ;
         _ebP_SetFile( _ebV_Path ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference592) ;
         _ebP_SetElementIndex( _ebV_ActiveTokenTable , _ebV_TokenIndex ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference593) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference594) ;
         _ebP_Warning( "XIncludeFile \"" + _ebV_IncludePath + "\" was already included and will not be included again." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference595) ;
         if ( _ebV_CommentPassthrough == true ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference596) ;
            _ebP_AddProcedureEmit( "" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference597) ;
            _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference598) ;
            _ebP_AddProcedureEmit( "// ; XIncludeFile was already included and was not included again. " ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference599) ;
            _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference600) ;
            _ebP_AddProcedureEmit( "" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference601) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference602) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference603) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference604) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference605) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference606) ;
         _ebP_AddProcedureEmit( "// ; End of XIncludeFile " + _ebV_IncludePath ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference607) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference608) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference609) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference610) ;
      _ebP_GarbageCheck( ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference611) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference612) ;
      _ebP_ErrorSuggest( "Syntax error. Expected path to include." , "Syntax: XIncludeFile \"Path/File.eb\"'" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference613) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference614) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckInclude() {
   std::string _ebV_Path ;
   std::string _ebV_IncludePath ;
   long int _ebV_TokenIndex = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference615) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference616) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference617) ;
   if ( _ebP_TokenType( ) == 48 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference618) ;
      _ebV_Path = _ebP_CurrentMapKey( _ebV_Files ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference619) ;
      _ebV_IncludePath = _ebP_AbsolutePath( _ebP_GetPathPart( _ebV_Path ) + _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference620) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference621) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference622) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference623) ;
         _ebP_AddProcedureEmit( "// ; IncludeFile " + _ebV_IncludePath ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference624) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference625) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference626) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference627) ;
      _ebV_TokenIndex = _ebP_ElementIndex( _ebV_ActiveTokenTable ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference628) ;
      _ebP_Print( "Including " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference628) ;
      _ebP_ConsoleTextColor( 4 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference628) ;
      _ebP_Print( "\"" + _ebV_IncludePath + "\"" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference628) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference628) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference628) ;
      _ebP_Print( "...\n" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference629) ;
      _ebP_TokenizeSourceFile( _ebV_IncludePath ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference630) ;
      _ebP_SetFile( _ebV_IncludePath ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference631) ;
      _ebP_ResetElement( _ebV_ActiveTokenTable ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference632) ;
      _ebP_CompileCPP11( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference633) ;
      _ebP_SetFile( _ebV_Path ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference634) ;
      _ebP_SetElementIndex( _ebV_ActiveTokenTable , _ebV_TokenIndex ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference635) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference636) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference637) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference638) ;
         _ebP_AddProcedureEmit( "// ; End of IncludeFile " + _ebV_IncludePath ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference639) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference640) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference641) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference642) ;
      _ebP_GarbageCheck( ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference643) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference644) ;
      _ebP_ErrorSuggest( "Syntax error. Expected path to include." , "Syntax: XIncludeFile \"Path/File.eb\"'" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference645) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference646) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckTInclude() {
   std::string _ebV_IncludePath ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference647) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference648) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference649) ;
   if ( _ebP_TokenType( ) != 49 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference650) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference651) ;
      _ebP_ErrorSuggest( "Syntax error. Expected library to include." , "Syntax: TIncludeFile <File>'" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference652) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference653) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference654) ;
   _ebV_IncludePath = _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference655) ;
   _ebP_SetMapElement( _ebV_TIncludeFileMap , _ebV_IncludePath ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference656) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckTLink() {
   std::string _ebV_LinkPath ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference657) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference658) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference659) ;
   if ( _ebP_TokenType( ) != 49 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference660) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference661) ;
      _ebP_ErrorSuggest( "Syntax error. Expected library to link." , "Syntax: TLink <-lLibraryName>'" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference662) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference663) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference664) ;
   _ebV_LinkPath = _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference665) ;
   _ebP_SetMapElement( _ebV_TLinkMap , _ebV_LinkPath ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference666) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckTInitialize() {
   std::string _ebV_InitializeString ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference667) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference668) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference669) ;
   if ( _ebP_TokenType( ) != 49 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference670) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference671) ;
      _ebP_ErrorSuggest( "Syntax error. Expected initialization statement." , "Syntax: TInitialize <InitStatement>'" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference672) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference673) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference674) ;
   _ebV_InitializeString = _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference675) ;
   _ebP_SetMapElement( _ebV_TInitializeMap , _ebV_InitializeString ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference676) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckTConstant() {
   signed char _ebV_CurrentType = 0 ;
   std::string _ebV_ConstantName ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference677) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference678) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference679) ;
   if ( _ebP_TokenType( ) != 51 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference680) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference681) ;
      _ebP_ErrorSuggest( "Syntax error. Expected constant type." , "Syntax: TConstant.type #ConstantName = <Macro>" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference682) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference683) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference684) ;
   _ebV_CurrentType = _ebP_CheckType( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference685) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference686) ;
   if ( _ebP_TokenType( ) != 55 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference687) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference688) ;
      _ebP_ErrorSuggest( "Syntax error. Expected constant name." , "Syntax: TConstant.type #ConstantName = <Macro>" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference689) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference690) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference691) ;
   _ebV_ConstantName = _ebP_Token( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference692) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference693) ;
   if ( _ebP_TokenType( ) != 65 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference694) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference695) ;
      _ebP_ErrorSuggest( "Syntax error. Constant must be assigned a value." , "Syntax: TConstant.type #ConstantName = <Macro>" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference696) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference697) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference698) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference699) ;
   if ( _ebP_TokenType( ) != 49 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference700) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference701) ;
      _ebP_ErrorSuggest( "Syntax error. Constant must be assigned a value." , "Syntax: TConstant.type #ConstantName = <Macro>" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference702) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference703) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference704) ;
   _ebP_CreateConstant( _ebV_ConstantName , _ebV_CurrentType , _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 1 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference705) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckExternalProcedure() {
   std::string _ebV_ResetProcedure ;
   signed char _ebV_ReturnType = 0 ;
   std::string _ebV_OldProcedure ;
   std::string _ebV_NewProcedure ;
   std::string _ebV_ParameterName ;
   signed char _ebV_ParameterType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference706) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference707) ;
   _ebV_ResetProcedure = _ebP_CurrentMapKey( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference708) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference709) ;
   if ( _ebP_TokenType( ) == 51 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference710) ;
      _ebV_ReturnType = _ebP_CheckType( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference711) ;
      _ebP_NextToken( ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference712) ;
      _ebV_ReturnType = 0 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference713) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference714) ;
   if ( _ebP_TokenType( ) != 4 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference715) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference716) ;
      _ebP_ErrorSuggest( "Syntax error. Expected procedure name. " , "Syntax: Procedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference717) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference718) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference719) ;
   _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference720) ;
   if ( _ebV_CommentPassthrough == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference721) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference722) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference723) ;
      _ebP_AddProcedureEmit( "// ; ExternalProcedure Declaration " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference724) ;
      _ebP_AddProcedureEmit( "// ;    " + _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference725) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference726) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference727) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference728) ;
   _ebV_OldProcedure = _ebP_CurrentMapKey( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference729) ;
   _ebV_NewProcedure = _ebP_Token( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference730) ;
   _ebP_CreateProcedure( _ebV_NewProcedure , _ebV_ReturnType , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 1 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference731) ;
   _ebP_NextToken( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference732) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference733) ;
   if ( _ebP_TokenType( ) != 61 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference734) ;
      static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference735) ;
      while ( _ebP_TokenType( ) != 61 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference736) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference737) ;
         if ( _ebP_TokenType( ) != 59 && _ebP_TokenType( ) != 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference738) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference739) ;
            _ebP_ErrorSuggest( "Syntax error. Expected parameter name. " , "Syntax: ExternalProcedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference740) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference741) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference742) ;
         _ebV_ParameterName = _ebP_Token( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference743) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference744) ;
         if ( _ebP_TokenType( ) != 51 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference745) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference746) ;
            _ebP_ErrorSuggest( "Syntax error. Expected parameter type. " , "Syntax: ExternalProcedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference747) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference748) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference749) ;
         _ebV_ParameterType = _ebP_CheckType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference750) ;
         _ebP_AddProcedureParameter( _ebV_ParameterName , _ebV_ParameterType ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference751) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference752) ;
         if ( _ebP_TokenType( ) != 64 && _ebP_TokenType( ) != 61 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference753) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference754) ;
            _ebP_ErrorSuggest( "Syntax error. Expected delimiter {,} or closing parenthesis {)}. " , "Syntax: ExternalProcedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference755) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference756) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference757) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference758) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference759) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference760) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ResetProcedure ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference761) ;
   _ebV_ActiveProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckFor() {
   std::string _ebV_VariableName ;
   signed char _ebV_CurrentType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference762) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference763) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference764) ;
   _ebP_IncreaseBlockLevel( 2 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference765) ;
   _ebV_WorkingEmit = "for ( " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference766) ;
   static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference767) ;
   if ( _ebP_TokenType( ) == 59 || _ebP_TokenType( ) == 58 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference768) ;
      if ( _ebP_TokenType( ) == 58 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference769) ;
         _ebV_VariableName = _ebP_Token( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference770) ;
         _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference771) ;
         _ebV_CurrentType = _ebP_CheckType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference772) ;
         _ebP_AddProcedureVariable( _ebV_VariableName , _ebV_CurrentType ) ;
      } else if ( _ebP_TokenType( ) == 59 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference773) ;
         _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference774) ;
         if ( _ebP_VariableType( _ebV_VariableName ) == 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference775) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference776) ;
            _ebP_Warning( "Variable type was not explicitly given. Defaulting to Integer." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference777) ;
            _ebP_AddProcedureVariable( _ebV_VariableName , 4 ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference778) ;
            _ebV_CurrentType = 4 ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference779) ;
            _ebV_CurrentType = _ebP_VariableType( _ebV_VariableName ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference780) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference781) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference782) ;
      _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference783) ;
      if ( _ebP_TokenType( ) != 65 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference784) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference785) ;
         _ebP_Error( "Syntax error. Expected inital assignment with '='." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference786) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference787) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference788) ;
      if ( _ebV_CurrentType == 10 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference789) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference790) ;
         _ebP_Error( "Syntax error. Cannot iterate a string." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference791) ;
         exit( EXIT_FAILURE ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference792) ;
         _ebV_WorkingEmit += "_ebV_" + _ebV_VariableName + " = " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference793) ;
         static_cast<void>( _ebP_SeekToken( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference794) ;
         _ebP_CheckExpression( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference795) ;
      }
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference796) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference797) ;
      _ebP_Error( "Syntax error. Expected variable to iterate." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference798) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference799) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference800) ;
   if ( _ebP_TokenType( ) != 30 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference801) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference802) ;
      _ebP_Error( "Syntax error. Expected 'To'." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference803) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference804) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference805) ;
   static_cast<void>( _ebP_SeekToken( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference806) ;
   _ebV_WorkingEmit += "; _ebV_" + _ebV_VariableName + " != " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference807) ;
   _ebP_CheckExpression( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference808) ;
   _ebV_WorkingEmit += "; " ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference809) ;
   if ( _ebP_TokenType( ) == 31 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference810) ;
      _ebV_WorkingEmit += "_ebV_" + _ebV_VariableName + " = _ebV_" + _ebV_VariableName + " + " ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference811) ;
      static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference812) ;
      _ebP_CheckExpression( ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference813) ;
      _ebV_WorkingEmit += "_ebV_" + _ebV_VariableName + "++ " ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference814) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference815) ;
      _ebP_Warning( "Step value not specified. Defaulting to 1." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference816) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference817) ;
   _ebV_WorkingEmit += ") { " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference818) ;
   static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference819) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference820) ;
      _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference821) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference822) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference823) ;
   _ebP_AddDeBlockedProcedureEmit( _ebV_WorkingEmit ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference824) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference825) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference826) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckNext() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference827) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference828) ;
   if ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) == 0 || _ebP_BlockLevelType( ) != 2 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference829) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference830) ;
      _ebP_Error( "Syntax error. Next without a For." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference831) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference832) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference833) ;
   _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference834) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference835) ;
      _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference836) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference837) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference838) ;
   _ebP_AddDeBlockedProcedureEmit( "}" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference839) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference840) ;
   _ebP_DecreaseBlockLevel( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference841) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference842) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckWhile() {
   signed char _ebV_TokenType = 0 ;
   std::string _ebV_VariableName ;
   signed char _ebV_CurrentType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference843) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference844) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference845) ;
   _ebP_IncreaseBlockLevel( 1 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference846) ;
   _ebV_WorkingEmit = "while ( " ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference847) ;
   while ( _ebP_TokenType( ) != 81 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference848) ;
      static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference849) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference850) ;
      if ( _ebV_TokenType == 53 || _ebV_TokenType == 52 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference851) ;
         _ebP_CompareExpression( ) ;
      } else if ( _ebV_TokenType == 58 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference852) ;
         _ebV_VariableName = _ebP_Token( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference853) ;
         _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference854) ;
         _ebV_CurrentType = _ebP_CheckType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference855) ;
         static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference856) ;
         if ( _ebV_CurrentType == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference857) ;
            _ebP_CompareConcatenate( ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference858) ;
            _ebP_CompareExpression( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference859) ;
         }
      } else if ( _ebV_TokenType == 59 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference860) ;
         _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference861) ;
         if ( _ebP_VariableType( _ebV_VariableName ) == 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference862) ;
            _ebP_CompareExpression( ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference863) ;
            _ebV_CurrentType = _ebP_VariableType( _ebV_VariableName ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference864) ;
            if ( _ebV_CurrentType == 10 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference865) ;
               _ebP_CompareConcatenate( ) ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference866) ;
               _ebP_CompareExpression( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference867) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference868) ;
         }
      } else if ( _ebV_TokenType == 4 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference869) ;
         if ( _ebP_ProcedureReturnType( _ebP_Token( ) ) == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference870) ;
            _ebP_CompareConcatenate( ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference871) ;
            _ebP_CompareExpression( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference872) ;
         }
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference873) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference874) ;
         _ebP_Error( "Syntax error." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference875) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference876) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference877) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference878) ;
      if ( _ebV_TokenType == 24 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference879) ;
         _ebV_WorkingEmit += "&& " ;
      } else if ( _ebV_TokenType == 25 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference880) ;
         _ebV_WorkingEmit += "|| " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference881) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference882) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference883) ;
   _ebV_WorkingEmit += ") { " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference884) ;
   static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference885) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference886) ;
      _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference887) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference888) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference889) ;
   _ebP_AddDeBlockedProcedureEmit( _ebV_WorkingEmit ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference890) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference891) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference892) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckWend() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference893) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference894) ;
   if ( _ebP_ElementIndex( _ebP_PeekP( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_BlockLevel)  ) ) == 0 || _ebP_BlockLevelType( ) != 1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference895) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference896) ;
      _ebP_Error( "Syntax error. Wend without a While." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference897) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference898) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference899) ;
   _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference900) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference901) ;
      _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference902) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference903) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference904) ;
   _ebP_AddDeBlockedProcedureEmit( "}" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference905) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference906) ;
   _ebP_DecreaseBlockLevel( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference907) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference908) ;
   _ebV_WorkingEmit = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckImport() {
   std::string _ebV_Path ;
   std::string _ebV_IncludePath ;
   std::string _ebV_Repath ;
   long int _ebV_TokenIndex = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference909) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference910) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference911) ;
   if ( _ebP_TokenType( ) == 49 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference912) ;
      _ebV_Path = _ebP_CurrentMapKey( _ebV_Files ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference913) ;
      _ebV_IncludePath = _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference914) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference915) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference916) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference917) ;
         _ebP_AddProcedureEmit( "// ; Import <" + _ebV_IncludePath + ">" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference918) ;
         _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference919) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference920) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference921) ;
      _ebP_ResetElement( _ebV_ModulePath ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference922) ;
      _ebV_Repath = "" ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference923) ;
      while ( _ebP_NextElement( _ebV_ModulePath ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference924) ;
         if ( _ebP_FileSize( _ebP_CurrentMapKey( _ebV_ModulePath ) + _ebV_IncludePath + ".eb" ) >= 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference925) ;
            _ebV_Repath = _ebP_CurrentMapKey( _ebV_ModulePath ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference926) ;
            break ;;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference927) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference928) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference929) ;
      if ( _ebV_Repath == std::string( "" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference930) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference931) ;
         _ebP_Error( "Module was not found in search paths." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference932) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference933) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference934) ;
      if ( _ebP_FindMapKey( _ebV_Files , _ebV_Repath + _ebV_IncludePath ) == false && _ebP_FindMapKey( _ebV_ImportMap , _ebV_IncludePath ) == false ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference935) ;
         _ebV_TokenIndex = _ebP_ElementIndex( _ebV_ActiveTokenTable ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference936) ;
         _ebP_Print( "Importing " ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference936) ;
         _ebP_ConsoleTextColor( 4 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference936) ;
         _ebP_Print( "<" + _ebV_IncludePath + ">" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference936) ;
         _ebP_ResetConsole( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference936) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference936) ;
         _ebP_Print( "...\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference937) ;
         _ebP_SetMapElement( _ebV_ImportMap , _ebV_IncludePath ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference938) ;
         _ebP_TokenizeSourceFile( _ebV_Repath + _ebV_IncludePath + ".eb" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference939) ;
         _ebP_SetFile( _ebV_Repath + _ebV_IncludePath + ".eb" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference940) ;
         _ebP_ResetElement( _ebV_ActiveTokenTable ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference941) ;
         _ebP_CompileCPP11( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference942) ;
         _ebP_SetFile( _ebV_Path ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference943) ;
         _ebP_SetElementIndex( _ebV_ActiveTokenTable , _ebV_TokenIndex ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference944) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference945) ;
      _ebP_GarbageCheck( ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference946) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference947) ;
      _ebP_ErrorSuggest( "Syntax error. Expected library to import." , "Syntax: Import <Path/File>" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference948) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference949) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_List(char * _ebV_List, std::string _ebV_Commands) {
   signed char _ebV_Result = 0 ;
   signed char _ebV_Offset = 0 ;
   long int _ebV_Length = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference950) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference951) ;
   _ebV_Result = true ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference952) ;
   _ebV_Offset = 0 ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference953) ;
   _ebV_Length = _ebP_Len( _ebV_Commands ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference954) ;
   while ( _ebV_Offset < _ebV_Length ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference955) ;
      if ( _ebP_Left( _ebV_Commands , 1 ) == std::string( "*" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference956) ;
         _ebP_ResetElement( _ebV_List ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference957) ;
         _ebV_Commands = _ebP_Right( _ebV_Commands , _ebV_Length - 1 ) ;
      } else if ( _ebP_Left( _ebV_Commands , 1 ) == std::string( ">" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference958) ;
         _ebV_Result = _ebV_Result & _ebP_NextElement( _ebV_List ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference959) ;
         _ebV_Commands = _ebP_Right( _ebV_Commands , _ebV_Length - 1 ) ;
      } else if ( _ebP_Left( _ebV_Commands , 1 ) == std::string( "<" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference960) ;
         _ebV_Result = _ebV_Result & _ebP_PreviousElement( _ebV_List ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference961) ;
         _ebV_Commands = _ebP_Right( _ebV_Commands , _ebV_Length - 1 ) ;
      } else if ( _ebP_Left( _ebV_Commands , 1 ) == std::string( "!" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference962) ;
         _ebP_SetElementIndex( _ebV_List , _ebP_ElementCount( _ebV_List ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference963) ;
         _ebV_Commands = _ebP_Right( _ebV_Commands , _ebV_Length - 1 ) ;
      } else if ( _ebP_Left( _ebV_Commands , 1 ) == std::string( "&" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference964) ;
         _ebP_SetElementIndex( _ebV_List , 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference965) ;
         _ebV_Commands = _ebP_Right( _ebV_Commands , _ebV_Length - 1 ) ;
      } else if ( _ebP_Left( _ebV_Commands , 1 ) == std::string( "+" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference966) ;
         _ebP_AddListElement( _ebV_List ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference967) ;
         _ebV_Commands = _ebP_Right( _ebV_Commands , _ebV_Length - 1 ) ;
      } else if ( _ebP_Left( _ebV_Commands , 1 ) == std::string( "-" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference968) ;
         _ebP_TruncateListElement( _ebV_List ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference969) ;
         _ebV_Commands = _ebP_Right( _ebV_Commands , _ebV_Length - 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference970) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference971) ;
      _ebV_Offset = _ebV_Offset + 1 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference972) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference973) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebV_Result ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

char * _ebP_NewList(long int _ebV_Size) {
   char * _ebV_List ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference974) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference975) ;
   _ebV_List = _ebP_AllocateMemory( sizeof( struct _ebS_ListRoot)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference976) ;
   _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  , _ebV_Size ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference977) ;
   _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  , 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference978) ;
   _ebP_PokeP( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  , 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference979) ;
   _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_CurrentPosition)  , 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference980) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebV_List ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

char * _ebP_CurrentListElement(char * _ebV_List) {
   long int _ebV_NodeSize = 0 ;
   char * _ebV_NodeRoot ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference981) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference982) ;
   _ebV_NodeSize = _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference983) ;
   _ebV_NodeRoot = _ebP_PeekP( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference984) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebV_NodeRoot + ( _ebV_NodeSize * ( _ebP_PeekL( _ebV_List ) - 1 ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_AddListElement(char * _ebV_List) {
   long int _ebV_Size = 0 ;
   long int _ebV_NodeSize = 0 ;
   char * _ebV_NewNode ;
   char * _ebV_NodeRoot ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference985) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference986) ;
   _ebV_Size = _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference987) ;
   if ( _ebV_Size == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference988) ;
      _ebV_NodeSize = _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference989) ;
      _ebV_NewNode = _ebP_AllocateMemory( _ebV_NodeSize ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference990) ;
      _ebP_PokeP( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  , _ebV_NewNode ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference991) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_CurrentPosition)  , _ebV_Size + 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference992) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  , _ebV_Size + 1 ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference993) ;
      _ebV_NodeSize = _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference994) ;
      _ebV_NodeRoot = _ebP_PeekP( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference995) ;
      _ebV_NewNode = _ebP_ReAllocateMemory( _ebV_NodeRoot , ( _ebV_NodeSize * _ebV_Size ) + _ebV_NodeSize ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference996) ;
      _ebP_PokeP( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  , _ebV_NewNode ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference997) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_CurrentPosition)  , _ebV_Size + 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference998) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  , _ebV_Size + 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference999) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_TruncateListElement(char * _ebV_List) {
   long int _ebV_Size = 0 ;
   long int _ebV_NodeSize = 0 ;
   char * _ebV_NodeRoot ;
   char * _ebV_NewNode ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1000) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1001) ;
   _ebV_Size = _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1002) ;
   if ( _ebV_Size == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1003) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   } else if ( _ebV_Size == 1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1004) ;
      _ebP_FreeMemory( _ebP_PeekP( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1005) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_CurrentPosition)  , 0 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1006) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  , 0 ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1007) ;
      _ebV_NodeSize = _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1008) ;
      _ebV_NodeRoot = _ebP_PeekP( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1009) ;
      _ebV_NewNode = _ebP_ReAllocateMemory( _ebV_NodeRoot , ( _ebV_NodeSize * _ebV_Size ) - _ebV_NodeSize ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1010) ;
      _ebP_PokeP( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  , _ebV_NewNode ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1011) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_CurrentPosition)  , _ebV_Size - 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1012) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  , _ebV_Size - 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1013) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1014) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

char * _ebP_NewMap(long int _ebV_Size) {
   char * _ebV_Map ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1015) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1016) ;
   _ebV_Map = _ebP_AllocateMemory( sizeof( struct _ebS_ListRoot)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1017) ;
   _ebP_PokeL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  , sizeof(char*) + _ebV_Size ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1018) ;
   _ebP_PokeL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  , 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1019) ;
   _ebP_PokeP( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  , 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1020) ;
   _ebP_PokeL( _ebV_Map , 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1021) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebV_Map ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_SetMapElement(char * _ebV_Map, std::string _ebV_Key) {
   long int _ebV_Size = 0 ;
   long int _ebV_NodeSize = 0 ;
   char * _ebV_NewNode ;
   char * _ebV_NodeRoot ;
   signed char _ebV_KeyFound = 0 ;
   long int _ebV_e = 0 ;
   std::string _ebV_CurrentKey ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1022) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1023) ;
   _ebV_Size = _ebP_PeekL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1024) ;
   if ( _ebV_Size == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1025) ;
      _ebV_NodeSize = _ebP_PeekL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1026) ;
      _ebV_NewNode = _ebP_AllocateMemory( _ebV_NodeSize ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1027) ;
      _ebP_PokeP( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  , _ebV_NewNode ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1028) ;
      _ebP_PokeP( _ebV_NewNode , _ebP_DuplicateString( _ebV_Key ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1029) ;
      _ebP_PokeL( _ebV_Map , 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1030) ;
      _ebP_PokeL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  , _ebV_Size + 1 ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1031) ;
      _ebV_NodeSize = _ebP_PeekL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1032) ;
      _ebV_NodeRoot = _ebP_PeekP( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1033) ;
      _ebV_KeyFound = false ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1034) ;
      for ( _ebV_e = 0 ; _ebV_e != _ebV_Size ; _ebV_e = _ebV_e + 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1035) ;
         _ebV_CurrentKey = _ebP_PeekS( _ebP_PeekP( _ebV_NodeRoot + ( _ebV_NodeSize * _ebV_e ) ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1036) ;
         if ( _ebV_CurrentKey == _ebV_Key ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1037) ;
            _ebV_KeyFound = true ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1038) ;
            break ;;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1039) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1040) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1041) ;
      if ( _ebV_KeyFound == false ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1042) ;
         _ebV_NewNode = _ebP_ReAllocateMemory( _ebV_NodeRoot , ( _ebV_NodeSize * _ebV_Size ) + _ebV_NodeSize ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1043) ;
         _ebP_PokeP( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  , _ebV_NewNode ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1044) ;
         _ebP_PokeP( _ebV_NewNode + ( _ebV_NodeSize * _ebV_Size ) , _ebP_DuplicateString( _ebV_Key ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1045) ;
         _ebP_PokeL( _ebV_Map , _ebV_Size + 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1046) ;
         _ebP_PokeL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  , _ebV_Size + 1 ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1047) ;
         _ebP_PokeL( _ebV_Map , _ebV_e + 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1048) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1049) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

int _ebP_FindMapKey(char * _ebV_Map, std::string _ebV_Key) {
   long int _ebV_Size = 0 ;
   signed char _ebV_KeyFound = 0 ;
   long int _ebV_NodeSize = 0 ;
   char * _ebV_NodeRoot ;
   long int _ebV_e = 0 ;
   std::string _ebV_CurrentKey ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1050) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1051) ;
   _ebV_Size = _ebP_PeekL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1052) ;
   if ( _ebV_Size == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1053) ;
      _ebV_KeyFound = false ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1054) ;
      _ebV_NodeSize = _ebP_PeekL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1055) ;
      _ebV_NodeRoot = _ebP_PeekP( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1056) ;
      _ebV_KeyFound = false ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1057) ;
      for ( _ebV_e = 0 ; _ebV_e != _ebV_Size ; _ebV_e = _ebV_e + 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1058) ;
         _ebV_CurrentKey = _ebP_PeekS( _ebP_PeekP( _ebV_NodeRoot + ( _ebV_NodeSize * _ebV_e ) ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1059) ;
         if ( _ebV_CurrentKey == _ebV_Key ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1060) ;
            _ebV_KeyFound = true ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1061) ;
            break ;;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1062) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1063) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1064) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1065) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebV_KeyFound ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

char * _ebP_CurrentMapElement(char * _ebV_Map) {
   long int _ebV_NodeSize = 0 ;
   char * _ebV_NodeRoot ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1066) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1067) ;
   _ebV_NodeSize = _ebP_PeekL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1068) ;
   _ebV_NodeRoot = _ebP_PeekP( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1069) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebV_NodeRoot + ( _ebV_NodeSize * ( _ebP_PeekL( _ebV_Map ) - 1 ) ) + sizeof(char*) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

std::string _ebP_CurrentMapKey(char * _ebV_Map) {
   long int _ebV_NodeSize = 0 ;
   char * _ebV_NodeRoot ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1070) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1071) ;
   _ebV_NodeSize = _ebP_PeekL( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementSize)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1072) ;
   _ebV_NodeRoot = _ebP_PeekP( _ebV_Map + offsetof( struct _ebS_ListRoot, _ebF_ElementRoot)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1073) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekS( _ebP_PeekP( _ebV_NodeRoot + ( _ebV_NodeSize * ( _ebP_PeekL( _ebV_Map ) - 1 ) ) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

int _ebP_NextElement(char * _ebV_List) {
   long int _ebV_Size = 0 ;
   long int _ebV_CurrentElement = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1074) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1075) ;
   _ebV_Size = _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1076) ;
   _ebV_CurrentElement = _ebP_PeekL( _ebV_List ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1077) ;
   if ( _ebV_CurrentElement < _ebV_Size ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1078) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_CurrentPosition)  , _ebV_CurrentElement + 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1079) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return true ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1080) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return false ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1081) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

int _ebP_PreviousElement(char * _ebV_List) {
   long int _ebV_Size = 0 ;
   long int _ebV_CurrentElement = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1082) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1083) ;
   _ebV_Size = _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1084) ;
   _ebV_CurrentElement = _ebP_PeekL( _ebV_List ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1085) ;
   if ( _ebV_CurrentElement > 1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1086) ;
      _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_CurrentPosition)  , _ebV_CurrentElement - 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1087) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return true ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1088) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return false ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1089) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_SetElementIndex(char * _ebV_List, long int _ebV_Index) {
   long int _ebV_Size = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1090) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1091) ;
   _ebV_Size = _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1092) ;
   if ( _ebV_Index > _ebV_Size || _ebV_Index < 1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1093) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1094) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1095) ;
   _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_CurrentPosition)  , _ebV_Index ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1096) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_ResetElement(char * _ebV_List) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1097) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1098) ;
   _ebP_PokeL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_CurrentPosition)  , 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

long int _ebP_ElementCount(char * _ebV_List) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1099) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1100) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekL( _ebV_List + offsetof( struct _ebS_ListRoot, _ebF_ListSize)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

long int _ebP_ElementIndex(char * _ebV_List) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1101) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1102) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekL( _ebV_List ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_Error(std::string _ebV_Error, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1103) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1104) ;
   _ebP_PrintN( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1105) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1105) ;
   _ebP_ConsoleTextColor( 4 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1105) ;
   _ebP_Print( _ebV_Path ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1106) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1106) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1106) ;
   _ebP_Print( ":" + _ebP_Str( _ebV_Line ) + ":" + _ebP_Str( _ebV_Position ) + ": " ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1107) ;
   _ebP_ConsoleTextColor( 1 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1107) ;
   _ebP_Print( "Error" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1108) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1108) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1108) ;
   _ebP_PrintN( ": " + _ebV_Error ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1109) ;
   _ebP_Print( _ebP_Str( _ebV_Line ) + " ||" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1110) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1110) ;
   _ebP_PrintN( " " + _ebV_LineString ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1111) ;
   _ebP_ConsoleTextColor( 1 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1111) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1111) ;
   _ebP_Print( _ebP_Space( _ebP_Len( _ebP_Str( _ebV_Line ) ) - 1 ) + "E ||" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1112) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1112) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1112) ;
   _ebP_PrintN( _ebP_Space( _ebV_Position ) + "^" + _ebP_RepeatString( "~" , _ebP_Len( _ebP_Token( ) ) - 1 ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1113) ;
   _ebP_PrintN( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1114) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_Call, "/home/tyraindreams/Dropbox/Epic Crossings/EpicBasic/EpicBasic/src/ebcompiler/Error.eb:21") ;;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1115) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_Warning(std::string _ebV_Warning, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1116) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1117) ;
   if ( _ebV_WarningThreshold > _ebV_TotalWarnings ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1118) ;
      _ebP_PrintN( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1119) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1119) ;
      _ebP_ConsoleTextColor( 4 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1119) ;
      _ebP_Print( _ebV_Path ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1120) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1120) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1120) ;
      _ebP_Print( ":" + _ebP_Str( _ebV_Line ) + ":" + _ebP_Str( _ebV_Position ) + ": " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1121) ;
      _ebP_ConsoleTextColor( 5 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1121) ;
      _ebP_Print( "Warning" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1122) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1122) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1122) ;
      _ebP_PrintN( ": " + _ebV_Warning ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1123) ;
      _ebP_Print( _ebP_Str( _ebV_Line ) + " ||" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1124) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1124) ;
      _ebP_PrintN( " " + _ebV_LineString ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1125) ;
      _ebP_ConsoleTextColor( 5 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1125) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1125) ;
      _ebP_Print( _ebP_Space( _ebP_Len( _ebP_Str( _ebV_Line ) ) - 1 ) + "W ||" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1126) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1126) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1126) ;
      _ebP_PrintN( _ebP_Space( _ebV_Position ) + "^" + _ebP_RepeatString( "~" , _ebP_Len( _ebP_Token( ) ) - 1 ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1127) ;
      _ebP_PrintN( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1128) ;
      _ebV_TotalWarnings = _ebV_TotalWarnings + 1 ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1129) ;
      if ( _ebV_TotalWarnings == _ebV_WarningThreshold ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1130) ;
         _ebP_ResetConsole( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1130) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1130) ;
         _ebP_PrintN( "\nWarning threshold was reached. No more warnings will be reported.\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1131) ;
      }
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1132) ;
      _ebV_TotalWarnings = _ebV_TotalWarnings + 1 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1133) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1134) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_ErrorNote(std::string _ebV_Note, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1135) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1136) ;
   _ebP_PrintN( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1137) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1137) ;
   _ebP_ConsoleTextColor( 4 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1137) ;
   _ebP_Print( _ebV_Path ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1138) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1138) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1138) ;
   _ebP_Print( ":" + _ebP_Str( _ebV_Line ) + ":" + _ebP_Str( _ebV_Position ) + ": " ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1139) ;
   _ebP_ConsoleTextColor( 7 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1139) ;
   _ebP_Print( "Note" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1140) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1140) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1140) ;
   _ebP_PrintN( ": " + _ebV_Note ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1141) ;
   _ebP_Print( _ebP_Str( _ebV_Line ) + " ||" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1142) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1142) ;
   _ebP_PrintN( " " + _ebV_LineString ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1143) ;
   _ebP_ConsoleTextColor( 7 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1143) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1143) ;
   _ebP_Print( _ebP_Space( _ebP_Len( _ebP_Str( _ebV_Line ) ) - 1 ) + "N ||" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1144) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1144) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1144) ;
   _ebP_PrintN( _ebP_Space( _ebV_Position ) + "^" + _ebP_RepeatString( "~" , _ebP_Len( _ebP_Token( ) ) - 1 ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1145) ;
   _ebP_PrintN( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1146) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_Note(std::string _ebV_Note, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1147) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1148) ;
   if ( _ebV_WarningThreshold > _ebV_TotalWarnings ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1149) ;
      _ebP_PrintN( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1150) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1150) ;
      _ebP_ConsoleTextColor( 4 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1150) ;
      _ebP_Print( _ebV_Path ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1151) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1151) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1151) ;
      _ebP_Print( ":" + _ebP_Str( _ebV_Line ) + ":" + _ebP_Str( _ebV_Position ) + ": " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1152) ;
      _ebP_ConsoleTextColor( 7 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1152) ;
      _ebP_Print( "Note" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1153) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1153) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1153) ;
      _ebP_PrintN( ": " + _ebV_Note ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1154) ;
      _ebP_Print( _ebP_Str( _ebV_Line ) + " ||" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1155) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1155) ;
      _ebP_PrintN( " " + _ebV_LineString ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1156) ;
      _ebP_ConsoleTextColor( 7 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1156) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1156) ;
      _ebP_Print( _ebP_Space( _ebP_Len( _ebP_Str( _ebV_Line ) ) - 1 ) + "N ||" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1157) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1157) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1157) ;
      _ebP_PrintN( _ebP_Space( _ebV_Position ) + "^" + _ebP_RepeatString( "~" , _ebP_Len( _ebP_Token( ) ) - 1 ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1158) ;
      _ebP_PrintN( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1159) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1160) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_ErrorSuggest(std::string _ebV_Error, std::string _ebV_Suggest, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1161) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1162) ;
   _ebP_PrintN( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1163) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1163) ;
   _ebP_ConsoleTextColor( 4 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1163) ;
   _ebP_Print( _ebV_Path ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1164) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1164) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1164) ;
   _ebP_Print( ":" + _ebP_Str( _ebV_Line ) + ":" + _ebP_Str( _ebV_Position ) + ": " ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1165) ;
   _ebP_ConsoleTextColor( 1 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1165) ;
   _ebP_Print( "Error" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1166) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1166) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1166) ;
   _ebP_PrintN( ": " + _ebV_Error ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1167) ;
   _ebP_Print( _ebP_Str( _ebV_Line ) + " ||" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1168) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1168) ;
   _ebP_PrintN( " " + _ebV_LineString ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1169) ;
   _ebP_ConsoleTextColor( 1 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1169) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1169) ;
   _ebP_Print( _ebP_Space( _ebP_Len( _ebP_Str( _ebV_Line ) ) - 1 ) + "E ||" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1170) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1170) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1170) ;
   _ebP_PrintN( _ebP_Space( _ebV_Position ) + "^" + _ebP_RepeatString( "~" , _ebP_Len( _ebP_Token( ) ) - 1 ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1171) ;
   _ebP_ConsoleTextColor( 2 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1171) ;
   _ebP_Print( _ebP_Space( _ebP_Len( _ebP_Str( _ebV_Line ) ) - 1 ) + "S ||" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1172) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1172) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1172) ;
   _ebP_PrintN( _ebP_Space( _ebV_Position ) + _ebV_Suggest ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1173) ;
   _ebP_PrintN( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1174) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_WarningSuggest(std::string _ebV_Warning, std::string _ebV_Suggest, std::string _ebV_Path, std::string _ebV_LineString, long int _ebV_Line, long int _ebV_Position) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1175) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1176) ;
   if ( _ebV_WarningThreshold > _ebV_TotalWarnings ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1177) ;
      _ebP_PrintN( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1178) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1178) ;
      _ebP_ConsoleTextColor( 4 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1178) ;
      _ebP_Print( _ebV_Path ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1179) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1179) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1179) ;
      _ebP_Print( ":" + _ebP_Str( _ebV_Line ) + ":" + _ebP_Str( _ebV_Position ) + ": " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1180) ;
      _ebP_ConsoleTextColor( 5 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1180) ;
      _ebP_Print( "Warning" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1181) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1181) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1181) ;
      _ebP_PrintN( ": " + _ebV_Warning ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1182) ;
      _ebP_Print( _ebP_Str( _ebV_Line ) + " ||" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1183) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1183) ;
      _ebP_PrintN( " " + _ebV_LineString ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1184) ;
      _ebP_ConsoleTextColor( 5 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1184) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1184) ;
      _ebP_Print( _ebP_Space( _ebP_Len( _ebP_Str( _ebV_Line ) ) - 1 ) + "W ||" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1185) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1185) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1185) ;
      _ebP_PrintN( _ebP_Space( _ebV_Position ) + "^" + _ebP_RepeatString( "~" , _ebP_Len( _ebP_Token( ) ) - 1 ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1186) ;
      _ebP_ConsoleTextColor( 2 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1186) ;
      _ebP_Print( _ebP_Space( _ebP_Len( _ebP_Str( _ebV_Line ) ) - 1 ) + "S ||" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1187) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1187) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1187) ;
      _ebP_PrintN( _ebP_Space( _ebV_Position ) + _ebV_Suggest ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1188) ;
      _ebP_PrintN( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1189) ;
      _ebV_TotalWarnings = _ebV_TotalWarnings + 1 ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1190) ;
      if ( _ebV_TotalWarnings == _ebV_WarningThreshold ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1191) ;
         _ebP_ResetConsole( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1191) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1191) ;
         _ebP_PrintN( "\nWarning threshold was reached. No more warnings will be reported.\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1192) ;
      }
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1193) ;
      _ebV_TotalWarnings = _ebV_TotalWarnings + 1 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1194) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1195) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_TokenizeSourceFile(std::string _ebV_Path) {
   char * _ebV_CurrentFilesElement ;
   int _ebV_File = 0 ;
   char * _ebV_Tokens ;
   char * _ebV_Lines ;
   long int _ebV_Line = 0 ;
   std::string _ebV_CurrentLine ;
   char * _ebV_CurrentLinesElement ;
   signed char _ebV_TokenType = 0 ;
   char * _ebV_CurrentElement ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1196) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1197) ;
   if ( _ebP_FindMapKey( _ebV_Files , _ebV_Path ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1198) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1199) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1200) ;
   _ebP_SetMapElement( _ebV_Files , _ebV_Path ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1201) ;
   _ebV_CurrentFilesElement = _ebP_CurrentMapElement( _ebV_Files ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1202) ;
   _ebV_File = _ebP_ReadFile( _ebV_Path ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1203) ;
   if ( _ebV_File == -1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1204) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1204) ;
      _ebP_ConsoleTextColor( 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1204) ;
      _ebP_Print( "Error" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1205) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1205) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1205) ;
      _ebP_PrintN( ": Unable to open file: " + _ebV_Path ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1206) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1207) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1208) ;
   _ebV_Tokens = _ebP_NewList( sizeof(char*) + sizeof(char) + sizeof(long) + sizeof(long) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1209) ;
   _ebV_Lines = _ebP_NewList( sizeof(char*) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1210) ;
   _ebP_PokeP( _ebV_CurrentFilesElement + offsetof( struct _ebS_Files, _ebF_Tokens)  , _ebV_Tokens ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1211) ;
   _ebP_PokeP( _ebV_CurrentFilesElement + offsetof( struct _ebS_Files, _ebF_Lines)  , _ebV_Lines ) ;
   //    Line.l
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1212) ;
   while ( _ebP_Eof( _ebV_File ) != true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1213) ;
      _ebV_Line = _ebV_Line + 1 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1214) ;
      _ebV_CurrentLine = _ebP_ReadString( _ebV_File ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1215) ;
      _ebP_AddListElement( _ebV_Lines ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1216) ;
      _ebV_CurrentLinesElement = _ebP_CurrentListElement( _ebV_Lines ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1217) ;
      _ebP_PokeP( _ebV_CurrentLinesElement , _ebP_DuplicateString( _ebV_CurrentLine ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1218) ;
      if ( _ebP_ExamineRegularExpression( _ebV_Regex , _ebV_CurrentLine ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1219) ;
         while ( _ebP_NextRegularExpressionMatch( _ebV_Regex ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1220) ;
            _ebV_TokenType = _ebP_RegularExpressionMatchGroup( _ebV_Regex ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1221) ;
            if ( _ebV_TokenType == 82 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1222) ;
               _ebP_Error( "Syntax Error. Unknown token." , _ebV_Path , _ebV_CurrentLine , _ebV_Line , _ebP_RegularExpressionMatchPosition( _ebV_Regex ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1223) ;
               exit( EXIT_FAILURE ) ;
            } else if ( _ebV_TokenType != 1 ) { 
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1224) ;
               if ( _ebV_TokenType == 2 && _ebV_CommentPassthrough == false ) { 
               } else { 
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1225) ;
                  _ebP_AddListElement( _ebV_Tokens ) ;
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1226) ;
                  _ebV_CurrentElement = _ebP_CurrentListElement( _ebV_Tokens ) ;
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1227) ;
                  _ebP_PokeP( _ebV_CurrentElement , _ebP_DuplicateString( _ebP_RegularExpressionMatchString( _ebV_Regex ) ) ) ;
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1228) ;
                  _ebP_PokeB( _ebV_CurrentElement + sizeof(char*) , _ebV_TokenType ) ;
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1229) ;
                  _ebP_PokeL( _ebV_CurrentElement + sizeof(char*) + sizeof(char) , _ebV_Line ) ;
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1230) ;
                  _ebP_PokeL( _ebV_CurrentElement + sizeof(char*) + sizeof(char) + sizeof(long) , _ebP_RegularExpressionMatchPosition( _ebV_Regex ) ) ;
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1231) ;
               }
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1232) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1233) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1234) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1235) ;
      _ebP_AddListElement( _ebV_Tokens ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1236) ;
      _ebV_CurrentElement = _ebP_CurrentListElement( _ebV_Tokens ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1237) ;
      _ebP_PokeP( _ebV_CurrentElement , _ebP_DuplicateString( ":" ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1238) ;
      _ebP_PokeB( _ebV_CurrentElement + sizeof(char*) , 81 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1239) ;
      _ebP_PokeL( _ebV_CurrentElement + sizeof(char*) + sizeof(char) , _ebV_Line ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1240) ;
      _ebP_PokeL( _ebV_CurrentElement + sizeof(char*) + sizeof(char) + sizeof(long) , _ebP_Len( _ebV_CurrentLine ) + 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1241) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1242) ;
   _ebP_ResetElement( _ebV_Tokens ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1243) ;
   _ebP_ResetElement( _ebV_Lines ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1244) ;
   _ebV_TotalLines = _ebV_TotalLines + _ebV_Line ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1245) ;
   _ebP_CloseFile( _ebV_File ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_SetFile(std::string _ebV_Path) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1246) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1247) ;
   _ebP_SetMapElement( _ebV_Files , _ebV_Path ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1248) ;
   _ebV_ActiveTokenTable = _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Tokens)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1249) ;
   _ebP_ResetElement( _ebV_ActiveTokenTable ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1250) ;
   _ebV_ActiveLines = _ebP_PeekP( _ebP_CurrentMapElement( _ebV_Files ) + offsetof( struct _ebS_Files, _ebF_Lines)  ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

std::string _ebP_Token() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1251) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1252) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveTokenTable ) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_TokenType() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1253) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1254) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebP_CurrentListElement( _ebV_ActiveTokenTable ) + sizeof(char*) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

long int _ebP_TokenLine() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1255) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1256) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekL( _ebP_CurrentListElement( _ebV_ActiveTokenTable ) + sizeof(char*) + sizeof(char) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

long int _ebP_TokenPosition() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1257) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1258) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekL( _ebP_CurrentListElement( _ebV_ActiveTokenTable ) + sizeof(char*) + sizeof(char) + sizeof(long) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_SeekToken() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1259) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1260) ;
   while ( _ebP_NextElement( _ebV_ActiveTokenTable ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1261) ;
      if ( _ebP_TokenType( ) == 2 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1262) ;
         _ebP_AddProcedureEmit( "// " + _ebP_Token( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1263) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1264) ;
      if ( _ebP_TokenType( ) != 81 && _ebP_TokenType( ) != 2 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1265) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return true ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1266) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1267) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1268) ;
   _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1269) ;
   _ebP_Error( "Unexpected end of file." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1270) ;
   exit( EXIT_FAILURE ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_NextToken() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1271) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1272) ;
   if ( _ebP_NextElement( _ebV_ActiveTokenTable ) == false || _ebP_TokenType( ) == 81 || _ebP_TokenType( ) == 2 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1273) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1274) ;
      _ebP_Error( "Unexpected end of line." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1275) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1276) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_CheckType() {
   std::string _ebV_CurrentToken ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1277) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1278) ;
   _ebV_CurrentToken = _ebP_LCase( _ebP_Token( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1279) ;
   if ( _ebV_CurrentToken == std::string( ".b" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1280) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 1 ;
   } else if ( _ebV_CurrentToken == std::string( ".c" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1281) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 2 ;
   } else if ( _ebV_CurrentToken == std::string( ".w" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1282) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 3 ;
   } else if ( _ebV_CurrentToken == std::string( ".i" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1283) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 4 ;
   } else if ( _ebV_CurrentToken == std::string( ".l" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1284) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 5 ;
   } else if ( _ebV_CurrentToken == std::string( ".q" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1285) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 6 ;
   } else if ( _ebV_CurrentToken == std::string( ".p" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1286) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 7 ;
   } else if ( _ebV_CurrentToken == std::string( ".f" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1287) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 8 ;
   } else if ( _ebV_CurrentToken == std::string( ".d" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1288) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 9 ;
   } else if ( _ebV_CurrentToken == std::string( ".s" ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1289) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 10 ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1290) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return 11 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1291) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_GarbageCheck() {
   signed char _ebV_TokenType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1292) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1293) ;
   if ( _ebP_NextElement( _ebV_ActiveTokenTable ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1294) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1295) ;
      if ( _ebV_TokenType == 2 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1296) ;
         if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1297) ;
            _ebV_WorkingEmit += "; // " + _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1298) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1299) ;
         static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1300) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      } else if ( _ebV_TokenType == 81 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1301) ;
         if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1302) ;
            _ebV_WorkingEmit += ";" ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1303) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1304) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1305) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1306) ;
         _ebP_Error( "Garbage at end of line." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1307) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1308) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1309) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_BlockConstant() {
   std::string _ebV_ConstantName ;
   signed char _ebV_CurrentTokenType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1310) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1311) ;
   while ( _ebP_TokenType( ) == 55 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1312) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1313) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1314) ;
         _ebP_AddProcedureEmit( "// " + _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1315) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1316) ;
      _ebV_ConstantName = _ebP_Token( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1317) ;
      _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1318) ;
      if ( _ebP_TokenType( ) != 65 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1319) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1320) ;
         _ebP_ErrorSuggest( "Syntax error. Constant must be assigned a literal value." , "Syntax: '#ConstantName = 1' Or '#ConstantName = 1.5' Or '#ConstantName = \"String\"'" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1321) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1322) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1323) ;
      _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1324) ;
      _ebV_CurrentTokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1325) ;
      if ( _ebV_CurrentTokenType == 53 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1326) ;
         _ebP_CreateConstant( _ebV_ConstantName , 5 , _ebP_Token( ) , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 0 ) ;
      } else if ( _ebV_CurrentTokenType == 52 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1327) ;
         _ebP_CreateConstant( _ebV_ConstantName , 8 , _ebP_Token( ) , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 0 ) ;
      } else if ( _ebV_CurrentTokenType == 48 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1328) ;
         _ebP_CreateConstant( _ebV_ConstantName , 10 , _ebP_Token( ) , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 0 ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1329) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1330) ;
         _ebP_ErrorSuggest( "Syntax error. Constant must be assigned a literal value." , "Syntax: #ConstantName = 1 Or 1.5 Or \"String\"" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1331) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1332) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1333) ;
      _ebP_GarbageCheck( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1334) ;
      static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1335) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1336) ;
   static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1337) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_BlockEnumeration() {
   long int _ebV_EnumerateValue = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1338) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1339) ;
   _ebV_EnumerateValue = 0 ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1340) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1341) ;
   static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1342) ;
   while ( _ebP_TokenType( ) != 11 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1343) ;
      if ( _ebP_TokenType( ) == 55 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1344) ;
         _ebP_CreateConstant( _ebP_Token( ) , 5 , _ebP_Str( _ebV_EnumerateValue ) , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 0 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1345) ;
         _ebV_EnumerateValue = _ebV_EnumerateValue + 1 ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1346) ;
         _ebP_GarbageCheck( ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1347) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1348) ;
         _ebP_Error( "Syntax error. Only constants can be enumerated." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1349) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1350) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1351) ;
      static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1352) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1353) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1354) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_ConfigureConstant() {
   signed char _ebV_CurrentType = 0 ;
   std::string _ebV_ConstantName ;
   std::string _ebV_Value ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1355) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1356) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1357) ;
   if ( _ebP_TokenType( ) != 51 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1358) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1359) ;
      _ebP_ErrorSuggest( "Syntax error. Expected constant type." , "Syntax: CompilerConfigure.type #ConstantName = \"Default Value\"" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1360) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1361) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1362) ;
   _ebV_CurrentType = _ebP_CheckType( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1363) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1364) ;
   if ( _ebP_TokenType( ) != 55 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1365) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1366) ;
      _ebP_ErrorSuggest( "Syntax error. Expected constant name." , "Syntax: CompilerConfigure.type #ConstantName = \"Default Value\"" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1367) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1368) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1369) ;
   _ebV_ConstantName = _ebP_Token( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1370) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1371) ;
   if ( _ebP_TokenType( ) != 65 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1372) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1373) ;
      _ebP_ErrorSuggest( "Syntax error. Constant must be assigned a default value." , "Syntax: CompilerConfigure.type #ConstantName = \"Default Value\"" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1374) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1375) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1376) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1377) ;
   if ( _ebP_TokenType( ) != 48 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1378) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1379) ;
      _ebP_ErrorSuggest( "Syntax error. Constant must be assigned a default value." , "Syntax: CompilerConfigure.type #ConstantName = \"Default Value\"" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1380) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1381) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1382) ;
   if ( _ebV_ConfigureDefaults ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1383) ;
      _ebP_Print( _ebV_ConstantName + "=[" + _ebP_Token( ) + "]? " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1384) ;
      _ebV_Value = _ebP_Input( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1385) ;
      if ( _ebP_Len( _ebV_Value ) == 0 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1386) ;
         _ebV_Value = _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1387) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1388) ;
      if ( _ebV_CurrentType == 10 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1389) ;
         _ebP_CreateConstant( _ebV_ConstantName , _ebV_CurrentType , "\"" + _ebV_Value + "\"" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 1 ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1390) ;
         _ebP_CreateConstant( _ebV_ConstantName , _ebV_CurrentType , _ebV_Value , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1391) ;
      }
   } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1392) ;
      if ( _ebV_CurrentType == 10 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1393) ;
         _ebP_CreateConstant( _ebV_ConstantName , _ebV_CurrentType , _ebP_Token( ) , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 1 ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1394) ;
         _ebP_CreateConstant( _ebV_ConstantName , _ebV_CurrentType , _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1395) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1396) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1397) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

signed char _ebP_ConstantType(std::string _ebV_ConstantName) {
   char * _ebV_CurrentConstant ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1398) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1399) ;
   if ( _ebP_FindMapKey( _ebV_ConstantMap , _ebV_ConstantName ) == false ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1400) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1401) ;
      _ebP_Error( _ebV_ConstantName + " is not a declared constant." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1402) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1403) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1404) ;
   _ebP_SetMapElement( _ebV_ConstantMap , _ebV_ConstantName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1405) ;
   _ebV_CurrentConstant = _ebP_CurrentMapElement( _ebV_ConstantMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1406) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekB( _ebV_CurrentConstant + sizeof(char) + sizeof(char*) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

std::string _ebP_ConstantMacro(std::string _ebV_ConstantName) {
   char * _ebV_CurrentConstant ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1407) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1408) ;
   if ( _ebP_FindMapKey( _ebV_ConstantMap , _ebV_ConstantName ) == false ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1409) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1410) ;
      _ebP_Error( _ebV_ConstantName + " is not a declared constant." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1411) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1412) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1413) ;
   _ebP_SetMapElement( _ebV_ConstantMap , _ebV_ConstantName ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1414) ;
   _ebV_CurrentConstant = _ebP_CurrentMapElement( _ebV_ConstantMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1415) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return _ebP_PeekS( _ebP_PeekP( _ebV_CurrentConstant + sizeof(char) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_BlockComment() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1416) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1417) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1418) ;
   _ebP_AddProcedureEmit( "// " + _ebP_Token( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1419) ;
   static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1420) ;
   while ( _ebP_NextElement( _ebV_ActiveTokenTable ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1421) ;
      if ( _ebP_TokenType( ) == 2 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1422) ;
         _ebP_AddProcedureEmit( "// " + _ebP_Token( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1423) ;
         static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1424) ;
         static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1425) ;
         _ebP_AddProcedureEmit( "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1426) ;
         break ;;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1427) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1428) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1429) ;
   _ebP_AddProcedureEmit( "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CallProcedure() {
   std::string _ebV_ProcedureCall ;
   long int _ebV_p = 0 ;
   long int _ebV_ParameterCount = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1430) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1431) ;
   _ebV_ProcedureCall = _ebP_Token( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1432) ;
   _ebV_p = 0 ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1433) ;
   _ebV_ParameterCount = _ebP_ProcedureParameterCount( _ebV_ProcedureCall ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1434) ;
   _ebV_WorkingEmit += "_ebP_" + _ebV_ProcedureCall + "( " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1435) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1436) ;
   if ( _ebV_ParameterCount == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1437) ;
      _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1438) ;
      if ( _ebP_TokenType( ) == 61 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1439) ;
         _ebV_WorkingEmit += ") " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1440) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      } else if ( _ebP_TokenType( ) != 61 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1441) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1442) ;
         _ebP_Error( "Syntax error. Incorrect number of parameters." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1443) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1444) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1445) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1446) ;
   while ( 1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1447) ;
      _ebV_p = _ebV_p + 1 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1448) ;
      static_cast<void>( _ebP_SeekToken( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1449) ;
      if ( _ebP_ProcedureParameterType( _ebV_ProcedureCall , _ebV_p ) == 10 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1450) ;
         _ebP_CheckConcatenate( ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1451) ;
         _ebP_CheckExpression( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1452) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1453) ;
      if ( _ebP_TokenType( ) == 64 && _ebV_p >= _ebV_ParameterCount ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1454) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1455) ;
         _ebP_Error( "Syntax error. Incorrect number of parameters." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1456) ;
         exit( EXIT_FAILURE ) ;
      } else if ( _ebP_TokenType( ) == 64 && _ebV_p < _ebV_ParameterCount ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1457) ;
         _ebV_WorkingEmit += ", " ;
      } else if ( _ebP_TokenType( ) == 61 && _ebV_p != _ebV_ParameterCount ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1458) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1459) ;
         _ebP_Error( "Syntax error. Incorrect number of parameters." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1460) ;
         exit( EXIT_FAILURE ) ;
      } else if ( _ebP_TokenType( ) == 61 && _ebV_p == _ebV_ParameterCount ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1461) ;
         _ebV_WorkingEmit += ") " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1462) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      } else if ( _ebP_TokenType( ) == 81 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1463) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1464) ;
         _ebP_Error( "Unexpected end of line." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1465) ;
         exit( EXIT_FAILURE ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1466) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1467) ;
         _ebP_Error( "Syntax error. Expected delimiter." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1468) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1469) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1470) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckProcedure() {
   signed char _ebV_ReturnType = 0 ;
   std::string _ebV_OldProcedure ;
   std::string _ebV_NewProcedure ;
   long int _ebV_p = 0 ;
   long int _ebV_ParameterCount = 0 ;
   signed char _ebV_TokenType = 0 ;
   std::string _ebV_ParameterName ;
   signed char _ebV_ParameterType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1471) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1472) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1473) ;
   if ( _ebP_TokenType( ) == 51 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1474) ;
      _ebV_ReturnType = _ebP_CheckType( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1475) ;
      _ebP_NextToken( ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1476) ;
      _ebV_ReturnType = 0 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1477) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1478) ;
   if ( _ebP_TokenType( ) != 4 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1479) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1480) ;
      _ebP_ErrorSuggest( "Syntax error. Expected procedure name. " , "Syntax: Procedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1481) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1482) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1483) ;
   _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1484) ;
   if ( _ebV_CommentPassthrough == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1485) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1486) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1487) ;
      _ebP_AddProcedureEmit( "// ; Procedure Definition " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1488) ;
      _ebP_AddProcedureEmit( "// ;    " + _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1489) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1490) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1491) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1492) ;
   _ebV_OldProcedure = _ebP_CurrentMapKey( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1493) ;
   _ebV_NewProcedure = _ebP_Token( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1494) ;
   if ( _ebP_ProcedureDeclared( _ebV_NewProcedure ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1495) ;
      if ( _ebP_ProcedureReturnType( _ebV_NewProcedure ) != _ebV_ReturnType ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1496) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1497) ;
         _ebP_Error( "Procedure definition does not match declared procedure's return type." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1498) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1499) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1500) ;
      _ebV_p = 0 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1501) ;
      _ebV_ParameterCount = _ebP_ProcedureParameterCount( _ebV_NewProcedure ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1502) ;
      _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1503) ;
      if ( _ebV_ParameterCount == 0 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1504) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1505) ;
         if ( _ebP_TokenType( ) == 61 ) { 
         } else if ( _ebP_TokenType( ) != 61 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1506) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1507) ;
            _ebP_Error( "Procedure definition does not match declared procedure's parameter count." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1508) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1509) ;
         }
      } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1510) ;
         while ( 1 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1511) ;
            _ebV_p = _ebV_p + 1 ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1512) ;
            _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1513) ;
            _ebP_NextToken( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1514) ;
            if ( _ebP_ProcedureParameterType( _ebV_NewProcedure , _ebV_p ) != _ebP_CheckType( ) ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1515) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1516) ;
               _ebP_Error( "Procedure definition does not match declared procedure's parameter type." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1517) ;
               exit( EXIT_FAILURE ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1518) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1519) ;
            _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1520) ;
            _ebV_TokenType = _ebP_TokenType( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1521) ;
            if ( _ebV_TokenType == 64 && _ebV_p >= _ebV_ParameterCount ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1522) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1523) ;
               _ebP_Error( "Procedure definition does not match declared procedure's parameter count." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1524) ;
               exit( EXIT_FAILURE ) ;
            } else if ( _ebV_TokenType == 64 && _ebV_p < _ebV_ParameterCount ) { 
            } else if ( _ebV_TokenType == 61 && _ebV_p != _ebV_ParameterCount ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1525) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1526) ;
               _ebP_Error( "Procedure definition does not match declared procedure's parameter count." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1527) ;
               exit( EXIT_FAILURE ) ;
            } else if ( _ebV_TokenType == 61 && _ebV_p == _ebV_ParameterCount ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1528) ;
               break ;;
            } else if ( _ebV_TokenType == 81 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1529) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1530) ;
               _ebP_Error( "Unexpected end of line." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1531) ;
               exit( EXIT_FAILURE ) ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1532) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1533) ;
               _ebP_Error( "Syntax error. Expected delimiter." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1534) ;
               exit( EXIT_FAILURE ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1535) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1536) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1537) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1538) ;
      _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_NewProcedure ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1539) ;
      _ebV_ActiveProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1540) ;
      _ebP_PokeB( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Declared)  , false ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1541) ;
      _ebP_CreateProcedure( _ebV_NewProcedure , _ebV_ReturnType , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 0 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1542) ;
      _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1543) ;
      _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1544) ;
      if ( _ebP_TokenType( ) != 61 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1545) ;
         static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1546) ;
         while ( _ebP_TokenType( ) != 61 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1547) ;
            _ebP_NextToken( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1548) ;
            if ( _ebP_TokenType( ) != 59 && _ebP_TokenType( ) != 58 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1549) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1550) ;
               _ebP_ErrorSuggest( "Syntax error. Expected parameter name. " , "Syntax: Procedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1551) ;
               exit( EXIT_FAILURE ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1552) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1553) ;
            _ebV_ParameterName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1554) ;
            _ebP_NextToken( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1555) ;
            if ( _ebP_TokenType( ) != 51 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1556) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1557) ;
               _ebP_ErrorSuggest( "Syntax error. Expected parameter type. " , "Syntax: Procedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1558) ;
               exit( EXIT_FAILURE ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1559) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1560) ;
            _ebV_ParameterType = _ebP_CheckType( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1561) ;
            _ebP_AddProcedureParameter( _ebV_ParameterName , _ebV_ParameterType ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1562) ;
            _ebP_NextToken( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1563) ;
            if ( _ebP_TokenType( ) != 64 && _ebP_TokenType( ) != 61 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1564) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1565) ;
               _ebP_ErrorSuggest( "Syntax error. Expected delimiter {,} or closing parenthesis {)}. " , "Syntax: Procedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1566) ;
               exit( EXIT_FAILURE ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1567) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1568) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1569) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1570) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1571) ;
   _ebP_GarbageCheck( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1572) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1573) ;
      _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebV_NewProcedure + "():" + _ebP_Str( _ebP_TokenLine( ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1574) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1575) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1576) ;
   _ebP_CompileCPP11( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1577) ;
   _ebP_CheckBlockLevel( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1578) ;
   _ebP_QueueProcedure( _ebV_NewProcedure ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1579) ;
   if ( _ebP_TokenType( ) != 34 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1580) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1581) ;
      _ebP_Error( "Syntax error. Procedure declarations must end in the same file they are declared in. " , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1582) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1583) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1584) ;
   _ebP_GarbageCheck( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1585) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1586) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, \"\") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1587) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1588) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_OldProcedure ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1589) ;
   _ebV_ActiveProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckDeclare() {
   std::string _ebV_ResetProcedure ;
   signed char _ebV_ReturnType = 0 ;
   std::string _ebV_OldProcedure ;
   std::string _ebV_NewProcedure ;
   std::string _ebV_ParameterName ;
   signed char _ebV_ParameterType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1590) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1591) ;
   _ebV_ResetProcedure = _ebP_CurrentMapKey( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1592) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1593) ;
   if ( _ebP_TokenType( ) == 51 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1594) ;
      _ebV_ReturnType = _ebP_CheckType( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1595) ;
      _ebP_NextToken( ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1596) ;
      _ebV_ReturnType = 0 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1597) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1598) ;
   if ( _ebP_TokenType( ) != 4 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1599) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1600) ;
      _ebP_ErrorSuggest( "Syntax error. Expected procedure name. " , "Syntax: Procedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1601) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1602) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1603) ;
   _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1604) ;
   if ( _ebV_CommentPassthrough == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1605) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1606) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1607) ;
      _ebP_AddProcedureEmit( "// ; Procedure Declaration " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1608) ;
      _ebP_AddProcedureEmit( "// ;    " + _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1609) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1610) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1611) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1612) ;
   _ebV_OldProcedure = _ebP_CurrentMapKey( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1613) ;
   _ebV_NewProcedure = _ebP_Token( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1614) ;
   _ebP_CreateProcedure( _ebV_NewProcedure , _ebV_ReturnType , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_TokenLine( ) , 0 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1615) ;
   _ebP_NextToken( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1616) ;
   _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1617) ;
   if ( _ebP_TokenType( ) != 61 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1618) ;
      static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1619) ;
      while ( _ebP_TokenType( ) != 61 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1620) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1621) ;
         if ( _ebP_TokenType( ) != 59 && _ebP_TokenType( ) != 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1622) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1623) ;
            _ebP_ErrorSuggest( "Syntax error. Expected parameter name. " , "Syntax: Procedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1624) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1625) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1626) ;
         _ebV_ParameterName = _ebP_Token( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1627) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1628) ;
         if ( _ebP_TokenType( ) != 51 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1629) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1630) ;
            _ebP_ErrorSuggest( "Syntax error. Expected parameter type. " , "Syntax: Procedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1631) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1632) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1633) ;
         _ebV_ParameterType = _ebP_CheckType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1634) ;
         _ebP_AddProcedureParameter( _ebV_ParameterName , _ebV_ParameterType ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1635) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1636) ;
         if ( _ebP_TokenType( ) != 64 && _ebP_TokenType( ) != 61 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1637) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1638) ;
            _ebP_ErrorSuggest( "Syntax error. Expected delimiter {,} or closing parenthesis {)}. " , "Syntax: Procedure{.type} ProcedureName(Parameter1{.type}, Parameter2{.type})" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1639) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1640) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1641) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1642) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1643) ;
   _ebP_GarbageCheck( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1644) ;
   _ebP_PokeB( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_Declared)  , true ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1645) ;
   _ebP_SetMapElement( _ebV_ProcedureMap , _ebV_ResetProcedure ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1646) ;
   _ebV_ActiveProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckProcedureReturn() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1647) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1648) ;
   _ebV_WorkingEmit = "return " ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1649) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1650) ;
      _ebV_WorkingEmit = "_ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, \"\") ; " + _ebV_WorkingEmit ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1651) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1652) ;
   if ( _ebP_ProcedureReturnType( _ebP_CurrentMapKey( _ebV_ProcedureMap ) ) == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1653) ;
      _ebP_GarbageCheck( ) ;
   } else if ( _ebP_ProcedureReturnType( _ebP_CurrentMapKey( _ebV_ProcedureMap ) ) == 10 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1654) ;
      _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1655) ;
      _ebP_CheckConcatenate( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1656) ;
      if ( _ebP_TokenType( ) == 2 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1657) ;
         if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1658) ;
            _ebV_WorkingEmit += "; // " + _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1659) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1660) ;
         static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
      } else if ( _ebP_TokenType( ) == 81 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1661) ;
         if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1662) ;
            _ebV_WorkingEmit += ";" ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1663) ;
         }
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1664) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1665) ;
         _ebP_Error( "Garbage at end of line." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1666) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1667) ;
      }
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1668) ;
      _ebP_NextToken( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1669) ;
      _ebP_CheckExpression( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1670) ;
      if ( _ebP_TokenType( ) == 2 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1671) ;
         if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1672) ;
            _ebV_WorkingEmit += "; // " + _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1673) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1674) ;
         static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
      } else if ( _ebP_TokenType( ) == 81 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1675) ;
         if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1676) ;
            _ebV_WorkingEmit += ";" ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1677) ;
         }
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1678) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1679) ;
         _ebP_Error( "Garbage at end of line." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1680) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1681) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1682) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckEnd() {
   signed char _ebV_TokenType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1683) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1684) ;
   _ebV_WorkingEmit = "exit( " ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1685) ;
   static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1686) ;
   _ebV_TokenType = _ebP_TokenType( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1687) ;
   if ( _ebV_TokenType == 2 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1688) ;
      if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1689) ;
         _ebV_WorkingEmit += "0 ) ; // " + _ebP_Token( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1690) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1691) ;
      static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
   } else if ( _ebV_TokenType == 81 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1692) ;
      if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1693) ;
         _ebV_WorkingEmit += "0 ) ;" ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1694) ;
      }
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1695) ;
      _ebP_CheckExpression( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1696) ;
      _ebV_WorkingEmit += ") " ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1697) ;
      static_cast<void>( _ebP_PreviousElement( _ebV_ActiveTokenTable ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1698) ;
      _ebP_GarbageCheck( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1699) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckExpression() {
   long int _ebV_ExpressionScope = 0 ;
   signed char _ebV_TokenType = 0 ;
   std::string _ebV_ProcedureName ;
   std::string _ebV_VariableName ;
   signed char _ebV_Reference = 0 ;
   signed char _ebV_CurrentType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1700) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1701) ;
   _ebV_ExpressionScope = 0 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1702) ;
   while ( 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1703) ;
      while ( _ebP_TokenType( ) == 60 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1704) ;
         _ebV_ExpressionScope = _ebV_ExpressionScope + 1 ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1705) ;
         _ebV_WorkingEmit += "( " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1706) ;
         _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1707) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1708) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1709) ;
      if ( _ebV_TokenType == 53 || _ebV_TokenType == 52 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1710) ;
         _ebV_WorkingEmit += _ebP_Token( ) + " " ;
      } else if ( _ebV_TokenType == 54 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1711) ;
         _ebV_WorkingEmit += _ebP_Str( _ebP_Val( _ebP_Right( _ebP_Token( ) , _ebP_Len( _ebP_Token( ) ) - 1 ) ) ) + " " ;
      } else if ( _ebV_TokenType == 72 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1712) ;
         _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1713) ;
         _ebV_WorkingEmit += "-" + _ebP_Token( ) + " " ;
      } else if ( _ebV_TokenType == 4 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1714) ;
         if ( _ebP_ProcedureReturnType( _ebP_Token( ) ) == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1715) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1716) ;
            _ebP_Error( "Syntax error. Numeric expressions can only contain integer and float type literals, variables, and procedure return values." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1717) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1718) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1719) ;
         _ebP_CallProcedure( ) ;
      } else if ( _ebV_TokenType == 55 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1720) ;
         if ( _ebP_ConstantType( _ebP_Token( ) ) == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1721) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1722) ;
            _ebP_Error( "Syntax error. Numeric expressions can only contain integer and float type literals, variables, and procedure return values." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1723) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1724) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1725) ;
         _ebV_WorkingEmit += _ebP_ConstantMacro( _ebP_Token( ) ) + " " ;
      } else if ( _ebV_TokenType == 3 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1726) ;
         _ebV_ProcedureName = _ebP_Right( _ebP_Token( ) , _ebP_Len( _ebP_Token( ) ) - 1 ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1727) ;
         if ( _ebP_FindMapKey( _ebV_ProcedureMap , _ebV_ProcedureName ) == false ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1728) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1729) ;
            _ebP_Error( _ebV_ProcedureName + "() is not a declared procedure." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1730) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1731) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1732) ;
         _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1733) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1734) ;
         if ( _ebP_TokenType( ) != 61 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1735) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1736) ;
            _ebP_Error( "Procedure reference cannot take any parameters." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1737) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1738) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1739) ;
         _ebV_WorkingEmit += "reinterpret_cast<char *>( &_ebP_" + _ebV_ProcedureName + " ) " ;
      } else if ( _ebV_TokenType == 59 || _ebV_TokenType == 58 || _ebV_TokenType == 57 || _ebV_TokenType == 56 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1740) ;
         if ( _ebV_TokenType == 57 || _ebV_TokenType == 56 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1741) ;
            _ebV_VariableName = _ebP_Right( _ebP_Token( ) , _ebP_Len( _ebP_Token( ) ) - 1 ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1742) ;
            _ebV_Reference = true ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1743) ;
            _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1744) ;
            _ebV_Reference = false ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1745) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1746) ;
         if ( _ebV_TokenType == 58 || _ebV_TokenType == 56 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1747) ;
            _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1748) ;
            _ebV_CurrentType = _ebP_CheckType( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1749) ;
            _ebP_AddProcedureVariable( _ebV_VariableName , _ebV_CurrentType ) ;
         } else if ( _ebV_TokenType == 59 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1750) ;
            if ( _ebP_VariableType( _ebV_VariableName ) == 0 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1751) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1752) ;
               _ebP_Warning( "Variable type was not explicitly given. Defaulting to Integer." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1753) ;
               _ebP_AddProcedureVariable( _ebV_VariableName , 4 ) ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1754) ;
               _ebV_CurrentType = _ebP_VariableType( _ebV_VariableName ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1755) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1756) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1757) ;
         if ( _ebV_CurrentType == 10 && _ebV_Reference == false ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1758) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1759) ;
            _ebP_Error( "Syntax error. Can't mix strings with integers or floats." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1760) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1761) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1762) ;
         if ( _ebV_Reference == true ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1763) ;
            _ebV_WorkingEmit += "reinterpret_cast<char*>( &_ebV_" + _ebV_VariableName + " ) " ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1764) ;
            _ebV_WorkingEmit += "_ebV_" + _ebV_VariableName + " " ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1765) ;
         }
      } else if ( _ebV_TokenType == 48 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1766) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1767) ;
         _ebP_Error( "Syntax error. Can't mix strings with integers or floats." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1768) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1769) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1770) ;
      static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1771) ;
      while ( _ebP_TokenType( ) == 61 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1772) ;
         _ebV_ExpressionScope = _ebV_ExpressionScope - 1 ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1773) ;
         if ( _ebV_ExpressionScope < 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1774) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1775) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1776) ;
         _ebV_WorkingEmit += ") " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1777) ;
         static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1778) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1779) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1780) ;
      if ( _ebV_TokenType == 81 || _ebV_TokenType == 64 || _ebV_TokenType == 30 || _ebV_TokenType == 31 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1781) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1782) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1783) ;
      if ( _ebV_TokenType >= 71 && _ebV_TokenType <= 75 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1784) ;
         _ebV_WorkingEmit += _ebP_Token( ) + " " ;
      } else if ( _ebV_TokenType == 76 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1785) ;
         _ebV_WorkingEmit += "~ " ;
      } else if ( _ebV_TokenType == 77 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1786) ;
         _ebV_WorkingEmit += "& " ;
      } else if ( _ebV_TokenType == 78 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1787) ;
         _ebV_WorkingEmit += "| " ;
      } else if ( _ebV_TokenType == 79 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1788) ;
         _ebV_WorkingEmit += "^ " ;
      } else if ( _ebV_TokenType == 65 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1789) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1790) ;
         _ebP_Error( "Syntax error. Expression is not assignable." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1791) ;
         exit( EXIT_FAILURE ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1792) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1793) ;
         _ebP_Error( "Syntax error. Expected operator." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1794) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1795) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1796) ;
      static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1797) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CompareExpression() {
   signed char _ebV_Compares = 0 ;
   long int _ebV_ExpressionScope = 0 ;
   signed char _ebV_TokenType = 0 ;
   std::string _ebV_VariableName ;
   signed char _ebV_CurrentType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1798) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1799) ;
   _ebV_Compares = false ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1800) ;
   _ebV_ExpressionScope = 0 ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1801) ;
   while ( 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1802) ;
      while ( _ebP_TokenType( ) == 60 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1803) ;
         _ebV_ExpressionScope = _ebV_ExpressionScope + 1 ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1804) ;
         _ebV_WorkingEmit += "( " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1805) ;
         _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1806) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1807) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1808) ;
      if ( _ebV_TokenType == 53 || _ebV_TokenType == 52 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1809) ;
         _ebV_WorkingEmit += _ebP_Token( ) + " " ;
      } else if ( _ebV_TokenType == 54 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1810) ;
         _ebV_WorkingEmit += _ebP_Str( _ebP_Val( _ebP_Right( _ebP_Token( ) , _ebP_Len( _ebP_Token( ) ) - 1 ) ) ) + " " ;
      } else if ( _ebV_TokenType == 72 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1811) ;
         _ebP_NextToken( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1812) ;
         _ebV_WorkingEmit += "-" + _ebP_Token( ) + " " ;
      } else if ( _ebV_TokenType == 4 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1813) ;
         if ( _ebP_ProcedureReturnType( _ebP_Token( ) ) == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1814) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1815) ;
            _ebP_Error( "Syntax error. Numeric expressions can only contain integer and float type literals, variables, and procedure return values." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1816) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1817) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1818) ;
         _ebP_CallProcedure( ) ;
      } else if ( _ebV_TokenType == 55 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1819) ;
         if ( _ebP_ConstantType( _ebP_Token( ) ) == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1820) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1821) ;
            _ebP_Error( "Syntax error. Numeric expressions can only contain integer and float type literals, variables, and procedure return values." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1822) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1823) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1824) ;
         _ebV_WorkingEmit += _ebP_ConstantMacro( _ebP_Token( ) ) + " " ;
      } else if ( _ebV_TokenType == 59 || _ebP_TokenType( ) == 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1825) ;
         if ( _ebV_TokenType == 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1826) ;
            _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1827) ;
            _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1828) ;
            _ebV_CurrentType = _ebP_CheckType( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1829) ;
            _ebP_AddProcedureVariable( _ebV_VariableName , _ebV_CurrentType ) ;
         } else if ( _ebV_TokenType == 59 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1830) ;
            _ebV_VariableName = _ebP_Token( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1831) ;
            if ( _ebP_VariableType( _ebV_VariableName ) == 0 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1832) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1833) ;
               _ebP_Warning( "Variable type was not explicitly given. Defaulting to Integer." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1834) ;
               _ebP_AddProcedureVariable( _ebV_VariableName , 4 ) ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1835) ;
               _ebV_CurrentType = _ebP_VariableType( _ebV_VariableName ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1836) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1837) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1838) ;
         if ( _ebV_CurrentType == 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1839) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1840) ;
            _ebP_Error( "Syntax error. Can't mix strings with integers or floats." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1841) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1842) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1843) ;
         _ebV_WorkingEmit += "_ebV_" + _ebV_VariableName + " " ;
      } else if ( _ebV_TokenType == 48 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1844) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1845) ;
         _ebP_Error( "Syntax error. Can't mix strings with integers or floats." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1846) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1847) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1848) ;
      static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1849) ;
      while ( _ebP_TokenType( ) == 61 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1850) ;
         _ebV_ExpressionScope = _ebV_ExpressionScope - 1 ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1851) ;
         if ( _ebV_ExpressionScope < 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1852) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1853) ;
            _ebP_Error( "Syntax error. Expected '(' before ')'" , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1854) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1855) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1856) ;
         _ebV_WorkingEmit += ") " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1857) ;
         static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1858) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1859) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1860) ;
      if ( _ebV_TokenType == 81 || _ebV_TokenType == 30 || _ebV_TokenType == 31 || _ebV_TokenType == 24 || _ebV_TokenType == 25 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1861) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1862) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1863) ;
      if ( _ebV_TokenType >= 71 && _ebV_TokenType <= 75 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1864) ;
         _ebV_WorkingEmit += _ebP_Token( ) + " " ;
      } else if ( _ebV_TokenType == 76 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1865) ;
         _ebV_WorkingEmit += "~ " ;
      } else if ( _ebV_TokenType == 77 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1866) ;
         _ebV_WorkingEmit += "& " ;
      } else if ( _ebV_TokenType == 78 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1867) ;
         _ebV_WorkingEmit += "| " ;
      } else if ( _ebV_TokenType == 79 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1868) ;
         _ebV_WorkingEmit += "^ " ;
      } else if ( _ebV_TokenType == 80 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1869) ;
         _ebV_WorkingEmit += "% " ;
      } else if ( _ebV_TokenType >= 65 && _ebV_TokenType <= 70 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1870) ;
         if ( _ebV_Compares == true ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1871) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1872) ;
            _ebP_Error( "Syntax error. Expression already compares values." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1873) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1874) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1875) ;
         _ebV_Compares = true ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1876) ;
         if ( _ebV_TokenType == 65 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1877) ;
            _ebV_WorkingEmit += "== " ;
         } else if ( _ebV_TokenType == 66 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1878) ;
            _ebV_WorkingEmit += "!= " ;
         } else if ( _ebV_TokenType == 67 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1879) ;
            _ebV_WorkingEmit += ">= " ;
         } else if ( _ebV_TokenType == 68 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1880) ;
            _ebV_WorkingEmit += "<= " ;
         } else if ( _ebV_TokenType == 69 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1881) ;
            _ebV_WorkingEmit += "> " ;
         } else if ( _ebV_TokenType == 70 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1882) ;
            _ebV_WorkingEmit += "< " ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1883) ;
         }
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1884) ;
         _ebP_PrintN( _ebP_Token( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1884) ;
         _ebP_PrintN( _ebP_Str( _ebP_TokenType( ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1885) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1886) ;
         _ebP_Error( "Syntax error. Expected operator or keyword." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1887) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1888) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1889) ;
      static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1890) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckConcatenate() {
   long int _ebV_WriteString = 0 ;
   std::string _ebV_LookBehind ;
   signed char _ebV_TokenType = 0 ;
   std::string _ebV_VariableName ;
   signed char _ebV_CurrentType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1891) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1892) ;
   _ebV_WriteString = 0 ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1893) ;
   _ebV_LookBehind = "" ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1894) ;
   while ( 1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1895) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1896) ;
      if ( _ebV_TokenType == 48 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1897) ;
         _ebV_WriteString = _ebV_WriteString + 1 ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1898) ;
         _ebV_LookBehind += _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) ;
      } else if ( _ebV_TokenType == 4 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1899) ;
         if ( _ebV_WriteString > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1900) ;
            _ebV_WorkingEmit += "\"" + _ebV_LookBehind + "\" + " ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1901) ;
            _ebV_LookBehind = "" ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1902) ;
            _ebV_WriteString = 0 ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1903) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1904) ;
         if ( _ebP_ProcedureReturnType( _ebP_Token( ) ) != 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1905) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1906) ;
            _ebP_Error( "Syntax error. String concatenations can only contain literal strings and string type variables and the only operator is '+'." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1907) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1908) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1909) ;
         _ebP_CallProcedure( ) ;
      } else if ( _ebV_TokenType == 55 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1910) ;
         if ( _ebV_WriteString > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1911) ;
            _ebV_WorkingEmit += "\"" + _ebV_LookBehind + "\" + " ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1912) ;
            _ebV_LookBehind = "" ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1913) ;
            _ebV_WriteString = 0 ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1914) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1915) ;
         if ( _ebP_ConstantType( _ebP_Token( ) ) != 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1916) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1917) ;
            _ebP_Error( "Syntax error. String concatenations can only contain literal strings and string type variables and the only operator is '+'." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1918) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1919) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1920) ;
         _ebV_WorkingEmit += "std::string( " + _ebP_ConstantMacro( _ebP_Token( ) ) + " ) " ;
      } else if ( _ebV_TokenType == 59 || _ebP_TokenType( ) == 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1921) ;
         if ( _ebV_WriteString > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1922) ;
            _ebV_WorkingEmit += "\"" + _ebV_LookBehind + "\" + " ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1923) ;
            _ebV_LookBehind = "" ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1924) ;
            _ebV_WriteString = 0 ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1925) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1926) ;
         if ( _ebV_TokenType == 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1927) ;
            _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1928) ;
            _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1929) ;
            _ebV_CurrentType = _ebP_CheckType( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1930) ;
            _ebP_AddProcedureVariable( _ebV_VariableName , _ebV_CurrentType ) ;
         } else if ( _ebV_TokenType == 59 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1931) ;
            _ebV_VariableName = _ebP_Token( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1932) ;
            if ( _ebP_VariableType( _ebV_VariableName ) == 0 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1933) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1934) ;
               _ebP_Warning( "Variable type was not explicitly given. Defaulting to String." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1935) ;
               _ebP_AddProcedureVariable( _ebV_VariableName , 10 ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1936) ;
               _ebV_CurrentType = 10 ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1937) ;
               _ebV_CurrentType = _ebP_VariableType( _ebV_VariableName ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1938) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1939) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1940) ;
         if ( _ebV_CurrentType != 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1941) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1942) ;
            _ebP_Error( "Syntax error. Can't mix integers or floats with strings." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1943) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1944) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1945) ;
         _ebV_WorkingEmit += "_ebV_" + _ebV_VariableName + " " ;
      } else if ( _ebV_TokenType == 53 || _ebV_TokenType == 52 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1946) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1947) ;
         _ebP_Error( "Syntax error. Can't mix integers or floats with strings." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1948) ;
         exit( EXIT_FAILURE ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1949) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1950) ;
         _ebP_Error( "Syntax error. String concatenations can only contain literal strings, string type variables, constant strings and operator '+'." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1951) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1952) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1953) ;
      static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1954) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1955) ;
      if ( _ebV_TokenType == 81 || _ebV_TokenType == 61 || _ebV_TokenType == 64 || _ebV_TokenType == 24 || _ebV_TokenType == 25 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1956) ;
         if ( _ebV_WriteString > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1957) ;
            _ebV_WorkingEmit += "\"" + _ebV_LookBehind + "\" " ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1958) ;
            _ebV_LookBehind = "" ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1959) ;
            _ebV_WriteString = 0 ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1960) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1961) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1962) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1963) ;
      if ( _ebV_TokenType != 71 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1964) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1965) ;
         _ebP_Error( "Syntax error. String concatenations can only contain literal strings, string type variables, constant strings and operator '+'." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1966) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1967) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1968) ;
      if ( _ebV_WriteString == 0 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1969) ;
         _ebV_WorkingEmit += "+ " ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1970) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1971) ;
      static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1972) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CompareConcatenate() {
   signed char _ebV_Compares = 0 ;
   signed char _ebV_TokenType = 0 ;
   std::string _ebV_VariableName ;
   signed char _ebV_CurrentType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1973) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1974) ;
   _ebV_Compares = false ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1975) ;
   while ( 1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1976) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1977) ;
      if ( _ebV_TokenType == 48 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1978) ;
         _ebV_WorkingEmit += "std::string( " + _ebP_Token( ) + " ) " ;
      } else if ( _ebV_TokenType == 4 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1979) ;
         if ( _ebP_ProcedureReturnType( _ebP_Token( ) ) != 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1980) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1981) ;
            _ebP_Error( "Syntax error. String comparisons can only contain literal strings and string type variables and the only operator is '+'." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1982) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1983) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1984) ;
         _ebP_CallProcedure( ) ;
      } else if ( _ebV_TokenType == 55 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1985) ;
         if ( _ebP_ConstantType( _ebP_Token( ) ) != 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1986) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1987) ;
            _ebP_Error( "Syntax error. String comparisons can only contain literal strings and string type variables and the only operator is '+'." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1988) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1989) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1990) ;
         _ebV_WorkingEmit += "std::string( " + _ebP_ConstantMacro( _ebP_Token( ) ) + " ) " ;
      } else if ( _ebV_TokenType == 59 || _ebP_TokenType( ) == 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1991) ;
         if ( _ebV_TokenType == 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1992) ;
            _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1993) ;
            _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1994) ;
            _ebV_CurrentType = _ebP_CheckType( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1995) ;
            _ebP_AddProcedureVariable( _ebV_VariableName , _ebV_CurrentType ) ;
         } else if ( _ebV_TokenType == 59 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1996) ;
            _ebV_VariableName = _ebP_Token( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1997) ;
            if ( _ebP_VariableType( _ebV_VariableName ) == 0 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1998) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference1999) ;
               _ebP_Warning( "Variable type was not explicitly given. Defaulting to String." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2000) ;
               _ebP_AddProcedureVariable( _ebV_VariableName , 10 ) ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2001) ;
               _ebV_CurrentType = _ebP_VariableType( _ebV_VariableName ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2002) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2003) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2004) ;
         if ( _ebV_CurrentType != 10 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2005) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2006) ;
            _ebP_Error( "Syntax error. Can't compare integers or floats with strings." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2007) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2008) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2009) ;
         _ebV_WorkingEmit += "_ebV_" + _ebV_VariableName + " " ;
      } else if ( _ebV_TokenType == 53 || _ebV_TokenType == 52 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2010) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2011) ;
         _ebP_Error( "Syntax error. Can't mix integers or floats with strings." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2012) ;
         exit( EXIT_FAILURE ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2013) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2014) ;
         _ebP_Error( "Syntax error. String comparisons can only contain literal strings and string type variables and the only operator is '+'." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2015) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2016) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2017) ;
      static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2018) ;
      _ebV_TokenType = _ebP_TokenType( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2019) ;
      if ( _ebV_TokenType == 81 || _ebV_TokenType == 61 || _ebV_TokenType == 64 || _ebV_TokenType == 24 || _ebV_TokenType == 25 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2020) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2021) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2022) ;
      if ( _ebV_TokenType == 71 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2023) ;
         _ebV_WorkingEmit += "+ " ;
      } else if ( _ebV_TokenType >= 65 && _ebV_TokenType <= 70 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2024) ;
         if ( _ebV_Compares == true ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2025) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2026) ;
            _ebP_Error( "Syntax error. Expression already compares values." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2027) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2028) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2029) ;
         _ebV_Compares = true ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2030) ;
         if ( _ebV_TokenType == 65 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2031) ;
            _ebV_WorkingEmit += "== " ;
         } else if ( _ebV_TokenType == 66 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2032) ;
            _ebV_WorkingEmit += "!= " ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2033) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2034) ;
            _ebP_Error( "Syntax error. Strings can only be compared with '=' and '!='." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2035) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2036) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2037) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2038) ;
      static_cast<void>( _ebP_SeekToken( ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2039) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CompileMainPath() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference2040) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2041) ;
   _ebP_TokenizeSourceFile( _ebV_MainPath ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2042) ;
   _ebP_SetFile( _ebV_MainPath ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2043) ;
   _ebP_CreateProcedure( "_EBMain" , 0 , _ebV_MainPath , 1 , 0 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2044) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2045) ;
      _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":_EBMain():1" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2046) ;
      _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2047) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2048) ;
   if ( _ebV_CommentPassthrough == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2049) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2050) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2051) ;
      _ebP_AddProcedureEmit( "// ; MainFile " + _ebV_MainPath ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2052) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2053) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2054) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2055) ;
   if ( _ebV_SyntaxCheck == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2056) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2056) ;
      _ebP_Print( "\nSyntax checking MainFile " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2056) ;
      _ebP_ConsoleTextColor( 4 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2056) ;
      _ebP_Print( "\"" + _ebV_MainPath + "\"" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2056) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2056) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2056) ;
      _ebP_Print( "...\n" ) ;
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2057) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2057) ;
      _ebP_Print( "\nCompiling MainFile " ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2057) ;
      _ebP_ConsoleTextColor( 4 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2057) ;
      _ebP_Print( "\"" + _ebV_MainPath + "\"" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2057) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2057) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2057) ;
      _ebP_Print( "...\n" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2058) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2059) ;
   _ebP_CompileCPP11( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2060) ;
   _ebP_CheckBlockLevel( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2061) ;
   if ( _ebV_CommentPassthrough == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2062) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2063) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2064) ;
      _ebP_AddProcedureEmit( "// ; End of MainFile " + _ebV_MainPath ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2065) ;
      _ebP_AddProcedureEmit( "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2066) ;
      _ebP_AddProcedureEmit( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2067) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2068) ;
   _ebP_QueueProcedure( "_EBMain" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_NotImplemented() {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference2069) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2070) ;
   _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2071) ;
   _ebP_AddProcedureEmit( "// " + _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2072) ;
   while ( _ebP_NextElement( _ebV_ActiveTokenTable ) == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2073) ;
      if ( _ebP_TokenType( ) == 81 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2074) ;
         break ;;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2075) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2076) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CompileCPP11() {
   signed char _ebV_CurrentTokenType = 0 ;
   std::string _ebV_VariableName ;
   signed char _ebV_CurrentType = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference2077) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2078) ;
   while ( _ebP_NextElement( _ebV_ActiveTokenTable ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2079) ;
      _ebV_CurrentTokenType = _ebP_TokenType( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2080) ;
      std::cout << _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) )  << std::endl ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2081) ;
      _ebV_WorkingEmit = "" ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2082) ;
      if ( _ebV_CurrentTokenType == 44 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2083) ;
         if ( _ebV_DebuggerFlag == true ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2084) ;
            _ebV_WorkingEmit = "_ebD_CallDebugger(_ebD_Debugger_Flag_Call, \"" + _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) ) + "\") ;" ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2085) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2086) ;
         _ebP_GarbageCheck( ) ;
      } else if ( _ebV_CurrentTokenType == 45 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2087) ;
         if ( _ebV_DebuggerFlag == true ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2088) ;
            _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2089) ;
            _ebV_WorkingEmit = "std::cout << " ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2090) ;
            _ebP_CheckConcatenate( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2091) ;
            _ebV_WorkingEmit += " << std::endl ;" ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2092) ;
            _ebP_NotImplemented( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2093) ;
         }
      } else if ( _ebV_CurrentTokenType == 46 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2094) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2095) ;
         if ( _ebP_TokenType( ) != 48 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2096) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2097) ;
            _ebP_Error( "Expected string to print." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2098) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2099) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2100) ;
         _ebP_PrintN( _ebP_Mid( _ebP_Token( ) , 1 , _ebP_Len( _ebP_Token( ) ) - 2 ) ) ;
      } else if ( _ebV_CurrentTokenType == 47 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2101) ;
         _ebP_ConfigureConstant( ) ;
      } else if ( _ebV_CurrentTokenType == 2 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2102) ;
         _ebP_BlockComment( ) ;
      } else if ( _ebV_CurrentTokenType == 9 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2103) ;
         _ebP_CheckStructure( ) ;
      } else if ( _ebV_CurrentTokenType == 5 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2104) ;
         _ebP_NotImplemented( ) ;
      } else if ( _ebV_CurrentTokenType == 15 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2105) ;
         _ebP_CheckImport( ) ;
      } else if ( _ebV_CurrentTokenType == 41 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2106) ;
         _ebP_CheckXInclude( ) ;
      } else if ( _ebV_CurrentTokenType == 42 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2107) ;
         _ebP_CheckInclude( ) ;
      } else if ( _ebV_CurrentTokenType == 40 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2108) ;
         _ebP_CheckTInclude( ) ;
      } else if ( _ebV_CurrentTokenType == 37 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2109) ;
         _ebP_CheckTLink( ) ;
      } else if ( _ebP_TokenType( ) == 39 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2110) ;
         _ebP_CheckTInitialize( ) ;
      } else if ( _ebV_CurrentTokenType == 38 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2111) ;
         _ebP_CheckTConstant( ) ;
      } else if ( _ebV_CurrentTokenType == 36 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2112) ;
         _ebP_CheckProcedure( ) ;
      } else if ( _ebV_CurrentTokenType == 6 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2113) ;
         _ebP_CheckDeclare( ) ;
      } else if ( _ebV_CurrentTokenType == 7 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2114) ;
         _ebP_CheckExternalProcedure( ) ;
      } else if ( _ebV_CurrentTokenType == 34 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2115) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
      } else if ( _ebV_CurrentTokenType == 35 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2116) ;
         _ebP_CheckProcedureReturn( ) ;
      } else if ( _ebV_CurrentTokenType == 43 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2117) ;
         _ebP_CheckEnd( ) ;
      } else if ( _ebV_CurrentTokenType == 33 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2118) ;
         _ebV_WorkingEmit = "break ;" ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2119) ;
         _ebP_GarbageCheck( ) ;
      } else if ( _ebV_CurrentTokenType == 16 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2120) ;
         _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2121) ;
         if ( _ebP_TokenType( ) != 58 && _ebP_TokenType( ) != 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2122) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2123) ;
            _ebP_Error( "Expected variable declaration." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2124) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2125) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2126) ;
         if ( _ebP_TokenType( ) == 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2127) ;
            _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2128) ;
            _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2129) ;
            _ebP_AddGlobalVariable( _ebV_VariableName , _ebP_CheckType( ) ) ;
         } else if ( _ebP_TokenType( ) == 59 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2130) ;
            _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2131) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2132) ;
            _ebP_Warning( "Variable type was not explicitly given. Defaulting to Integer." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2133) ;
            _ebP_AddGlobalVariable( _ebV_VariableName , 4 ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2134) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2135) ;
         _ebP_GarbageCheck( ) ;
      } else if ( _ebV_CurrentTokenType == 29 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2136) ;
         _ebP_CheckFor( ) ;
      } else if ( _ebV_CurrentTokenType == 32 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2137) ;
         _ebP_CheckNext( ) ;
      } else if ( _ebV_CurrentTokenType == 17 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2138) ;
         _ebP_CheckSelect( ) ;
      } else if ( _ebV_CurrentTokenType == 18 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2139) ;
         _ebP_CheckCase( ) ;
      } else if ( _ebV_CurrentTokenType == 19 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2140) ;
         _ebP_CheckDefault( ) ;
      } else if ( _ebV_CurrentTokenType == 20 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2141) ;
         _ebP_CheckEndSelect( ) ;
      } else if ( _ebV_CurrentTokenType == 21 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2142) ;
         _ebP_CheckWhile( ) ;
      } else if ( _ebV_CurrentTokenType == 22 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2143) ;
         _ebP_CheckWend( ) ;
      } else if ( _ebV_CurrentTokenType == 23 || _ebV_CurrentTokenType == 26 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2144) ;
         _ebP_CheckIf( ) ;
      } else if ( _ebV_CurrentTokenType == 27 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2145) ;
         _ebP_CheckElse( ) ;
      } else if ( _ebV_CurrentTokenType == 28 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2146) ;
         _ebP_CheckEndIf( ) ;
      } else if ( _ebV_CurrentTokenType == 10 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2147) ;
         _ebP_BlockEnumeration( ) ;
      } else if ( _ebV_CurrentTokenType == 48 || _ebV_CurrentTokenType == 52 || _ebV_CurrentTokenType == 53 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2148) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2149) ;
         _ebP_Error( "Expression is not assignable." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2150) ;
         exit( EXIT_FAILURE ) ;
      } else if ( _ebV_CurrentTokenType == 55 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2151) ;
         _ebP_BlockConstant( ) ;
      } else if ( _ebV_CurrentTokenType == 59 || _ebV_CurrentTokenType == 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2152) ;
         if ( _ebV_CurrentTokenType == 58 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2153) ;
            _ebV_VariableName = _ebP_Token( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2154) ;
            _ebP_NextToken( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2155) ;
            _ebV_CurrentType = _ebP_CheckType( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2156) ;
            _ebP_AddProcedureVariable( _ebV_VariableName , _ebV_CurrentType ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2157) ;
            _ebV_VariableName = _ebP_Token( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2158) ;
            if ( _ebP_VariableType( _ebV_VariableName ) == 0 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2159) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2160) ;
               _ebP_Warning( "Variable type was not explicitly given. Defaulting to Integer." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2161) ;
               _ebP_AddProcedureVariable( _ebV_VariableName , 4 ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2162) ;
               _ebV_CurrentType = 4 ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2163) ;
               _ebV_CurrentType = _ebP_VariableType( _ebV_VariableName ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2164) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2165) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2166) ;
         static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2167) ;
         if ( _ebP_TokenType( ) != 81 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2168) ;
            _ebV_WorkingEmit = "_ebV_" + _ebV_VariableName + " " ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2169) ;
            if ( _ebV_CurrentType == 10 && _ebP_TokenType( ) != 71 && _ebP_TokenType( ) != 65 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2170) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2171) ;
               _ebP_Error( "Syntax error. String concatenations can only use the '=' and '+' operators." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2172) ;
               exit( EXIT_FAILURE ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2173) ;
            }
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2174) ;
            if ( _ebP_TokenType( ) >= 71 && _ebP_TokenType( ) <= 80 ) { 
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2175) ;
               if ( _ebV_CurrentType == 10 ) { 
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2176) ;
                  _ebV_WorkingEmit += "+= " ;
               } else { 
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2177) ;
                  _ebV_WorkingEmit += "= _ebV_" + _ebV_VariableName + " " + _ebP_Token( ) + " " ;
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2178) ;
               }
            } else if ( _ebP_TokenType( ) == 65 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2179) ;
               _ebV_WorkingEmit += "= " ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2180) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2181) ;
               _ebP_Error( "Syntax error. Expected operator." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2182) ;
               exit( EXIT_FAILURE ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2183) ;
            }
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2184) ;
            if ( _ebV_CurrentType == 10 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2185) ;
               static_cast<void>( _ebP_SeekToken( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2186) ;
               _ebP_CheckConcatenate( ) ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2187) ;
               static_cast<void>( _ebP_SeekToken( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2188) ;
               _ebP_CheckExpression( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2189) ;
            }
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2190) ;
            if ( _ebP_TokenType( ) == 2 ) { 
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2191) ;
               if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2192) ;
                  _ebV_WorkingEmit += "; // " + _ebP_Token( ) ;
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2193) ;
               }
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2194) ;
               static_cast<void>( _ebP_NextElement( _ebV_ActiveTokenTable ) ) ;
            } else if ( _ebP_TokenType( ) == 81 ) { 
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2195) ;
               if ( _ebP_Len( _ebV_WorkingEmit ) > 0 ) { 
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2196) ;
                  _ebV_WorkingEmit += ";" ;
                  _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2197) ;
               }
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2198) ;
               _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2199) ;
               _ebP_Error( "Garbage at end of line." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2200) ;
               exit( EXIT_FAILURE ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2201) ;
            }
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2202) ;
            _ebP_NotImplemented( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2203) ;
         }
      } else if ( _ebV_CurrentTokenType == 4 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2204) ;
         if ( _ebP_ProcedureReturnType( _ebP_Token( ) ) != 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2205) ;
            _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2206) ;
            _ebP_Warning( "Unused procedure return value." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2207) ;
            _ebV_WorkingEmit = "static_cast<void>( " ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2208) ;
            _ebP_CallProcedure( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2209) ;
            _ebV_WorkingEmit += ") " ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2210) ;
            _ebP_CallProcedure( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2211) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2212) ;
         _ebP_GarbageCheck( ) ;
      } else if ( _ebV_CurrentTokenType == 81 ) { 
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2213) ;
         _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_TokenLine( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2214) ;
         _ebP_Error( "Syntax error. Expression not understood." , _ebP_CurrentMapKey( _ebV_Files ) , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) , _ebP_TokenLine( ) , _ebP_TokenPosition( ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2215) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2216) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2217) ;
      if ( _ebP_Len( _ebV_WorkingEmit ) != 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2218) ;
         if ( _ebV_DebuggerFlag == true ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2219) ;
            _ebP_SetMapElement( _ebV_DebuggerReference , _ebP_CurrentMapKey( _ebV_Files ) + ":" + _ebP_Str( _ebP_TokenLine( ) ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2220) ;
            _ebP_AddProcedureEmit( "_ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + ") ;" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2221) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2222) ;
         _ebP_AddProcedureEmit( _ebV_WorkingEmit ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2223) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2224) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2225) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

std::string _ebP_CPP11Type(signed char _ebV_Type) {
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference2226) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2227) ;
   if ( _ebV_Type == 10 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2228) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "std::string " ;
   } else if ( _ebV_Type == 8 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2229) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "float " ;
   } else if ( _ebV_Type == 4 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2230) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "int " ;
   } else if ( _ebV_Type == 2 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2231) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "unsigned char " ;
   } else if ( _ebV_Type == 1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2232) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "signed char " ;
   } else if ( _ebV_Type == 5 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2233) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "long int " ;
   } else if ( _ebV_Type == 7 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2234) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "char * " ;
   } else if ( _ebV_Type == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2235) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "void " ;
   } else if ( _ebV_Type == 3 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2236) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "short " ;
   } else if ( _ebV_Type == 6 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2237) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "long long " ;
   } else if ( _ebV_Type == 9 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2238) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "double " ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2239) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2240) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ; return "___EBCOMPILERBUG___" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_EmitCPP11(std::string _ebV_OutPutPath) {
   int _ebV_File = 0 ;
   std::string _ebV_Repath ;
   int _ebV_HeaderFile = 0 ;
   char * _ebV_Current ;
   char * _ebV_StructField ;
   signed char _ebV_Type = 0 ;
   char * _ebV_Parameters ;
   long int _ebV_c = 0 ;
   signed char _ebV_ReturnType = 0 ;
   char * _ebV_Variables ;
   char * _ebV_Emit ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference2241) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2242) ;
   _ebV_File = _ebP_CreateFile( _ebV_OutPutPath ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2243) ;
   if ( _ebV_File == -1 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2244) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2244) ;
      _ebP_ConsoleTextColor( 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2244) ;
      _ebP_Print( "Error" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2245) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2245) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2245) ;
      _ebP_PrintN( ": Unable to open output file: " + _ebV_OutPutPath ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2246) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2247) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2248) ;
   if ( _ebV_CommentPassthrough == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2249) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2250) ;
      _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2251) ;
      _ebP_WriteStringN( _ebV_File , "// ; TIncludeFile List" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2252) ;
      _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2253) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2254) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2255) ;
   _ebP_ResetElement( _ebV_TIncludeFileMap ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2256) ;
   while ( _ebP_NextElement( _ebV_TIncludeFileMap ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2257) ;
      _ebP_WriteStringN( _ebV_File , "#include <" + _ebP_CurrentMapKey( _ebV_TIncludeFileMap ) + ">" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2258) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2259) ;
   _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2260) ;
   if ( _ebV_CommentPassthrough == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2261) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2262) ;
      _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2263) ;
      _ebP_WriteStringN( _ebV_File , "// ; Import" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2264) ;
      _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2265) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2266) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2267) ;
   _ebP_ResetElement( _ebV_ImportMap ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2268) ;
   while ( _ebP_NextElement( _ebV_ImportMap ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2269) ;
      _ebP_ResetElement( _ebV_HeaderPath ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2270) ;
      _ebV_Repath = "" ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2271) ;
      while ( _ebP_NextElement( _ebV_HeaderPath ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2272) ;
         if ( _ebP_FileSize( _ebP_CurrentMapKey( _ebV_HeaderPath ) + _ebP_CurrentMapKey( _ebV_ImportMap ) + ".cpp" ) != -1 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2273) ;
            _ebV_Repath = _ebP_CurrentMapKey( _ebV_HeaderPath ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2274) ;
            break ;;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2275) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2276) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2277) ;
      if ( _ebV_Repath == std::string( "" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2278) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2278) ;
         _ebP_ConsoleTextColor( 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2278) ;
         _ebP_Print( "Error" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2279) ;
         _ebP_ResetConsole( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2279) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2279) ;
         _ebP_PrintN( ": Header file was not found in search paths: " + _ebP_CurrentMapKey( _ebV_ImportMap ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2280) ;
         exit( EXIT_FAILURE ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2281) ;
      }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2282) ;
      if ( _ebV_InlineHeader == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2283) ;
         _ebV_HeaderFile = _ebP_ReadFile( _ebV_Repath + _ebP_CurrentMapKey( _ebV_ImportMap ) + ".cpp" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2284) ;
         if ( _ebV_HeaderFile == -1 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2285) ;
            _ebP_ConsoleTextBold( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2285) ;
            _ebP_ConsoleTextColor( 1 ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2285) ;
            _ebP_Print( "Error" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2286) ;
            _ebP_ResetConsole( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2286) ;
            _ebP_ConsoleTextBold( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2286) ;
            _ebP_PrintN( ": Unable to open header file: " + _ebV_Repath + _ebP_CurrentMapKey( _ebV_ImportMap ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2287) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2288) ;
         }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2289) ;
         while ( _ebP_Eof( _ebV_HeaderFile ) == false ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2290) ;
            _ebP_WriteStringN( _ebV_File , _ebP_ReadString( _ebV_HeaderFile ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2291) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2292) ;
         _ebP_CloseFile( _ebV_HeaderFile ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2293) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2294) ;
         _ebP_WriteStringN( _ebV_File , "#include <" + _ebV_Repath + _ebP_CurrentMapKey( _ebV_ImportMap ) + ".cpp>" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2295) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2296) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2297) ;
   _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2298) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2299) ;
      _ebP_ResetElement( _ebV_DebuggerReference ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2300) ;
      while ( _ebP_NextElement( _ebV_DebuggerReference ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2301) ;
         _ebP_WriteStringN( _ebV_File , "#define _ebD_LineReference" + _ebP_Str( _ebP_ElementIndex( _ebV_DebuggerReference ) ) + " \"" + _ebP_CurrentMapKey( _ebV_DebuggerReference ) + "\"" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2302) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2303) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2304) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2305) ;
   if ( _ebP_ElementCount( _ebV_StructureMap ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2306) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2307) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2308) ;
         _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2309) ;
         _ebP_WriteStringN( _ebV_File , "// ; Structured type declarations." ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2310) ;
         _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2311) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2312) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2313) ;
      _ebP_ResetElement( _ebV_StructureMap ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2314) ;
      while ( _ebP_NextElement( _ebV_StructureMap ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2315) ;
         if ( _ebV_CommentPassthrough == true ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2316) ;
            _ebP_WriteStringN( _ebV_File , "" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2317) ;
            _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2318) ;
            _ebP_WriteStringN( _ebV_File , "// ; Structure " + _ebP_CurrentMapKey( _ebV_StructureMap ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2319) ;
            _ebP_WriteStringN( _ebV_File , "// ;    Declared in " + _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_StructureMap ) + sizeof(char*) ) ) + " on line " + _ebP_Str( _ebP_PeekL( _ebP_CurrentMapElement( _ebV_StructureMap ) + sizeof(char*) + sizeof(char*) ) ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2320) ;
            _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2321) ;
            _ebP_WriteStringN( _ebV_File , "" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2322) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2323) ;
         _ebP_WriteStringN( _ebV_File , "struct _ebS_" + _ebP_CurrentMapKey( _ebV_StructureMap ) + " {" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2324) ;
         _ebV_Current = _ebP_CurrentMapElement( _ebV_StructureMap ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2325) ;
         _ebV_StructField = _ebP_PeekP( _ebV_Current ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2326) ;
         _ebP_ResetElement( _ebV_StructField ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2327) ;
         while ( _ebP_NextElement( _ebV_StructField ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2328) ;
            _ebV_Type = _ebP_PeekB( _ebP_CurrentMapElement( _ebV_StructField ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2329) ;
            _ebP_WriteString( _ebV_File , _ebP_Space( 3 ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2330) ;
            _ebP_WriteString( _ebV_File , _ebP_CPP11Type( _ebV_Type ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2331) ;
            _ebP_WriteString( _ebV_File , "_ebF_" ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2332) ;
            if ( _ebV_Type == 10 || _ebV_Type == 7 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2333) ;
               _ebP_WriteStringN( _ebV_File , _ebP_CurrentMapKey( _ebV_StructField ) + " ;" ) ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2334) ;
               _ebP_WriteStringN( _ebV_File , _ebP_CurrentMapKey( _ebV_StructField ) + " = 0 ;" ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2335) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2336) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2337) ;
         _ebP_WriteStringN( _ebV_File , "} ;" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2338) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2339) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2340) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2341) ;
   _ebP_ResetElement( _ebV_ProcedureList ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2342) ;
   if ( _ebV_CommentPassthrough == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2343) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2344) ;
      _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2345) ;
      _ebP_WriteStringN( _ebV_File , "// ; Procedure Declarations" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2346) ;
      _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2347) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2348) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2349) ;
   while ( _ebP_NextElement( _ebV_ProcedureList ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2350) ;
      _ebP_SetMapElement( _ebV_ProcedureMap , _ebP_CurrentMapKey( _ebV_ProcedureList ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2351) ;
      _ebV_ActiveProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2352) ;
      _ebP_SetFile( _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2353) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_PeekL( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_DeclaredLine)  ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2354) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2355) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2356) ;
         _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2357) ;
         if ( _ebP_CurrentMapKey( _ebV_ProcedureList ) == std::string( "_EBMain" ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2358) ;
            _ebP_WriteStringN( _ebV_File , "// ; This is the entry point of the main file." ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2359) ;
            _ebP_WriteStringN( _ebV_File , "// ; " + _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2360) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2361) ;
         _ebP_WriteStringN( _ebV_File , "// ;    Declared in " + _ebP_CurrentMapKey( _ebV_Files ) + " on line " + _ebP_Str( _ebP_PeekL( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_DeclaredLine)  ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2362) ;
         _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2363) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2364) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2365) ;
      _ebV_Current = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2366) ;
      _ebV_Type = _ebP_PeekB( _ebV_Current + offsetof( struct _ebS_Procedures, _ebF_ReturnType)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2367) ;
      _ebP_WriteString( _ebV_File , _ebP_CPP11Type( _ebV_Type ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2368) ;
      _ebP_WriteString( _ebV_File , "_ebP_" + _ebP_CurrentMapKey( _ebV_ProcedureList ) + "(" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2369) ;
      _ebV_Parameters = _ebP_PeekP( _ebV_Current + offsetof( struct _ebS_Procedures, _ebF_Parameters)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2370) ;
      _ebP_ResetElement( _ebV_Parameters ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2371) ;
      if ( _ebP_ElementCount( _ebV_Parameters ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2372) ;
         for ( _ebV_c = 1 ; _ebV_c != _ebP_ElementCount( _ebV_Parameters ) + 1 ; _ebV_c = _ebV_c + 1 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2373) ;
            static_cast<void>( _ebP_NextElement( _ebV_Parameters ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2374) ;
            _ebV_Type = _ebP_PeekB( _ebP_CurrentMapElement( _ebV_Parameters ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2375) ;
            _ebP_WriteString( _ebV_File , _ebP_CPP11Type( _ebV_Type ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2376) ;
            _ebP_WriteString( _ebV_File , "_ebV_" + _ebP_CurrentMapKey( _ebV_Parameters ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2377) ;
            if ( _ebV_c != _ebP_ElementCount( _ebV_Parameters ) ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2378) ;
               _ebP_WriteString( _ebV_File , ", " ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2379) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2380) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2381) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2382) ;
      _ebP_WriteStringN( _ebV_File , ") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2383) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2384) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2385) ;
   if ( _ebV_CommentPassthrough == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2386) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2387) ;
      _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2388) ;
      _ebP_WriteStringN( _ebV_File , "// ; Main initializations." ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2389) ;
      _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2390) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2391) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2392) ;
   _ebP_WriteStringN( _ebV_File , "int main(int argc, char ** argv){" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2393) ;
   _ebP_WriteStringN( _ebV_File , "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2394) ;
   _ebP_ResetElement( _ebV_TInitializeMap ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2395) ;
   while ( _ebP_NextElement( _ebV_TInitializeMap ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2396) ;
      _ebP_WriteStringN( _ebV_File , _ebP_Space( 3 ) + _ebP_CurrentMapKey( _ebV_TInitializeMap ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2397) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2398) ;
   _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2399) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2400) ;
      _ebP_WriteStringN( _ebV_File , _ebP_Space( 3 ) + "try {" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2401) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2402) ;
   _ebP_WriteStringN( _ebV_File , _ebP_Space( 3 ) + "_ebP__EBMain() ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2403) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2404) ;
      _ebP_WriteStringN( _ebV_File , _ebP_Space( 3 ) + "} catch (...) {" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2405) ;
      _ebP_WriteStringN( _ebV_File , _ebP_Space( 3 ) + "_ebD_CallDebugger(_ebD_Debugger_Flag_Exception, \"\") ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2406) ;
      _ebP_WriteStringN( _ebV_File , _ebP_Space( 3 ) + "}" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2407) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2408) ;
   _ebP_WriteStringN( _ebV_File , _ebP_Space( 3 ) + "return EXIT_SUCCESS ;" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2409) ;
   _ebP_WriteStringN( _ebV_File , "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2410) ;
   _ebP_WriteStringN( _ebV_File , "}" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2411) ;
   _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2412) ;
   if ( _ebP_ElementCount( _ebV_GlobalMap ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2413) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2414) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2415) ;
         _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2416) ;
         _ebP_WriteStringN( _ebV_File , "// ; Global variables declarations." ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2417) ;
         _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2418) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2419) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2420) ;
      _ebP_ResetElement( _ebV_GlobalMap ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2421) ;
      while ( _ebP_NextElement( _ebV_GlobalMap ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2422) ;
         _ebV_Current = _ebP_CurrentMapElement( _ebV_GlobalMap ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2423) ;
         _ebV_Type = _ebP_PeekB( _ebV_Current + offsetof( struct _ebS_Globals, _ebF_GlobalType)  ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2424) ;
         _ebP_WriteString( _ebV_File , _ebP_CPP11Type( _ebV_Type ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2425) ;
         if ( _ebV_Type == 10 || _ebV_Type == 7 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2426) ;
            _ebP_WriteStringN( _ebV_File , "_ebV_" + _ebP_CurrentMapKey( _ebV_GlobalMap ) + " ;" ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2427) ;
            _ebP_WriteStringN( _ebV_File , "_ebV_" + _ebP_CurrentMapKey( _ebV_GlobalMap ) + " = 0 ;" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2428) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2429) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2430) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2431) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2432) ;
   _ebP_WriteStringN( _ebV_File , "" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2433) ;
   _ebP_ResetElement( _ebV_ProcedureList ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2434) ;
   while ( _ebP_NextElement( _ebV_ProcedureList ) ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2435) ;
      _ebP_SetMapElement( _ebV_ProcedureMap , _ebP_CurrentMapKey( _ebV_ProcedureList ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2436) ;
      _ebV_ActiveProcedure = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2437) ;
      _ebP_SetFile( _ebP_PeekS( _ebP_PeekP( _ebP_CurrentMapElement( _ebV_ProcedureMap ) + offsetof( struct _ebS_Procedures, _ebF_DeclaredPath)  ) ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2438) ;
      _ebP_SetElementIndex( _ebV_ActiveLines , _ebP_PeekL( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_DeclaredLine)  ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2439) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2440) ;
         _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2441) ;
         if ( _ebP_CurrentMapKey( _ebV_ProcedureList ) == std::string( "_EBMain" ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2442) ;
            _ebP_WriteStringN( _ebV_File , "// ; This is the entry point of the main file." ) ;
         } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2443) ;
            _ebP_WriteStringN( _ebV_File , "// ; " + _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_ActiveLines ) ) ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2444) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2445) ;
         _ebP_WriteStringN( _ebV_File , "// ;    Declared in " + _ebP_CurrentMapKey( _ebV_Files ) + " on line " + _ebP_Str( _ebP_PeekL( _ebV_ActiveProcedure + offsetof( struct _ebS_Procedures, _ebF_DeclaredLine)  ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2446) ;
         _ebP_WriteStringN( _ebV_File , "// ; ------------------------------------------------------------------------------------------" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2447) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2448) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2449) ;
      _ebV_Current = _ebP_CurrentMapElement( _ebV_ProcedureMap ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2450) ;
      _ebV_ReturnType = _ebP_PeekB( _ebV_Current + offsetof( struct _ebS_Procedures, _ebF_ReturnType)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2451) ;
      _ebP_WriteString( _ebV_File , _ebP_CPP11Type( _ebV_ReturnType ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2452) ;
      _ebP_WriteString( _ebV_File , "_ebP_" + _ebP_CurrentMapKey( _ebV_ProcedureList ) + "(" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2453) ;
      _ebV_Parameters = _ebP_PeekP( _ebV_Current + offsetof( struct _ebS_Procedures, _ebF_Parameters)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2454) ;
      _ebP_ResetElement( _ebV_Parameters ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2455) ;
      if ( _ebP_ElementCount( _ebV_Parameters ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2456) ;
         for ( _ebV_c = 1 ; _ebV_c != _ebP_ElementCount( _ebV_Parameters ) + 1 ; _ebV_c = _ebV_c + 1 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2457) ;
            static_cast<void>( _ebP_NextElement( _ebV_Parameters ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2458) ;
            _ebV_Type = _ebP_PeekB( _ebP_CurrentMapElement( _ebV_Parameters ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2459) ;
            _ebP_WriteString( _ebV_File , _ebP_CPP11Type( _ebV_Type ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2460) ;
            _ebP_WriteString( _ebV_File , "_ebV_" + _ebP_CurrentMapKey( _ebV_Parameters ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2461) ;
            if ( _ebV_c != _ebP_ElementCount( _ebV_Parameters ) ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2462) ;
               _ebP_WriteString( _ebV_File , ", " ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2463) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2464) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2465) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2466) ;
      _ebP_WriteStringN( _ebV_File , ") {" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2467) ;
      _ebV_Variables = _ebP_PeekP( _ebV_Current + offsetof( struct _ebS_Procedures, _ebF_Variables)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2468) ;
      _ebP_ResetElement( _ebV_Variables ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2469) ;
      if ( _ebP_ElementCount( _ebV_Variables ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2470) ;
         for ( _ebV_c = 1 ; _ebV_c != _ebP_ElementCount( _ebV_Variables ) + 1 ; _ebV_c = _ebV_c + 1 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2471) ;
            _ebP_WriteString( _ebV_File , _ebP_Space( 3 ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2472) ;
            static_cast<void>( _ebP_NextElement( _ebV_Variables ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2473) ;
            _ebV_Type = _ebP_PeekB( _ebP_CurrentMapElement( _ebV_Variables ) + offsetof( struct _ebS_Variables, _ebF_Type)  ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2474) ;
            _ebP_WriteString( _ebV_File , _ebP_CPP11Type( _ebV_Type ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2475) ;
            _ebP_WriteString( _ebV_File , "_ebV_" + _ebP_CurrentMapKey( _ebV_Variables ) ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2476) ;
            if ( _ebV_Type == 10 || _ebV_Type == 7 ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2477) ;
               _ebP_WriteStringN( _ebV_File , " ;" ) ;
            } else { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2478) ;
               _ebP_WriteStringN( _ebV_File , " = 0 ;" ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2479) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2480) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2481) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2482) ;
      _ebP_WriteStringN( _ebV_File , _ebP_Space( 3 ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2483) ;
      _ebV_Emit = _ebP_PeekP( _ebV_Current + offsetof( struct _ebS_Procedures, _ebF_Emit)  ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2484) ;
      _ebP_ResetElement( _ebV_Emit ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2485) ;
      while ( _ebP_NextElement( _ebV_Emit ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2486) ;
         _ebP_WriteStringN( _ebV_File , _ebP_PeekS( _ebP_PeekP( _ebP_CurrentListElement( _ebV_Emit ) ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2487) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2488) ;
      _ebP_WriteStringN( _ebV_File , _ebP_Space( 3 ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2489) ;
      _ebP_WriteStringN( _ebV_File , "}" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2490) ;
      _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2491) ;
      if ( _ebV_CommentPassthrough == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2492) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2493) ;
         _ebP_WriteStringN( _ebV_File , "" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2494) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2495) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2496) ;
   _ebP_CloseFile( _ebV_File ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP_CheckProgramParameters() {
   int _ebV_p = 0 ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference2497) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2498) ;
   _ebV_MainPath = "" ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2499) ;
   _ebV_ExecutablePath = "./epicbasic.out" ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2500) ;
   for ( _ebV_p = 1 ; _ebV_p != _ebP_CountProgramParameters( ) ; _ebV_p = _ebV_p + 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2501) ;
      if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-v" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--version" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2502) ;
         _ebP_PrintN( _ebV_CompilerVersion ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2503) ;
         exit( EXIT_FAILURE ) ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-t" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--translation" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2504) ;
         _ebV_CommentedOnly = true ;
      } else if ( _ebP_Left( _ebP_ProgramParameter( _ebV_p ) , 2 ) == std::string( "-O" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2505) ;
         _ebV_OLevel = _ebP_ProgramParameter( _ebV_p ) ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-s" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--syntax" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2506) ;
         _ebV_SyntaxCheck = true ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-c" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--commented" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2507) ;
         _ebV_CommentPassthrough = true ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-cd" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--configure-defaults" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2508) ;
         _ebV_ConfigureDefaults = true ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-d" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--debugger" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2509) ;
         _ebV_DebuggerFlag = true ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-coff" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--colors-off" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2510) ;
         _ebP_SetTTYStatus( false ) ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-con" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--colors-on" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2511) ;
         _ebP_SetTTYStatus( true ) ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-ih" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--inline-header" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2512) ;
         _ebV_InlineHeader = true ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-wt" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--warning-threshold" ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2513) ;
         if ( _ebV_p == _ebP_CountProgramParameters( ) - 1 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2514) ;
            _ebP_PrintN( "Missing warning threshold value: " + _ebP_ProgramParameter( _ebV_p ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2515) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2516) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2517) ;
         _ebV_WarningThreshold = _ebP_Val( _ebP_ProgramParameter( _ebV_p + 1 ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2518) ;
         _ebV_p = _ebV_p + 1 ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-o" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--output" ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2519) ;
         if ( _ebV_p == _ebP_CountProgramParameters( ) - 1 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2520) ;
            _ebP_PrintN( "Missing output file path: " + _ebP_ProgramParameter( _ebV_p ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2521) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2522) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2523) ;
         _ebV_ExecutablePath = _ebP_ProgramParameter( _ebV_p + 1 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2524) ;
         _ebV_p = _ebV_p + 1 ;
      } else if ( _ebP_ProgramParameter( _ebV_p ) == std::string( "-h" ) || _ebP_ProgramParameter( _ebV_p ) == std::string( "--help" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2525) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2525) ;
         _ebP_ConsoleTextColor( 2 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2525) ;
         _ebP_PrintN( _ebP_RepeatString( "-" , _ebP_Len( _ebV_CompilerVersion ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2526) ;
         _ebP_ConsoleTextColor( 3 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2526) ;
         _ebP_PrintN( _ebV_CompilerVersion ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2527) ;
         _ebP_ConsoleTextColor( 2 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2527) ;
         _ebP_PrintN( _ebP_RepeatString( "-" , _ebP_Len( _ebV_CompilerVersion ) ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2528) ;
         _ebP_ResetConsole( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2528) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2528) ;
         _ebP_Print( "\nUsage\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2529) ;
         _ebP_ResetConsole( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2529) ;
         _ebP_Print( "   ebcompiler \"path/file.eb\"\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2530) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2530) ;
         _ebP_Print( "\nOptions\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2531) ;
         _ebP_ResetConsole( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2532) ;
         _ebP_Print( "   -t     | --translation              Output a target translation file instead of a binary.\n   -c     | --commented                Output a target translation file that includes comments from the EpicBasic source file.\n   -cd    | --configure-defaults       Prompt to configure the value of configurable constants.\n   -ih    | --inline-header            Inline the library headers to make the translation portable.\n   -coff  | --colors-off               Force disable colorized console output. (auto select mode by default)\n   -con   | --colors-on                Force enable colorized console output.\n   -O#    |                            Set the optimizer level to pass to the C++ compiler\n   -s     | --syntax                   Syntax check the MainFile only.\n   -wt    | --warning-threshhold #     Set the maximum number of warnings to report in detail. (Default is 0)\n   -d     | --debugger                 Compile with debugger.\n   -o     | --output \"/path/file\"      Outputs binary to path.\n   -v     | --version                  Output the compiler's version string\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2533) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2533) ;
         _ebP_Print( "\nEnvironment Variables\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2534) ;
         _ebP_ResetConsole( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2535) ;
         _ebP_Print( "   EPICBASIC_HOME_PATH   | Home path of the EpicBasic directory. (Not implemented)\n   EPICBASIC_HEADER_PATH | Path to module headers.\n   EPICBASIC_MODULE_PATH | Path to module resources.\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2536) ;
         exit( EXIT_SUCCESS ) ;
      } else if ( _ebP_Right( _ebP_ProgramParameter( _ebV_p ) , 1 ) == std::string( "-" ) ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2537) ;
         _ebP_PrintN( "Unknown option: " + _ebP_ProgramParameter( _ebV_p ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2538) ;
         _ebP_Print( "\"ebcompiler -h\" help.\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2539) ;
         exit( EXIT_FAILURE ) ;
      } else { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2540) ;
         if ( _ebP_Left( _ebP_ProgramParameter( _ebV_p ) , 1 ) == std::string( "-" ) || _ebP_Len( _ebV_MainPath ) > 0 ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2541) ;
            _ebP_PrintN( "Error: command line switch not understood: " + _ebP_ProgramParameter( _ebV_p ) ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2542) ;
            exit( EXIT_FAILURE ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2543) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2544) ;
         _ebV_MainPath = _ebP_ProgramParameter( _ebV_p ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2545) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2546) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2547) ;
   if ( _ebP_Len( _ebV_MainPath ) == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2548) ;
      _ebP_PrintN( _ebV_CompilerVersion ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2549) ;
      _ebP_Print( "An input file must be specified.\n" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2550) ;
      _ebP_Print( "\nHelp:\nebcompiler -h\n" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2551) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2552) ;
   }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2553) ;
   if ( _ebV_DebuggerFlag == true ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2554) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2554) ;
      _ebP_PrintN( "\nAttaching the debugger to the output" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2555) ;
      _ebP_SetMapElement( _ebV_TIncludeFileMap , "string" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2556) ;
      _ebP_SetMapElement( _ebV_TIncludeFileMap , "iostream" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2557) ;
      _ebP_SetMapElement( _ebV_TIncludeFileMap , "cstdlib" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2558) ;
      _ebP_SetMapElement( _ebV_TIncludeFileMap , "csignal" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2559) ;
      _ebP_SetMapElement( _ebV_TIncludeFileMap , "vector" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2560) ;
      _ebP_SetMapElement( _ebV_TInitializeMap , "_ebD_CallDebugger(_ebD_Debugger_Flag_Initialize, \"\" ) ;" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2561) ;
      _ebP_SetMapElement( _ebV_ImportMap , "Debugger" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2562) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallPop, "") ;
   
}

void _ebP__EBMain() {
   std::string _ebV_LinkPaths ;
   
   _ebD_CallDebugger(_ebD_Debugger_Flag_CallTrace, _ebD_LineReference1) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference50) ;
   _ebV_Regex = _ebP_CompileRegularExpression( std::string( "([\t]+|[ ]+)" ) + std::string( "|" ) + std::string( "(;+.*)" ) + std::string( "|" ) + std::string( "([@][a-zA-Z0-9][a-zA-Z0-9_]*)(?=[ ]*\\()" ) + std::string( "|" ) + std::string( "([a-zA-Z0-9][a-zA-Z0-9_]*)(?=[ ]*\\()" ) + std::string( "|" ) + std::string( "([a-zA-Z0-9][a-zA-Z0-9_]*)(?=(?:[\\.][a-zA-Z0-9]+)?[ ]*\\[)" ) + std::string( "|" ) + std::string( "((?i)declare(?=\\.| ))" ) + std::string( "|" ) + std::string( "((?i)externalprocedure(?=\\.| ))" ) + std::string( "|" ) + std::string( "((?i)endstructure)" ) + std::string( "|" ) + std::string( "((?i)structure(?=\\ ))" ) + std::string( "|" ) + std::string( "((?i)enumeration)" ) + std::string( "|" ) + std::string( "((?i)endenumeration)" ) + std::string( "|" ) + std::string( "((?i)newlist)" ) + std::string( "|" ) + std::string( "((?i)newmap)" ) + std::string( "|" ) + std::string( "((?i)newvector)" ) + std::string( "|" ) + std::string( "((?i)import(?= ))" ) + std::string( "|" ) + std::string( "((?i)global(?= ))" ) + std::string( "|" ) + std::string( "((?i)select(?= ))" ) + std::string( "|" ) + std::string( "((?i)case(?= ))" ) + std::string( "|" ) + std::string( "((?i)default)" ) + std::string( "|" ) + std::string( "((?i)endselect)" ) + std::string( "|" ) + std::string( "((?i)while(?= ))" ) + std::string( "|" ) + std::string( "((?i)wend)" ) + std::string( "|" ) + std::string( "((?i)if(?= ))" ) + std::string( "|" ) + std::string( "((?i)and(?= ))" ) + std::string( "|" ) + std::string( "((?i)or(?= ))" ) + std::string( "|" ) + std::string( "((?i)elseif(?= ))" ) + std::string( "|" ) + std::string( "((?i)else)" ) + std::string( "|" ) + std::string( "((?i)endif)" ) + std::string( "|" ) + std::string( "((?i)for(?= ))" ) + std::string( "|" ) + std::string( "((?i)to(?= ))" ) + std::string( "|" ) + std::string( "((?i)step(?= ))" ) + std::string( "|" ) + std::string( "((?i)next)" ) + std::string( "|" ) + std::string( "((?i)break)" ) + std::string( "|" ) + std::string( "((?i)endprocedure)" ) + std::string( "|" ) + std::string( "((?i)procedurereturn(?=$| )|return(?=$| ))" ) + std::string( "|" ) + std::string( "((?i)procedure(?=\\.| ))" ) + std::string( "|" ) + std::string( "((?i)tlink(?= ))" ) + std::string( "|" ) + std::string( "((?i)tconstant(?=\\.))" ) + std::string( "|" ) + std::string( "((?i)tinitialize(?= ))" ) + std::string( "|" ) + std::string( "((?i)tincludefile(?= ))" ) + std::string( "|" ) + std::string( "((?i)xincludefile(?= ))" ) + std::string( "|" ) + std::string( "((?i)includefile(?= ))" ) + std::string( "|" ) + std::string( "((?i)end(?= ))" ) + std::string( "|" ) + std::string( "((?i)calldebugger)" ) + std::string( "|" ) + std::string( "((?i)debug(?= ))" ) + std::string( "|" ) + std::string( "((?i)compilerprint(?= ))" ) + std::string( "|" ) + std::string( "((?i)compilerconfigure)" ) + std::string( "|" ) + std::string( "(\"(?:[^\\\\\"]+|\\\\.)*\")" ) + std::string( "|" ) + std::string( "([<].+[>])" ) + std::string( "|" ) + std::string( "([\\\\])" ) + std::string( "|" ) + std::string( "([\\.][a-zA-Z0-9]+)" ) + std::string( "|" ) + std::string( "([0-9]+[\\.][0-9]+)" ) + std::string( "|" ) + std::string( "([0-9]+)" ) + std::string( "|" ) + std::string( "([\\$][0][xX][0-9a-fA-F]+)" ) + std::string( "|" ) + std::string( "([#][a-zA-Z0-9_]+)" ) + std::string( "|" ) + std::string( "([@][a-zA-Z0-9][a-zA-Z0-9_]*)(?=(?:[\\.][a-zA-Z0-9]+))" ) + std::string( "|" ) + std::string( "([@][a-zA-Z0-9][a-zA-Z0-9_]*)" ) + std::string( "|" ) + std::string( "([a-zA-Z0-9][a-zA-Z0-9_]*)(?=(?:[\\.][a-zA-Z0-9]+))" ) + std::string( "|" ) + std::string( "([a-zA-Z0-9][a-zA-Z0-9_]*)" ) + std::string( "|" ) + std::string( "([\\(])" ) + std::string( "|" ) + std::string( "([\\)])" ) + std::string( "|" ) + std::string( "([\\[])" ) + std::string( "|" ) + std::string( "([\\]])" ) + std::string( "|" ) + std::string( "([\\,])" ) + std::string( "|" ) + std::string( "([=])" ) + std::string( "|" ) + std::string( "([!][=])" ) + std::string( "|" ) + std::string( "([>][=])" ) + std::string( "|" ) + std::string( "([<][=])" ) + std::string( "|" ) + std::string( "([>])" ) + std::string( "|" ) + std::string( "([<])" ) + std::string( "|" ) + std::string( "([\\+])" ) + std::string( "|" ) + std::string( "([\\-](?=(?:[ ]*)(?:[0-9]+)|(?:[0-9]+[\\.][0-9]+)))" ) + std::string( "|" ) + std::string( "([\\-])" ) + std::string( "|" ) + std::string( "([\\*])" ) + std::string( "|" ) + std::string( "([\\/])" ) + std::string( "|" ) + std::string( "([\\!])" ) + std::string( "|" ) + std::string( "([\\&])" ) + std::string( "|" ) + std::string( "([\\|])" ) + std::string( "|" ) + std::string( "([\\^])" ) + std::string( "|" ) + std::string( "([\\%])" ) + std::string( "|" ) + std::string( "([:])" ) + std::string( "|" ) + std::string( "([.]++)" ) ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference51) ;
   if ( _ebV_Regex == 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference52) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference52) ;
      _ebP_ConsoleTextColor( 1 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference52) ;
      _ebP_Print( "Error" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference53) ;
      _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference53) ;
      _ebP_ConsoleTextBold( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference53) ;
      _ebP_PrintN( ": Could not compile regular expression." ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference54) ;
      exit( EXIT_FAILURE ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference55) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2563) ;
   _ebP_InitializeGlobals( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2564) ;
   _ebP_CheckProgramParameters( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2565) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2565) ;
   _ebP_ConsoleTextColor( 2 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2565) ;
   _ebP_Print( "\n" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2565) ;
   _ebP_PrintN( _ebP_RepeatString( "-" , _ebP_Len( _ebV_CompilerVersion ) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2566) ;
   _ebP_ConsoleTextColor( 3 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2566) ;
   _ebP_PrintN( _ebV_CompilerVersion ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2567) ;
   _ebP_ConsoleTextColor( 2 ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2567) ;
   _ebP_PrintN( _ebP_RepeatString( "-" , _ebP_Len( _ebV_CompilerVersion ) ) ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2568) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2569) ;
   _ebV_MainPath = _ebP_AbsolutePath( _ebV_MainPath ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2570) ;
   _ebP_CompileMainPath( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2571) ;
   _ebP_FreeRegularExpression( _ebV_Regex ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2572) ;
   _ebP_PrintN( "" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2573) ;
   if ( _ebV_TotalWarnings > 0 ) { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2574) ;
      _ebP_ConsoleTextColor( 2 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2574) ;
      _ebP_Print( _ebP_Str( _ebV_TotalLines ) + " lines processed" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2575) ;
      if ( _ebV_TotalWarnings > 1 ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2576) ;
         _ebP_ConsoleTextColor( 5 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2576) ;
         _ebP_Print( " with " + _ebP_Str( _ebV_TotalWarnings ) + " warnings.\n" ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2577) ;
         _ebP_ConsoleTextColor( 5 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2577) ;
         _ebP_Print( " with " + _ebP_Str( _ebV_TotalWarnings ) + " warning.\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2578) ;
      }
   } else { 
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2579) ;
      _ebP_ConsoleTextColor( 2 ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2579) ;
      _ebP_Print( _ebP_Str( _ebV_TotalLines ) + " lines processed.\n" ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2580) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2581) ;
   _ebP_ResetConsole( ) ;
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2582) ;
   if ( _ebV_SyntaxCheck == false ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2583) ;
      if ( _ebV_CommentedOnly == true ) { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2584) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2584) ;
         _ebP_Print( "Creating the commented translation file " ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2585) ;
         _ebP_ConsoleTextColor( 4 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2585) ;
         _ebP_Print( "\"" + _ebV_MainPath + ".cpp\"\n\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2586) ;
         _ebP_EmitCPP11( _ebV_MainPath + ".cpp" ) ;
      } else { 
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2587) ;
         _ebP_EmitCPP11( _ebV_MainPath + ".cpp" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2588) ;
         _ebV_ExecutablePath = _ebP_AbsolutePath( _ebV_ExecutablePath ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2589) ;
         _ebP_ResetElement( _ebV_TLinkMap ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2590) ;
         while ( _ebP_NextElement( _ebV_TLinkMap ) ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2591) ;
            _ebV_LinkPaths += _ebP_CurrentMapKey( _ebV_TLinkMap ) + " " ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2592) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2593) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2593) ;
         _ebP_Print( "Creating the binary " ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2594) ;
         _ebP_ConsoleTextColor( 4 ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2594) ;
         _ebP_Print( "\"" + _ebV_ExecutablePath + "\"\n\n" ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2595) ;
         _ebP_ResetConsole( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2596) ;
         if ( _ebP_RunProgram( "clang++ \"" + _ebV_MainPath + ".cpp\" -o \"" + _ebV_ExecutablePath + "\" " + _ebV_OLevel + " " + _ebV_LinkPaths ) != EXIT_SUCCESS ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2597) ;
            _ebP_ConsoleTextBold( ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2597) ;
            _ebP_Print( "\nError: Unable to assemble for target using clang++\n" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2598) ;
            _ebP_Print( "Falling back to g++\n\n" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2599) ;
            _ebP_ResetConsole( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2600) ;
            if ( _ebP_RunProgram( "g++ \"" + _ebV_MainPath + ".cpp\" -o \"" + _ebV_ExecutablePath + "\" " + _ebV_OLevel + " " + _ebV_LinkPaths ) != EXIT_SUCCESS ) { 
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2601) ;
               _ebP_ConsoleTextBold( ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2601) ;
               _ebP_Print( "\nError: Unable to assemble for target using g++\n" ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2602) ;
               exit( EXIT_FAILURE ) ;
               _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2603) ;
            }
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2604) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2605) ;
         static_cast<void>( _ebP_DeleteFile( _ebV_MainPath + ".cpp" ) ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2606) ;
         _ebP_ConsoleTextBold( ) ;
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2606) ;
         _ebP_Print( "Attempting to strip the object symbols from the binary\n" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2607) ;
         if ( _ebP_RunProgram( "strip \"" + _ebV_ExecutablePath + "\"" ) == EXIT_FAILURE ) { 
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2608) ;
            _ebP_Print( "Unable to strip the object symbols from the binary.\n" ) ;
            _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2609) ;
         }
         _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2610) ;
      }
      _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2611) ;
   }
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2612) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2612) ;
   _ebP_ConsoleTextBold( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2612) ;
   _ebP_PrintN( "\n- I've got a fever and the only prescription is MoreCowbell() -\n" ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2613) ;
   _ebP_ResetConsole( ) ;
   _ebD_CallDebugger(_ebD_Debugger_Flag_LineTrace, _ebD_LineReference2614) ;
   exit( EXIT_SUCCESS ) ;
   
}

