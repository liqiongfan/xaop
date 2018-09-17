CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(xaop)

set(PHP_NTS_SOURCE /usr/local/php-7.1.15/include/php)
set(PHP_ZTS_SOURCE /usr/local/php-7.1.15-zts/include/php)

include_directories(${PHP_ZTS_SOURCE})
include_directories(${PHP_ZTS_SOURCE}/main)
include_directories(${PHP_ZTS_SOURCE}/Zend)
include_directories(${PHP_ZTS_SOURCE}/TSRM)
include_directories(${PHP_ZTS_SOURCE}/sapi)
include_directories(~/xaop)
include_directories(~/xaop/kernel)

add_executable(xaop xaop.c php_xaop.h kernel/exec.c kernel/exec.h kernel/parsing.c kernel/parsing.h kernel/helper.c
        kernel/helper.h kernel/interface/annotation.c kernel/interface/annotation.h kernel/classes.c kernel/annotations/annotation.c
        kernel/annotations/annotation.h kernel/xaop.c kernel/xaop.h)

#add_custom_target(txaop COMMAND sudo make clean && sudo make -j && sudo make install WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_custom_target(txaop COMMAND sudo make clean && /usr/local/php-7.1.15-zts/bin/phpize && ./configure --with-php-config=/usr/local/php-7.1.15-zts/bin/php-config && make && sudo make install WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})