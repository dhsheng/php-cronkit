#### Cronkit

> Periodic task for PHP


```php
<?php
use Cronkit\Application;

/**
 * @name hello
 * @cron * * * * * * 
 */
class Hello {
    public function execute() {
        echo sprintf("hello\n");
    }
}


$app = new Application();
$app->register(Hello::class);
$app->start();
```
