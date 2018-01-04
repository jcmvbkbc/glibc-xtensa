
#define TLS_GD(x)							\
  ({ int *__l;								\
     asm ("movi  a8, " #x "@TLSFUNC\n\t"				\
	  "movi a10, " #x "@TLSARG\n\t"					\
	  "callx8.tls a8, " #x "@TLSCALL\n\t"				\
	  "mov %0, a10\n\t"						\
	  : "=r" (__l)							\
	  :								\
	  : "a8", "a9", "a10", "a11", "a12", "a13", "a14", "a15");	\
      __l; })

#define TLS_LD(x)							\
  ({ int *__l;								\
     asm ("movi  a8, _TLS_MODULE_BASE_@TLSFUNC\n\t"			\
	  "movi a10, _TLS_MODULE_BASE_@TLSARG\n\t"			\
	  "callx8.tls a8, _TLS_MODULE_BASE_@TLSCALL\n\t"		\
	  "movi %0, " #x "@TPOFF\n\t"					\
	  "add %0, %0, a10\n\t"						\
	  : "=r" (__l)							\
	  :								\
	  : "a8", "a9", "a10", "a11", "a12", "a13", "a14", "a15");	\
      __l; })


#define TLS_IE(x) TLS_LE(x)

#define TLS_LE(x)							\
  ({ int *__l;								\
     int __t;								\
     asm ("rur %0, threadptr\n\t"					\
	  "movi %1, " #x "@TPOFF\n\t"					\
	  "add %0, %0, %1\n\t"						\
	  : "=r" (__l), "=r" (__t) );					\
     __l; });								\

