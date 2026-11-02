from flask import Flask, request, jsonify, render_template
import sqlite3
from datetime import datetime

app = Flask(__name__)
DB = "mesures.db"

def init_db():
    with sqlite3.connect(DB) as c:
        c.execute("""CREATE TABLE IF NOT EXISTS mesures (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            horodatage TEXT,
            temperature REAL,
            humidity REAL,
            pressure REAL)""")

@app.route("/")
def accueil():
    return render_template("index.html")

@app.route("/api/data", methods=["POST"])
def recevoir():
    d = request.get_json()
    with sqlite3.connect(DB) as c:
        c.execute("INSERT INTO mesures (horodatage, temperature, humidity, pressure) VALUES (?,?,?,?)",
                  (datetime.now().isoformat(timespec="seconds"),
                   d["temperature"], d["humidity"], d["pressure"]))
    return jsonify(ok=True)

@app.route("/api/latest")
def derniere():
    with sqlite3.connect(DB) as c:
        r = c.execute("SELECT horodatage, temperature, humidity, pressure FROM mesures ORDER BY id DESC LIMIT 1").fetchone()
    if not r:
        return jsonify({})
    return jsonify(horodatage=r[0], temperature=r[1], humidity=r[2], pressure=r[3])

@app.route("/api/history")
def historique():
    with sqlite3.connect(DB) as c:
        rows = c.execute("SELECT horodatage, temperature, humidity, pressure FROM mesures ORDER BY id DESC LIMIT 500").fetchall()
    rows.reverse()
    return jsonify([dict(horodatage=a, temperature=b, humidity=c_, pressure=d) for a, b, c_, d in rows])

if __name__ == "__main__":
    init_db()
    app.run(host="0.0.0.0", port=5000)