/*

*/

#ifndef LIBUTIL_VERSION_H
#define LIBUTIL_VERSION_H 1

extern char const * lu_version_; 
extern char const * lu_release_;
extern char const * lu_version_full;

#define LIBUTIL_VERSION       lu_version_full;
#define LIBUTIL_VERSION_SHORT lu_version_;
#define LIBUTIL_RELEASE       lu_release_;

#endif

