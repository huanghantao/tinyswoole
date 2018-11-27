<?php

$serv = new tinyswoole_server('127.0.0.1', 9501, TSWOOLE_SOCK_TCP);
$serv->start();