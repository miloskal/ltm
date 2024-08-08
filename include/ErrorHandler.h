#define error_fatal(msg) do{fprintf(stderr, "%s:%d:%s\n", __FILE__, __LINE__, msg); \
                      exit(EXIT_FAILURE);} while(0)

