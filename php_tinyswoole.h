/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_TINYSWOOLE_H
#define PHP_TINYSWOOLE_H

extern zend_module_entry tinyswoole_module_entry;
#define phpext_tinyswoole_ptr &tinyswoole_module_entry

#define PHP_TINYSWOOLE_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_TINYSWOOLE_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_TINYSWOOLE_API __attribute__ ((visibility("default")))
#else
#	define PHP_TINYSWOOLE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(tinyswoole)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(tinyswoole)
*/

/* Always refer to the globals in your function as TINYSWOOLE_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define TINYSWOOLE_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(tinyswoole, v)

#if defined(ZTS) && defined(COMPILE_DL_TINYSWOOLE)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#include "include/epoll.h"
#include "include/log.h"
#include "include/process_pool.h"
#include "include/server.h"
#include "include/socket.h"
#include "include/tinyswoole.h"
#include "include/tswoole_config.h"
#include "include/worker.h"


#define tinyswoole_php_fatal_error(level, fmt_str, ...)		php_error_docref(NULL TSRMLS_CC, level, fmt_str, ##__VA_ARGS__)
#define TSW_MAKE_STD_ZVAL(p)		zval _stack_zval_##p; p = &(_stack_zval_##p)

enum php_tinyswoole_server_callback_type {
    TSW_SERVER_CB_onStart,
    TSW_SERVER_CB_onConnect,
    TSW_SERVER_CB_onReceive,
    TSW_SERVER_CB_onClose,
};

#define PHP_SERVER_CALLBACK_NUM             (TSW_SERVER_CB_onClose+1)

zval *server_object;
extern zend_class_entry *tinyswoole_server_ce_ptr;

extern tswServerG TSwooleG;
extern tswWorkerG TSwooleWG;

zval *tsw_zend_read_property(zend_class_entry *class_ptr, zval *obj, const char *s, int len, int silent);
char *tsw_zend_println_zval_type(zval *v);

void php_tswoole_register_callback(tswServer *serv);
void php_tswoole_onStart(tswServer *serv);
void php_tswoole_onConnect(int fd);
void php_tswoole_onReceive(tswServer *serv, tswEventData *event_data);

PHP_METHOD(tinyswoole_server, __construct);
PHP_METHOD(tinyswoole_server, set);
PHP_METHOD(tinyswoole_server, start);
PHP_METHOD(tinyswoole_server, on);
PHP_METHOD(tinyswoole_server, send);
PHP_FUNCTION(call_function);

extern PHPAPI int php_array_merge(HashTable *dest, HashTable *src);

#define php_tinyswoole_array_get_value(ht, str, v)     ((v = zend_hash_str_find(ht, str, sizeof(str)-1)) && !ZVAL_IS_NULL(v))
#define php_tinyswoole_array_separate(arr)       zval *_new_##arr;\
    TSW_MAKE_STD_ZVAL(_new_##arr);\
    array_init(_new_##arr);\
    php_array_merge(Z_ARRVAL_P(_new_##arr), Z_ARRVAL_P(arr));\
    arr = _new_##arr;

#endif	/* PHP_TINYSWOOLE_H */

