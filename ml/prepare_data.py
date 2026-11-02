import sqlite3
import numpy as np
import pandas as pd

# 1. Lecture de la base
con = sqlite3.connect("../server/mesures.db")
df = pd.read_sql("SELECT * FROM mesures", con, parse_dates=["horodatage"])
con.close()
df = df.set_index("horodatage").sort_index()
print("Mesures brutes :", len(df))

# 2. Filtre : temperature et humidite uniquement (la pression du BMP280 est instable)
df = df[df["temperature"].between(5, 45) & df["humidity"].between(0, 100)]
df = df[["temperature", "humidity"]]
print("Mesures conservees :", len(df))

# 3. Regroupement toutes les 10 minutes, petits trous combles (20 min max)
d = df.resample("10min").mean().interpolate(limit=2)

# 4. Variables pour le modele
HORIZON = 6  # 6 x 10 min = 1 heure
d["t_lag3"] = d["temperature"].shift(3)   # il y a 30 min
d["t_lag6"] = d["temperature"].shift(6)   # il y a 1 h
d["h_lag6"] = d["humidity"].shift(6)      # humidite il y a 1 h
heure = d.index.hour + d.index.minute / 60
d["heure_sin"] = np.sin(2 * np.pi * heure / 24)
d["heure_cos"] = np.cos(2 * np.pi * heure / 24)

# 5. Cible : temperature dans 1 heure
d["cible"] = d["temperature"].shift(-HORIZON)

data = d.dropna()
print("Echantillons exploitables :", len(data))
if len(data) < 100:
    print("ATTENTION : peu de donnees, modele a considerer comme un prototype.")

data.to_csv("dataset.csv")

# 6. Separation chronologique entrainement / test (80 % / 20 %)
n = len(data)
coupure = int(n * 0.8)
test = data.iloc[coupure:]
print("Entrainement :", coupure, "| Test :", len(test))

# 7. Reference naive : "dans 1 h, la temperature sera la meme qu'maintenant"
if len(test) > 0:
    mae_naif = np.mean(np.abs(test["cible"] - test["temperature"]))
    print(f"Erreur moyenne de la reference naive (MAE) : {mae_naif:.2f} C")