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

    /**
     * To add an property before read aop
     * @param string $className             The class name  can be `*`
     * @param string $propertyName          The property name   can't be `*`
     * @param \Closure|array $aopCallback   The callback when aop meet the condition
     * @return array
     */
    public static function addPropertyBeforeReadAop($className, $propertyName, $aopCallback) {

    }

    /**
     * To add an property before set aop
     * @param string $className             The class name  can be `*`
     * @param string $propertyName          The property name   can't be `*`
     * @param \Closure|array $aopCallback   The callback when aop meet the condition
     * @return array
     */
    public static function addPropertyBeforeSetAop($className, $propertyName, $aopCallback) {

    }

    /**
     * To add an property after_read aop
     * @param string $className             The class name  can be `*`
     * @param string $propertyName          The property name   can't be `*`
     * @param \Closure|array $aopCallback   The callback when aop meet the condition
     * @return array
     */
    public static function addPropertyAfterReadAop($className, $propertyName, $aopCallback) {

    }

    /**
     * To add an property after_set aop
     * @param string $className             The class name  can be `*`
     * @param string $propertyName          The property name   can't be `*`
     * @param \Closure|array $aopCallback   The callback when aop meet the condition
     * @return array
     */
    public static function addPropertyAfterSetAop($className, $propertyName, $aopCallback) {

    }
    
    /**
     * To execute the around context
     * @param Resource|mixed $xaopPropertyExec
     */
    public static function execProperty($xaopPropertyExec) {

    }

}