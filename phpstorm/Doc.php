<?php

/**
 * Powered by Xaop.
 * Xaop is an High performance PHP extension for WEB develop based on server such as Nginx/Apache
 * User can use, modify, copy Xan only when maintain the declare words
 * User: Josin
 * Date: 2018/9/10
 * License: PHP License 3.x
 * Contact: xeapplee@gmail.com or 774542602@qq.com
 */

namespace Xaop;

/**
 * Class Doc
 *
 * Xaop\Doc will parse the doc-comment for the PHP annotation
 * it will parse the following type of the doc-comment:
 *
 * <pre>
 * @api
 * @disable( level = ERROR )
 * @output( v = "Hello,Xaop", t = "Xaop\Doc" )
 * </pre>
 *
 * @package Xaop
 */
class Doc
{
    /**
     * Obtain the class document parsing results
     * @param string|object $className     The class name or class object
     * @return array                       The result contains the doc-parsing results
     */
    public function getClassDoc($className) {

    }

    /**
     * Obtain the function document parsing rsult
     * @param string|object $className             The class to fetch the function doc.
     * @param string $functionName                  The function name
     * @return array
     */
    public function getFunctionDoc($className, $functionName) {

    }

    /**
     * Obtain the class' const document parsing results
     * @param string|object $className             The class name to get the const doc from
     * @param string $constName                     The const name contains the doc.
     * @return array
     */
    public function getClassConstDoc($className, $constName) {

    }

    /**
     * Obtain the property doc parsing result
     * @param string|object $className              The property doc fetch from which class
     * @param string $propertyName                  The property name contains the doc.
     * @return array
     */
    public function getAttributeDoc($className, $propertyName) {

    }
}