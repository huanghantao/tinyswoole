#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_tinyswoole.h"

zval *php_tsw_server_callbacks[PHP_SERVER_CALLBACK_NUM];
zend_fcall_info_cache *php_tsw_server_caches[PHP_SERVER_CALLBACK_NUM];


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
	zend_update_property_string(tinyswoole_server_ce_ptr, server_object, "ip", sizeof("ip") - 1, serv_host);
	zend_update_property_long(tinyswoole_server_ce_ptr, server_object, "port", sizeof("port") - 1, serv_port);
	zend_update_property_long(tinyswoole_server_ce_ptr, server_object, "sock", sizeof("sock") - 1, sock);
}

PHP_METHOD(tinyswoole_server, on)
{
	int i;
	char property_name[128]; // on + event name eg onConnect onReceive
	int property_name_len = 0;

	zval *server_object;

	zval *name;
	zval *callable;
	zend_fcall_info_cache *func_cache;
	zend_string *callable_name;
	char *callback_name[PHP_SERVER_CALLBACK_NUM] = {
		"Start",
		"Connect",
		"Receive",
		"Close",
	};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &name, &callable) == FAILURE) {
		return;
    }

	func_cache = emalloc(sizeof(zend_fcall_info_cache));
	if (zend_is_callable_ex(callable, NULL, 0, &callable_name, func_cache, NULL) < 0) {
		tinyswoole_php_fatal_error(E_ERROR, "function '%s' is not callable", Z_STRVAL(*name));
		return;
	}

	server_object = getThis();
	memcpy(property_name, "on", 2);
	for (i = 0; i < PHP_SERVER_CALLBACK_NUM; i++) {
		// Update the properties of the server class if the callback function event name of the first parameter matches
		if (strncasecmp(callback_name[i], Z_STRVAL(*name), Z_STRLEN(*name)) == 0) {
			memcpy(property_name + 2, callback_name[i], Z_STRLEN(*name));
			property_name_len = Z_STRLEN(*name) + 2;
			property_name[property_name_len] = 0;
			// Update the callback function to the corresponding property
			zend_update_property(tinyswoole_server_ce_ptr, server_object, property_name, property_name_len, callable);
			// Store callback function
			php_tsw_server_callbacks[i] = tsw_zend_read_property(tinyswoole_server_ce_ptr, server_object, property_name, property_name_len, 0);
			// Store callback function zend_fcall_info_cache structure
			php_tsw_server_caches[i] = func_cache;
		}
	}

	RETURN_TRUE;
}

PHP_METHOD(tinyswoole_server, start)
{
	zval *sock;
	tswServer *serv;

	sock = tsw_zend_read_property(tinyswoole_server_ce_ptr, getThis(), "sock", sizeof("sock") - 1, 0);
	serv = (tswServer *)malloc(sizeof(tswServer));
	if (serv == NULL) {
		tinyswoole_php_fatal_error(E_ERROR, "malloc tswServer error");
		return;
	}

	php_tswoole_register_callback(serv);
	serv->onStart();

	start(Z_LVAL(*sock));
}

void php_tswoole_register_callback(tswServer *serv)
{
	if (php_tsw_server_callbacks[TSW_SERVER_CB_onStart] != NULL) {
		serv->onStart = php_tswoole_onStart;
	}
}

void php_tswoole_onStart(void)
{
	zval  retval;

	call_user_function_ex(EG(function_table), NULL, php_tsw_server_callbacks[TSW_SERVER_CB_onStart], &retval, 0, NULL, 0, NULL);
}
