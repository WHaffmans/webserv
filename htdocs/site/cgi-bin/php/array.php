#!/usr/bin/env php-cgi
<?php
require __DIR__ . "/_common.php";

$original = [
    ["id"=>1,"name"=>"Apple","price"=>1.5,"category"=>"fruit"],
    ["id"=>2,"name"=>"Banana","price"=>0.75,"category"=>"fruit"],
    ["id"=>3,"name"=>"Carrot","price"=>0.5,"category"=>"vegetable"],
    ["id"=>4,"name"=>"Date","price"=>2.0,"category"=>"fruit"],
    ["id"=>5,"name"=>"Eggplant","price"=>1.25,"category"=>"vegetable"],
];

$action = $_GET['action'] ?? 'reset';
$products = $original;
$code = '';
switch ($action) {
    case 'shuffle':
        shuffle($products);
        $code = "<?php\nshuffle(\$products);\n// Array shuffled randomly";
        break;
    case 'array_reverse':
        $products = array_reverse($products);
        $code = "<?php\n\$result = array_reverse(\$products);\n// Array reversed";
        break;
    case 'usort_price':
        usort($products, fn($a,$b) => $a['price'] <=> $b['price']);
        $code = "<?php\nusort(\$products, fn(\$a,\$b) => \$a['price'] <=> \$b['price']);\n// Sorted by price ascending";
        break;
    case 'usort_name':
        usort($products, fn($a,$b) => strcmp($a['name'], $b['name']));
        $code = "<?php\nusort(\$products, fn(\$a,\$b) => strcmp(\$a['name'], \$b['name']));\n// Sorted alphabetically";
        break;
    case 'array_filter_fruit':
        $products = array_values(array_filter($products, fn($i) => $i['category']==='fruit'));
        $code = "<?php\n\$result = array_filter(\$products, fn(\$i)=>\$i['category']==='fruit');\n// Filter fruit";
        break;
    case 'array_filter_vegetable':
        $products = array_values(array_filter($products, fn($i) => $i['category']==='vegetable'));
        $code = "<?php\n\$result = array_filter(\$products, fn(\$i)=>\$i['category']==='vegetable');\n// Filter vegetable";
        break;
    case 'reset':
    default:
        $code = "<?php\n\$products = \$original_products;\n// Array reset to original state";
        break;
}

$total = 0.0;
foreach ($products as $p) $total += (float)$p['price'];

ob_start();
print_r($products);
$pr = ob_get_clean();

send_json([
    "array"  => $products,
    "total"  => $total,
    "code"   => $code,
    "printr" => $pr,
]);
