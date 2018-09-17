# Xaop PHP高性能的AOP扩展 beta

### 功能特色 ###

- **基于对象的文档注解AOP模式**
- **方法注入AOP模式(推荐使用)**

#### 框架 ####

- **Yaf**
- **CSpeed**
- **Xannotation**

#### 系统指令及其含义 ####

- **xaop.method_prefix**

  AOP文档注解需要排除的方法前缀，也就是说如果方法以此前缀开头，那么就会跳过AOP解析

- **xaop.aop_mode**

  AOP工作模式，可选值： 1 | 2 | 3

### 安装 ###

```php
git clone https://github.com/liqiongfan/xaop.git

cd xaop

/usr/local/path_to_php/bin/phpize

./configure --with-php-config=/usr/local/path_to_php/bin/php-config
    
make -j && sudo make install
```

#### 启用对应功能扩展需要在 php.ini 文件配置指令： xaop.aop_mode，如下： ####

```ini
; To enable the AOP mode
; 1 不启用AOP
; 2 文档注解AOP模式
; 3 方法注入AOP模式
xaop.aop_mode = 2
```

#### 1、方法注入AOP模式 DEMO: ####

```php
<?php
    
class Swing
{
    public function __get($name)
    {
        echo '__get<br>';
    }


    public function __set($name, $value)
    {
        echo '__set<br>';
    }

}

// 注入前置AOP
Xaop::addBeforeAop(Swing::class, "__get*", function(){
    echo '__get*_before<br>';
});

// 注入后置AOP
Xaop::addAfterAop(Swing::class, "__get*", function(){
    echo '__get*_after<br>';
});

// 注入后置返回AOP(当方法返回不是null的内容后，此AOP生效)
Xaop::addAfterReturnAop(Swing::class, "__get*", function(){
    echo '__get*_after_return<br>';
});

// 注入后置抛出异常AOP(当方法抛出异常的时候，此AOP生效)
Xaop::addAfterThrowAop(Swing::class, "__get*", function(){
    echo '__get*_after_throw<br>';
});

// 注入环绕AOP(注意环绕AOP与其他的AOP不可同用，存在环绕AOP的情况下，一切以环绕AOP为准)
//Xaop::addAroundAop(NULL, "__get*", function($exec){
//    echo '_before<br>';
//    var_dump(Xaop::exec($exec));
//    echo '_after<br>';
//});
```

```php
<?php
    
$swing = new Swing();
$swing->di;

//输出结果如下
__get*_before
__get
__get*_after
```

**注意**

**Xaop** 支持 五种 **AOP** 模式，分别是 **前置AOP(addBeforeAop)**、**后置AOP(addAfterAop)**、**后置返回AOP(addAfterReturnAop)**、**后置异常AOP(addAfterThrowAop)**、**环绕AOP(addAroundAop)**

其中 **环绕AOP** 跟其他的 **AOP** 互斥，如果存在环绕 **AOP** ，系统将会优先以 **环绕AOP** 模式处理，并且 **环绕AOP** 回调函数存在一个参数： **$xaopExec** 的一个资源表示当前的方法上下文，环绕AOP模式下，如果不在环绕AOP方法内，调用 ：``` Xaop::exec($xaopExec);``` 那么实际的方法将会丢失，不会调用，在环绕模式下，实际方法需要开发者自行调用，并且在同个回调方法内，调用多次 ```Xaop::exec($xaopExec);```，**仅生效一次**，重复调用无效。如：

```php
Xaop::addAroundAop(NULL, "__get*", function($exec){
    echo '_before<br>';
    Xaop::exec($exec); // 此处调用多次，Xaop自动拦截，只执行一次
    echo '_after<br>';
});
```

#### 2、文档注解AOP模式 DEMO: ####

```php
<?php
 
/**
 *@Aspect
 */
class Swing
{
    public function _before() {
        echo '_before';
    }
    
    /**
     *@before(value="Swing._before")
     */
    public function goodLists() {
    	echo 'goodLists';
    }
}
```

因为基于 **ZendOPcode**，所以不需要使用代理对象完成切面，**直接调用方法** 即可：

```php
<?php

$swig = new Swing();
$swig->goodLists();

// 输出如下：

_before goodLists
```

文档注解支持 **自定义注解** 与扩展 **内置注解**：

- **自定义注解**

  自定义注解必须继承自 **Xaop\Annotations** 接口，并且实现 **input** 方法即可，如下示例自定义了一个 **@Tag** 注解：

```php
namespace app;

use Xaop\Annotations;

class Tag implements Annotations {

    function input($object, $annotations) {
        var_dump($object);

        foreach($annotations as $key => $val) {
            echo $key . '=' . $val . PHP_EOL;
        }
    }
}
```

使用的时候只需要传入全名即可：

```php
<?php

/**
 * @Aspect
 */
class Swing
{
    /**
     * @app\Tag( money = 5000, user = "Xaop" )
     */
    public function getMoney() {
        
    }
}
```

- **内置注解**

  内置强大的七个专用注解： **@api**、**@disable**、**@before** 、**@after**、**@success**、**@failure**、 **@deprecated**

  1. **@api**

     开发 **API** 推荐使用，使用本注解，直接可以向客户端返回 **JSON** 或者 **XML** 数据，只需要在修饰的方法体返回数组数据即可，注解包含两个参数：

     **type** 与 **charset**， 如下使用：

     ```php
     /**
      *@api(type=JSON, charset=UTF-8)
      */
     public function newLists() {
     	return [ ['12' => [xxx,xxx], ['23'=>[xxx,xxx] ];
     }
     ```

     **或者**

     ```php
     /**
      *@api(type=xml, charset=UTF-8)
      */
     public function newLists() {
     	return [ ['12' => [xxx,xxx], ['23'=>[xxx,xxx] ];
     }
     ```

     **注意：参数名区分大小写，参数值不区分大小写。**

  2. **@disable**

     使用本注解可以禁用类的方法，使用本注解修饰的方法，就不会调用，并且不会提示任何错误信息，直接返回，本注解不包含任何参数。

  3. **@before**

     前置通知，在方法之前执行本注解包含的方法：如：

     ```php
     @before(value="app\models\User.startTransaction")
     ```

     使用场景：**在执行业务代码逻辑之前开启事务支持**

  4. **@after**

     后置通知，在方法之后执行本注解包含的方法：如：

     ```php
     @after(value="app\log\InvokeLog.record")
     ```

     使用场景：**在接口调用之后进行日志记录**

  5. **@success**

     方法体返回 **true** 的时候调用的通知：如：

     ```php
     @success(value="app\models\User.commit")
     ```

     使用场景：**在业务逻辑代码执行成功之后提交事务**

  6. **@failure**

     方法体返回 **false**之后调用的通知，如下：

     ```php
     @failure(value="app\models\User.rollback")
     ```

     使用场景：**在业务逻辑代码方法体返回失败的时候回滚事务**

  7. **@deprecated**

     标注类的方法是**过期**方法，当调用此方法的时候，会提示一条 **E_DEPRECATED** 的警告信息，需要在 php.ini 文件中开启

     ```php
     @deprecated
     ```

     

     
