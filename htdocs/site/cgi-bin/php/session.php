#!/usr/bin/env php-cgi
<?php
require __DIR__ . "/_common.php";

// We let PHP manage the cookie (PHPSESSID) for us
$action = $_GET['action'] ?? 'get';

function current_state() {
    $active = session_status() === PHP_SESSION_ACTIVE && isset($_SESSION['_meta']);
    $data = [
        "active" => $active
    ];
    if ($active) {
        $data += [
            "session_id" => session_id(),
            "ip_address" => $_SESSION['_meta']['ip'] ?? '',
            "user_agent" => $_SESSION['_meta']['ua'] ?? '',
            "created_at" => $_SESSION['_meta']['created'] ?? '',
            "last_access"=> $_SESSION['_meta']['last'] ?? '',
            "data"       => $_SESSION['data'] ?? new stdClass(),
        ];
    }
    return $data;
}

if ($action === 'start') {
    session_start();
    if (!isset($_SESSION['_meta'])) {
        $_SESSION['_meta'] = [
            "ip"     => $_SERVER['REMOTE_ADDR'] ?? '',
            "ua"     => $_SERVER['HTTP_USER_AGENT'] ?? '',
            "created"=> date('c'),
            "last"   => date('c'),
        ];
        $_SESSION['data'] = ["visits" => 1, "theme" => "dark"];
    } else {
        $_SESSION['_meta']['last'] = date('c');
        $_SESSION['data']['visits'] = (int)($_SESSION['data']['visits'] ?? 0) + 1;
    }
    send_json(current_state());
}

if ($action === 'add') {
    session_start();
    $key = $_POST['key'] ?? '';
    $val = $_POST['value'] ?? '';
    if ($key !== '') {
        $_SESSION['data'][$key] = $val;
        $_SESSION['_meta']['last'] = date('c');
    }
    send_json(current_state());
}

if ($action === 'remove') {
    session_start();
    $key = $_POST['key'] ?? '';
    if ($key !== '' && isset($_SESSION['data'][$key])) {
        unset($_SESSION['data'][$key]);
        $_SESSION['_meta']['last'] = date('c');
    }
    send_json(current_state());
}

if ($action === 'destroy') {
    session_start();
    $_SESSION = [];
    if (ini_get("session.use_cookies")) {
        $params = session_get_cookie_params();
        setcookie(session_name(), '', time()-42000, $params["path"], $params["domain"], $params["secure"], $params["httponly"]);
    }
    session_destroy();
    send_json(["active" => false]);
}

if ($action === 'get') {
    // Only start a session if PHPSESSID present; otherwise show inactive
    if (!empty($_COOKIE[session_name()])) {
        session_start();
        if (isset($_SESSION['_meta'])) {
            $_SESSION['_meta']['last'] = date('c');
            $_SESSION['data']['visits'] += 1;
            send_json(current_state());
        }
    }
    send_json(["active" => false]);
}
