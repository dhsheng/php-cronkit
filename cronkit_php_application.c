#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/pcre/php_pcre.h"
#include "ext/spl/spl_exceptions.h"

#include "cronkit_joblet.h"
#include "cronkit_php_application.h"
#include "cronkit_main.h"


zend_class_entry *cronkit_application_ce;

ZEND_BEGIN_ARG_INFO_EX(cronkit_application_void_arg, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cronkit_application_start_arg, 0, 0, 1)
ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cronkit_application_addtasklet_arg, 0, 0, 1)
ZEND_ARG_TYPE_INFO(0, taskletClass, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(cronkit_application_register_arg, 0, 0, 1)
ZEND_ARG_TYPE_INFO(0, joblet, IS_STRING, 0)
ZEND_END_ARG_INFO()

static struct cronkit_joblet_t *parse_cronkit_joblet_internal(zval *s) {

    zend_string *class_name = Z_STR_P(s);
    zend_class_entry *class_entry = zend_lookup_class(class_name);
    if (class_entry == NULL) {
        return NULL;
    }

    struct cronkit_joblet_t *internal_joblet;
    internal_joblet = cronkit_joblet_alloc();

    zval *joblet_name;
    zval *joblet_cron;
    zend_string *doc = class_entry->info.user.doc_comment;

    annotation_parse(ANNOTATION_PATTERN_NAME, doc, ANNOTATION_NAME, joblet_name);
    if (joblet_name != NULL) {
        char *sn = ZSTR_VAL(Z_STR_P(joblet_name));
        strcpy(internal_joblet->name, sn);
    } else {
        char *sn = ZSTR_VAL(class_name);
        strcpy(internal_joblet->name, sn);
    }

    annotation_parse(ANNOTATION_PATTERN_CRON, doc, ANNOTATION_CRON, joblet_cron);
    if (joblet_cron) {
        char *cron = ZSTR_VAL(Z_STR_P(joblet_cron));
        int i=0;
        while(*cron) {
            if(*cron != '\\') {
                internal_joblet->expr[i++] = *cron;
            }
            cron++;
        }
        internal_joblet->expr[i] = '\0';
    }   
    strcpy(internal_joblet->handler_class, ZSTR_VAL(class_name));
    return internal_joblet;
}


PHP_METHOD(cronkit_application, register) {
    
    zval *joblet;
    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ZVAL(joblet)
    ZEND_PARSE_PARAMETERS_END();

    zend_class_entry *joblet_ce;
    joblet_ce = zend_lookup_class(Z_STR_P(joblet));

    /**
     * @todo
     * check class exists 
     * check cron expr
     * etc ...
     */
    struct cronkit_joblet_t *internal_joblet;
    internal_joblet = parse_cronkit_joblet_internal(joblet);
    if (internal_joblet == NULL) {
        return;
    }
    cronkit_joblet_linked_list_add(
        &cronkit_joblet_list,
        internal_joblet
    );
}


PHP_METHOD(cronkit_application, __construct)
{
    // check errors
    cronkit_joblet_list_init();
}

PHP_METHOD(cronkit_application, start)
{   
    if (cronkit_joblet_list.size <= 0) {
        return;
    }

    // init heap    
    cronkit_joblet_heap_init(cronkit_joblet_list.size);

    struct cronkit_joblet_t *joblet;
    joblet = cronkit_joblet_list.head;
    while(joblet) {
        cronkit_joblet_entry_t *entry = cronkit_joblet_entry_build(joblet);
        cronkit_heap_put(&cronkit_joblet_heap, entry);
        joblet = joblet->next;
    }
    cronkit_real_main(0, NULL);
}


static zend_function_entry cronkit_application_methods[] = {
    PHP_ME(cronkit_application, __construct, cronkit_application_void_arg, ZEND_ACC_PUBLIC)
    PHP_ME(cronkit_application, start, cronkit_application_start_arg, ZEND_ACC_PUBLIC)
    PHP_ME(cronkit_application, register, cronkit_application_register_arg, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(cronkit_application)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Cronkit\\Application", cronkit_application_methods);
    cronkit_application_ce = zend_register_internal_class(&ce TSRMLS_CC);
    return SUCCESS;
}