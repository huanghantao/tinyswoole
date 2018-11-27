## Description

a tiny [swoole](https://github.com/swoole/swoole-src)

## Create server

```php
<?php

$serv = new tinyswoole_server('127.0.0.1', 9501, TSWOOLE_SOCK_TCP);
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

