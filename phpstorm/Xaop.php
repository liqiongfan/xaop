<?php
/**
 * Powered by Xaop.
 * Xaop is an High performance PHP extension for WEB develop based on server such as Nginx/Apache
 * User can use, modify, copy Xan only when keep the explain words
 * https://www.supjos.cn All Rights Reserved.
 * License: BSD
 * User: Josin
 * Date: 2018/9/12
 */

class Xaop
{
    /**
     * To add the Before advise
     * @param string      $className        The class waiting for matching, support one char `*` to match any thing.
     * @param string      $functionName     The function waiting for matching.  suppor `*` to match any char.
     * @param array $callback               The callback such as [ Swing::class, '_before' ]
     */
    public final function addBeforeAop($className, $functionName, $callback = [ ]) {

    }

    /**
     * To add the After advise
     * @param string      $className        The class waiting for matching, support one char `*` to match any thing.
     * @param string      $functionName     The function waiting for matching.  suppor `*` to match any char.
     * @param array $callback               The callback such as [ Swing::class, '_after' ]
     */
    public final function addAfterAop($className, $functionName, $callback = [ ] ) {

    }
}