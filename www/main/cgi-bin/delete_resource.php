#!/usr/bin/env php
<?php
// En-têtes HTTP requis
header('Content-Type: text/plain; charset=utf-8');

$upload_dir = './uploads/';

// Vérifier la méthode de requête
if ($_SERVER['REQUEST_METHOD'] === 'DELETE') {
    // Parse la requête pour récupérer le paramètre 'resource'
    parse_str(file_get_contents("php://input"), $_DELETE);
    $resource = isset($_GET['resource']) ? $_GET['resource'] : '';

    $file_path = $uploads_dir . $resource;

    // Vérifier que la ressource est valide
    if (!empty($resource) && file_exists($file_path)) {
        if (unlink($file_path)) {
            echo "Resource '$resource' has been deleted successfully.";
        } else {
            echo "Failed to delete the resource '$resource'.";
        }
    } else {
        echo "Resource '$resource' does not exist.";
    }
} else {
    echo "Invalid request method. Use DELETE.";
}
?>
