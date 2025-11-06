<pre>
<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

$target_dir = "uploads/";

function handle_upload($file) {
    global $target_dir;
    $target_file = $target_dir . basename($file["name"]);
    $imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));

    if (move_uploaded_file($file["tmp_name"], $target_file)) {
        echo "Uploaded: " . htmlspecialchars($file["name"]) . "\n";
    } else {
        echo "Error uploading: " . htmlspecialchars($file["name"]) . "\n";
        var_dump(error_get_last());
    }
}

if (isset($_FILES["fileToUpload"]["name"])) {
    // Single or multiple file handling
    if (is_array($_FILES["fileToUpload"]["name"])) {
        foreach ($_FILES["fileToUpload"]["name"] as $i => $name) {
            handle_upload([
                "name" => $_FILES["fileToUpload"]["name"][$i],
                "tmp_name" => $_FILES["fileToUpload"]["tmp_name"][$i]
            ]);
        }
    } else {
        handle_upload($_FILES["fileToUpload"]);
    }
} else {
    echo "No files received.\n";
}
?>
