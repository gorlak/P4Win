//
// Perforce
//
// File: wild.c
//
// Piece together enough MS C runtime code from the VS2015 vc/crt/src source
// to correct the erronious MS argument parsing.
//
// This code was left in the original format and is labeled as to where
// the code originated.  This should make it easier for future patches.
//
/***
*stdargv.c - standard & wildcard _setargv routine
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       processes program command line, with or without wildcard expansion
*
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <mbstring.h>
#include <vcruntime_startup.h>
#include <windows.h>

// Perforce
// Nullify MS CRT checks.
#define _ERRCHECK(e) e

//==== definitions from various headers ===========================

//==== crt/src/internal_shared.h =========================

#ifdef FROM_MS_CRT
    #define _calloc_crt   _calloc_base
    #define _free_crt     _free_base
    #define _malloc_crt   _malloc_base
    #define _msize_crt     _msize
    #define _recalloc_crt _recalloc
    #define _realloc_crt  _realloc_base
#else
    // Perforce
	// We must use the normal memory allocators.
    #define _calloc_crt   calloc
    #define _free_crt     free
    #define _malloc_crt   malloc
    #define _msize_crt     _msize
    #define _recalloc_crt _recalloc
    #define _realloc_crt  realloc
#endif

extern "C++" {

    struct __p4_crt_internal_free_policy
    {
        template <typename T>
        void operator()(T const* const p) const throw()
        {
            _free_crt(const_cast<T*>(p));
        }
    };

    struct __p4_crt_public_free_policy
    {
        template <typename T>
        void operator()(T const* const p) const throw()
        {
            free(const_cast<T*>(p));
        }
    };

    template <typename T, typename Free = __p4_crt_internal_free_policy>
    class __p4_crt_unique_heap_ptr
    {
    public:

        explicit __p4_crt_unique_heap_ptr(T* const p = nullptr) throw()
            : _p(p)
        {
        }

        __p4_crt_unique_heap_ptr(__p4_crt_unique_heap_ptr&& other) throw()
            : _p(other._p)
        {
            other._p = nullptr;
        }

        ~__p4_crt_unique_heap_ptr() throw()
        {
            release();
        }

        __p4_crt_unique_heap_ptr& operator=(__p4_crt_unique_heap_ptr&& other) throw()
        {
            release();
            _p = other._p;
            other._p = nullptr;
            return *this;
        }

        T* detach() throw()
        {
            T* const local_p{_p};
            _p = nullptr;
            return local_p;
        }

        void attach(T* const p) throw()
        {
            release();
            _p = p;
        }

        void release() throw()
        {
            Free()(_p);
            _p = nullptr;
        }

        bool is_valid() const throw()
        {
            return _p != nullptr;
        }

        explicit operator bool() const throw()
        {
            return is_valid();
        }

        T* get() const throw()
        {
            return _p;
        }

        T** get_address_of() throw()
        {
            return &_p;
        }

        T** release_and_get_address_of() throw()
        {
            release();
            return &_p;
        }

    private:

        __p4_crt_unique_heap_ptr(__p4_crt_unique_heap_ptr const&);
        __p4_crt_unique_heap_ptr& operator=(__p4_crt_unique_heap_ptr const&);

        T* _p;
    };

    // An internal-use scoped smart pointer for memory allocated by _malloca_crt.
    template <typename T>
    struct __p4_crt_scoped_stack_ptr_tag
    {
        __p4_crt_scoped_stack_ptr_tag(T* const p) throw()
            : _p(p)
        {
        }

        T* _p;
    };

    template <typename T>
    class __p4_crt_scoped_stack_ptr
    {
    public:

        explicit __p4_crt_scoped_stack_ptr(__p4_crt_scoped_stack_ptr_tag<T> const p) throw()
            : _p(p._p)
        {
        }

        ~__p4_crt_scoped_stack_ptr() throw()
        {
            _freea_crt(_p);
        }

        T* get() const throw() { return _p; }

        // Note that we do not provide a release() because one would not be
        // useful:  a stack allocation is only useful in the scope in which it
        // was allocated.

        explicit operator bool() const throw()
        {
            return _p != nullptr;
        }

    private:
        __p4_crt_scoped_stack_ptr(__p4_crt_scoped_stack_ptr const&) throw();
        void operator=(__p4_crt_scoped_stack_ptr const&) throw();

        T* const _p;
    };
    
    // Note that no overflow checks are necessary for the multiplications here,
    // because each multiplication operation is an argument to a parameter that
    // is decorated __declspec(guard(overflow)).
    #define _calloc_crt_t(t, n)      (__p4_crt_unique_heap_ptr     <t>(static_cast<t*>(_calloc_crt  (     (n),  sizeof(t)))))
    #define _malloc_crt_t(t, n)      (__p4_crt_unique_heap_ptr     <t>(static_cast<t*>(_malloc_crt  (     (n) * sizeof(t)))))
    #define _recalloc_crt_t(t, p, n) (__p4_crt_unique_heap_ptr     <t>(static_cast<t*>(_recalloc_crt((p), (n),  sizeof(t)))))
    #define _malloca_crt_t(t, n)     (__p4_crt_scoped_stack_ptr_tag<t>(static_cast<t*>(_malloca_crt (     (n) * sizeof(t)))))



#ifdef NOT_USED
    enum : int
    {
        __crt_maximum_pointer_shift = sizeof(uintptr_t) * 8
    };

    inline unsigned int __crt_rotate_pointer_value(unsigned int const value, int const shift) throw()
    {
        return RotateRight32(value, shift);
    }

    inline unsigned __int64 __crt_rotate_pointer_value(unsigned __int64 const value, int const shift) throw()
    {
        return RotateRight64(value, shift);
    }

    // Fast alternatives to the encode/decode pointer functions that do not use
    // the EncodePointer and DecodePointer functions.
    template <typename T>
    T __crt_fast_decode_pointer(T const p) throw()
    {
        return reinterpret_cast<T>(
            __crt_rotate_pointer_value(
                reinterpret_cast<uintptr_t>(p) ^ __security_cookie,
                __security_cookie % __crt_maximum_pointer_shift
            )
        );
    }

    template <typename T>
    T __crt_fast_encode_pointer(T const p) throw()
    {
        return reinterpret_cast<T>(
            __crt_rotate_pointer_value(
                reinterpret_cast<uintptr_t>(p),
                __crt_maximum_pointer_shift - (__security_cookie % __crt_maximum_pointer_shift)
            ) ^ __security_cookie
        );
    }

    // The primary __crt_fast_encode_pointer template does not work properly 
    // when it is called with the argument 'nullptr' because the encoded void*
    // pointer is casted back to nullptr_t, and nullptr_t can only represent a
    // single value:  the real, unencoded null pointer.  Therefore, we overload
    // the function for nullptr_t, and defer the cast until we know the actual
    // type that we need.
    struct __crt_fast_encoded_nullptr_t
    {
        template <typename T>
        operator T*() const throw() { return __crt_fast_encode_pointer(static_cast<T*>(nullptr)); }
    };

    inline __crt_fast_encoded_nullptr_t __crt_fast_encode_pointer(decltype(nullptr)) throw()
    {
        return __crt_fast_encoded_nullptr_t();
    }



    template <typename T>
    T __crt_get_proc_address(HMODULE const m, char const* const f) throw()
    {
        return reinterpret_cast<T>(::GetProcAddress(m, f));
    }

    template <typename T, typename V>
    T* __crt_interlocked_exchange_pointer(T* const volatile* target, V const value) throw()
    {
        // This is required to silence a spurious unreferenced formal parameter
        // warning.
        UNREFERENCED_PARAMETER(value);

        return reinterpret_cast<T*>(_InterlockedExchangePointer((void**)target, (void*)value));
    }

    template <typename T, typename E, typename C>
    T __crt_interlocked_compare_exchange(T* const volatile target, E const exchange, C const comparand) throw()
    {
        UNREFERENCED_PARAMETER(exchange);  // These are required to silence spurious
        UNREFERENCED_PARAMETER(comparand); // unreferenced formal parameter warnings.

        static_assert(sizeof(T) == sizeof(LONG), "Type being compared must be same size as a LONG.");
        return static_cast<T>(_InterlockedCompareExchange(
            reinterpret_cast<LONG*>(target), (LONG)exchange, (LONG)comparand));
    }

    template <typename T, typename E, typename C>
    T* __crt_interlocked_compare_exchange_pointer(T* const volatile* target, E const exchange, C const comparand) throw()
    {
        UNREFERENCED_PARAMETER(exchange);  // These are required to silence spurious
        UNREFERENCED_PARAMETER(comparand); // unreferenced formal parameter warnings.

        return reinterpret_cast<T*>(_InterlockedCompareExchangePointer(
            (void**)target, (void*)exchange, (void*)comparand));
    }

    template <typename T>
    T __crt_interlocked_read(T* const volatile target) throw()
    {
        static_assert(sizeof(T) == sizeof(LONG), "Type being read must be same size as a LONG.");
        return __crt_interlocked_compare_exchange(target, 0, 0);
    }

    template <typename T>
    T* __crt_interlocked_read_pointer(T* const volatile* target) throw()
    {
        return __crt_interlocked_compare_exchange_pointer(target, nullptr, nullptr);
    }
#endif // NOT_USED

} // extern "C++"

//==== crt/src/internal_shared.h =========================

//==== ucrt/inc/corecrt_internal.h =========================

#define _DEFINE_SET_FUNCTION(function_name, type, variable_name) \
    __inline void function_name(type value)                      \
    {                                                            \
        __pragma(warning(push))                                  \
        __pragma(warning(disable:4996))                          \
        variable_name = value;                                   \
        __pragma(warning(pop))                                   \
    }

#ifndef _M_CEE_PURE
    _DEFINE_SET_FUNCTION(_set_pgmptr,  _In_z_ char*,    _pgmptr)
    _DEFINE_SET_FUNCTION(_set_wpgmptr, _In_z_ wchar_t*, _wpgmptr)
#endif

extern "C++"
{
    struct __p4_crt_handle_traits
    {
        typedef HANDLE type;

        static bool close(_In_ type handle) throw()
        {
            return ::CloseHandle(handle) != FALSE;
        }

        static type get_invalid_value() throw()
        {
            return INVALID_HANDLE_VALUE;
        }
    };

    struct __p4_crt_hmodule_traits
    {
        typedef HMODULE type;

        static bool close(_In_ type handle) throw()
        {
            return ::FreeLibrary(handle) != FALSE;
        }

        static type get_invalid_value() throw()
        {
            return nullptr;
        }
    };

    struct __p4_crt_findfile_traits
    {
        typedef HANDLE type;

        static bool close(_In_ type handle) throw()
        {
            return ::FindClose(handle) != FALSE;
        }

        static type get_invalid_value() throw()
        {
            return INVALID_HANDLE_VALUE;
        }
    };



    template <typename Traits>
    class __p4_crt_unique_handle_t
    {
    public:

        typedef Traits                traits_type;
        typedef typename Traits::type handle_type;

        explicit __p4_crt_unique_handle_t(handle_type const h = traits_type::get_invalid_value()) throw()
            : _handle(h)
        {
        }

        __p4_crt_unique_handle_t(_Inout_ __p4_crt_unique_handle_t&& h) throw()
            : _handle(h._handle)
        {
            h._handle = traits_type::get_invalid_value();
        }

        ~__p4_crt_unique_handle_t() throw()
        {
            close();
        }

        __p4_crt_unique_handle_t& operator=(_Inout_ __p4_crt_unique_handle_t&& h) throw()
        {
            close();
            _handle = h._handle;
            h._handle = traits_type::get_invalid_value();
            return *this;
        }

        void attach(handle_type h) throw()
        {
            if (h != _handle)
            {
                close();
                _handle = h;
            }
        }

        handle_type detach() throw()
        {
            handle_type h = _handle;
            _handle = traits_type::get_invalid_value();
            return h;
        }

        handle_type get() const throw()
        {
            return _handle;
        }

        void close() throw()
        {
            if (_handle == traits_type::get_invalid_value())
                return;

            traits_type::close(_handle);
            _handle = traits_type::get_invalid_value();
        }

        bool is_valid() const throw()
        {
            return _handle != traits_type::get_invalid_value();
        }

        operator bool() const throw()
        {
            return is_valid();
        }

        handle_type* get_address_of() throw()
        {
            return &_handle;
        }

        handle_type* release_and_get_address_of() throw()
        {
            close();
            return &_handle;
        }

    private:

        __p4_crt_unique_handle_t(__p4_crt_unique_handle_t const&) throw();
        __p4_crt_unique_handle_t& operator=(__p4_crt_unique_handle_t const&) throw();

        handle_type _handle;
    };

    typedef __p4_crt_unique_handle_t<__p4_crt_handle_traits>   __p4_crt_unique_handle;
    typedef __p4_crt_unique_handle_t<__p4_crt_hmodule_traits>  __p4_crt_unique_hmodule;
    typedef __p4_crt_unique_handle_t<__p4_crt_findfile_traits> __p4_crt_findfile_handle;


} // extern "C++"

#define _CORECRT_GENERATE_FORWARDER(prefix, callconv, name, callee_name)                     \
    __pragma(warning(push))                                                                  \
    __pragma(warning(disable: 4100)) /* unreferenced formal parameter */                     \
    template <typename... Params>                                                            \
    prefix auto callconv name(Params&&... args) throw() -> decltype(callee_name(args...))    \
    {                                                                                        \
        return callee_name(args...);                                                         \
    }                                                                                        \
    __pragma(warning(pop))

