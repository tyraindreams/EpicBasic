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

#define _ebP_Time() time(0)

char * _ebP_GMTTime(const long &Time) {
   //time_t Time = time(0) ;
   
   return reinterpret_cast <char*>(gmtime(&Time)) ;
   
}

char * _ebP_LocalTime(const long &Time) {
   //time_t Time = time(0) ;
   
   return reinterpret_cast <char*>(localtime(&Time)) ;
   
}
