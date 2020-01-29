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

void _ebP_Flush() {
   
   std::cout.flush() ;
   
}

inline std::string _ebP_Input(void) {
   std::string Input ;
   
   std::getline (std::cin, Input);
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