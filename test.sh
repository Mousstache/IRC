#!/bin/bash

# Démarrer Irssi et se connecter automatiquement à un serveur
irssi -c irc.freenode.net &

# Attendre que Irssi soit prêt
sleep 5

# Utiliser execcmd pour exécuter les commandes Irssi
irssi --exec "/network add monserveur"
irssi --exec "/server add -network monserveur 127.0.0.1 6667"
irssi --exec "/connect monserveur"
irssi --exec "/set nick jmastora"
irssi --exec "/join #moncanal"
irssi --exec "/msg #moncanal hello"

