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

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xaop.h"
#include "zend_smart_str.h"
#include "ext/standard/php_string.h"
#include "kernel/common/anno_parsing.h"
#include "kernel/common/helper_lib.h"

/**
 * {{{ NOTE: the second parameter must be allocated memory to store the result
 * otherwise it will cause Segement fault.
 * This function parse the doc into serval line and parse it to the function `parse_line_phpdoc`
 * Parse the PHPDoc and store the result into the `result` parameter
 */
void parse_phpdoc(zend_string *phpdoc, zval *result)
{
    if ( Z_TYPE_P(result) != IS_ARRAY ) {
        array_init(result);
    }

    smart_str line_doc = { 0 };
    const char *doc = !phpdoc ? NULL : ZSTR_VAL(phpdoc), *c_r, *c_e;
    zval annotations, ret;
    array_init(&annotations);

    int pos, l_s = 0, pos_e, body_s = 0, body_e = 0, phpdoc_len = !phpdoc ? 0 : ZSTR_LEN(phpdoc);

    for ( pos = 0; pos < phpdoc_len; pos++ ) {

        c_r = doc + pos;

        if ( *c_r == '/' || *c_r == '*' || *c_r == ' ' ) {
            continue;
        } else {
            if ( !body_s ) body_s = pos;
        }

        if ( *c_r == '\n' ) {

            /* If current char is '\n' to find the next '\n' */
            for ( pos_e = pos + 1; pos_e < phpdoc_len; pos_e++, pos++ ) {
                
                c_e = doc + pos_e;

                if ( *c_e == '*' || *c_e == ' ' ) {
                    continue;
                } else {
                    if ( !l_s ) l_s = pos_e;
                }

                if ( *c_e == '\n' ) {
                    if ( pos_e == l_s ) {
                        l_s = 0;
                        continue;
                    }
                    smart_str_appendl(&line_doc, doc + l_s, pos_e - l_s);
                    smart_str_0(&line_doc);
                    parse_line_phpdoc(line_doc.s, &annotations, &body_e, l_s);
                    smart_str_free(&line_doc);
                    l_s = 0;
                }
            }
        }
    }
    /* After parsing the phpdoc. */
    smart_str_free(&line_doc);
    add_assoc_zval(result, "annotations", &annotations);
    
    smart_str body = { 0 };
    smart_str_appendl(&body, ZSTR_VAL(phpdoc) + body_s, body_e >= body_s ? body_e - body_s : 0 );
    smart_str_0(&body);
    xaop_method_with_3_char_params("preg_replace", ZSTR_VAL(body.s), "\n", "#\\n[ ]*\\**[ ]*#", &ret);
    smart_str_free(&body);
    add_assoc_stringl(result, "body", Z_STRVAL(ret) + 1, Z_STRLEN(ret) > 2 ? Z_STRLEN(ret) - 2 : Z_STRLEN(ret));
}/*}}}*/

/**
 * {{{
 * Parse one line phpdoc into serval parts and store it into the `result`
 * Note that: the second parameter must be write data into
 * such as : line_doc = '@api(type="a,b,c", value="aa,bb.cc")';
 */
void parse_line_phpdoc(zend_string *line_doc, zval *result, int *body_end_pos, int start_pos)
{
    if ( !line_doc || !ZSTR_LEN(line_doc) ) {
        return ;
    }

    const char *doc = ZSTR_VAL(line_doc), *c_r;

    if ( '@' == *doc ) {
        if ( 0 == *body_end_pos ){
            *body_end_pos = start_pos;
        }
    } else {
        return ;
    }

    zval line_result;

    smart_str name = { 0 }, data = { 0 };

    int pos = 0, doc_len = ZSTR_LEN(line_doc),
        sp = 0, p_left = 0, p_right = 0, 
        in_quote = 0, is_space = 0;

    for ( ; pos < doc_len; pos++ ) {

        c_r = doc + pos;

        if ( is_space && !in_quote && !p_left ) {
            if ( *c_r != ' ' ) {
                return ;
            }
        }

        if ( *c_r == '(' && !in_quote && !p_left ) {
            if ( is_space ){
                return ;
            }
            p_left = pos;
            smart_str_appendl(&name, doc + 1, pos - 1);
            smart_str_0(&name);
        }

        if ( *c_r == ' ' ) {
            is_space = 1;
        }

        if ( *c_r == '"' ) {
            in_quote = ~in_quote;
        }

        if ( *c_r == ')' && !in_quote && !p_right ) {
            p_right = pos;
        }
    }

    if ( ( p_left && !p_right ) || ( !p_left && p_right ) ) {
        smart_str_free(&name);
        smart_str_free(&data);
        return ;
    }

    if ( !p_left && !p_right ) {
        smart_str_appendl(&name, doc + 1, doc_len);
        smart_str_appendc(&data, ' ');
        smart_str_0(&data);
    }

    if ( p_left && p_right ) {
        smart_str_appendl(&data, doc + p_left + 1, p_right - p_left - 1);
        smart_str_0(&data);
    }

    array_init(&line_result);
    parse_key_value_pairs(data.s, &line_result);
    add_assoc_zval(result, ZSTR_VAL(name.s), &line_result);

    smart_str_free(&name);
    smart_str_free(&data);
}/*}}}*/

/**
 * {{{
 * Parse the string into key => value pair, such as the below one:
 * name = "hello, world", value = "list, do"
 */
void parse_key_value_pairs(zend_string *str, zval *result)
{
    if ( !str || !ZSTR_LEN(str) ) return ;

    const char *val = ZSTR_VAL(str), *c_r;

    smart_str key_value = { 0 };
    int pos, pos_len = ZSTR_LEN(str), in_quote = 0, kv_s = 0;

    for ( pos = 0; pos < pos_len; pos++ ) {

        c_r = val + pos;

        if ( *c_r == '"' ) {
            in_quote = ~in_quote;
        }

        if ( !in_quote && ',' == *c_r ) {
            smart_str_appendl(&key_value, val + kv_s, pos - kv_s );
            smart_str_0(&key_value);
            parse_key_value(key_value.s, result);
            smart_str_free(&key_value);
            kv_s = pos;
        }

        if ( pos == pos_len - 1 ) {
            smart_str_appendl(&key_value, val + kv_s + ( kv_s ? 1 : 0), pos_len - kv_s - ( kv_s ? 1 : 0));
            smart_str_0(&key_value);
            parse_key_value(key_value.s, result);
            smart_str_free(&key_value);
        } 
    }
}/*}}}*/

/**
 * {{{
 * Parse the key & the value
 * such as : name = "hello world"
 */
void parse_key_value(zend_string *str, zval *result)
{
    if ( !str || !ZSTR_LEN(str) ) return ;

    const char *val = ZSTR_VAL(str), *c_r;

    int pos, pos_len = ZSTR_LEN(str);

    zend_string *k = NULL, *v = NULL;
    smart_str key = { 0 }, value = { 0 };
    zval params;
    array_init(&params);
    
    for ( pos = 0; pos <  pos_len; pos++ ) {
        
        c_r = val + pos;

        if ( '=' == *c_r ) {
            smart_str_appendl(&key, val, pos);
            smart_str_0(&key);
            smart_str_appendl(&value, val + pos + 1, pos_len - pos - 1);
            smart_str_0(&value);

            k = php_trim( key.s, " \"", sizeof(" \""), 3);
            v = php_trim( value.s, " \"", sizeof(" \""), 3);

            add_assoc_str(result, ZSTR_VAL(k), v);
        }
    }

    smart_str_free(&key);
    smart_str_free(&value);
}/*}}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
