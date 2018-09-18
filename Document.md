### Xaop Licensed by PHP License 3.x ###

Hello everybody who uses/download and/or want to patch the PHP extension `Xaop`. 
Here were some help document for using or develop it.

#### Simple Introduction ####

In Zend Engine 3.x server on PHP 7.x, the hook function named `zend_execute_ex` or `zend_execute_internal_ex`
the two of them were the opcodes running method for all method|global function invoke.
for example, when running the following php code under the php7.x:
```php
function xaop_do() {
	echo 'This is the xaop_do function';
}
```

for the global function, when you invoke it with the method calling:

```
xaop_do();
```

The Zend Engine will put the request to the `zend_execute_ex` function to deal, so if we hook this function, then we
can add some other logic job for it. such as the Xaop do.

**Xaop** provider two way use the **AOP Programming** , such as **PHPDoc based AOP** & **Method Injection AOP**:

1. **PHPDoc based AOP** ( php.ini   **xaop.aop_mode = 2** )

   As the word means, This aop feature use the PHPDoc, following was a small example for use the phpdoc aop.

   ```php
   <?php
   
   /**
    * @Aspect
    */
   class Swing
   {
       /**
        * @api( type=JSON, charset=UTF-8 )
        */
       public function goods() {
           return [ 'name' => 'Xaop', 'version' => 'v0.99', 'website' => 'http://www.supjos.cn' ];
       }
   }
   ```

   Each PHPDoc based aop class must implement the **@Aspect** to the class, after add the @Aspect to the class, then you can add the AOP method annotation to function, such as **@api**, **Xaop** provider 7 internal  annotations for user to use:

   1) Internal annotations

   **@api**   Recommend for API use, it can return the JSON & XML data to the client directly

   **@disable**  Disable the function

   **@deprecated**  Let the engine to show a deprecated info.

   **@success** Running when the method returned **true** or **array**

   **@failure** Running when the method returned **false** or **null**

   **@before** Running before invoke the real target method

   **@after** Running after invoked the real target method

   **NOTE:**

   The order of the annotation was:

   @before --> @success or @failure --> @after

   2) User-defined annotations

   ```php
   namespace annotations;
   
   use Xaop\Annotations;
   
   class Tag implements Annotations
   {
       public function input($object, $annotations) {
           foreach ($annotations as $key => $val) {
               $object->$key = $val;
           }
       }
   }
   ```

   As the example shows before, every user-defined annotation must implement the interface **Xaop\Annotations** and override the **input** method, the **input** accept two paramters, they are:

   **Calling object** plus the **annotations**.

2. **Method Injection AOP** ( php.ini   **xaop.aop_mode = 3**  )

   Xaop provide the easy used & featured aop mode, provide five aop methods:

   **1). Xaop::addBeforeAop($className, $functionName, $callback );**

   **2). Xaop::addAfterAop($className, $functionName, $callback );**

   **3). Xaop::addAfterReturnAop($className, $functionName, $callback );**

   **4). Xaop::addAfterThrowAop($className, $functionName, $callback );**

   **5). Xaop::addAroundAop($className, $functionName, $callback );**

   params:

   1 **$className** was the class name you want to join point. can use **one** char `*` to replace 0 or more char. Such  as  `app*` will match all class start with app( also support namesapce) can use `*` to replace all classes

   2 **$functionName**  the function name you want to catch, support `*` to match any char, but at lease 2 char

   3 **$callback** The callback when the aop catch the satisfied running, the callback will be invoked by **Xaop**, only in **Around** mode, the **$callback** has one paramter named the **invoking method context**, then user can use the Xaop call: **Xaop::exec($methodContext)** to invoke the method context.

Here are some example:

Example1:

```php
<?php

// This is the file demonstrate the InjectionAOP mode which need set the php.ini with the config to 3:
//  `` xaop.aop_mode = 3 ``

class Swing
{
    public function __get($name) {
        echo 'Swing::__get()' . PHP_EOL;
    }

    public function __getBefore() {
        echo 'Swing::__getBefore()' . PHP_EOL;
    }

    public function __getAfter() {
        echo 'Swing::__getAfter()' . PHP_EOL;
    }
}

Xaop::addBeforeAop(Swing::class, "__get", function(){
    echo 'Closure before()' . PHP_EOL;
});
Xaop::addBeforeAop(Swing::class, "__get", [ Swing::class, "__getBefore" ] );
Xaop::addAfterAop(Swing::class, "__get", [ Swing::class, "__getAfter" ]);
Xaop::addAfterAop(Swing::class, "__get", function(){
    echo 'Closure after()' . PHP_EOL;
});

echo '<pre>';

$swing = new Swing();
$swing->di;

echo '</pre>';

/** output:
    Closure before()
    Swing::__getBefore()
    Swing::__get()
    Swing::__getAfter()
    Closure after()
 */
```

Example 2:

```php
<?php

// This is the file demonstrate the InjectionAOP mode which need set the php.ini with the config to 3:
//  `` xaop.aop_mode = 3 ``

class Swing
{
    public function __get($name) {
        echo 'Swing::__get()' . PHP_EOL;
    }

    public function __getBefore() {
        echo 'Swing::__getBefore()' . PHP_EOL;
    }

    public function __getAfter() {
        echo 'Swing::__getAfter()' . PHP_EOL;
    }
}

Xaop::addAroundAop(Swing::class, "__get", function($xaopExec){
    echo '_around_before()' . PHP_EOL;
    Xaop::exec($xaopExec); // To run the Swing::__get() function
    echo '_around_after()' . PHP_EOL;
});

echo '<pre>';

$swing = new Swing();
$swing->di;

echo '</pre>';

/** output:
    _around_before()
    Swing::__get()
    _around_after()
 */
```



Example 3:

```php
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
```



Example4:

```php
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
```

All examples can be found in the source code directory named `demos`, wish you like it. have a nice day.





























