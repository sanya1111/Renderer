#ifndef LOG_H_
#define LOG_H_

#ifdef DEBUG_LOG
#define DEB(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEB(...)
#endif


#endif /* LOG_H_ */
