from flask import Flask, render_template, request, redirect

# Configure app
app = Flask(__name__)

# registered students
students = []


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/registrants")
def registrants():
    return render_template("registrants.html", students=students)


@app.route("/register", methods=["POST"])
def register():
    form = request.form
    name = form.get("name")
    dorm = form.get("dorm")

    if not name or not dorm:
        return render_template("failure.html")

    students.append(f"{name} from {dorm}")
    return redirect("/registrants")