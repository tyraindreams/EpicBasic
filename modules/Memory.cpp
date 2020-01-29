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
