#ifndef CRONKIT_PHP_APPLICATION_H
#define CRONKIT_PHP_APPLICATION_H

extern zend_class_entry *cronkit_application_ce;

#define ANNOTATION_REGEXP_CRON    "/@cron\\s+(?P<cron>[^\r\n]+)/"
#define ANNOTATION_REGEXP_NAME    "/@name\\s+(?P<name>[^\\s]+)/"
#define ANNOTATION_REGEXP_TIMEOUT "/@timeout\\s+(?P<timeout>[\\d]+)/"

#define ANNOTATION_PATTERN_CRON zend_string_init(ANNOTATION_REGEXP_CRON, strlen(ANNOTATION_REGEXP_CRON), 0)

#define ANNOTATION_PATTERN_NAME zend_string_init( \ 
    ANNOTATION_REGEXP_NAME, \
    strlen(ANNOTATION_REGEXP_NAME), 0)

#define ANNOTATION_PATTERN_TIMEOUT  zend_string_init( \
   ANNOTATION_REGEXP_TIMEOUT,\
    strlen(ANNOTATION_REGEXP_TIMEOUT), 0)

#define ANNOTATION_CRON zend_string_init(ZEND_STRL("cron"), 0)
#define ANNOTATION_NAME zend_string_init(ZEND_STRL("name"), 0)
#define ANNOTATION_TIMEOUT zend_string_init(ZEND_STRL("timeout"), 0)


#define annotation_parse(pattern, zstr, name, ret) do  { \
    zend_string *__pattern = pattern;                                                 \
    zend_string *__an = name;                                                         \
    zval __m, __g;                                                                    \
    pcre_cache_entry *__p = pcre_get_compiled_regex_cache(pattern);                   \
    php_pcre_match_impl(__p, ZSTR_VAL(zstr), ZSTR_LEN(zstr), &__m, &__g, 0, 0, 0, 0); \
    ret = zend_hash_find(Z_ARRVAL_P(&__g), __an);                                     \
    zend_string_release(__an);                                                        \
    zend_string_release(__pattern);                                                   \
} while (0)

#endif