// Perforce
// These were to be surrounding the above return statement.  Removed them as we do
// not seem to cause a compiler deprecation warning.
//        _BEGIN_SECURE_CRT_DEPRECATION_DISABLE                                                \
//        _END_SECURE_CRT_DEPRECATION_DISABLE                                                  \

//==== ucrt/inc/corecrt_internal.h =========================

//==== ucrt/inc/corecrt_internal_traits.h =========================

template <typename Character>
struct __crt_char_traits;

#define _CORECRT_APPLY_TO_MAPPINGS(_APPLY)                                                                                                       \
    _APPLY(capture_argv,                       __acrt_capture_narrow_argv,                       __acrt_capture_wide_argv                      ) \
    _APPLY(create_process,                     CreateProcessA,                                   CreateProcessW                                ) \
    _APPLY(find_first_file_ex,                 FindFirstFileExA,                                 FindFirstFileExW                              ) \
    _APPLY(find_next_file,                     FindNextFileA,                                    FindNextFileW                                 ) \
    _APPLY(free_environment_strings,           FreeEnvironmentStringsA,                          FreeEnvironmentStringsW                       ) \
    _APPLY(ftprintf,                           fprintf,                                          fwprintf                                      ) \
    _APPLY(get_current_directory,              GetCurrentDirectoryA,                             GetCurrentDirectoryW                          ) \
    _APPLY(get_environment_from_os,            __dcrt_get_narrow_environment_from_os,            __dcrt_get_wide_environment_from_os           ) \
    _APPLY(get_module_file_name,               GetModuleFileNameA,                               GetModuleFileNameW                            ) \
    _APPLY(get_or_create_environment_nolock,   __dcrt_get_or_create_narrow_environment_nolock,   __dcrt_get_or_create_wide_environment_nolock  ) \
    _APPLY(get_temp_path,                      __acrt_GetTempPathA,                              GetTempPathW                                  ) \
    _APPLY(getc_nolock,                        _getc_nolock,                                     _getwc_nolock                                 ) \
    _APPLY(gettche_nolock,                     _getche_nolock,                                   _getwche_nolock                               ) \
    _APPLY(initialize_environment_nolock,      _initialize_narrow_environment,                   _initialize_wide_environment                  ) \
    _APPLY(istspace,                           isspace,                                          iswspace                                      ) \
    _APPLY(itot_s,                             _itoa_s,                                          _itow_s                                       ) \
    _APPLY(message_box,                        __acrt_MessageBoxA,                               __acrt_MessageBoxW                            ) \
    _APPLY(open_file,                          _openfile,                                        _wopenfile                                    ) \
    _APPLY(output_debug_string,                OutputDebugStringA,                               OutputDebugStringW                            ) \
    _APPLY(pack_command_line_and_environment,  __acrt_pack_narrow_command_line_and_environment,  __acrt_pack_wide_command_line_and_environment ) \
    _APPLY(puttc_nolock,                       _fputc_nolock,                                    _fputwc_nolock                                ) \
    _APPLY(puttch_nolock,                      _putch_nolock,                                    _putwch_nolock                                ) \
    _APPLY(set_current_directory,              SetCurrentDirectoryA,                             SetCurrentDirectoryW                          ) \
    _APPLY(set_environment_variable,           SetEnvironmentVariableA,                          SetEnvironmentVariableW                       ) \
    _APPLY(set_program_name,                   _set_pgmptr,                                      _set_wpgmptr                                  ) \
    _APPLY(set_variable_in_environment_nolock, __dcrt_set_variable_in_narrow_environment_nolock, __dcrt_set_variable_in_wide_environment_nolock) \
    _APPLY(show_message_box,                   __acrt_show_narrow_message_box,                   __acrt_show_wide_message_box                  ) \
    _APPLY(sntprintf_s,                        _snprintf_s,                                      _snwprintf_s                                  ) \
    _APPLY(taccess_s,                          _access_s,                                        _waccess_s                                    ) \
    _APPLY(tasctime,                           asctime,                                          _wasctime                                     ) \
    _APPLY(tasctime_s,                         asctime_s,                                        _wasctime_s                                   ) \
    _APPLY(tcscat_s,                           strcat_s,                                         wcscat_s                                      ) \
    _APPLY(tcschr,                             strchr,                                           wcschr                                        ) \
    _APPLY(tcscmp,                             strcmp,                                           wcscmp                                        ) \
    _APPLY(tcscpy_s,                           strcpy_s,                                         wcscpy_s                                      ) \
    _APPLY(tcserror_s,                         strerror_s,                                       _wcserror_s                                   ) \
    _APPLY(tcsicmp,                            _stricmp,                                         _wcsicmp                                      ) \
    _APPLY(tcslen,                             strlen,                                           wcslen                                        ) \
    _APPLY(tcsncat_s,                          strncat_s,                                        wcsncat_s                                     ) \
    _APPLY(tcsncmp,                            strncmp,                                          wcsncmp                                       ) \
    _APPLY(tcsncpy_s,                          strncpy_s,                                        wcsncpy_s                                     ) \
    _APPLY(tcsnicmp,                           _strnicmp,                                        _wcsnicmp                                     ) \
    _APPLY(tcsnicoll,                          _strnicoll,                                       _wcsnicoll                                    ) \
    _APPLY(tcsnlen,                            strnlen,                                          wcsnlen                                       ) \
    _APPLY(tcspbrk,                            strpbrk,                                          wcspbrk                                       ) \
    _APPLY(tcsrchr,                            strrchr,                                          wcsrchr                                       ) \
    _APPLY(tcstol,                             strtol,                                           wcstol                                        ) \
    _APPLY(tcstoul,                            strtoul,                                          wcstoul                                       ) \
    _APPLY(tcstoull,                           strtoull,                                         wcstoull                                      ) \
    _APPLY(tdupenv_s_crt,                      _dupenv_s_crt,                                    _wdupenv_s_crt                                ) \
    _APPLY(texecve,                            _execve,                                          _wexecve                                      ) \
    _APPLY(tfdopen,                            _fdopen,                                          _wfdopen                                      ) \
    _APPLY(tfullpath,                          _fullpath,                                        _wfullpath                                    ) \
    _APPLY(tgetcwd,                            _getcwd,                                          _wgetcwd                                      ) \
    _APPLY(tgetpath,                           __acrt_getpath,                                   __acrt_wgetpath                               ) \
    _APPLY(tmktemp_s,                          _mktemp_s,                                        _wmktemp_s                                    ) \
    _APPLY(tsopen_nolock,                      _sopen_nolock,                                    _wsopen_nolock                                ) \
    _APPLY(tsopen_s,                           _sopen_s,                                         _wsopen_s                                     ) \
    _APPLY(tspawnve,                           _spawnve,                                         _wspawnve                                     ) \
    _APPLY(tspawnvpe,                          _spawnvpe,                                        _wspawnvpe                                    ) \
    _APPLY(ulltot_s,                           _ui64toa_s,                                       _ui64tow_s                                    ) \
    _APPLY(ultot_s,                            _ultoa_s,                                         _ultow_s                                      ) \
    _APPLY(ungettc_nolock,                     _ungetc_nolock,                                   _ungetwc_nolock                               ) \
    _APPLY(ungettch_nolock,                    _ungetch_nolock,                                  _ungetwch_nolock                              )


