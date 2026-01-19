# Position Based Dynamics

Application Qt/C++ de simulation physique de particules en 2D. Le projet propose plusieurs modes de visualisation et d'interaction avec des systèmes de particules soumis à la gravité et avec une gestion des collisions.

## Fonctionnalités

- Simulation de particules avec physique réaliste
- Plusieurs types de colliders implémentés
- Mini-jeu Flappy Bird intégré
- Simulation de tissu interactif

## Changelog

### Après Mardi 13/01/2026 

- Ajout des particules liées dans `DrawArea` et `DrawAreaFancy` permettant de créer des connexions physiques entre particules

- Ajout d'un système de bascule pour la génération des différents types de particules

- Nouvel onglet dédié à la simulation de tissu (`ClothSimulation`) en étendant la liaison des particules en une grille de tissu interactif (version alpha)