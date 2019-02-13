<?php
/**
 * Powered by Xaop.
 * Xaop is an High performance PHP extension for WEB develop based on server such as Nginx/Apache
 * User can use, modify, copy Xan only when keep the explain words
 * https://www.supjos.cn All Rights Reserved.
 * License: BSD
 * User: Josin
 * Date: 2018/9/17
 */

/**
 * Class Swing
 * What you are doing were what you want.
 *
 * @Aspect
 */
class Swing
{
    function _magicGetBefore() {
        echo '_magicGetBefore()' . PHP_EOL;
    }

    function _magicGetAfter() {
        echo '_magicGetAfter()' . PHP_EOL;
    }

    function _magicSuccess() {
        echo '_magicSuccess()' . PHP_EOL;
    }

    function _magicFailure() {
        echo '_magicFailure()' . PHP_EOL;
    }

    /**
     * This is the __get magic function to do some magic
     * job.
     * @before( value="Swing._magicGetBefore" )
     * @after( value="Swing._magicGetAfter" )
     * @success( value="Swing._magicSuccess" )
     */
    public function __get($name)
    {
        echo '__get' . PHP_EOL;
        return true;
    }


    /**
     * @before( value="Swing._magicGetBefore" )
     * @after( value="Swing._magicGetAfter" )
     * @failure( value="Swing._magicFailure" )
     */
    public function __set($name, $value)
    {
        echo '__set' . PHP_EOL;
        return false;
    }

}

// echo '<pre>';

$swing = new Swing();

echo 'SET start' . PHP_EOL;

$swing->di = "di";

echo 'GET start' . PHP_EOL;

$swing->di;

// echo '</pre>';



