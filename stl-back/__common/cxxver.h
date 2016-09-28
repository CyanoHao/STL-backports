#if __cplusplus >= 201103L
#define __STL_BACKPORTS__HAVE_CXX11 1
#else
#define __STL_BACKPORTS__HAVE_CXX11 0
#endif

#if __cplusplus >= 201402L
#define __STL_BACKPORTS__HAVE_CXX14 1
#else
#define __STL_BACKPORTS__HAVE_CXX14 0
#endif

#define __STL_BACKPORTS__MSC_GE(version)                                       \
  (defined(_MSC_VER) && _MSC_VER >= version)
