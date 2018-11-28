#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_tinyswoole.h"


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

PHP_METHOD(tinyswoole_server, start)
{
	zval *sock;

	sock = tsw_zend_read_property(tinyswoole_server_ce_ptr, getThis(), "sock", sizeof("sock") - 1, 0);

	printf("running server...\n");
	start(Z_LVAL(*sock));
}