template <>
struct __crt_char_traits<char>
{
    typedef char           char_type;
    typedef unsigned char  unsigned_char_type;
    typedef wchar_t        other_char_type;
    typedef int            int_type;
    typedef STARTUPINFOA   startup_info_type;
    typedef WIN32_FIND_DATAA win32_find_data_type;

    #define _APPLY(name, narrow_name, wide_name) _CORECRT_GENERATE_FORWARDER(static, __cdecl, name, narrow_name)
    _CORECRT_APPLY_TO_MAPPINGS(_APPLY)
    #undef _APPLY
};

template <>
struct __crt_char_traits<wchar_t>
{
    typedef wchar_t        char_type;
    typedef wchar_t        unsigned_char_type;
    typedef char           other_char_type;
    typedef wint_t         int_type;
    typedef STARTUPINFOW   startup_info_type;
    typedef WIN32_FIND_DATAW win32_find_data_type;

    #define _APPLY(name, narrow_name, wide_name) _CORECRT_GENERATE_FORWARDER(static, __cdecl, name, wide_name)
    _CORECRT_APPLY_TO_MAPPINGS(_APPLY)
    #undef _APPLY
};

//==== ucrt/inc/corecrt_internal_traits.h =========================

//==== definitions from various headers ===========================


//==== argv_data.cpp ===========================

