from flask import Flask, render_template, request

app = Flask(__name__)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/register", methods=["POST"])
def register():
    form = request.form
    name = form.get("name")
    dorm = form.get("dorm")

    if not name or not dorm:
        return render_template("failure.html")
    return render_template("success.html")