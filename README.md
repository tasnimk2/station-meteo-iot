\# Station météo IoT avec prédiction par machine learning



Station météo connectée : un \*\*ESP32\*\* mesure la température, l'humidité et la pression, envoie les données en Wi-Fi à un \*\*serveur Flask\*\* qui les stocke dans \*\*SQLite\*\*, et un \*\*tableau de bord web\*\* les affiche en temps réel. Un pipeline de \*\*machine learning\*\* tente de prédire la température à +1 h.



\## Aperçu



!\[Dashboard (jauges)](docs/dashboard\_rose1.png)



!\[Dashboard (graphiques)](docs/dashboard\_rose2.png)



!\[Montage](docs/montage\_breadboard.jpeg)



\## Fonctionnalités



\- Mesure de la température et de l'humidité (DHT22) et de la pression (BMP280)

\- Envoi automatique d'une mesure par minute en HTTP (JSON)

\- Reconnexion Wi-Fi automatique, validation des mesures, redémarrage après échecs répétés

\- Stockage dans une base SQLite et API REST (`/api/latest`, `/api/history`)

\- Tableau de bord : jauges circulaires et graphiques mis à jour automatiquement

\- Pipeline ML : préparation des données, entraînement et évaluation face à une référence naïve



\## Matériel



| Composant | Rôle |

|---|---|

| ESP32 DevKit (30 broches, CP2102, USB-C) | Microcontrôleur Wi-Fi |

| Capteur DHT22 | Température et humidité |

| Capteur BMP280 | Pression atmosphérique (I2C) |

| Breadboard et fils de connexion | Câblage |

| PC (Windows) | Serveur Flask et entraînement ML |



\## Câblage



Les deux capteurs sont alimentés en \*\*3,3 V\*\*.



| Capteur | Broche | ESP32 |

|---|---|---|

| DHT22 | VCC / GND | 3V3 / GND |

| DHT22 | OUT (data) | GPIO 4 |

| BMP280 | VCC / GND | 3V3 / GND |

| BMP280 | SDA | GPIO 21 |

| BMP280 | SCL | GPIO 22 |



!\[Serial Monitor](docs/serial\_monitor.png)



\## Architecture



```

ESP32 (DHT22 + BMP280) --Wi-Fi/HTTP POST--> Flask --> SQLite --> Dashboard web

&#x20;                                                      |

&#x20;                                                      +--> Scripts ML (pandas, scikit-learn)

```



\## Structure du dépôt



```

firmware/   Code Arduino de l'ESP32

server/     Serveur Flask (app.py) et page web (templates/index.html)

ml/         Scripts d'analyse et de machine learning

docs/       Photos, captures et schémas

```



\## Installation



\### 1. ESP32



1\. Installer l'\*\*Arduino IDE\*\* et le support de carte \*\*esp32\*\* (Espressif).

2\. Installer les bibliothèques : \*DHT sensor library\*, \*Adafruit Unified Sensor\*, \*Adafruit BMP280 Library\*.

3\. Dans `firmware/lecture\_combinee/`, créer un fichier `secrets.h` (non publié, ignoré par Git) :



```cpp

\#define WIFI\_SSID "VOTRE\_WIFI"

\#define WIFI\_PASSWORD "VOTRE\_MOT\_DE\_PASSE"

\#define SERVER\_URL "http://IP\_DU\_PC:5000/api/data"

```



4\. Choisir la carte \*\*ESP32 Dev Module\*\*, puis téléverser.



\### 2. Serveur



```

pip install flask pandas scikit-learn joblib

cd server

python app.py

```



Ouvrir ensuite `http://localhost:5000`. Le PC et l'ESP32 doivent être sur le même réseau Wi-Fi (2,4 GHz).



\### 3. Machine learning



```

cd ml

python gaps.py            # contrôle des trous dans la collecte

python prepare\_data.py    # nettoyage et création des variables

python train\_model.py     # entraînement et évaluation

```



\## Méthode ML



\- \*\*Problème :\*\* prédire la température 1 h plus tard (régression sur série temporelle).

\- \*\*Préparation :\*\* filtrage des valeurs aberrantes, regroupement en blocs de 10 minutes, variables de tendance (température 30 min et 1 h avant, humidité 1 h avant) et heure du jour (sin/cos).

\- \*\*Évaluation :\*\* séparation chronologique 80 % / 20 % (pas de mélange aléatoire), erreur moyenne absolue (MAE).

\- \*\*Référence :\*\* méthode naïve de persistance (« dans 1 h, même température qu'actuellement »).

\- \*\*Modèles comparés :\*\* régression linéaire (Ridge) et Random Forest.



\## Résultats (premier prototype)



143 échantillons (114 pour l'entraînement, 29 pour le test).



| Méthode | MAE à +1 h |

|---|---|

| Référence naïve | \*\*0,20 °C\*\* |

| Random Forest | 0,34 °C |

| Régression linéaire (Ridge) | 1,14 °C |



!\[Résultats ML](docs/resultats\_ml.png)



\*\*Interprétation :\*\* sur ce jeu de test court (environ 5 h) et stable, la référence naïve reste meilleure que les modèles. C'est cohérent avec le contexte : la température varie peu, et le DHT22 présente un bruit de mesure du même ordre de grandeur que l'erreur de la référence. Les résultats sont à considérer comme un \*\*prototype\*\* : ils reposent sur un seul cycle jour/nuit.



\## Difficultés rencontrées



\- \*\*Pilote USB et port COM\*\* : la carte n'était pas détectée avant l'installation du pilote CP210x.

\- \*\*Pare-feu Windows\*\* : l'ESP32 ne pouvait pas joindre le serveur tant que le port 5000 n'était pas autorisé.

\- \*\*Arrêt de la collecte\*\* : une interruption de plus de 9 h a montré la nécessité d'empêcher la mise en veille du PC.

\- \*\*Pression aberrante\*\* : le BMP280 a produit des valeurs fausses après certains démarrages. Le code réinitialise désormais le capteur si la valeur est hors plage, et la pression est exclue du modèle en attendant que le capteur soit stabilisé.



\## Limites et améliorations



\- Collecter plusieurs jours de données pour couvrir plusieurs cycles et variations météo

\- Évaluation plus robuste (validation croisée pour séries temporelles)

\- Réintégrer la pression dans le modèle une fois le capteur fiabilisé (souder les broches)

\- Afficher la prédiction sur le tableau de bord

\- Tester d'autres modèles (gradient boosting)



\## Compétences mises en œuvre



Systèmes embarqués (ESP32, I2C), communication Wi-Fi/HTTP, API REST, Flask, SQLite, HTML/CSS/JavaScript (Chart.js), pandas, scikit-learn, Git et GitHub.



\## Auteur



\[Tasnim Karabaka] — étudiante en Licence TIC

