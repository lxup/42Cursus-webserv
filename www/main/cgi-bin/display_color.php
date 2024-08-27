<?php
// Afficher les informations de PHP (version, extensions, etc.)
phpinfo();

// Afficher les variables d'environnement CGI
echo "<h1>Environment Variables</h1>";
echo "<pre>";
print_r($_SERVER);
echo "</pre>";

// Afficher les données GET
if (!empty($_GET)) {
    echo "<h1>GET Data</h1>";
    echo "<pre>";
    print_r($_GET);
    echo "</pre>";
}

// Afficher les données POST
if (!empty($_POST)) {
    echo "<h1>POST Data</h1>";
    echo "<pre>";
    print_r($_POST);
    echo "</pre>";
}
?>
