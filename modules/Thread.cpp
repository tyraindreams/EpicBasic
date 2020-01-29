// --------------------------------------------------------------------------------
//
//                      EpicBasic Thread Library
//
// --------------------------------------------------------------------------------


char * _ebP_CreateThread(char * Procedure, char * Parameters) {
   pthread_t Thread;// = new pthread_t ;
   typedef void * (*ThreadPointer) (void *);
   
   if ( pthread_create(&Thread, 0, reinterpret_cast<ThreadPointer>(Procedure), reinterpret_cast<void *>(Parameters)) == 0 ) {
      
      return reinterpret_cast<char *>(Thread) ;
      
   }
   
   return 0 ;
   
}

#define _ebP_ExitThread() pthread_exit(NULL)

#define _ebP_WaitThread(Thread, Return) pthread_join(reinterpret_cast<pthread_t>(Thread),reinterpret_cast<void**>(Return))

#define _ebP_DetachThread(Thread) pthread_detach(reinterpret_cast<pthread_t>(Thread))

char * _ebP_CreateSemaphore(int DefaultValue) {
   sem_t * Semaphore = new sem_t;
   
   if (sem_init(Semaphore, 0, DefaultValue) == 0) {
      
      return reinterpret_cast<char *>(Semaphore) ;
      
   }
   
   return reinterpret_cast<char *>(0) ;
   
}

#define _ebP_WaitSemaphore(Semaphore) sem_wait(reinterpret_cast<sem_t*>(Semaphore))

#define _ebP_SignalSemaphore(Semaphore) sem_post(reinterpret_cast<sem_t*>(Semaphore))

int _ebP_CountSemaphore(char * Semaphore) {
   int Count ;
   
   if (sem_getvalue(reinterpret_cast<sem_t*>(Semaphore), &Count) == 0) {
      
      return Count ;
      
   }
   
   return -1 ;
   
}

void _ebP_FreeSemaphore(char * Semaphore) {
   
   sem_destroy(reinterpret_cast<sem_t*>(Semaphore)) ;
   delete reinterpret_cast<sem_t*>(Semaphore) ;
   
}

char * _ebP_CreateMutex() {
   pthread_mutex_t * Mutex = new pthread_mutex_t ;
   
   if (pthread_mutex_init(Mutex, 0) == 0) {
      
      return reinterpret_cast<char *>(Mutex) ;
      
   }
   
   return reinterpret_cast<char *>(0) ;
   
}

#define _ebP_LockMutex(Mutex) pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(Mutex))

#define _ebP_UnlockMutex(Mutex) pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(Mutex))

void _ebP_FreeMutex(char * Mutex) { 
   
   pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(Mutex)) ; 
   delete reinterpret_cast<pthread_mutex_t*>(Mutex) ;
   
}