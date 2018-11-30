## Description

a tiny [swoole](https://github.com/swoole/swoole-src)

## Create server

```php
<?php

$serv = new TinySwoole\Server('127.0.0.1', 9501, TSWOOLE_TCP);
$serv->start();

```

## Increase the server start event callback

```php
<?php

function onStart()
{
    print_r("server is running..." . PHP_EOL);
}

$serv = new TinySwoole\Server('127.0.0.1', 9501, TSWOOLE_TCP);
$serv->on("Start", "onStart");
$serv->start();

```

## Increase the server connect event callback

```php
<?php

function onConnect($fd)
{
    print_r("client[{$fd}] is connected" . PHP_EOL);
}

$serv = new TinySwoole\Server('127.0.0.1', 9501, TSWOOLE_TCP);
$serv->on("Connect", "onConnect");
$serv->start();

```

## Install from source

```shell
git clone git@github.com:huanghantao/tinyswoole.git && \
cd tinyswoole && \
phpize && \
./configure && \
make && make install
```

