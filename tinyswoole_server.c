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
	long sock_type = TSW_SOCK_TCP;
	int sock;
	tswServer *serv;

	serv = tswServer_new();
	if (serv == NULL) {
		tinyswoole_php_fatal_error(E_ERROR, "tswServer_new error");
		return;
	}
	TSwooleG.serv = serv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|l", &serv_host, &host_len, &serv_port, &sock_type) == FAILURE) {
		RETURN_NULL();
	}

	sock = tswSocket_create(sock_type);
	if (sock < 0) {
		tinyswoole_php_fatal_error(E_ERROR, "tswSocket_create error");
		RETURN_NULL();
	}
	if (tswSocket_bind(sock, sock_type, serv_host, serv_port) < 0) {
		tinyswoole_php_fatal_error(E_ERROR, "tswSocket_bind error");
		RETURN_NULL();
	}
	serv->serv_sock = sock;

	server_object = getThis(); // server_object is a global variable
	zend_update_property_string(tinyswoole_server_ce_ptr, server_object, ZEND_STRL("ip"), serv_host);
	zend_update_property_long(tinyswoole_server_ce_ptr, server_object, ZEND_STRL("port"), serv_port);
}

PHP_METHOD(tinyswoole_server, set)
{
	zval *zset = NULL;
	tswServer *serv;
	HashTable *vht;
	zval *v;

	serv = TSwooleG.serv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(zset)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	// php_tinyswoole_array_separate(zset);
    vht = Z_ARRVAL_P(zset);

	if (php_tinyswoole_array_get_value(vht, "reactor_num", v)) {
        convert_to_long(v);
        serv->reactor_num = (uint16_t)Z_LVAL_P(v);
    } else {
		serv->reactor_num = 4;
	}

	if (php_tinyswoole_array_get_value(vht, "worker_num", v)) {
        convert_to_long(v);
        serv->worker_num = (uint16_t)Z_LVAL_P(v);
    } else {
		serv->worker_num = 4;
	}

	if (serv->worker_num < serv->reactor_num) {
		serv->worker_num = 4;
		serv->reactor_num = 4;
	}
}

PHP_METHOD(tinyswoole_server, on)
{
	int i;
	char property_name[128]; // on + event name eg onConnect onReceive
	int property_name_len = 0;
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

	serv = TSwooleG.serv;
	php_tswoole_register_callback(serv);

	tswServer_start(serv);
}

PHP_METHOD(tinyswoole_server, send)
{
	zval *zfd;
	zval *zdata;
	int ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zfd, &zdata) == FAILURE) {
		return;
    }
	ret = tswServer_tcp_send(NULL, Z_LVAL(*zfd), Z_STRVAL(*zdata), Z_STRLEN(*zdata));

	RETURN_LONG(ret);
}

void php_tswoole_register_callback(tswServer *serv)
{
	if (php_tsw_server_callbacks[TSW_SERVER_CB_onStart] != NULL) {
		serv->onStart = php_tswoole_onStart;
	}
	if (php_tsw_server_callbacks[TSW_SERVER_CB_onConnect] != NULL) {
		serv->onConnect = php_tswoole_onConnect;
	}
	if (php_tsw_server_callbacks[TSW_SERVER_CB_onReceive] != NULL) {
		serv->onReceive = php_tswoole_onReceive;
	}

	serv->onMasterStart = tswServer_master_onStart;
	serv->onReactorStart = tswServer_reactor_onStart;
}

void php_tswoole_onStart(tswServer *serv)
{
	zval  retval;

	tswDebug("reactor thread num: %d, worker process num: %d", serv->reactor_num, serv->worker_num);
	call_user_function_ex(EG(function_table), NULL, php_tsw_server_callbacks[TSW_SERVER_CB_onStart], &retval, 0, NULL, 0, NULL);
}

/**
 * @fd: connection socket
 */
void php_tswoole_onConnect(int fd)
{
	zval *zfd;
	zval  retval;
	zval args[1];

	TSW_MAKE_STD_ZVAL(zfd); // Let zfd point to a piece of memory in the stack
	ZVAL_LONG(zfd, fd); // Before using ZVAL_LONG, you need to allocate memory first.
	args[0] = *zfd;

	call_user_function_ex(EG(function_table), NULL, php_tsw_server_callbacks[TSW_SERVER_CB_onConnect], &retval, 1, args, 0, NULL);
}

/**
 * @fd: session_id
 * 
 * Event_data saves the data sent from the client
 */
void php_tswoole_onReceive(tswServer *serv, tswEventData *event_data)
{
	zval *zfd;
	zval *zdata;
	zval retval;
	zval args[3];

	TSW_MAKE_STD_ZVAL(zfd);
	ZVAL_LONG(zfd, event_data->info.fd);
	TSW_MAKE_STD_ZVAL(zdata);
	ZVAL_STRINGL(zdata, event_data->data, event_data->info.len);

	args[0] = *server_object;
	args[1] = *zfd;
	args[2] = *zdata;

	call_user_function_ex(EG(function_table), NULL, php_tsw_server_callbacks[TSW_SERVER_CB_onReceive], &retval, 3, args, 0, NULL);
}
