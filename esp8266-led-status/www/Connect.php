<?php
$servername = "mysql"; // Nome do serviço Docker do MySQL
$username = "espuser";
$password = "esppass";
$dbname = "esp8266_db";

// Criar conexão
$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Conexão falhou: " . $conn->connect_error);
}

if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST['ledStatus'])) {
    $status = $_POST['ledStatus'];
    $timestamp = date('Y-m-d H:i:s');

    $stmt = $conn->prepare("INSERT INTO led_status (status, timestamp) VALUES (?, ?)");
    $stmt->bind_param("ss", $status, $timestamp);

    if ($stmt->execute()) {
        echo "Status do LED registrado com sucesso.";
    } else {
        echo "Erro ao registrar: " . $stmt->error;
    }

    $stmt->close();
} else {
    echo "Dados inválidos ou método incorreto.";
}

$conn->close();
?>
