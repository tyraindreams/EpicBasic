// --------------------------------------------------------------------------------
//
//                      EpicBasic HTTP Library
//
// --------------------------------------------------------------------------------



char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}


char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}


std::string _ebP_URLEncoder(const std::string &String) {
   char * str = strdup(String.c_str()) ;
   char *pstr = str;
   char *buf = (char *)malloc(strlen(str) * 3 + 1);
   char *pbuf = buf;
   std::string Result ;
   while (*pstr) {
      if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
         *pbuf++ = *pstr;
      else if (*pstr == ' ') 
         *pbuf++ = '+';
      else 
         *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
      pstr++;
   }
   *pbuf = '\0';
   
   Result = std::string (buf) ;
   free(buf) ;
   free(str) ;
   return Result;
}


std::string _ebP_URLDecoder(const std::string &String) {
   char * str = strdup(String.c_str()) ;
   char *pstr = str;
   char *buf = (char *)malloc(strlen(str) * 3 + 1);
   char *pbuf = buf;
   std::string Result ;
   
   while (*pstr) {
      if (*pstr == '%') {
         if (pstr[1] && pstr[2]) {
            *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
            pstr += 2;
         }
         } else if (*pstr == '+') { 
         *pbuf++ = ' ';
         } else {
         *pbuf++ = *pstr;
         }
      pstr++;
   }
   *pbuf = '\0';
   Result = std::string (buf) ;
   free(buf) ;
   free(str) ;
   return Result;
}