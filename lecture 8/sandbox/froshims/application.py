from flask import Flask, render_template, request
from cs50 import SQL

app = Flask(__name__)

db = SQL("sqlite:///froshims.db")


@app.route("/")
def index():
    q = request.args.get("q")
    # DON"T EVER DO QUERIES WITH f"" THIS, SUBJECT TO SQL INJECTION
    rows = db.execute("SELECT * FROM registrants WHERE name = :name", name=q) if q else db.execute("SELECT * FROM registrants")

    return render_template('index.html', rows=rows)