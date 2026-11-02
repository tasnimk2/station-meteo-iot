import pandas as pd
import joblib
from sklearn.ensemble import RandomForestRegressor
from sklearn.linear_model import Ridge
from sklearn.metrics import mean_absolute_error

data = pd.read_csv("dataset.csv", index_col=0, parse_dates=True)
variables = ["temperature", "humidity", "t_lag3", "t_lag6", "h_lag6",
             "heure_sin", "heure_cos"]

# Separation chronologique : on teste sur la fin de la periode
n = len(data)
coupure = int(n * 0.8)
train, test = data.iloc[:coupure], data.iloc[coupure:]
print(f"Echantillons : {n} | entrainement : {len(train)} | test : {len(test)}")

X_train, X_test = train[variables], test[variables]
# Le modele apprend la VARIATION de temperature sur 1 h (plus facile que la valeur brute)
y_train = train["cible"] - train["temperature"]
y_test = test["cible"]

resultats = {"Reference naive": mean_absolute_error(y_test, test["temperature"])}

modeles = {
    "Regression lineaire (Ridge)": Ridge(alpha=1.0),
    "Random Forest": RandomForestRegressor(n_estimators=200, min_samples_leaf=3,
                                           random_state=42),
}
for nom, modele in modeles.items():
    modele.fit(X_train, y_train)
    prediction = test["temperature"] + modele.predict(X_test)
    resultats[nom] = mean_absolute_error(y_test, prediction)

print("\nErreur moyenne (MAE) sur le jeu de test :")
for nom, mae in resultats.items():
    print(f"  {nom:30s} {mae:.2f} C")

meilleur = min(modeles, key=lambda nom: resultats[nom])
print("\nMeilleur modele :", meilleur)
if resultats[meilleur] < resultats["Reference naive"]:
    print("Il bat la reference naive.")
else:
    print("Il ne bat pas la reference naive : il faut plus de donnees.")

joblib.dump(modeles[meilleur], "modele.joblib")
print("Modele sauvegarde dans modele.joblib")