// Perforce
// Name change
extern "C" {
    extern char*     _p4_acmdln;
    extern wchar_t*  _p4_wcmdln;
}
extern "C" {


// Note:  In general, either the narrow or wide string variables will be set,
// but not both.  These get initialized by the CRT startup sequence before any
// user code is executed.  There are cases where any or all of the pointers may
// be null during execution.  Do not assume that they are non-null.

int       __p4_argc   = 0;       // The number of arguments in __argv or __wargv
char**    __p4_argv   = nullptr; // The arguments as narrow strings
wchar_t** __p4_wargv  = nullptr; // The arguments as wide strings
char*     _p4_pgmptr  = nullptr; // The name of the program as a narrow string
wchar_t*  _p4_wpgmptr = nullptr; // The name of the program as a wide string
char*     _p4_acmdln  = nullptr; // The raw command line as a narrow string
wchar_t*  _p4_wcmdln  = nullptr; // The raw command line as a wide string

//_BEGIN_SECURE_CRT_DEPRECATION_DISABLE

int*       __cdecl __p___p4_argc()   { return &__p4_argc;   }
char***    __cdecl __p___p4_argv()   { return &__p4_argv;   }
wchar_t*** __cdecl __p___p4_wargv()  { return &__p4_wargv;  }
char**     __cdecl __p__p4_pgmptr()  { return &_p4_pgmptr;  }
wchar_t**  __cdecl __p__p4_wpgmptr() { return &_p4_wpgmptr; }
char**     __cdecl __p__p4_acmdln()  { return &_p4_acmdln;  }
wchar_t**  __cdecl __p__p4_wcmdln()  { return &_p4_wcmdln;  }

#ifdef NOT_USED
errno_t __cdecl _get_wpgmptr(wchar_t** const result)
{
#ifdef FROM_MS_CRT
    _VALIDATE_RETURN_ERRCODE(result   != nullptr, EINVAL);
    _VALIDATE_RETURN_ERRCODE(_wpgmptr != nullptr, EINVAL);
#endif

    *result = _p4_wpgmptr;
    return 0;
}

errno_t __cdecl _get_pgmptr(char** const result)
{
#ifdef FROM_MS_CRT
    _VALIDATE_RETURN_ERRCODE(result  != nullptr, EINVAL);
    _VALIDATE_RETURN_ERRCODE(_pgmptr != nullptr, EINVAL);
#endif

    *result = _p4_pgmptr;
    return 0;
}

//_END_SECURE_CRT_DEPRECATION_DISABLE



bool __cdecl __acrt_initialize_command_line()
{
    _p4_acmdln = GetCommandLineA();
    _p4_wcmdln = GetCommandLineW();
    return true;
}

bool __cdecl __acrt_uninitialize_command_line(bool const /* terminating */)
{
    return true;
}
#endif // NOT_USED



} // extern "C"

