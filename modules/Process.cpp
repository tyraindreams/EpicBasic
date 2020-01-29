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
