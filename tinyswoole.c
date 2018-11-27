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

ZEND_BEGIN_ARG_INFO_EX(arginfo_tinyswoole__construct, 0, 0, 2) // The last parameter is the minimum number of required parameters.
	ZEND_ARG_INFO(0, ip) // Whether the first parameter is a reference. 1 yes, 0 is not
    ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, sock_type)
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
	ZEND_ME(tinyswoole_server, __construct, arginfo_tinyswoole__construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
	ZEND_ME(tinyswoole_server, start, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/**
 * Define zend class entry
 */
zend_class_entry *tinyswoole_server_ce;


static inline zval* tsw_zend_read_property(zend_class_entry *class_ptr, zval *obj, const char *s, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, s, len, silent, &rv);
}

PHP_METHOD(tinyswoole_server, __construct)
{
	char *serv_host;
	size_t host_len;
	long serv_port;
	zval *server_object;
	long sock_type = TSW_SOCK_TCP;
	int sock;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|l", &serv_host, &host_len, &serv_port, &sock_type) == FAILURE) {
		RETURN_NULL();
	}

	sock = tswSocket_create(sock_type);
	if (sock < 0) {
		RETURN_NULL();
	}
	if (tswSocket_bind(sock, sock_type, serv_host, serv_port) < 0) {
		RETURN_NULL();
	}

	server_object = getThis();
	zend_update_property_string(tinyswoole_server_ce, server_object, "ip", sizeof("ip") - 1, serv_host);
	zend_update_property_long(tinyswoole_server_ce, server_object, "port", sizeof("port") - 1, serv_port);
	zend_update_property_long(tinyswoole_server_ce, server_object, "sock", sizeof("sock") - 1, sock);
}

PHP_METHOD(tinyswoole_server, start)
{
	zval *sock;

	sock = tsw_zend_read_property(tinyswoole_server_ce, getThis(), "sock", sizeof("sock") - 1, 0);

	printf("running server...\n");
	start(Z_LVAL(*sock));
}



PHP_MINIT_FUNCTION(tinyswoole)
{
	/**
     * socket type
     */
    REGISTER_LONG_CONSTANT("TSWOOLE_SOCK_TCP", TSW_SOCK_TCP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("TSWOOLE_SOCK_UDP", TSW_SOCK_UDP, CONST_CS | CONST_PERSISTENT);

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "tinyswoole_server", tinyswoole_server_methods);
	tinyswoole_server_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(tinyswoole_server_ce, "ip", sizeof("ip") - 1, ZEND_ACC_PRIVATE);
	zend_declare_property_null(tinyswoole_server_ce, "port", sizeof("port") - 1, ZEND_ACC_PRIVATE);
	zend_declare_property_null(tinyswoole_server_ce, "sock", sizeof("sock") - 1, ZEND_ACC_PRIVATE);

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

#ifdef COMPILE_DL_TINYSWOOLE
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(tinyswoole)
#endif