//==== argv_data.cpp ===========================

//==== argv_wildcards.cpp ===========================

namespace
{
    template <typename Character>
    class argument_list
    {
    public:

        argument_list() throw() : _first(nullptr), _last(nullptr), _end(nullptr) { }

        size_t      size()  const throw() { return _last - _first; }
        Character** begin() const throw() { return _first;         }
        Character** end()   const throw() { return _last;          }

        errno_t append(Character* const element) throw()
        {
            errno_t const expand_status = expand_if_necessary();
            if (expand_status != 0)
            {
                _free_crt(element);
                return expand_status;
            }

            *_last++ = element;
            return 0;
        }

        Character** detach() throw()
        {
            _last = nullptr;
            _end  = nullptr;

            Character** const first = _first;
            _first = nullptr;
            return first;
        }

        ~argument_list() throw()
        {
            for (auto it = _first; it != _last; ++it)
                _free_crt(*it);

            _free_crt(_first);
        }

    private:

        argument_list(argument_list const&) throw();            // not implemented
        argument_list& operator=(argument_list const&) throw(); // not implemented

        errno_t expand_if_necessary() throw()
        {
            // If there is already room for more elements, just return:
            if (_last != _end)
            {
                return 0;
            }
            // If the list has not yet had an array allocated for it, allocate one:
            if (!_first)
            {
                size_t const initial_count = 4;

                _first = _calloc_crt_t(Character*, initial_count).detach();
                if (!_first)
                    return ENOMEM;

                _last = _first;
                _end  = _first + initial_count;
                return 0;
            }
            // Otherwise, double the size of the array:
            else
            {
                size_t const old_count = _end - _first;
                if (old_count > SIZE_MAX / 2)
                    return ENOMEM;

                size_t const new_count = old_count * 2;
                __p4_crt_unique_heap_ptr<Character*> new_array(_recalloc_crt_t(Character*, _first, new_count));
                if (!new_array)
                    return ENOMEM;

                _first = new_array.detach();
                _last = _first + old_count;
                _end  = _first + new_count;
                return 0;
            }
        }

        Character** _first;
        Character** _last;
        Character** _end;
    };
}

_Check_return_
static char*
previous_character(_In_reads_z_(current - first + 1) char* const first,
                   _In_z_ char* const current) throw()
{
    return reinterpret_cast<char*>(_mbsdec(
        reinterpret_cast<unsigned char*>(first),
        reinterpret_cast<unsigned char*>(current)));
}

static wchar_t*
previous_character(_In_reads_(0) wchar_t*, _In_reads_(0) wchar_t* const current) throw()
{
    return current - 1;
}



template <typename Character>
static errno_t copy_and_add_argument_to_buffer(
    _In_z_ Character const*    const file_name,
    _In_z_ Character const*    const directory,
               size_t          const directory_length,
    argument_list<Character>&        buffer
    ) throw()
{
    typedef __crt_char_traits<Character> traits;

    size_t const file_name_count = traits::tcslen(file_name) + 1;
    if (file_name_count > SIZE_MAX - directory_length)
        return ENOMEM;

    size_t const required_count = directory_length + file_name_count + 1;
    __p4_crt_unique_heap_ptr<Character> argument_buffer(_calloc_crt_t(Character, required_count));

    if (directory_length > 0)
    {
        _ERRCHECK(traits::tcsncpy_s(argument_buffer.get(), required_count, directory, directory_length));
    }

    _ERRCHECK(traits::tcsncpy_s(
        argument_buffer.get() + directory_length,
        required_count        - directory_length,
        file_name,
        file_name_count));

    return buffer.append(argument_buffer.detach());
}



