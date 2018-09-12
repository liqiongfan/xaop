/*
 +----------------------------------------------------------------------+
 | Extreme AOP extension for PHP 7                                      |
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
 | Author: Josin https://www.supjos.cn                                  |
 +----------------------------------------------------------------------+
 */

#ifndef XAOP_EXEC_H
#define XAOP_EXEC_H

#define IN_XAOP_OPLINE
#define IN_XAOP_OPLINE_END
#define GET_OBJECT_PHPDOC(object) Z_OBJCE_P(object)->info.user.doc_comment
#define GET_FUNCTI_PHPDOC(func) (func)->op_array.doc_comment
#define IN_ASPECT_MODE(aspect) if ( aspect )
#define IN_CLASS_MODE(func) if ( getThis() && GET_OBJECT_PHPDOC(getThis()) && (func) && GET_FUNCTI_PHPDOC(func) ) {
#define IN_CLASS_MODE_END }
#define IN_FUNCTION_MODE(func) if ( (func) && (func)->op_array.function_name ) {
#define IN_INTERNAL_FUNCTION_MODE(func) if ( (func) && (func)->internal_function.function_name ) {
#define IN_FUNCTION_MODE_END }
#define LOST_OBJECT if ( !this ) {
#define LOST_ELSE } else {
#define LOST_OBJECT_END }
#define NOT_AROUND_MODE(var) if ( !var ) {
#define NOT_AROUND_MODE_END }
#define IN_ARRAY_CONDITION_P(arg) if ( (arg) && Z_TYPE_P(arg) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(arg)) ) {
#define IN_ARRAY_CONDITION(arg) if ( Z_TYPE(arg) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL(arg)) ) {
#define IN_ARRAY_END }
#define RETURN_CONTEXT(cond) if ( cond ) return ;
#define BACK_TRUE(val) if ( (val) && ( Z_TYPE_P(val) == IS_TRUE || Z_TYPE_P(val) == IS_ARRAY ) ) {
#define OUTPUT_JSON(type) if ( (type) == 1 ) {
#define OUTPUT_XML(type) } else if ( (type) == 2 ) {
#define OUTPUT_END }
#define XAOP_OUTPUT(info...) do {\
    zend_string *output = strpprintf(0, ##info);\
    php_write(ZSTR_VAL(output), ZSTR_LEN(output));\
} while (0);
#define XAOP_MATCH_FUNC_AOPS(aop_name) \
    do {\
        zend_string *func_name;\
        zval *callback;\
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL(XAOP_G(aop_name)), func_name, callback) {\
            if ( SUCCESS == xaop_match_string(ZSTR_VAL(func_name), ZSTR_VAL(function_name)) ) {\
                invoke_zval_arg(callback);\
            }\
        } ZEND_HASH_FOREACH_END();\
    } while (0);

#define XAOP_MATCH_CLASS_AOPS(aop_name) \
    do {    \
        zval *callback; \
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL(XAOP_G(aop_name)), callback) {\
            zval *class_name = zend_hash_str_find(Z_ARRVAL_P(callback), ZEND_STRL("class"));\
            zval *func_name  = zend_hash_str_find(Z_ARRVAL_P(callback), ZEND_STRL("func"));\
            zval *closure  = zend_hash_str_find(Z_ARRVAL_P(callback), ZEND_STRL("closure"));\
            if ( SUCCESS == xaop_match_string(Z_STRVAL_P(class_name), ZSTR_VAL(Z_OBJCE_P(this)->name)) &&\
                    SUCCESS == xaop_match_string(Z_STRVAL_P(func_name), ZSTR_VAL(function_name)) ) {\
                    invoke_zval_arg(closure);\
            }\
        } ZEND_HASH_FOREACH_END();\
    } while (0);

#define XAOP_MATCH_FUNC_AROUND_AOPS(type) \
    do {    \
        zend_string *aop_func_name;\
        zval *aop_zval_arg;\
        ZEND_HASH_FOREACH_STR_KEY_VAL( Z_ARRVAL(XAOP_G(around_func_aops)), aop_func_name, aop_zval_arg ) {\
            if ( SUCCESS == xaop_match_string( ZSTR_VAL(aop_func_name), ZSTR_VAL(function_name) ) ) {\
                XAOP_G(around_data) = execute_data;\
                XAOP_G(around_type) = type;\
                invoke_zval_arg(aop_zval_arg);\
                around = 1;\
                break;\
            }\
        } ZEND_HASH_FOREACH_END();\
    } while (0);
#define XAOP_MATCH_CLASS_AROUND_AOPS(type) \
    do {    \
        zval *aop_zval_arg; \
        ZEND_HASH_FOREACH_VAL( Z_ARRVAL(XAOP_G(around_aops)), aop_zval_arg ) {\
            zval *class_name = zend_hash_str_find(Z_ARRVAL_P(aop_zval_arg), ZEND_STRL("class"));\
            zval *func_name  = zend_hash_str_find(Z_ARRVAL_P(aop_zval_arg), ZEND_STRL("func"));\
            zval *closure  = zend_hash_str_find(Z_ARRVAL_P(aop_zval_arg), ZEND_STRL("closure"));\
            if ( SUCCESS == xaop_match_string(Z_STRVAL_P(class_name), ZSTR_VAL(Z_OBJCE_P(this)->name)) &&\
                SUCCESS == xaop_match_string(Z_STRVAL_P(func_name), ZSTR_VAL(function_name)) ) {\
                XAOP_G(around_data) = execute_data;\
                XAOP_G(around_type) = type;\
                invoke_zval_arg(closure);\
                around = 1;\
                break ;\
            }\
        } ZEND_HASH_FOREACH_END();\
    } while (0);

/* Execute the OPCodes & annalyse the annotations */
void xaop_annotation_execute(zend_execute_data *execute_data);

/* Execute the OPCodes for Injection aop */
void xaop_execute(zend_execute_data *execute_data);
void xaop_execute_internal(zend_execute_data *execute_data, zval *return_value);

/* Compare the two string with the preg_str `*` to match 0 or more any char */
int xaop_match_string(char *str1, char *str2);

/* Call the zval method & class info. */
void invoke_zval_arg(zval *arg);

/* Invoke the aop method */
void invoke_kernel_aop_method(zval *aop_zval);

/* Invoke method with php array parameters */
void xaop_call_method_with_php_params(zval *object, char *method_name, zval *params, zval *retval);

/* Invoke method with the c array parameters */
void xaop_call_method_with_c_params(zval *object, char *method_name, int param_count, zval *params, zval *retval);

/* Setting the HTTP header */
void xaop_api_handler(zval *api_zval, int *tp, zval *charset);

/* Generate the XML data */
void xaop_xml_data(zval *data, smart_str *result);

#endif




/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
