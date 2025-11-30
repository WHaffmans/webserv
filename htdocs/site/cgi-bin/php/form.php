#!/usr/bin/env php-cgi
<?php
require __DIR__ . "/_common.php";

$username = $_POST['username'] ?? '';
$password = $_POST['password'] ?? '';
$remember = isset($_POST['remember']);

$response = [
    "status"    => "success",
    "received"  => [
        "username" => $username,
        "password" => $password ? "********" : "(empty)",
        "remember" => $remember,
    ],
    "validation" => [
        "username_valid" => strlen($username) >= 3,
        "password_valid" => strlen($password) >= 6,
    ],
    "timestamp" => date('c'),
    "server"    => [
        "REQUEST_METHOD" => $_SERVER['REQUEST_METHOD'] ?? '',
        "REMOTE_ADDR"    => $_SERVER['REMOTE_ADDR'] ?? '',
    ]
];

send_json($response);
