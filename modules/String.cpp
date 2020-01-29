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

std::string _ebP_StrF(const float Number, int Precision) {
   std::stringstream String ;
   
   String <<std::fixed << std::setprecision(Precision) << Number ;
   
   return String.str() ;
   
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
   
   return std::find_if(String.begin(), String.end(), ::isdigit) != String.end();
    
}
