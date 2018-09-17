<?php

// This is the file demonstrate the InjectionAOP mode which need set the php.ini with the config to 3:
//  `` xaop.aop_mode = 3 ``

class Swing
{
    public function __get($name) {
        echo 'Swing::__get()' . PHP_EOL;
        // To test the after_throw aop feature
        throw new Exception("Xaop extension.", 200);
    }

    public function __getBefore() {
        echo 'Swing::__getBefore()' . PHP_EOL;
    }

    public function __getAfter() {
        echo 'Swing::__getAfter()' . PHP_EOL;
    }
}

// This aop feature will run the following method when the main function throw exception
// And the exception was not caught by the php code.
// And after running the function ,it will throw the exception upper to the catcher.
Xaop::addAfterThrowAop(Swing::class, "__get", function(){
    echo 'caught_exception'. PHP_EOL;
});


echo '<pre>';

try {
    $swing = new Swing();
    $swing->di;
} catch (Exception $e) {
    echo $e->getMessage();
}

echo '</pre>';

/** output:
    Swing::__get()
    caught_exception
    Xaop extension.
 */

