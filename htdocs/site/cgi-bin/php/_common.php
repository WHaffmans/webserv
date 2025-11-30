<?php
function send_json($arr) {
    header("Content-Type: application/json");
    echo json_encode($arr);
    exit;
}
