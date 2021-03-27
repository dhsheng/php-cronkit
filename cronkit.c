#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cronkit.h"

#include "cronkit_php_application.h"

/* If you declare any globals in php_cronkit.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(cronkit)
*/

/* True global resources - no need for thread safety here */
static int le_cronkit;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("cronkit.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_cronkit_globals, cronkit_globals)
    STD_PHP_INI_ENTRY("cronkit.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_cronkit_globals, cronkit_globals)
PHP_INI_END()
*/
/* }}} */

/* }}} */


/* {{{ php_cronkit_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_cronkit_init_globals(zend_cronkit_globals *cronkit_globals)
{
	cronkit_globals->global_value = 0;
	cronkit_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(cronkit)
{
    ZEND_MODULE_STARTUP_N(cronkit_application)(INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(cronkit)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(cronkit)
{
#if defined(COMPILE_DL_CRONKIT) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(cronkit)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cronkit)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "cronkit support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ cronkit_functions[]
 *
 * Every user visible function must have an entry in cronkit_functions[].
 */
const zend_function_entry cronkit_functions[] = {
	PHP_FE_END	/* Must be the last line in cronkit_functions[] */
};
/* }}} */

/* {{{ cronkit_module_entry
 */
zend_module_entry cronkit_module_entry = {
	STANDARD_MODULE_HEADER,
	"cronkit",
	cronkit_functions,
	PHP_MINIT(cronkit),
	PHP_MSHUTDOWN(cronkit),
	PHP_RINIT(cronkit),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(cronkit),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(cronkit),
	PHP_CRONKIT_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CRONKIT
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(cronkit)
#endif