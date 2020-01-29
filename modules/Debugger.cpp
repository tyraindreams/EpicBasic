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
      
      std::cout << "Debugger terminated program due to a fatal error." << std::endl ;
      
      exit(1) ;
      
   } else if (SignalNumber == SIGFPE) {
      
      _ebD_CallDebugger(_ebD_Debugger_Flag_Error, "Arithmetic Error.") ;
      
      std::cout << "Debugger terminated program due to a fatal error." << std::endl ;
      
      exit(1) ;
      
   } else if (SignalNumber == SIGILL) {
      
      _ebD_CallDebugger(_ebD_Debugger_Flag_Error, "Illegal Instruction.") ;
      
      std::cout << "Debugger terminated program due to a fatal error." << std::endl ;
      
      exit(1) ;
      
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
      
      std::cout << std::endl ;
      
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
         std::getline(std::cin, Command) ;
         
         if (Command == "help" || Command == "h") {
            
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
