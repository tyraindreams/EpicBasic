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
   int SubStringVector[3000] ;
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
   RegularExpressionObject->ExecResult = pcre_exec(RegularExpressionObject->CompiledExpression, RegularExpressionObject->Optimizations, RegularExpressionObject->CurrentQuery.c_str(), RegularExpressionObject->CurrentQuery.length(), RegularExpressionObject->Offset, 0, RegularExpressionObject->SubStringVector, 3000) ;
   
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

