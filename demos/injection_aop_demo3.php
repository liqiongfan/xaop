<?php

// This is the file demonstrate the InjectionAOP mode which need set the php.ini with the config to 3:
//  `` xaop.aop_mode = 3 ``

class Swing
{
    public function __get($name) {
        echo 'Swing::__get()' . PHP_EOL;
        return []; // to test the after_return aop feature
    }

    public function __getBefore() {
        echo 'Swing::__getBefore()' . PHP_EOL;
    }

    public function __getAfter() {
        echo 'Swing::__getAfter()' . PHP_EOL;
    }
}

// This aop will caught all return value except the null
// So if you use return null, it will be omitted by the kernel, for the kernel default return null
Xaop::addAfterReturnAop(Swing::class, "__get", function(){
    echo '_after_return_caught' . PHP_EOL;
});


echo '<pre>';

$swing = new Swing();
$swing->di;

echo '</pre>';

/** output:
    Swing::__get()
    _after_return_caught
 */

