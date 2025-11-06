<pre>
<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);
ini_set('log_errors', 1);
ini_set('error_log', __DIR__ . '/error_log.txt');


$target_dir = "uploads/";
$target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);
$uploadOk = 1;
$imageFileType = strtolower(pathinfo($target_file,PATHINFO_EXTENSION));

// add debugging output to help trace issues

echo "\n=== POST Data ===\n";
$input = file_get_contents('php://input');
echo "Raw input length: " . strlen($input) . "\n";
echo "Raw input (first 200 chars): " . substr($input, 0, 200) . "\n";

echo "\n=== $_FILES ===\n";
var_dump($_FILES);

echo "\n=== $_POST ===\n";
var_dump($_POST);

// Check if $uploadOk is set to 0 by an error
if ($uploadOk == 0) {
  echo "Sorry, your file was not uploaded.";
// if everything is ok, try to upload file
} else {
  if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
    echo "The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.";
  } else {
    echo "Sorry, there was an error uploading your file.";
    var_dump(error_get_last());
  }
}
?>
