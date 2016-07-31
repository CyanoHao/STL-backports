#if (defined(_MSC_VER) && _MSC_VER >= 1800) || __cplusplus >= 201103L
#define __STL_BACKPORTS__HAVE_CXX11 1
#if (defined(_MSC_VER) && _MSC_VER >= 1900) || __cplusplus >= 201402L
#define __STL_BACKPORTS__HAVE_CXX14 1
#else
#define __STL_BACKPORTS__HAVE_CXX14 0
#endif
#else
#define __STL_BACKPORTS__HAVE_CXX11 0
#endif
