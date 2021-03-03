#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "php_cronkit.h"

zend_class_entry *cronkit_joblet_ce;

ZEND_BEGIN_ARG_INFO_EX(cronkit_joblet_voidarg_info, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(cronkit_joblet, execute)
{
    
}

static zend_function_entry cronkit_joblet_methods[] = {
    PHP_ABSTRACT_ME(cronkit_joblet, execute, cronkit_joblet_voidarg_info)
        PHP_FE_END};

PHP_MINIT_FUNCTION(cronkit_joblet)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Cronkit\\Joblet", cronkit_joblet_methods);
    cronkit_joblet_ce = zend_register_internal_class(&ce TSRMLS_CC);
    return SUCCESS;
}