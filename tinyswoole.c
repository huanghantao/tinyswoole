#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_tinyswoole.h"


// arginfo ----------------------------------------------------------

ZEND_BEGIN_ARG_INFO_EX(arginfo_tinyswoole_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tinyswoole_server__construct, 0, 0, 2) // The last parameter is the minimum number of required parameters.
	ZEND_ARG_INFO(0, ip) // Whether the first parameter is a reference. 1 yes, 0 is not
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, sock_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tinyswoole_server_set, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tinyswoole_server_on, 0, 0, 2)
	ZEND_ARG_INFO(0, event_name)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tinyswoole_server_send, 0, 0, 2)
	ZEND_ARG_INFO(0, fd)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

// arginfo end ----------------------------------------------------------

/**
 * Register the function pointer to the Zend Engine
 */
const zend_function_entry tinyswoole_functions[] = {
	// PHP_FE(tinyswoole_version, arginfo_tinyswoole_void)
	PHP_FE_END
};

/**
 * Register function pointers of class swoole_server to the Zend Engine
 */
zend_function_entry tinyswoole_server_methods[] = {
	PHP_ME(tinyswoole_server, __construct, arginfo_tinyswoole_server__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
	PHP_ME(tinyswoole_server, set, arginfo_tinyswoole_server_set, ZEND_ACC_PUBLIC)
	PHP_ME(tinyswoole_server, on, arginfo_tinyswoole_server_on, ZEND_ACC_PUBLIC)
	PHP_ME(tinyswoole_server, start, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(tinyswoole_server, send, arginfo_tinyswoole_server_send, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/**
 * Define zend class entry
 */
zend_class_entry tinyswoole_server_ce;
zend_class_entry *tinyswoole_server_ce_ptr; // Without thread safety protection

PHP_MINIT_FUNCTION(tinyswoole)
{
    /**
     * socket type
     */
    REGISTER_LONG_CONSTANT("TSWOOLE_SOCK_TCP", TSW_SOCK_TCP, CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("TSWOOLE_SOCK_UDP", TSW_SOCK_UDP, CONST_CS | CONST_PERSISTENT);

    /**
     * socket type
     */
	REGISTER_LONG_CONSTANT("TSWOOLE_TCP", TSW_SOCK_TCP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TSWOOLE_UDP", TSW_SOCK_UDP, CONST_CS | CONST_PERSISTENT);

	// INIT_CLASS_ENTRY(ce, "tinyswoole_server", tinyswoole_server_methods);
	INIT_NS_CLASS_ENTRY(tinyswoole_server_ce, "TinySwoole", "Server", tinyswoole_server_methods);
	tinyswoole_server_ce_ptr = zend_register_internal_class(&tinyswoole_server_ce TSRMLS_CC); // Registered in the Zend Engine

	zend_declare_property_null(tinyswoole_server_ce_ptr, ZEND_STRL("ip"), ZEND_ACC_PRIVATE);
	zend_declare_property_null(tinyswoole_server_ce_ptr, ZEND_STRL("port"), ZEND_ACC_PRIVATE);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(tinyswoole)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(tinyswoole)
{
#if defined(COMPILE_DL_TINYSWOOLE) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(tinyswoole)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(tinyswoole)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "tinyswoole support", "enabled");
	php_info_print_table_end();
}

/**
 * The structure contains a wealth of information that tells the Zend Engine about the extension's dependencies
 */
zend_module_entry tinyswoole_module_entry = {
	STANDARD_MODULE_HEADER,
	"tinyswoole", // Extension module name
	tinyswoole_functions,
	PHP_MINIT(tinyswoole), // Module initialization
	PHP_MSHUTDOWN(tinyswoole), // Module shutdown
	PHP_RINIT(tinyswoole), // Request initialization
	PHP_RSHUTDOWN(tinyswoole), // Request shutdown
	PHP_MINFO(tinyswoole),
	PHP_TINYSWOOLE_VERSION,
	STANDARD_MODULE_PROPERTIES
};

zval *tsw_zend_read_property(zend_class_entry *class_ptr, zval *obj, const char *s, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, s, len, silent, &rv);
}

char *tsw_zend_println_zval_type(zval *v)
{
	switch (Z_TYPE_P(v)) {
		case IS_UNDEF:
			printf("UNDEFINE\n");
			break;
		case IS_NULL:
			printf("NULL\n");
			break;
		case IS_FALSE:
			printf("FALSE\n");
			break;
		case IS_TRUE:
			printf("TRUE\n");
			break;
		case IS_LONG:
			printf("LONG\n");
			break;
		case IS_DOUBLE:
			printf("DOUBLE\n");
			break;
		case IS_STRING:
			printf("STRING\n");
			break;
		case IS_ARRAY:
			printf("ARRAY\n");
			break;
		case IS_OBJECT:
			printf("OBJECT\n");
			break;
		case IS_RESOURCE:
			printf("RESOURCE\n");
			break;
		case IS_REFERENCE:
			printf("REFERENCE\n");
			break;
		default:
			break;
	}
}

#ifdef COMPILE_DL_TINYSWOOLE
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(tinyswoole)
#endif
