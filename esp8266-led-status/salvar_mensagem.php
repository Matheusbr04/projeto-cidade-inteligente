<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "esp_test";

$mensagem = $_GET['mensagem'] ?? 'Sem mensagem';

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Falha na conexão: " . $conn->connect_error);
}

$sql = "INSERT INTO mensagens (mensagem) VALUES ('$mensagem')";

if ($conn->query($sql) === TRUE) {
    echo "Mensagem salva com sucesso";
} else {
    echo "Erro: " . $conn->error;
}

$conn->close();
?>