template <typename Character>
static errno_t expand_argument_wildcards(
    Character*          const argument,
    Character*          const wildcard,
    argument_list<Character>& buffer
    ) throw()
{
    typedef __crt_char_traits<Character>          traits;
    typedef typename traits::win32_find_data_type find_data_type;

    auto const is_directory_separator = [](Character const c) { return c == '/' || c == '\\' || c == ':'; };

    // Find the first slash or colon before the wildcard:
    Character* it = wildcard;
    while (it != argument && !is_directory_separator(*it))
        it = previous_character(argument, it);

    // If we found a colon that can't form a drive name (e.g. it can't be 'D:'),
    // then just add the argument as-is (we don't know how to expand it):
    if (*it == ':' && it != argument + 1)
        return copy_and_add_argument_to_buffer(argument, static_cast<Character*>(nullptr), 0, buffer);

    size_t const directory_length = is_directory_separator(*it)
        ? it - argument + 1 // it points to the separator, so add 1 to include it.
        : 0;

    // Try to begin the find operation:
    find_data_type find_data{};
    __p4_crt_findfile_handle const find_handle(traits::find_first_file_ex(
        argument,
        FindExInfoStandard,
        &find_data,
        FindExSearchNameMatch,
        nullptr,
        0));

    // If the find operation failed, there was no match, so just add the argument:
    if (find_handle.get() == INVALID_HANDLE_VALUE)
        return copy_and_add_argument_to_buffer(argument, static_cast<Character*>(nullptr), 0, buffer);

    size_t const old_argument_count = buffer.size();

    do
    {
        Character* const file_name = find_data.cFileName;

        // Skip . and ..:
        if (file_name[0] == '.' && file_name[1] == '\0')
            continue;

        if (file_name[0] == '.' && file_name[1] == '.' && file_name[2] == '\0')
            continue;

        errno_t const add_status = copy_and_add_argument_to_buffer(file_name, argument, directory_length, buffer);
        if (add_status != 0)
            return add_status;
    }
    while (traits::find_next_file(find_handle.get(), &find_data));

    // If we didn't add any arguments to the buffer, then we're done:
    size_t const new_argument_count = buffer.size();
    if (old_argument_count == new_argument_count)
        return 0;

    // If we did add new arguments, let's helpfully sort them:
    qsort(
        buffer.begin()     + old_argument_count,
        new_argument_count - old_argument_count,
        sizeof(Character*),
        [](void const* lhs, void const* rhs) -> int
        {
            if (lhs < rhs) { return -1; }
            if (lhs > rhs) { return  1; }
            return 0;
        });

    return 0;
}


template <typename Character>
static errno_t
common_expand_argv_wildcards(Character** const argv, int* arg_quoted, Character*** const result) throw()
{
    typedef __crt_char_traits<Character> traits;

#ifdef FROM_MS_CRT
    _VALIDATE_RETURN_ERRCODE(result != nullptr, EINVAL);
#endif
    *result = nullptr;

    argument_list<Character> expansion_buffer;
    // Perforce
    // Using i to traverse the arg_quoted array.
    int i;
    for (Character** it = argv, i=0; *it != nullptr; ++it, ++i)
    {
        Character const wildcard_characters[] = { '*', '?', '\0' };
        Character* const wildcard = traits::tcspbrk(*it, wildcard_characters);

        // If no wildcard characters were found in the argument string, just
        // append it to the list and continue on.  Otherwise, do the expansion:
        // Perforce
		// If the arg was quoted, pretned there was no wildcard.
        if (!wildcard || arg_quoted[i]==1)
        {
            errno_t const append_status = copy_and_add_argument_to_buffer(
                *it,
                static_cast<Character*>(nullptr),
                0,
                expansion_buffer);

            if (append_status != 0)
                return append_status;
        }
        else
        {
            errno_t const expand_status = expand_argument_wildcards(*it, wildcard, expansion_buffer);
            if (expand_status != 0)
                return expand_status;
        }
    }

    // Now that we've accumulated the expanded arguments into the expansion
    // buffer, we want to re-pack them in the form used by the argv parser,
    // in a single array, with everything "concatenated" together.
    size_t const argument_count  = expansion_buffer.size() + 1;
    size_t       character_count = 0;
    for (auto it = expansion_buffer.begin(); it != expansion_buffer.end(); ++it)
        character_count += traits::tcslen(*it) + 1;

    __p4_crt_unique_heap_ptr<unsigned char> expanded_argv(__p4_acrt_allocate_buffer_for_argv(
        argument_count,
        character_count,
        sizeof(Character)));

    if (!expanded_argv)
        return -1;

    Character** const argument_first  = reinterpret_cast<Character**>(expanded_argv.get());
    Character*  const character_first = reinterpret_cast<Character*>(
        expanded_argv.get() +
        argument_count * sizeof(Character*));

    Character** argument_it  = argument_first;
    Character*  character_it = character_first;
    for (auto it = expansion_buffer.begin(); it != expansion_buffer.end(); ++it)
    {
        size_t const count = traits::tcslen(*it) + 1;

        _ERRCHECK(traits::tcsncpy_s(
            character_it,
            character_count - (character_it - character_first),
            *it,
            count));

        *argument_it++ = character_it;
        character_it += count;
    }

    *result = reinterpret_cast<Character**>(expanded_argv.detach());
    return 0;
}


extern "C" errno_t
__p4_acrt_expand_narrow_argv_wildcards(char** const argv, int* arg_quoted, char*** const result)
{
    return common_expand_argv_wildcards(argv, arg_quoted, result);
}

extern "C" errno_t
__p4_acrt_expand_wide_argv_wildcards(wchar_t** const argv, int* arg_quoted, wchar_t*** const result)
{
    return common_expand_argv_wildcards(argv, arg_quoted, result);
}

//==== argv_wildcards.cpp ===========================


//==== argv_parsing.cpp ===========================

// In the function below, we need to ensure that we've initialized the mbc table
// before we start performing character transformations.
// Perforce
// Naming change
#ifdef FROM_MS_CRT
extern "C" bool __cdecl __acrt_initialize_multibyte();
static void do_locale_initialization(char)    throw() { __acrt_initialize_multibyte(); }
static void do_locale_initialization(wchar_t) throw() { /* no-op */                    }
#endif

static char*    get_command_line(char)    throw() { return GetCommandLineA(); }
static wchar_t* get_command_line(wchar_t) throw() { return GetCommandLineW(); }

static char**&    get_argv(char)    throw() { return __p4_argv;  }
static wchar_t**& get_argv(wchar_t) throw() { return __p4_wargv; }

