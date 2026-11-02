import sqlite3
import pandas as pd

con = sqlite3.connect("../server/mesures.db")
df = pd.read_sql("SELECT * FROM mesures", con, parse_dates=["horodatage"])
df = df.sort_values("horodatage")
df["ecart_min"] = df["horodatage"].diff().dt.total_seconds() / 60

print("Nombre de mesures :", len(df))
print("Du", df["horodatage"].min(), "au", df["horodatage"].max())

trous = df[df["ecart_min"] > 5]
print("\nTrous de plus de 5 minutes :")
for _, r in trous.iterrows():
    fin = r["horodatage"]
    debut = fin - pd.Timedelta(minutes=r["ecart_min"])
    print(f"  {debut:%d/%m %H:%M} -> {fin:%d/%m %H:%M}  ({r['ecart_min']:.0f} min)")