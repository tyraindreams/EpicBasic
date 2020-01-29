// --------------------------------------------------------------------------------
//
//                      EpicBasic Network Library
//
// --------------------------------------------------------------------------------

void __ebI_InitNetwork() {
   
   signal(SIGPIPE, SIG_IGN);
   
}


int _ebP_CreateNetworkConnection(const std::string &Address, const long Port) {
   int Socket;  
   struct addrinfo Hints, *ServerInfo, *AddrPointer;

   memset(&Hints, 0, sizeof Hints);
   Hints.ai_family = AF_UNSPEC ;
   Hints.ai_socktype = SOCK_STREAM ;

   if ((getaddrinfo(Address.c_str(), std::to_string(Port).c_str(), &Hints, &ServerInfo)) != 0) {
      
      return -1 ;
      
   }
   
   for(AddrPointer = ServerInfo; AddrPointer != NULL; AddrPointer = AddrPointer->ai_next) {
      if ((Socket = socket(AddrPointer->ai_family, AddrPointer->ai_socktype, AddrPointer->ai_protocol)) == -1) {
         
         continue;
         
      }

      if (connect(Socket, AddrPointer->ai_addr, AddrPointer->ai_addrlen) == -1) {
         
         close(Socket);
         continue;
         
      }

      break ;
   }

   if (AddrPointer == NULL) {
      
      return -2 ;
      
   }

   freeaddrinfo(ServerInfo) ;
   return Socket ;
   
}

int _ebP_CreateNetworkServer(const std::string &Address, const long Port) {
   int Socket ;
   int One = 1 ;
   struct sockaddr_in ServerAddress;
   
   Socket = socket(AF_INET, SOCK_STREAM, 0) ;
   
   if (Socket < 0) {
      
      return -1 ;
      
   }
   
   setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, &One, sizeof(int));
   
   ServerAddress.sin_family = AF_INET;
   
   if (Address == "" ) {
      
      ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY) ;
      
   } else {
      
      ServerAddress.sin_addr.s_addr = inet_addr(Address.c_str()) ;
      
   }
   
   ServerAddress.sin_port = htons(Port) ;
   
   if (bind(Socket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)) == -1) {
      
      close(Socket) ;
      return -2 ;
      
   }
   
   listen(Socket, 1024) ;
   
   return Socket ;
   
}


char _ebP_WaitNetworkEvent(int Socket, long Timeout) {
   
   fd_set fds;
   FD_ZERO(&fds);
   FD_SET(Socket, &fds);
   
   timeval tv; tv.tv_sec = 0; tv.tv_usec = Timeout*1000;
   
   select (Socket+1, &fds /* read*/, NULL /* write*/, NULL /* exception */, &tv);
   
   if (FD_ISSET(Socket, &fds)) {
      
       return true ;
      
   } else {
      
      return false ;
      
   }
   
}


struct _ebI_ClientObject {
   int ClientFile ;
   struct sockaddr_in ClientAddress ;
};

#define _ebP_ClientFile(Client) (reinterpret_cast<_ebI_ClientObject *> (Client)->ClientFile)
#define _ebP_ClientAddress(Client) (std::string(inet_ntoa(reinterpret_cast<_ebI_ClientObject *> (Client)->ClientAddress.sin_addr)))

char * _ebP_AcceptServerConnection(int Socket) {
   _ebI_ClientObject * NewClient = new _ebI_ClientObject ;
   socklen_t SocketLength = sizeof(NewClient->ClientAddress) ;
   
   NewClient->ClientFile = accept(Socket, reinterpret_cast<sockaddr *> (&NewClient->ClientAddress), &SocketLength) ;
   
   return reinterpret_cast <char *> (NewClient) ;
   
}

#define _ebP_CloseServerConnection(Socket) close(Socket)
#define _ebP_CloseNetworkConnection(Socket) close(Socket)

void _ebP_CloseClientConnection(char * Client) {
   
   close(_ebP_ClientFile(Client)) ;
   delete reinterpret_cast<_ebI_ClientObject *> (Client) ;
   
}