static errno_t expand_argv_wildcards(
    _In_z_               char**  const argv,
    _In_z_               int*    arg_quoted,
    _Out_ _Deref_post_z_ char*** const expanded_argv) throw()
{
    return __p4_acrt_expand_narrow_argv_wildcards(argv, arg_quoted, expanded_argv);
}

static errno_t expand_argv_wildcards(
    _In_z_               wchar_t**  const argv,
    _In_z_               int*    arg_quoted,
    _Out_ _Deref_post_z_ wchar_t*** const expanded_argv) throw()
{
    return __p4_acrt_expand_wide_argv_wildcards(argv, arg_quoted, expanded_argv);
}



/***
*static void parse_cmdline(cmdstart, argv, args, expand_wildarg, argument_count, character_count)
*
*Purpose:
*       Parses the command line and sets up the argv[] array.
*       On entry, cmdstart should point to the command line,
*       argv should point to memory for the argv array, args
*       points to memory to place the text of the arguments.
*       If these are nullptr, then no storing (only counting)
*       is done.  On exit, *argument_count has the number of
*       arguments (plus one for a final nullptr argument),
*       and *character_count has the number of bytes used in the buffer
*       pointed to by args.
*
*Entry:
*       Character *cmdstart - pointer to command line of the form
*           <progname><nul><args><nul>
*       Character **argv - where to build argv array; nullptr means don't
*                       build array
*       Character *args - where to place argument text; nullptr means don't
*                       store text
*
*Exit:
*       no return value
*       int expand_wildarg[] - apply wild card expansion to this argument
*       int *argument_count - returns number of argv entries created
*       int *character_count - number of characters used in args buffer
*
*Exceptions:
*
*******************************************************************************/

static bool __cdecl should_copy_another_character(char const c) throw()
{
    return _ismbblead(c) != 0;
}

static bool __cdecl should_copy_another_character(wchar_t) throw()
{
    return false;
}

template <typename Character>
static void __cdecl parse_command_line(
    Character*  cmdstart,
    Character** argv,
    Character*  args,
    int*        arg_quoted,
    size_t*     argument_count,
    size_t*     character_count
    ) throw()
{
    bool quoted=false;

    *character_count = 0;
    *argument_count  = 1; // We'll have at least the program name

    Character c;
    int copy_character;             /* 1 = copy char to *args */
    unsigned numslash;              /* num of backslashes seen */

    /* first scan the program name, copy it, and count the bytes */
    Character* p = cmdstart;
    if (argv)
        *argv++ = args;

    // A quoted program name is handled here. The handling is much
    // simpler than for other arguments. Basically, whatever lies
    // between the leading double-quote and next one, or a terminal null
    // character is simply accepted. Fancier handling is not required
    // because the program name must be a legal NTFS/HPFS file name.
    // Note that the double-quote characters are not copied, nor do they
    // contribute to character_count.
    //
    // Perforce
    // If an executable has a space in the name, it will be double quoted.
    // Visual Studio will have the full executable path when debugging.
    // Otherwise the executable name will be as the user entered it in the
    // Command Prompt.
    //
    bool in_quotes = false;
    do
    {
        if (*p == '"')
        {
            in_quotes = !in_quotes;
            c = *p++;
            continue;
        }

        ++*character_count;
        if (args)
            *args++ = *p;

        c = *p++;

        if (should_copy_another_character(c))
        {
            ++*character_count;
            if (args)
                *args++ = *p; // Copy 2nd byte too
            ++p; // skip over trail byte
        }
    }
    while (c != '\0' && (in_quotes || (c != ' ' && c != '\t')));

    if (c == '\0')
    {
        p--;
    }
    else
    {
        if (args)
            *(args - 1) = '\0';
    }
    // Perforce
	// The program name is never expanded.
    if (arg_quoted)
        *arg_quoted++ = 0;

    in_quotes = false;

    // Loop on each argument
    //
    // Perforce
    // The Command Prompt will double quote an argment which has spaces.
    // GetCommandLine can place more than one space between the appname and the first arg.
    // Examples of argument handling,
    //    \"arg\"    ==> argv[n]="arg"
    //    "arg"      ==> argv[n]=arg
    //    "arg sp"   ==> argv[n]=arg sp
    //    ""arg sp"" ==> argv[n]=arg and argv[n+1]=sp
    //    a\b        ==> a\b
    //    a\\b       ==> a\\b
    //    a\\\b      ==> a\\\b
    //    a\"b       ==> a"b
    //    a\\"b      ==> a\b
    //    a\\\"b     ==> a\"b
    //
    for (;;)
    {
        if (*p)
        {
            while (*p == ' ' || *p == '\t')
                ++p;
        }

        if (*p == '\0')
            break; // End of arguments

        // Scan an argument:
        if (argv)
            *argv++ = args;

        ++*argument_count;

        // Loop through scanning one argument:
        for (;;)
        {
            copy_character = 1;

            // Rules:
            // 2N     backslashes   + " ==> N backslashes and begin/end quote
            // 2N + 1 backslashes   + " ==> N backslashes + literal "
            // N      backslashes       ==> N backslashes
            numslash = 0;

            while (*p == '\\')
            {
                // Count number of backslashes for use below
                ++p;
                ++numslash;
            }

            if (*p == '"')
            {
                // if 2N backslashes before, start/end quote, otherwise
                // copy literally:
                if (numslash % 2 == 0)
                {
                    if (in_quotes && p[1] == '"')
                    {
                        p++; // Double quote inside quoted string
                    }
                    else
                    {   
                        // Skip first quote char and copy second:
                        copy_character = 0; // Don't copy quote
                        in_quotes = !in_quotes;
                        quoted = true;
                    }
                }

                numslash /= 2;
            }

            // Copy slashes:
            while (numslash--)
            {
                if (args)
                    *args++ = '\\';
                ++*character_count;
            }

            // If at end of arg, break loop:
            if (*p == '\0' || (!in_quotes && (*p == ' ' || *p == '\t')))
                break;

            // Copy character into argument:
            if (copy_character)
            {
                if (args)
                    *args++ = *p;

                if (should_copy_another_character(*p))
                {
                    ++p;
                    ++*character_count;

                    if (args)
                        *args++ = *p;
                }

                ++*character_count;
            }
            
            ++p;
        }

        // Null-terminate the argument:
        if (args)
            *args++ = '\0'; // Terminate the string

        // Perforce
		// Track arg quoting.
        if (arg_quoted)
            *arg_quoted++ = quoted ? 1 : 0;
        quoted = false;
        ++*character_count;
    }

    // We put one last argument in -- a null pointer:
    if (argv)
        *argv++ = nullptr;

    ++*argument_count;
}



