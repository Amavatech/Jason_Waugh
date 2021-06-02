<?php

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

$servername = "sql5.nur4.host-h.net";

// REPLACE with your Database name
$dbname = "amavasgznz_new";
// REPLACE with Database user
$username = "amavasgznz_9";
// REPLACE with Database user password
$password = "ydhZdqDR5i1erFS6j9B8";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. 
// If you change this value, the ESP32 sketch needs to match
$api_key_value = "tPmAT5Ab3j7F9";

$api_key= $sensor = $location = $value1 = $value2 = $value3 = "";

//if ($_SERVER["REQUEST_METHOD"] == "POST") {
    //$api_key = test_input($_POST["api_key"]);
    // if($api_key == $api_key_value) {
        $TEMP_SET = test_input($_GET["TEMP_SET"]);
        $TIME_SET = test_input($_GET["TIME_SET"]);
        $PROD_TEMP = test_input($_GET["PROD_TEMP"]);
        $STEAM_VALVE_STATUS = test_input($_GET["STEAM_VALVE_STATUS"]);
        $EVENT = test_input($_GET["EVENT"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO CookingPotDataLog (TEMP_SET, TIME_SET, PROD_TEMP, STEAM_VALVE_STATUS, EVENT)
        VALUES ('" . $TEMP_SET . "', '" . $TIME_SET . "', '" . $PROD_TEMP . "', '" . $STEAM_VALVE_STATUS . "', '" . $EVENT . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
    //}
    // else {
       // echo "Wrong API Key provided.";
    // }

//}
// else {
//     echo "No data posted with HTTP POST.";
// }

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}

?>