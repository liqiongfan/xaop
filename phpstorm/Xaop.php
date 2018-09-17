<?php
/**
 * Powered by Xaop.
 * Xaop is an High performance PHP extension for WEB develop based on server such as Nginx/Apache
 * User can use, modify, copy Xan only when keep the explain words
 * https://www.supjos.cn All Rights Reserved.
 * License: BSD
 * User: Josin
 * Date: 2018/9/16
 */

class Xaop
{
    /**
     * Add the before aops for the given class name
     * @param string|NULL $className
     * @param string $functionName
     * @param array|\Closure $aopCallback
     * @return array
     */
    public static function addBeforeAop($className, $functionName, $aopCallback) {

    }

    /**
     * Add the after aops for the given class name
     * @param string|NULL $className
     * @param string $functionName
     * @param array|\Closure $aopCallback
     * @return array
     */
    public static function addAfterAop($className, $functionName, $aopCallback) {

    }

    /**
     * Add the after_return aops for the given class name
     * @param string|NULL $className
     * @param string $functionName
     * @param array|\Closure $aopCallback
     * @return array
     */
    public static function addAfterReturnAop($className, $functionName, $aopCallback) {

    }

    /**
     * Add the after_throw aops for the given class name
     * @param string|NULL $className
     * @param string $functionName
     * @param array|\Closure $aopCallback
     * @return array
     */
    public static function addAfterThrowAop($className, $functionName, $aopCallback) {

    }

    /**
     * Add the around aops for the given class name
     * @param string|NULL $className
     * @param string $functionName
     * @param array|\Closure $aopCallback, Each callback will automatically has a XaopExecResource param
     * @return array
     */
    public static function addAroundAop($className, $functionName, $aopCallback) {

    }

    /**
     * To execute the function which you apply the around aop
     * @param mixed $xaopExecResource       The XaopExec resource data to call the context
     */
    public static function exec($xaopExecResource) {

    }


}