extern "C" unsigned char* __cdecl
__p4_acrt_allocate_buffer_for_argv(
    size_t const argument_count,
    size_t const character_count,
    size_t const character_size
    )
{
    if (argument_count >= SIZE_MAX / sizeof(void*))
        return nullptr;

    if (character_count >= SIZE_MAX / character_size)
        return nullptr;

    size_t const argument_array_size  = argument_count  * sizeof(void*);
    size_t const character_array_size = character_count * character_size;

    if (SIZE_MAX - argument_array_size <= character_array_size)
        return nullptr;

    size_t const total_size = argument_array_size + character_array_size;
    __p4_crt_unique_heap_ptr<unsigned char> buffer(_calloc_crt_t(unsigned char, total_size));
    if (!buffer)
        return nullptr;

    return buffer.detach();
}



/***
*_setargv, __setargv - set up "argc" and "argv" for C programs
*
*Purpose:
*       Read the command line and create the argv array for C
*       programs.
*
*Entry:
*       Arguments are retrieved from the program command line,
*       pointed to by _acmdln.
*
*Exit:
*       Returns 0 if successful, -1 if memory allocation failed.
*       "argv" points to a null-terminated list of pointers to ASCIZ
*       strings, each of which is an argument from the command line.
*       "argc" is the number of arguments.  The strings are copied from
*       the environment segment into space allocated on the heap/stack.
*       The list of pointers is also located on the heap or stack.
*       _pgmptr points to the program name.
*
*Exceptions:
*       Terminates with out of memory error if no memory to allocate.
*
*******************************************************************************/
template <typename Character>
static errno_t __cdecl
common_configure_argv(_crt_argv_mode const mode) throw()
{
    typedef __crt_char_traits<Character> traits;
    // Perforce
	// Keep track of quoted arguments.
    int *arg_quoted=NULL;

#ifdef FROM_MS_CRT
    _VALIDATE_RETURN_ERRCODE(
        mode == _crt_argv_expanded_arguments ||
        mode == _crt_argv_unexpanded_arguments, EINVAL);

    do_locale_initialization(Character());
#endif

    
    static Character program_name[MAX_PATH + 1];
    traits::get_module_file_name(nullptr, program_name, MAX_PATH);
    traits::set_program_name(&program_name[0]);

    // If there's no command line at all, then use the program name as the
    // command line to parse, so that argv[0] is initialized with the program
    // name.  (This won't happen when the program is run by cmd.exe, but it
    // could happen if the program is spawned via some other means.)
    Character* const raw_command_line = get_command_line(Character());
    Character* const command_line = raw_command_line == nullptr || raw_command_line[0] == '\0'
        ? program_name
        : raw_command_line;

    size_t argument_count  = 0;
    size_t character_count = 0;
    parse_command_line(
        command_line,
        static_cast<Character**>(nullptr),
        static_cast<Character*>(nullptr),
        NULL,
        &argument_count,
        &character_count);

    __p4_crt_unique_heap_ptr<unsigned char> buffer(__p4_acrt_allocate_buffer_for_argv(
        argument_count,
        character_count,
        sizeof(Character)));

    arg_quoted = (int *)_calloc_crt(argument_count, sizeof(int));
    for (int i=0; i<argument_count; i++)
        arg_quoted[i] = false;

#ifdef FROM_MS_CRT
    _VALIDATE_RETURN_ERRCODE_NOEXC(buffer, ENOMEM);
#endif

    Character** const first_argument = reinterpret_cast<Character**>(buffer.get());
    Character*  const first_string   = reinterpret_cast<Character*>(buffer.get() + argument_count * sizeof(Character*));

    parse_command_line(
        command_line,
        first_argument,
        first_string,
        arg_quoted,
        &argument_count,
        &character_count);

    // If we are not expanding wildcards, then we are done...
    if (mode == _crt_argv_unexpanded_arguments)
    {
        __p4_argc = static_cast<int>(argument_count - 1);
        get_argv(Character()) = reinterpret_cast<Character**>(buffer.detach());
        free(arg_quoted);
        return 0;
    }

    // ... otherwise, we try to do the wildcard expansion:
    __p4_crt_unique_heap_ptr<Character*> expanded_argv;
    errno_t const argv_expansion_status =
        expand_argv_wildcards(first_argument, arg_quoted, expanded_argv.get_address_of());
    free(arg_quoted);
    if (argv_expansion_status != 0)
        return argv_expansion_status;

    __p4_argc = [&]()
    {
        size_t n = 0;
        for (auto it = expanded_argv.get(); *it; ++it, ++n) { }
        return static_cast<int>(n);
    }();

    get_argv(Character()) = expanded_argv.detach();
    return 0;
}



// Perforce
// Naming changes.
extern "C" errno_t __cdecl
_p4_configure_narrow_argv(_crt_argv_mode const mode)
{
    return common_configure_argv<char>(mode);
}

// Perforce
// Naming changes.
extern "C" errno_t __cdecl
_p4_configure_wide_argv(_crt_argv_mode const mode)
{
    return common_configure_argv<wchar_t>(mode);
}

//==== argv_parsing.cpp ===========================
