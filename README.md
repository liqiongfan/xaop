# Xaop PHP高性能的AOP扩展

### 功能特色 ###

- **基于对象的文档注解AOP模式**
- **方法注入AOP模式**
- **属性AOP模式**

#### 系统指令及其含义 ####

- **xaop.method_prefix**

  AOP文档注解需要排除的方法前缀，也就是说如果方法以此前缀开头，那么就会跳过AOP解析

- **xaop.aop_mode**

  AOP工作模式，可选值： 1(正常模式)  2(文档注解AOP)  3(方法注入AOP)

- **xaop.property_aop**

  属性AOP模式开启状态，1(正常模式)  2(属性AOP模式开启)

### 安装 ###

```php
git clone https://github.com/liqiongfan/xaop.git

cd xaop

sudo ./install
```

#### 指令配置含义： ####

```ini
; 1 不启用AOP
; 2 文档注解AOP模式
; 3 方法注入AOP模式
xaop.aop_mode = 2

; 是否配置过滤方法前缀
; 此模式仅在 文档注解AOP模式生效
xaop.method_prefix = ""

; 是否开启属性AOP模式
; 1 不开启 2 开启
xaop.property_aop = 2
```

#### 1、方法注入AOP模式: ####

```php
<?php
  
class Xaop
{
  public function beforeGetAction()
  {
    echo 'Before GET Action' . PHP_EOL;
  }
  
  public function afterGetAction()
  {
    echo 'After GET Action' . PHP_EOL;
  }
  
  public function getXaop()
  {
    echo 'getXaop method' . PHP_EOL;
  }
}
```

注意：添加AOP方法的第二个参数支持 `一个` 模糊关键字`*`来替代零个或者多个字符，使用多个 `*` 会造成匹配失败，AOP失效。

**下面示例一个使用两种前置AOP的姿势(其他类型的AOP类似)：**

- 闭包模式

```php
Xaop::addBeforeAop(Xaop::class, "getXaop", function(){
  echo 'Before';
});
```

- 类的方法模式，注意这里存在类的生成开销

```php
Xaop::addBeforeAop(Xaop::class, "getXaop", [Xaop::class, "beforeGetAction"]);
```

**[注意]：Xaop的环绕模式AOP仅在此模式下生效，文档注解模式不支持环绕模式AOP，环绕AOP的回调函数存在一个资源参数，表示原方法的OPCODE上下文，如果用户不执行 Xaop::exec($resOpCode)的话，那么就会自动丢失**

- 闭包环绕AOP

```php
Xaop::addAroundAop(Xaop::class, "getXaop", function($resCode){
  echo 'Before';
  Xaop::exec($resCode); // 如果不执行本行代码，原始方法不执行，并且会丢失
  echo 'After';
});
```

- 类方法环绕AOP

```php
class Xaop
{
  public function actionBefore($resCode)
  {
    echo 'Before Code'. PHP_EOL;
    Xaop::exec($resCode);
    echo 'After Code' . PHP_EOL;
  }
  
  public function getXaop()
  {
    echo 'getXaop' . PHP_EOL;
  }
}

Xaop::addAroundAop(Xaop::class, "getXaop", [Xaop::class, "actionBefore"]);
```

**【注意】：如果同时指定了环绕AOP和其他的AOP，那么除了环绕AOP之外，其他的AOP都失效，以环绕AOP为准。**



**Xaop** 支持 五种 **AOP** 模式，分别是 **前置AOP(addBeforeAop)**、**后置AOP(addAfterAop)**、**后置返回AOP(addAfterReturnAop)**、**后置异常AOP(addAfterThrowAop)**、**环绕AOP(addAroundAop)**

其中 **环绕AOP** 跟其他的 **AOP** 互斥，如果存在环绕 **AOP** ，系统将会优先以 **环绕AOP** 模式处理，并且 **环绕AOP** 回调函数存在一个参数： **$xaopExec** 的一个资源表示切入方法的上下文，环绕AOP模式下，如果不在环绕AOP方法内，调用 ：``` Xaop::exec($xaopExec);``` 那么实际的方法将会丢失，不会调用，在环绕模式下，实际方法需要开发者自行调用，并且在同个回调方法内，调用多次 ```Xaop::exec($xaopExec);```，**仅生效一次**，重复调用无效。如：

```php
Xaop::addAroundAop(NULL, "__get*", function($exec){
    echo '_before<br>';
    Xaop::exec($exec); // 此处调用多次，Xaop自动拦截，只执行一次
    echo '_after<br>';
});
```

#### 2、对象属性AOP模式: ####

**Xaop支持对象的属性AOP模式** ：支持前置与后置切入，不实现环绕切入是因为开发者可以通过魔术方法进行处理，简单的示例：

```php
<?php

class Swing
{
    public $di = 'Xaop';
}

Xaop::addPropertyBeforeReadAop(Swing::class, 'di', function($object, $propertyName){
    echo '_before';
});

Xaop::addPropertyAfterReadAop(Swing::class, 'di', function($object, $propertyName){
    echo '_after';
});

// 调用属性
$swing = new Swing();
echo $swing->di;

// 输出如下：
_before Xaop _after
```

#### 3、基于对象调用的文档注解AOP模式: ####

```php
<?php

/**
 * Class Swing
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
```

示例1：

```php
// 调用 __get
$swing = new Swing();
$swing->di
```

输出结果如下：

```php
_magicGetBefore() 
__get 
_magicSuccess() 
_magicGetAfter()
```

示例2：

```php
// 调用 __set
$swing = new Swing();
$swing->di = "di";
```

输出结果如下：

```php
_magicGetBefore() 
__set 
_magicFailure() 
_magicGetAfter()
```

**Xaop** 目前 **基于对象的文档注解 AOP模式** ，如果使用 **静态调用(self::|parent::|static::|class)** 等都不会被捕捉，核心不进行捕捉的原因在于文档注解存在调用注解类的 `input`方法，而 `input`方法的第一个参数为类的对象，因此会额外增加一次对象的生成开销，为了减少对象生成开销，核心去除了静态方法的捕捉功能。

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

