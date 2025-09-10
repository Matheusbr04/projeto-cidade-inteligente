<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "esp_test";

// Pega o valor da URL
$mensagem = $_GET['mensagem'] ?? 'Sem mensagem';

// Conecta ao MySQL
$conn = new mysqli($servername, $username, $password, $dbname);

// Checa conexão
if ($conn->connect_error) {
    die("Falha na conexão: " . $conn->connect_error);
}

// Insere no banco
$sql = "INSERT INTO mensagens (mensagem) VALUES ('$mensagem')";

if ($conn->query($sql) === TRUE) {
    echo "Mensagem salva com sucesso";
} else {
    echo "Erro: " . $conn->error;
}

$conn->close();
?>
